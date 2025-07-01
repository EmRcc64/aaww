#include "StdAfx.h"
#include "../EterLib/StateManager.h"
#include "../GameLib/MapManager.h"
#include "ActorInstance.h"
#include "../UserInterface/PythonBackground.h"
#include "../EterLib/Camera.h"
#include <random>

bool CActorInstance::ms_isDirLine = false;

bool CActorInstance::IsDirLine()
{
	return ms_isDirLine;
}

void CActorInstance::ShowDirectionLine(bool isVisible)
{
	ms_isDirLine = isVisible;
}

void CActorInstance::SetMaterialColor(DWORD dwColor)
{
	if (m_pkHorse)
		m_pkHorse->SetMaterialColor(dwColor);

	m_dwMtrlColor &= 0xff000000;
	m_dwMtrlColor |= (dwColor & 0x00ffffff);
}

void CActorInstance::SetMaterialAlpha(DWORD dwAlpha)
{
	m_dwMtrlAlpha = dwAlpha;
}

float timer = 0.0f;
void CActorInstance::OnRender()
{
	auto* FlyManager = CFlyingManager::InstancePtr();
	auto* MapManager = FlyManager->GetMapManagerPtr();
	auto& OutDoor = MapManager->GetMapOutdoorRef();

	const float EPSILON = 1e-5f;
	timer += 0.000000060f;

	if (timer > 10)
		timer = 0.0f;

	if (Frustum::Instance().dynamicLight == 1)
	{
		if (FlyManager) {
			if (MapManager && MapManager->IsMapOutdoor() && MapManager->IsMapReady())
			{
				STATEMANAGER.SaveRenderState(D3DRS_FOGENABLE, TRUE);
			}
		}
	}

	D3DMATERIAL8 kMtrl;
	STATEMANAGER.GetMaterial(&kMtrl);
	kMtrl.Diffuse = D3DXCOLOR(m_dwMtrlColor);

	STATEMANAGER.SetMaterial(&kMtrl);
	STATEMANAGER.SaveRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	STATEMANAGER.SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

	if (FlyManager) {
		if (MapManager && MapManager->IsMapOutdoor() && MapManager->IsMapReady())
		{
			if (Frustum::Instance().renderShadow != 4 && m_fAlphaValue == 1.0f)
			{

				//HRESULT hr = D3DXSaveTextureToFile("depth.png", D3DXIFF_PNG, m_lpCharacterShadowMapTexture, nullptr);
				CCamera* pCurrentCamera = CCameraManager::Instance().GetCurrentCamera();
				STATEMANAGER.SetRenderState(D3DRS_FOGENABLE, 0);

				STATEMANAGER.SetVertexShader(OutDoor.m_lpShadowMapVertexShaderMain);
				STATEMANAGER.SetPixelShader(OutDoor.m_lpShadowMapPixelShaderMain);

				float isPC = 0.0f;
				if (IsPC())
				{
					isPC = 1.0f;
				}

				// Vertex shader constants ----
				// Transformation matrix
				D3DXMATRIX worldMat;
				//STATEMANAGER.GetTransform(D3DTS_WORLD, &worldMat);
				ms_lpd3dDevice->GetTransform(D3DTS_WORLD, &worldMat);
				D3DXMatrixTranspose(&worldMat, &worldMat);
				//D3DXMatrixIdentity(&worldMat);
				ms_lpd3dDevice->SetVertexShaderConstantF(0, (const float*)(&worldMat), 4);
				// View-Projection
				D3DXMATRIX viewProjMat, viewMat, projMat;
				ms_lpd3dDevice->GetTransform(D3DTS_VIEW, &viewMat);
				ms_lpd3dDevice->GetTransform(D3DTS_PROJECTION, &projMat);
				// Projection matrix is not w-friendly, so fog doesn't work properly
				ms_lpd3dDevice->SetRenderState(D3DRS_FOGENABLE, 0); // replaced with fog in the shader

				D3DXMatrixMultiplyTranspose(&viewProjMat, &viewMat, &projMat);
				ms_lpd3dDevice->SetVertexShaderConstantF(4, (const float*)(&viewProjMat), 4);
				// Light space
				ms_lpd3dDevice->SetVertexShaderConstantF(8, (const float*)(&OutDoor.m_MatLightViewProj), 4);

				STATEMANAGER.SetVertexShaderConstant(16, (const float*)(&OutDoor.m_MatLightViewProj2), 4);

				// Light ViewProj matrix (TODO)


				// Pixel shader constants ----
				//D3DXVECTOR4 diffuseMatColor = { 1.0f, 1.0f, 1.0f, 1.0f };
				D3DMATERIAL9 material;
				STATEMANAGER.GetMaterial(&material);
				D3DLIGHT8 light;
				STATEMANAGER.GetLight(0, &light);
				STATEMANAGER.SetPixelShaderConstant(0, &light.Diffuse, 1);
				STATEMANAGER.SetPixelShaderConstant(1, &OutDoor.current_v3SunPosition, 1);
				STATEMANAGER.SetPixelShaderConstant(2, &light.Diffuse, 1);

				D3DXVECTOR3 AmbientLight = { 0.7f, 0.7f, 0.7f };

				STATEMANAGER.SetPixelShaderConstant(3, &AmbientLight, 1);
				// Fog settings
				D3DVECTOR cameraPos = CCameraManager::instance().GetCurrentCamera()->GetEye();
				STATEMANAGER.SetPixelShaderConstant(4, &cameraPos, 1);
				D3DCOLOR fogColor;
				STATEMANAGER.GetRenderState(D3DRS_FOGCOLOR, &fogColor);
				D3DXCOLOR fogColorX(fogColor);
				STATEMANAGER.SetPixelShaderConstant(5, (float*)fogColorX, 1);
				D3DVECTOR fogProps;
				STATEMANAGER.GetRenderState(D3DRS_FOGSTART, (DWORD*)&fogProps.x);
				STATEMANAGER.GetRenderState(D3DRS_FOGEND, (DWORD*)&fogProps.y);
				STATEMANAGER.SetPixelShaderConstant(6, &fogProps, 1);

				STATEMANAGER.SetPixelShaderConstant(7, &material.Ambient, 1);
				float shiness;
				D3DXVECTOR3 specularLight;
				float reducePointLightIntensity = 4.0f;

				shiness = 169.0f;
				specularLight = { 10.5f, 10.5f, 10.5f };

				STATEMANAGER.SetPixelShaderConstant(8, &shiness, 1);
				STATEMANAGER.SetPixelShaderConstant(9, &specularLight, 1);

				float shadowOpacity = 75.0f;
				if (shadowOpacity < 40.0f)
					shadowOpacity = 40.0f;

				if (Frustum::Instance().selectRainFrustun == 16 || Frustum::Instance().selectRainFrustun == 25 || Frustum::Instance().selectRainFrustun == 8)
				{
					shadowOpacity = 60.0f;
				}

				STATEMANAGER.SetPixelShaderConstant(10, (int*)&shadowOpacity, 1);

				if (OutDoor.m_lpCharacterShadowMapTexture)
					STATEMANAGER.SetTexture(2, OutDoor.m_lpCharacterShadowMapTexture);
				if (OutDoor.m_lpCharacterShadowMapTexture4)
					STATEMANAGER.SetTexture(3, OutDoor.m_lpCharacterShadowMapTexture4);

				// Transform	
				STATEMANAGER.SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_TEXTURE);
				STATEMANAGER.SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_CURRENT);
				STATEMANAGER.SetSamplerState(1, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
				STATEMANAGER.SetSamplerState(1, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);

				STATEMANAGER.SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_MODULATE);			
				STATEMANAGER.SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

			}
			else
			{
				STATEMANAGER.SaveRenderState(D3DRS_FOGENABLE, TRUE);
			}
		}
	}

	switch (m_iRenderMode)
	{
	case RENDER_MODE_NORMAL:
		BeginDiffuseRender();
		RenderWithOneTexture();
		EndDiffuseRender();
		BeginOpacityRender();
		BlendRenderWithOneTexture();
		EndOpacityRender();
		break;
	case RENDER_MODE_BLEND:
		if (m_fAlphaValue == 1.0f)
		{
			BeginDiffuseRender();
			RenderWithOneTexture();
			EndDiffuseRender();
			BeginOpacityRender();
			BlendRenderWithOneTexture();
			EndOpacityRender();
		}
		else if (m_fAlphaValue > 0.0f)
		{
			BeginBlendRender();
			RenderWithOneTexture();
			BlendRenderWithOneTexture();
			EndBlendRender();
		}
		break;
	case RENDER_MODE_ADD:
		BeginAddRender();
		RenderWithOneTexture();
		BlendRenderWithOneTexture();
		EndAddRender();
		break;
	case RENDER_MODE_MODULATE:
		BeginModulateRender();
		RenderWithOneTexture();
		BlendRenderWithOneTexture();
		EndModulateRender();
		break;
	}

	STATEMANAGER.RestoreRenderState(D3DRS_CULLMODE);

	if (FlyManager) {
		if (MapManager && MapManager->IsMapOutdoor() && MapManager->IsMapReady())
		{
			if (Frustum::Instance().renderShadow != 4 && m_fAlphaValue == 1.0f)
			{
				ms_lpd3dDevice->SetVertexShader(nullptr);
				ms_lpd3dDevice->SetPixelShader(nullptr);
			}
			else
			{
				STATEMANAGER.RestoreRenderState(D3DRS_FOGENABLE);
			}
		}
	}

	kMtrl.Diffuse = D3DXCOLOR(0xffffffff);
	STATEMANAGER.SetMaterial(&kMtrl);

	if (ms_isDirLine)
	{
		D3DXVECTOR3 kD3DVt3Cur(m_x, m_y, m_z);

		D3DXVECTOR3 kD3DVt3LookDir(0.0f, -1.0f, 0.0f);
		D3DXMATRIX kD3DMatLook;
		D3DXMatrixRotationZ(&kD3DMatLook, D3DXToRadian(GetRotation()));
		D3DXVec3TransformCoord(&kD3DVt3LookDir, &kD3DVt3LookDir, &kD3DMatLook);
		D3DXVec3Scale(&kD3DVt3LookDir, &kD3DVt3LookDir, 200.0f);
		D3DXVec3Add(&kD3DVt3LookDir, &kD3DVt3LookDir, &kD3DVt3Cur);

		D3DXVECTOR3 kD3DVt3AdvDir(0.0f, -1.0f, 0.0f);
		D3DXMATRIX kD3DMatAdv;
		D3DXMatrixRotationZ(&kD3DMatAdv, D3DXToRadian(GetAdvancingRotation()));
		D3DXVec3TransformCoord(&kD3DVt3AdvDir, &kD3DVt3AdvDir, &kD3DMatAdv);
		D3DXVec3Scale(&kD3DVt3AdvDir, &kD3DVt3AdvDir, 200.0f);
		D3DXVec3Add(&kD3DVt3AdvDir, &kD3DVt3AdvDir, &kD3DVt3Cur);

		static CScreen s_kScreen;

		STATEMANAGER.SaveTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
		STATEMANAGER.SaveTextureStageState(0, D3DTSS_COLOROP, D3DTOP_DISABLE);
		STATEMANAGER.SaveTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
		STATEMANAGER.SaveRenderState(D3DRS_ZENABLE, FALSE);
		STATEMANAGER.SetRenderState(D3DRS_LIGHTING, TRUE);

		s_kScreen.SetDiffuseColor(1.0f, 1.0f, 0.0f);
		s_kScreen.RenderLine3d(kD3DVt3Cur.x, kD3DVt3Cur.y, kD3DVt3Cur.z, kD3DVt3AdvDir.x, kD3DVt3AdvDir.y, kD3DVt3AdvDir.z);

		s_kScreen.SetDiffuseColor(0.0f, 1.0f, 1.0f);
		s_kScreen.RenderLine3d(kD3DVt3Cur.x, kD3DVt3Cur.y, kD3DVt3Cur.z, kD3DVt3LookDir.x, kD3DVt3LookDir.y, kD3DVt3LookDir.z);

		STATEMANAGER.SetRenderState(D3DRS_LIGHTING, TRUE);
		STATEMANAGER.RestoreRenderState(D3DRS_ZENABLE);

		STATEMANAGER.RestoreTextureStageState(0, D3DTSS_COLORARG1);
		STATEMANAGER.RestoreTextureStageState(0, D3DTSS_COLOROP);
		STATEMANAGER.RestoreTextureStageState(0, D3DTSS_ALPHAOP);

#ifdef DIRECTX9
		STATEMANAGER.RestoreFVF();
#else
		STATEMANAGER.RestoreVertexShader();
#endif
	}

	if (Frustum::Instance().dynamicLight == 1)
	{
		if (FlyManager) {
			if (MapManager && MapManager->IsMapOutdoor() && MapManager->IsMapReady())
			{
				STATEMANAGER.RestoreRenderState(D3DRS_FOGENABLE);
			}
		}
	}
}

void CActorInstance::BeginDiffuseRender()
{
	STATEMANAGER.SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);

	STATEMANAGER.SaveRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
}

void CActorInstance::EndDiffuseRender()
{
	STATEMANAGER.RestoreRenderState(D3DRS_ALPHABLENDENABLE);
}

void CActorInstance::BeginOpacityRender()
{
	STATEMANAGER.SaveRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	STATEMANAGER.SaveRenderState(D3DRS_ALPHAREF, 0);
	STATEMANAGER.SaveRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	STATEMANAGER.SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
}

void CActorInstance::EndOpacityRender()
{
	STATEMANAGER.RestoreRenderState(D3DRS_ALPHATESTENABLE);
	STATEMANAGER.RestoreRenderState(D3DRS_ALPHAREF);
	STATEMANAGER.RestoreRenderState(D3DRS_ALPHAFUNC);
}

void CActorInstance::BeginBlendRender()
{
	STATEMANAGER.SaveRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	STATEMANAGER.SaveRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	STATEMANAGER.SaveRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	STATEMANAGER.SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
	STATEMANAGER.SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

	STATEMANAGER.SetRenderState(D3DRS_TEXTUREFACTOR, D3DXCOLOR(1.0f, 1.0f, 1.0f, m_fAlphaValue));
	STATEMANAGER.SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2);
}

void CActorInstance::EndBlendRender()
{
	STATEMANAGER.RestoreRenderState(D3DRS_ALPHABLENDENABLE);
	STATEMANAGER.RestoreRenderState(D3DRS_SRCBLEND);
	STATEMANAGER.RestoreRenderState(D3DRS_DESTBLEND);
}

void CActorInstance::BeginAddRender()
{
	STATEMANAGER.SetRenderState(D3DRS_TEXTUREFACTOR, m_AddColor);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);

	STATEMANAGER.SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_CURRENT);
	STATEMANAGER.SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_TFACTOR);
	STATEMANAGER.SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_ADD);
	STATEMANAGER.SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

	STATEMANAGER.SaveRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
}

void CActorInstance::EndAddRender()
{
	STATEMANAGER.SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
	STATEMANAGER.SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
	STATEMANAGER.RestoreRenderState(D3DRS_ALPHABLENDENABLE);
}

void CActorInstance::RestoreRenderMode()
{
	// NOTE : This is temporary code. I wanna convert this code to that restore the mode to
	//        model's default setting which had has as like specular or normal. - [levites]
	m_iRenderMode = RENDER_MODE_NORMAL;
	if (m_kBlendAlpha.m_isBlending)
	{
		m_kBlendAlpha.m_iOldRenderMode = m_iRenderMode;
	}
}


void CActorInstance::SetAddRenderMode()
{
	m_iRenderMode = RENDER_MODE_ADD;
	if (m_kBlendAlpha.m_isBlending)
	{
		m_kBlendAlpha.m_iOldRenderMode = m_iRenderMode;
	}
}

void CActorInstance::SetRenderMode(int iRenderMode)
{
	m_iRenderMode = iRenderMode;
	if (m_kBlendAlpha.m_isBlending)
	{
		m_kBlendAlpha.m_iOldRenderMode = iRenderMode;
	}
}

void CActorInstance::SetAddColor(const D3DXCOLOR& c_rColor)
{
	m_AddColor = c_rColor;
	m_AddColor.a = 1.0f;
}

void CActorInstance::BeginModulateRender()
{
	STATEMANAGER.SetRenderState(D3DRS_TEXTUREFACTOR, m_AddColor);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);

	STATEMANAGER.SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_CURRENT);
	STATEMANAGER.SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_TFACTOR);
	STATEMANAGER.SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_MODULATE);
	STATEMANAGER.SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

	STATEMANAGER.SaveRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
}

void CActorInstance::EndModulateRender()
{
	STATEMANAGER.SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
	STATEMANAGER.SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
	STATEMANAGER.RestoreRenderState(D3DRS_ALPHABLENDENABLE);
}

void CActorInstance::SetModulateRenderMode()
{
	m_iRenderMode = RENDER_MODE_MODULATE;
	if (m_kBlendAlpha.m_isBlending)
	{
		m_kBlendAlpha.m_iOldRenderMode = m_iRenderMode;
	}
}

void CActorInstance::RenderCollisionData()
{
	static CScreen s_Screen;

	STATEMANAGER.SetRenderState(D3DRS_LIGHTING, FALSE);
	STATEMANAGER.SaveRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	if (m_pAttributeInstance)
	{
		for (DWORD col = 0; col < GetCollisionInstanceCount(); ++col)
		{
			CBaseCollisionInstance* pInstance = GetCollisionInstanceData(col);
			pInstance->Render();
		}
	}

	STATEMANAGER.SetRenderState(D3DRS_ZENABLE, FALSE);
	s_Screen.SetColorOperation();
	s_Screen.SetDiffuseColor(1.0f, 0.0f, 0.0f);
	TCollisionPointInstanceList::iterator itor;
	/*itor = m_AttackingPointInstanceList.begin();
	for (; itor != m_AttackingPointInstanceList.end(); ++itor)
	{
		const TCollisionPointInstance & c_rInstance = *itor;
		for (DWORD i = 0; i < c_rInstance.SphereInstanceVector.size(); ++i)
		{
			const CDynamicSphereInstance & c_rSphereInstance = c_rInstance.SphereInstanceVector[i];
			s_Screen.RenderCircle3d(c_rSphereInstance.v3Position.x,
									c_rSphereInstance.v3Position.y,
									c_rSphereInstance.v3Position.z,
									c_rSphereInstance.fRadius);
		}
	}*/
	s_Screen.SetDiffuseColor(1.0f, (isShow()) ? 1.0f : 0.0f, 0.0f);
	D3DXVECTOR3 center;
	float r;
	GetBoundingSphere(center, r);
	s_Screen.RenderCircle3d(center.x, center.y, center.z, r);

	s_Screen.SetDiffuseColor(0.0f, 0.0f, 1.0f);
	itor = m_DefendingPointInstanceList.begin();
	for (; itor != m_DefendingPointInstanceList.end(); ++itor)
	{
		const TCollisionPointInstance& c_rInstance = *itor;
		for (DWORD i = 0; i < c_rInstance.SphereInstanceVector.size(); ++i)
		{
			const CDynamicSphereInstance& c_rSphereInstance = c_rInstance.SphereInstanceVector[i];
			s_Screen.RenderCircle3d(c_rSphereInstance.v3Position.x,
				c_rSphereInstance.v3Position.y,
				c_rSphereInstance.v3Position.z,
				c_rSphereInstance.fRadius);
		}
	}

	s_Screen.SetDiffuseColor(0.0f, 1.0f, 0.0f);
	itor = m_BodyPointInstanceList.begin();
	for (; itor != m_BodyPointInstanceList.end(); ++itor)
	{
		const TCollisionPointInstance& c_rInstance = *itor;
		for (DWORD i = 0; i < c_rInstance.SphereInstanceVector.size(); ++i)
		{
			const CDynamicSphereInstance& c_rSphereInstance = c_rInstance.SphereInstanceVector[i];
			s_Screen.RenderCircle3d(c_rSphereInstance.v3Position.x,
				c_rSphereInstance.v3Position.y,
				c_rSphereInstance.v3Position.z,
				c_rSphereInstance.fRadius);
		}
	}

	s_Screen.SetDiffuseColor(1.0f, 0.0f, 0.0f);
	//	if (m_SplashArea.fDisappearingTime > GetLocalTime())
	{
		CDynamicSphereInstanceVector::iterator itor = m_kSplashArea.SphereInstanceVector.begin();
		for (; itor != m_kSplashArea.SphereInstanceVector.end(); ++itor)
		{
			const CDynamicSphereInstance& c_rInstance = *itor;
			s_Screen.RenderCircle3d(c_rInstance.v3Position.x,
				c_rInstance.v3Position.y,
				c_rInstance.v3Position.z,
				c_rInstance.fRadius);
		}
	}

	STATEMANAGER.SetRenderState(D3DRS_ZENABLE, TRUE);
	STATEMANAGER.RestoreRenderState(D3DRS_CULLMODE);
	STATEMANAGER.SetRenderState(D3DRS_LIGHTING, TRUE);
}


void CActorInstance::RenderToShadowMap()
{
	if (RENDER_MODE_BLEND == m_iRenderMode)
	{
		if (GetAlphaValue() < 0.5f)
			return;
	}

	CGraphicThingInstance::RenderToShadowMap();

	if (m_pkHorse)
		m_pkHorse->RenderToShadowMap();
}
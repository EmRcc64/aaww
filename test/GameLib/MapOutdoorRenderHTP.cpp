#include "StdAfx.h"
#include "MapOutdoor.h"
#include "../EterLib/Camera.h"

#include "../eterlib/StateManager.h"

#include "../ScriptLib/StdAfx.h"

#include "../UserInterface/PythonSystem.h"
#include "../UserInterface/PythonBackground.h"

void CMapOutdoor::__RenderTerrain_RenderHardwareTransformPatch()
{
	DWORD dwFogColor;
	float fFogFarDistance;
	float fFogNearDistance;
	if (mc_pEnvironmentData)
	{
		dwFogColor = mc_pEnvironmentData->FogColor;
		fFogNearDistance = mc_pEnvironmentData->GetFogNearDistance();
		fFogFarDistance = mc_pEnvironmentData->GetFogFarDistance();
	}
	else
	{
		dwFogColor = 0xffffffff;
		fFogNearDistance = 5000.0f;
		fFogFarDistance = 10000.0f;
	}

	//////////////////////////////////////////////////////////////////////////
	// Render State & TextureStageState

	STATEMANAGER.SaveTextureStageState(0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEPOSITION);
	STATEMANAGER.SaveTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);
	STATEMANAGER.SaveTextureStageState(1, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEPOSITION);
	STATEMANAGER.SaveTextureStageState(1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);

	STATEMANAGER.SaveRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	STATEMANAGER.SaveRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	STATEMANAGER.SaveRenderState(D3DRS_ALPHAREF, 0x00000000);
	STATEMANAGER.SaveRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	STATEMANAGER.SaveRenderState(D3DRS_TEXTUREFACTOR, dwFogColor);

	STATEMANAGER.SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CURRENT);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
#ifdef DIRECTX9
	STATEMANAGER.SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	STATEMANAGER.SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
#else
	STATEMANAGER.SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP);
#endif

	STATEMANAGER.SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_CURRENT);
	STATEMANAGER.SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	STATEMANAGER.SetTextureStageState(1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	STATEMANAGER.SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
#ifdef DIRECTX9
	STATEMANAGER.SetSamplerState(1, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	STATEMANAGER.SetSamplerState(1, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
#else
	STATEMANAGER.SetTextureStageState(1, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
	STATEMANAGER.SetTextureStageState(1, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
#endif

	if (Frustum::Instance().dynamicLight == 3)
	{
		STATEMANAGER.SetSamplerState(2, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
		STATEMANAGER.SetSamplerState(2, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
	}

#ifdef WORLD_EDITOR
	if (GetAsyncKeyState(VK_CAPITAL))
	{
		CSpeedTreeWrapper::ms_bSelfShadowOn = false;
		STATEMANAGER.SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_GAUSSIANCUBIC);
		STATEMANAGER.SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_GAUSSIANCUBIC);
		STATEMANAGER.SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTEXF_GAUSSIANCUBIC);
		STATEMANAGER.SetTextureStageState(1, D3DTSS_MINFILTER, D3DTEXF_GAUSSIANCUBIC);
		STATEMANAGER.SetTextureStageState(1, D3DTSS_MAGFILTER, D3DTEXF_GAUSSIANCUBIC);
		STATEMANAGER.SetTextureStageState(1, D3DTSS_MIPFILTER, D3DTEXF_GAUSSIANCUBIC);
	}
	else
	{
		CSpeedTreeWrapper::ms_bSelfShadowOn = true;
		STATEMANAGER.SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
		STATEMANAGER.SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
		STATEMANAGER.SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTEXF_LINEAR);
		STATEMANAGER.SetTextureStageState(1, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
		STATEMANAGER.SetTextureStageState(1, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
		STATEMANAGER.SetTextureStageState(1, D3DTSS_MIPFILTER, D3DTEXF_LINEAR);
	}
#else
	CSpeedTreeWrapper::ms_bSelfShadowOn = true;
	STATEMANAGER.SetBestFiltering(2);
#endif

	m_matWorldForCommonUse._41 = 0.0f;
	m_matWorldForCommonUse._42 = 0.0f;
	STATEMANAGER.SetTransform(D3DTS_WORLD, &m_matWorldForCommonUse);
	D3DXMATRIX worldMat;
	D3DXMatrixTranspose(&worldMat, &m_matWorldForCommonUse);
	STATEMANAGER.SetVertexShaderConstant(20, &worldMat, 4);

	STATEMANAGER.SaveTransform(D3DTS_TEXTURE0, &m_matWorldForCommonUse);
	STATEMANAGER.SaveTransform(D3DTS_TEXTURE1, &m_matWorldForCommonUse);

	// Render State & TextureStageState
	//////////////////////////////////////////////////////////////////////////

#ifdef DIRECTX9
	STATEMANAGER.SetFVF(D3DFVF_XYZ | D3DFVF_NORMAL);
#else
	STATEMANAGER.SetVertexShader(D3DFVF_XYZ | D3DFVF_NORMAL);
#endif

	m_iRenderedSplatNumSqSum = 0;
	m_iRenderedPatchNum = 0;
	m_iRenderedSplatNum = 0;
	m_RenderedTextureNumVector.clear();

	std::pair<float, long> fog_far(fFogFarDistance + 1600.0f, 0);
	std::pair<float, long> fog_near(fFogNearDistance - 3200.0f, 0);

	std::vector<std::pair<float, long> >::iterator far_it = std::upper_bound(m_PatchVector.begin(), m_PatchVector.end(), fog_far);
	std::vector<std::pair<float, long> >::iterator near_it = std::upper_bound(m_PatchVector.begin(), m_PatchVector.end(), fog_near);

#ifdef WORLD_EDITOR
	near_it = m_PatchVector.begin();
	far_it = m_PatchVector.end();
#endif

	WORD wPrimitiveCount;
	D3DPRIMITIVETYPE ePrimitiveType;

	BYTE byCUrrentLODLevel = 0;

	float fLODLevel1Distance = __GetNoFogDistance();
	float fLODLevel2Distance = __GetFogDistance();

	SelectIndexBuffer(0, &wPrimitiveCount, &ePrimitiveType);

	DWORD dwFogEnable = STATEMANAGER.GetRenderState(D3DRS_FOGENABLE);
	std::vector<std::pair<float, long> >::iterator it = m_PatchVector.begin();

#ifndef WORLD_EDITOR
	STATEMANAGER.SetRenderState(D3DRS_FOGENABLE, FALSE);

	for (; it != near_it; ++it)
	{
		if (byCUrrentLODLevel == 0 && fLODLevel1Distance <= it->first)
		{
			byCUrrentLODLevel = 1;
			SelectIndexBuffer(1, &wPrimitiveCount, &ePrimitiveType);
		}
		else if (byCUrrentLODLevel == 1 && fLODLevel2Distance <= it->first)
		{
			byCUrrentLODLevel = 2;
			SelectIndexBuffer(2, &wPrimitiveCount, &ePrimitiveType);
		}

		__HardwareTransformPatch_RenderPatchSplat(it->second, wPrimitiveCount, ePrimitiveType);
		if (m_iRenderedSplatNum >= m_iSplatLimit)
			break;

		if (m_bDrawWireFrame)
			DrawWireFrame(it->second, wPrimitiveCount, ePrimitiveType);
	}
#endif

	STATEMANAGER.SetRenderState(D3DRS_FOGENABLE, dwFogEnable);

	if (m_iRenderedSplatNum < m_iSplatLimit)
	{
		for (it = near_it; it != far_it; ++it)
		{
			if (byCUrrentLODLevel == 0 && fLODLevel1Distance <= it->first)
			{
				byCUrrentLODLevel = 1;
				SelectIndexBuffer(1, &wPrimitiveCount, &ePrimitiveType);
			}
			else if (byCUrrentLODLevel == 1 && fLODLevel2Distance <= it->first)
			{
				byCUrrentLODLevel = 2;
				SelectIndexBuffer(2, &wPrimitiveCount, &ePrimitiveType);
			}

			__HardwareTransformPatch_RenderPatchSplat(it->second, wPrimitiveCount, ePrimitiveType);

			if (m_iRenderedSplatNum >= m_iSplatLimit)
				break;

			if (m_bDrawWireFrame)
				DrawWireFrame(it->second, wPrimitiveCount, ePrimitiveType);
		}
	}

	STATEMANAGER.SetRenderState(D3DRS_FOGENABLE, FALSE);
	STATEMANAGER.SetRenderState(D3DRS_LIGHTING, FALSE);

	STATEMANAGER.SetTexture(0, NULL);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, FALSE);

	STATEMANAGER.SetTexture(1, NULL);
	STATEMANAGER.SetTextureStageState(1, D3DTSS_TEXTURETRANSFORMFLAGS, FALSE);

	STATEMANAGER.SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TFACTOR);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

	STATEMANAGER.SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
	STATEMANAGER.SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

	if (m_iRenderedSplatNum < m_iSplatLimit)
	{
		for (it = far_it; it != m_PatchVector.end(); ++it)
		{
			if (byCUrrentLODLevel == 0 && fLODLevel1Distance <= it->first)
			{
				byCUrrentLODLevel = 1;
				SelectIndexBuffer(1, &wPrimitiveCount, &ePrimitiveType);
			}
			else if (byCUrrentLODLevel == 1 && fLODLevel2Distance <= it->first)
			{
				byCUrrentLODLevel = 2;
				SelectIndexBuffer(2, &wPrimitiveCount, &ePrimitiveType);
			}

			__HardwareTransformPatch_RenderPatchNone(it->second, wPrimitiveCount, ePrimitiveType);

			if (m_iRenderedSplatNum >= m_iSplatLimit)
				break;

			if (m_bDrawWireFrame)
				DrawWireFrame(it->second, wPrimitiveCount, ePrimitiveType);
		}
	}

	STATEMANAGER.SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CURRENT);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);

	STATEMANAGER.SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_CURRENT);
	STATEMANAGER.SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	STATEMANAGER.SetTextureStageState(1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	STATEMANAGER.SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);

	STATEMANAGER.SetRenderState(D3DRS_FOGENABLE, 0);
	STATEMANAGER.SetRenderState(D3DRS_LIGHTING, TRUE);

	std::sort(m_RenderedTextureNumVector.begin(), m_RenderedTextureNumVector.end());

	//////////////////////////////////////////////////////////////////////////
	// Render State & TextureStageState

	STATEMANAGER.RestoreRenderState(D3DRS_TEXTUREFACTOR);

	STATEMANAGER.RestoreTransform(D3DTS_TEXTURE0);
	STATEMANAGER.RestoreTransform(D3DTS_TEXTURE1);

	STATEMANAGER.RestoreTextureStageState(0, D3DTSS_TEXCOORDINDEX);
	STATEMANAGER.RestoreTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS);
	STATEMANAGER.RestoreTextureStageState(1, D3DTSS_TEXCOORDINDEX);
	STATEMANAGER.RestoreTextureStageState(1, D3DTSS_TEXTURETRANSFORMFLAGS);

	STATEMANAGER.RestoreRenderState(D3DRS_ALPHABLENDENABLE);
	STATEMANAGER.RestoreRenderState(D3DRS_ALPHATESTENABLE);
	STATEMANAGER.RestoreRenderState(D3DRS_ALPHAREF);
	STATEMANAGER.RestoreRenderState(D3DRS_ALPHAFUNC);

	// Render State & TextureStageState
	//////////////////////////////////////////////////////////////////////////
}

void CMapOutdoor::__RenderTerrain_RenderHardwareTransformPatch2()
{
	m_matWorldForCommonUse._41 = 0.0f;
	m_matWorldForCommonUse._42 = 0.0f;
	STATEMANAGER.SetTransform(D3DTS_WORLD, &m_matWorldForCommonUse);
	D3DXMATRIX worldMat;
	D3DXMatrixTranspose(&worldMat, &m_matWorldForCommonUse);
	STATEMANAGER.SetVertexShaderConstant(20, &worldMat, 4);

	std::pair<float, long> fog_near(400000.0f, 0);

	std::vector<std::pair<float, long> >::iterator near_it = std::upper_bound(m_PatchVector.begin(), m_PatchVector.end(), fog_near);


	WORD wPrimitiveCount;
	D3DPRIMITIVETYPE ePrimitiveType;

	SelectIndexBuffer(0, &wPrimitiveCount, &ePrimitiveType);

	DWORD dwFogEnable = STATEMANAGER.GetRenderState(D3DRS_FOGENABLE);
	std::vector<std::pair<float, long> >::iterator it = m_PatchVector.begin();

#ifndef WORLD_EDITOR
	STATEMANAGER.SetRenderState(D3DRS_FOGENABLE, FALSE);

	for (; it != near_it; ++it)
	{
		__HardwareTransformPatch_RenderPatchSplat2(it->second, wPrimitiveCount, ePrimitiveType);
	}
#endif

	std::sort(m_RenderedTextureNumVector.begin(), m_RenderedTextureNumVector.end());
}

float time3 = 0.0f;
float timeAuraEnabled = 0.0f;
float wetness = 1.0f;
float checkMap = 0.0f;
float shinessRain = 0.0f;
float puddleThreshold = 0.0f;
void CMapOutdoor::__HardwareTransformPatch_RenderPatchSplat(long patchnum, WORD wPrimitiveCount, D3DPRIMITIVETYPE ePrimitiveType)
{
	assert(NULL != m_pTerrainPatchProxyList && "__HardwareTransformPatch_RenderPatchSplat");
	CTerrainPatchProxy* pTerrainPatchProxy = &m_pTerrainPatchProxyList[patchnum];

	if (!pTerrainPatchProxy->isUsed())
		return;

	long sPatchNum = pTerrainPatchProxy->GetPatchNum();
	if (sPatchNum < 0)
		return;

	BYTE ucTerrainNum = pTerrainPatchProxy->GetTerrainNum();
	if (0xFF == ucTerrainNum)
		return;

	CTerrain* pTerrain;
	if (!GetTerrainPointer(ucTerrainNum, &pTerrain))
		return;

	DWORD dwFogColor;
	if (mc_pEnvironmentData)
		dwFogColor = mc_pEnvironmentData->FogColor;
	else
		dwFogColor = 0xffffffff;

	WORD wCoordX, wCoordY;
	pTerrain->GetCoordinate(&wCoordX, &wCoordY);

	TTerrainSplatPatch& rTerrainSplatPatch = pTerrain->GetTerrainSplatPatch();

	D3DXMATRIX matTexTransform, matSplatAlphaTexTransform, matSplatColorTexTransform;
	m_matWorldForCommonUse._41 = -(float)(wCoordX * CTerrainImpl::TERRAIN_XSIZE);
	m_matWorldForCommonUse._42 = (float)(wCoordY * CTerrainImpl::TERRAIN_YSIZE);
	D3DXMatrixMultiply(&matTexTransform, &m_matViewInverse, &m_matWorldForCommonUse);
	D3DXMatrixMultiply(&matSplatAlphaTexTransform, &matTexTransform, &m_matSplatAlpha);
	STATEMANAGER.SetTransform(D3DTS_TEXTURE1, &matSplatAlphaTexTransform);

	D3DXMATRIX matTexTransformAlpha;
	D3DXMatrixMultiplyTranspose(&matTexTransformAlpha, &m_matWorldForCommonUse, &m_matSplatAlpha);
	STATEMANAGER.SetVertexShaderConstant(28, &matTexTransformAlpha, 4);

	D3DXMATRIX matTiling;
	D3DXMatrixScaling(&matTiling, 1.0f / 640.0f, -1.0f / 640.0f, 0.0f);
	matTiling._41 = 0.0f;
	matTiling._42 = 0.0f;

	D3DXMatrixMultiply(&matSplatColorTexTransform, &m_matViewInverse, &matTiling);
	STATEMANAGER.SetTransform(D3DTS_TEXTURE0, &matSplatColorTexTransform);

	CGraphicVertexBuffer* pkVB = pTerrainPatchProxy->HardwareTransformPatch_GetVertexBufferPtr();
	if (!pkVB)
		return;

	STATEMANAGER.SetStreamSource(0, pkVB->GetD3DVertexBuffer(), m_iPatchTerrainVertexSize);

	STATEMANAGER.SetRenderState(D3DRS_LIGHTING, TRUE);
	int iPrevRenderedSplatNum = m_iRenderedSplatNum;

#ifdef WORLD_EDITOR
#else
	float fDepthBias = 0.00001f;
	ms_lpd3dDevice->SetRenderState(D3DRS_DEPTHBIAS, *(DWORD*)(&fDepthBias));
#if defined(ENABLE_ENVIRONMENT_EFFECT_OPTION)
#else

	bool isFirst = true;
	for (DWORD j = 1; j < pTerrain->GetNumTextures(); ++j)
	{
		TTerainSplat& rSplat = rTerrainSplatPatch.Splats[j];

		if (!rSplat.Active)
			continue;

		if (rTerrainSplatPatch.PatchTileCount[sPatchNum][j] == 0)
			continue;

		const TTerrainTexture& rTexture = m_TextureSet.GetTexture(j);


			D3DXMatrixMultiply(&matSplatColorTexTransform, &m_matViewInverse, &rTexture.m_matTransform);
			STATEMANAGER.SetTransform(D3DTS_TEXTURE0, &matSplatColorTexTransform);
			if (isFirst)
			{
				STATEMANAGER.SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);  // ?? << undefined behavior
				STATEMANAGER.SetTexture(0, rTexture.pd3dTexture);
				STATEMANAGER.SetTexture(1, rSplat.pd3dTexture);
				STATEMANAGER.DrawIndexedPrimitive(ePrimitiveType, 0, m_iPatchTerrainVertexCount, 0, wPrimitiveCount);
				STATEMANAGER.SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
				isFirst = false;
			}
			else
			{
				STATEMANAGER.SetTexture(0, rTexture.pd3dTexture);
				STATEMANAGER.SetTexture(1, rSplat.pd3dTexture);
				STATEMANAGER.DrawIndexedPrimitive(ePrimitiveType, 0, m_iPatchTerrainVertexCount, 0, wPrimitiveCount);
			}


		std::vector<int>::iterator aIterator = std::find(m_RenderedTextureNumVector.begin(), m_RenderedTextureNumVector.end(), (int)j);
		if (aIterator == m_RenderedTextureNumVector.end())
			m_RenderedTextureNumVector.push_back(j);
		++m_iRenderedSplatNum;
		if (m_iRenderedSplatNum >= m_iSplatLimit)
			break;
	}

#endif
#endif // END_IFDEF_WORLD_EDITOR

	// 그림자

	fDepthBias = -0.00003f;
	ms_lpd3dDevice->SetRenderState(D3DRS_DEPTHBIAS, *(DWORD*)(&fDepthBias));

	time3 += 0.1f;

	if (Frustum::Instance().shadowType != 3 && Frustum::Instance().isIndoor == false)
	{
		//ms_lpd3dDevice->SetRenderState(D3DRS_FOGENABLE, 1); // replaced with fog in the shader
		STATEMANAGER.SetRenderState(D3DRS_LIGHTING, TRUE);

		STATEMANAGER.SetRenderState(D3DRS_FOGCOLOR, 0xFFFFFFFF);
		STATEMANAGER.SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ZERO);
		STATEMANAGER.SetRenderState(D3DRS_DESTBLEND, D3DBLEND_SRCCOLOR);

		D3DXMATRIX matShadowTexTransform;
		D3DXMatrixMultiply(&matShadowTexTransform, &matTexTransform, &m_matStaticShadow);
		STATEMANAGER.SetTexture(0, m_lpCharacterShadowMapTexture);

		if (Frustum::Instance().isIndoor == false && Frustum::Instance().shadowType != 3)
		{


			STATEMANAGER.SetVertexShader(m_lpShadowMapVertexShaderTerrain);
			STATEMANAGER.SetPixelShader(m_lpShadowMapPixelShaderTerrain);


			// Transformations

			D3DXMATRIX worldMat;
			STATEMANAGER.GetTransform(D3DTS_WORLD, &worldMat);
			D3DXMatrixTranspose(&worldMat, &worldMat);
			STATEMANAGER.SetVertexShaderConstant(20, &worldMat, 4);
			// View-Projection
			D3DXMATRIX viewProjMat, viewMat, projMat;
			STATEMANAGER.GetTransform(D3DTS_VIEW, &viewMat);
			STATEMANAGER.GetTransform(D3DTS_PROJECTION, &projMat);
			D3DXMatrixMultiplyTranspose(&viewProjMat, &viewMat, &projMat);
			STATEMANAGER.SetVertexShaderConstant(4, &viewProjMat, 4);
			// Light space
			STATEMANAGER.SetVertexShaderConstant(8, &m_MatLightViewProj, 4);


			STATEMANAGER.SetVertexShaderConstant(16, &m_MatLightViewProj2, 4);


			D3DMATERIAL9 material;
			STATEMANAGER.GetMaterial(&material);
			D3DLIGHT9 light;
			STATEMANAGER.GetLight(0, &light);
			STATEMANAGER.SetPixelShaderConstant(0, &material.Diffuse, 1);

			STATEMANAGER.SetPixelShaderConstant(1, &current_v3SunPosition, 1);
			STATEMANAGER.SetPixelShaderConstant(2, &light.Diffuse, 1);
			STATEMANAGER.SetPixelShaderConstant(3, &light.Diffuse, 1);
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

			STATEMANAGER.SetPixelShaderConstant(7, &material.Diffuse, 1);

			float shadowOpacity = 62.0f;
			STATEMANAGER.SetPixelShaderConstant(10, (int*)&shadowOpacity, 1);

			int specularLightEnabled = Frustum::Instance().realtimeSpecular;
			STATEMANAGER.SetPixelShaderConstant(11, &specularLightEnabled, 1);

			float shiness;
			D3DXVECTOR3 specularLight;
			float reducePointLightIntensity = 15.0f;

			shiness = 15.0f;
			specularLight = { 0.5f, 0.5f, 0.5f };

			STATEMANAGER.SetPixelShaderConstant(8, &shiness, 1);
			STATEMANAGER.SetPixelShaderConstant(9, &specularLight, 1);
			STATEMANAGER.SetPixelShaderConstant(12, &time3, 1);

			float enableLights = 1.0f;
			STATEMANAGER.SetPixelShaderConstant(13, &enableLights, 1);

			float terrainLightMultiply;
			terrainLightMultiply = 1.75f;


			STATEMANAGER.SetPixelShaderConstant(14, &terrainLightMultiply, 1);
			STATEMANAGER.SetPixelShaderConstant(15, &reducePointLightIntensity, 1);
		}

		STATEMANAGER.SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		STATEMANAGER.SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CURRENT);
		STATEMANAGER.SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
		STATEMANAGER.SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		STATEMANAGER.SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_CURRENT);
		STATEMANAGER.SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
		STATEMANAGER.SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
		STATEMANAGER.SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);


		//STATEMANAGER.SetTransform(D3DTS_TEXTURE1, &m_matDynamicShadow);

		if (Frustum::Instance().isIndoor == false && Frustum::Instance().renderShadow != 4)
		{
			if (m_lpCharacterShadowMapTexture)
				STATEMANAGER.SetTexture(1, m_lpCharacterShadowMapTexture);
			if (m_lpCharacterShadowMapTexture4)
				STATEMANAGER.SetTexture(2, m_lpCharacterShadowMapTexture4);


			//STATEMANAGER.SetTexture(1, NULL);
			STATEMANAGER.SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_TEXTURE);
			STATEMANAGER.SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_CURRENT);
			STATEMANAGER.SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_MODULATE);
			STATEMANAGER.SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

			STATEMANAGER.SetSamplerState(1, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
			STATEMANAGER.SetSamplerState(1, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);

		}
		else
		{
			STATEMANAGER.SetTexture(1, NULL);
		}

		ms_faceCount += wPrimitiveCount;
		STATEMANAGER.DrawIndexedPrimitive(ePrimitiveType, 0, m_iPatchTerrainVertexCount, 0, wPrimitiveCount);
		++m_iRenderedSplatNum;

		STATEMANAGER.SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_CURRENT);
		STATEMANAGER.SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
		STATEMANAGER.SetTextureStageState(1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		STATEMANAGER.SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
		
		STATEMANAGER.SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		STATEMANAGER.SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CURRENT);
		STATEMANAGER.SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
		STATEMANAGER.SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		STATEMANAGER.SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
		STATEMANAGER.SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
		STATEMANAGER.SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);

		STATEMANAGER.SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		STATEMANAGER.SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		STATEMANAGER.SetRenderState(D3DRS_FOGCOLOR, dwFogColor);

		STATEMANAGER.SetRenderState(D3DRS_LIGHTING, FALSE);

		STATEMANAGER.SetVertexShader(nullptr);
		STATEMANAGER.SetPixelShader(nullptr);
	}
	++m_iRenderedPatchNum;

	int iCurRenderedSplatNum = m_iRenderedSplatNum - iPrevRenderedSplatNum;

	m_iRenderedSplatNumSqSum += iCurRenderedSplatNum * iCurRenderedSplatNum;

	fDepthBias = 0.0f;
	ms_lpd3dDevice->SetRenderState(D3DRS_DEPTHBIAS, *(DWORD*)(&fDepthBias));
}


void CMapOutdoor::__HardwareTransformPatch_RenderPatchSplat2(long patchnum, WORD wPrimitiveCount, D3DPRIMITIVETYPE ePrimitiveType)
{
	assert(NULL != m_pTerrainPatchProxyList && "__HardwareTransformPatch_RenderPatchSplat");
	CTerrainPatchProxy* pTerrainPatchProxy = &m_pTerrainPatchProxyList[patchnum];

	if (!pTerrainPatchProxy->isUsed())
		return;

	long sPatchNum = pTerrainPatchProxy->GetPatchNum();
	if (sPatchNum < 0)
		return;

	BYTE ucTerrainNum = pTerrainPatchProxy->GetTerrainNum();
	if (0xFF == ucTerrainNum)
		return;

	CTerrain* pTerrain;
	if (!GetTerrainPointer(ucTerrainNum, &pTerrain))
		return;

	DWORD dwFogColor;
	if (mc_pEnvironmentData)
		dwFogColor = mc_pEnvironmentData->FogColor;
	else
		dwFogColor = 0xffffffff;

	WORD wCoordX, wCoordY;
	pTerrain->GetCoordinate(&wCoordX, &wCoordY);

	TTerrainSplatPatch& rTerrainSplatPatch = pTerrain->GetTerrainSplatPatch();

	D3DXMATRIX matTexTransform, matSplatAlphaTexTransform, matSplatColorTexTransform;
	m_matWorldForCommonUse._41 = -(float)(wCoordX * CTerrainImpl::TERRAIN_XSIZE);
	m_matWorldForCommonUse._42 = (float)(wCoordY * CTerrainImpl::TERRAIN_YSIZE);
	D3DXMatrixMultiply(&matTexTransform, &m_matViewInverse, &m_matWorldForCommonUse);
	D3DXMatrixMultiply(&matSplatAlphaTexTransform, &matTexTransform, &m_matSplatAlpha);
	STATEMANAGER.SetTransform(D3DTS_TEXTURE1, &matSplatAlphaTexTransform);

	D3DXMATRIX matTiling;
	D3DXMatrixScaling(&matTiling, 1.0f / 640.0f, -1.0f / 640.0f, 0.0f);
	matTiling._41 = 0.0f;
	matTiling._42 = 0.0f;

	D3DXMatrixMultiply(&matSplatColorTexTransform, &m_matViewInverse, &matTiling);
	STATEMANAGER.SetTransform(D3DTS_TEXTURE0, &matSplatColorTexTransform);

	CGraphicVertexBuffer* pkVB = pTerrainPatchProxy->HardwareTransformPatch_GetVertexBufferPtr();
	if (!pkVB)
		return;

	STATEMANAGER.SetStreamSource(0, pkVB->GetD3DVertexBuffer(), m_iPatchTerrainVertexSize);


	int iPrevRenderedSplatNum = m_iRenderedSplatNum;


	TTerrainTexture& rTexture = m_TextureSet.GetTexture(10);

	D3DXMatrixMultiply(&matSplatColorTexTransform, &m_matViewInverse, &rTexture.m_matTransform);

	STATEMANAGER.SetTransform(D3DTS_TEXTURE0, &matSplatColorTexTransform);

	STATEMANAGER.SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
	STATEMANAGER.SetTexture(0, rTexture.pd3dTexture);
	STATEMANAGER.SetTexture(1, nullptr);  // Disable the splat texture

	STATEMANAGER.DrawIndexedPrimitive(ePrimitiveType, 0, m_iPatchTerrainVertexCount, 0, wPrimitiveCount);
	++m_iRenderedPatchNum;

	int iCurRenderedSplatNum = m_iRenderedSplatNum - iPrevRenderedSplatNum;

	m_iRenderedSplatNumSqSum += iCurRenderedSplatNum * iCurRenderedSplatNum;
}

void CMapOutdoor::__HardwareTransformPatch_RenderPatchNone(long patchnum, WORD wPrimitiveCount, D3DPRIMITIVETYPE ePrimitiveType)
{
	assert(NULL != m_pTerrainPatchProxyList && "__HardwareTransformPatch_RenderPatchNone");
	CTerrainPatchProxy* pTerrainPatchProxy = &m_pTerrainPatchProxyList[patchnum];

	if (!pTerrainPatchProxy->isUsed())
		return;

	CGraphicVertexBuffer* pkVB = pTerrainPatchProxy->HardwareTransformPatch_GetVertexBufferPtr();
	if (!pkVB)
		return;

	STATEMANAGER.SetStreamSource(0, pkVB->GetD3DVertexBuffer(), m_iPatchTerrainVertexSize);
	STATEMANAGER.DrawIndexedPrimitive(ePrimitiveType, 0, m_iPatchTerrainVertexCount, 0, wPrimitiveCount);
}
#include "StdAfx.h"
#include "../EterLib/StateManager.h"
#include "../EterLib/Camera.h"
#include "FlyingObjectManager.h"
#include "MapManager.h"
#include "MapOutdoor.h"
#include "../UserInterface/PythonBackground.h"
#include <sstream>
#include <string>
#include <cmath> // include this header for fmax
#include "MapManager.h"
#include <random>
#include <unordered_set>
#include <chrono>

int radomzPosition = (rand() % 5) + 1;

float z = 0.025f;
float zMultiplier = 15.0f;
float tolerance = 0.001f; // adjust this value to your desired tolerance level

bool a = true;
D3DXVECTOR3 VECTOR3(0.0f, 0.0f, 1.0f);

#include "../EterPack/EterPackManager.h"
#include "shadowmapVS.h"
#include "shadowmapPS.h"
#include "terrainShadowsVS.h"
#include "terrainShadowsPS.h"
#include "shadowmapObjectsVS.h"
#include "shadowmapObjectsPS.h"
#include "shadowmapDepthVS.h"
#include "shadowmapDepthPS.h"
#include "../UserInterface/InstanceBase.h"

LPDIRECT3DVERTEXSHADER9 CMapOutdoor::LoadAndBuildVertexShader(const char* filename)
{
	LPD3DXBUFFER pShaderBuffer = nullptr;
	LPD3DXBUFFER pErrorBuffer = nullptr;

	HRESULT hr = D3DXCompileShader(
		filename,            // Shader code
		strlen(filename),
		nullptr,       // Optional defines
		nullptr,       // Optional include handler
		"main",        // Entry point for the vertex shader
		"vs_3_0",      // Target shader model version 
		0,             // Compilation flags (0 for default)
		&pShaderBuffer,
		&pErrorBuffer,
		nullptr);

	if (FAILED(hr))
		if (pErrorBuffer) {
			const char* errorMsg = static_cast<const char*>(pErrorBuffer->GetBufferPointer());
			TraceError("Vertex shader compilation error %s", errorMsg);
			return nullptr;
		}

	LPDIRECT3DVERTEXSHADER9 shader = nullptr;
	hr = ms_lpd3dDevice->CreateVertexShader(
		reinterpret_cast<DWORD*>(pShaderBuffer->GetBufferPointer()),
		&shader
	);
	if (FAILED(hr)) TraceError("CreateVertexShader failed");

	if (pShaderBuffer)
		pShaderBuffer->Release();

	return shader;
}

LPDIRECT3DPIXELSHADER9 CMapOutdoor::LoadAndBuildPixelShader(const char* filename, bool HLSL)
{
	LPD3DXBUFFER pShaderBuffer = nullptr;
	LPD3DXBUFFER pErrorBuffer = nullptr;
	HRESULT hr;
	if (HLSL) {
		hr = D3DXCompileShader(
			filename,            // Shader code
			strlen(filename),
			nullptr,       // Optional defines
			nullptr,       // Optional include handler
			"main",        // Entry point for the vertex shader
			"ps_3_0",      // Target shader model version 
			0,             // Compilation flags (0 for default)
			&pShaderBuffer,
			&pErrorBuffer,
			nullptr
		);
	}
	else {
		hr = D3DXAssembleShader(
			filename,
			strlen(filename),
			nullptr,
			nullptr,
			0,
			&pShaderBuffer,
			&pErrorBuffer);
	}
	if (FAILED(hr))
	{
		if (pErrorBuffer) {
			const char* errorMsg = static_cast<const char*>(pErrorBuffer->GetBufferPointer());
			TraceError("Pixel shader compilation error %s", errorMsg);
			return nullptr;
		}
	}

	LPDIRECT3DPIXELSHADER9 shader = nullptr;
	hr = ms_lpd3dDevice->CreatePixelShader(
		reinterpret_cast<DWORD*>(pShaderBuffer->GetBufferPointer()),
		&shader
	);
	if (FAILED(hr))
		TraceError("CreatePixelShader failed");

	if (pShaderBuffer)
		pShaderBuffer->Release();

	return shader;
}

void CMapOutdoor::ReleaseShaders()
{

	if (m_lpShadowMapVertexShaderDepth)
	{
		m_lpShadowMapVertexShaderDepth->Release();
		m_lpShadowMapVertexShaderDepth = nullptr;
	}

	if (m_lpShadowMapPixelShaderDepth)
	{
		m_lpShadowMapPixelShaderDepth->Release();
		m_lpShadowMapPixelShaderDepth = nullptr;
	}

	if (m_lpShadowMapVertexShaderTerrain)
	{
		m_lpShadowMapVertexShaderTerrain->Release();
		m_lpShadowMapVertexShaderTerrain = nullptr;
	}

	if (m_lpShadowMapPixelShaderTerrain)
	{
		m_lpShadowMapPixelShaderTerrain->Release();
		m_lpShadowMapPixelShaderTerrain = nullptr;
	}

	if (m_lpPixelShaderTerrainBase)
	{
		m_lpPixelShaderTerrainBase->Release();
		m_lpPixelShaderTerrainBase = nullptr;
	}

	if (m_lpVertexShaderTerrainBase)
	{
		m_lpVertexShaderTerrainBase->Release();
		m_lpVertexShaderTerrainBase = nullptr;
	}

	if (m_lpShadowMapVertexShaderObjects)
	{
		m_lpShadowMapVertexShaderObjects->Release();
		m_lpShadowMapVertexShaderObjects = nullptr;
	}

	if (m_lpShadowMapPixelShaderObjects)
	{
		m_lpShadowMapPixelShaderObjects->Release();
		m_lpShadowMapPixelShaderObjects = nullptr;
	}

	if (m_lpShadowMapVertexShaderMain)
	{
		m_lpShadowMapVertexShaderMain->Release();
		m_lpShadowMapVertexShaderMain = nullptr;
	}

	if (m_lpShadowMapPixelShaderMain)
	{
		m_lpShadowMapPixelShaderMain->Release();
		m_lpShadowMapPixelShaderMain = nullptr;
	}

	shadersReleased = true;
}

void CMapOutdoor::CreateShadowmapShaders()
{
	if (!m_lpShadowMapVertexShaderDepth)
		m_lpShadowMapVertexShaderDepth = LoadAndBuildVertexShader(shadowmapDepthVS);

	if (!m_lpShadowMapPixelShaderDepth)
		m_lpShadowMapPixelShaderDepth = LoadAndBuildPixelShader(shadowmapDepthPS);

	// Terrain
	if (!m_lpShadowMapVertexShaderTerrain)
		m_lpShadowMapVertexShaderTerrain = LoadAndBuildVertexShader(terrainShadowsVS);

	if (!m_lpShadowMapPixelShaderTerrain)
		m_lpShadowMapPixelShaderTerrain = LoadAndBuildPixelShader(terrainShadowsPS);

	//objects
	if (!m_lpShadowMapVertexShaderObjects)
		m_lpShadowMapVertexShaderObjects = LoadAndBuildVertexShader(shadowmapObjectsVS);

	if (!m_lpShadowMapPixelShaderObjects)
		m_lpShadowMapPixelShaderObjects = LoadAndBuildPixelShader(shadowmapObjectsPS);

	// Main render stage shaders
	if (!m_lpShadowMapVertexShaderMain)
		m_lpShadowMapVertexShaderMain = LoadAndBuildVertexShader(shadowmapVS);
	if (!m_lpShadowMapPixelShaderMain)
		m_lpShadowMapPixelShaderMain = LoadAndBuildPixelShader(shadowmapPS);

	if (!m_lpShadowMapPixelShaderDepthTree)
		m_lpShadowMapPixelShaderDepthTree = LoadAndBuildPixelShader(shadowmapDepthTreePS, false);

	shadersReleased = false;
}

void CMapOutdoor::SetShadowTextureSize(WORD size)
{
	if (m_wShadowMapSize != size)
	{
		recreate = true;
		Tracenf("ShadowTextureSize changed %d -> %d", m_wShadowMapSize, size);
	}

	m_wShadowMapSize = size;
}

void CMapOutdoor::LoadDepthTerrainTexture(const char* textureName, IDirect3DTexture9** texture)
{
	CMappedFile	mappedFile;
	LPCVOID		c_pvMap;

	if (CEterPackManager::Instance().Get(mappedFile, textureName, &c_pvMap))
	{
		if (FAILED(D3DXCreateTextureFromFileInMemory(ms_lpd3dDevice, static_cast<const char*>(c_pvMap), mappedFile.Size(), texture)))
		{
			TraceError("Texture not loaded: %s", textureName);
		}
	}
}

bool loadTerrainTexture = true;
LPDIRECT3DTEXTURE9 grassTexture1 = nullptr;
LPDIRECT3DTEXTURE9 grassTexture2 = nullptr;
void CMapOutdoor::CreateCharacterShadowTexture()
{
	extern bool GRAPHICS_CAPS_CAN_NOT_DRAW_SHADOW;

	if (GRAPHICS_CAPS_CAN_NOT_DRAW_SHADOW)
		return;

	ReleaseCharacterShadowTexture();

	if (IsLowTextureMemory())
		SetShadowTextureSize(128);

	m_ShadowMapViewport.X = 1;
	m_ShadowMapViewport.Y = 1;
	m_ShadowMapViewport.Width = m_wShadowMapSize + 2;
	m_ShadowMapViewport.Height = m_wShadowMapSize + 2;
	m_ShadowMapViewport.MinZ = 0.0f;
	m_ShadowMapViewport.MaxZ = 1.0f;

	m_ShadowMapViewportTree.X = 1;
	m_ShadowMapViewportTree.Y = 1;
	if (Frustum::Instance().shadowType == 2)
	{
		m_ShadowMapViewportTree.Width = (m_wShadowMapSize - 2) / 1.0;
		m_ShadowMapViewportTree.Height = (m_wShadowMapSize - 2) / 1.0;
	}
	else
	{
		m_ShadowMapViewportTree.Width = m_wShadowMapSize;
		m_ShadowMapViewportTree.Height = m_wShadowMapSize;
	}
	m_ShadowMapViewportTree.MinZ = 0.0f;
	m_ShadowMapViewportTree.MaxZ = 1.0f;

	if (Frustum::Instance().shadowType == 2)
	{
		if (FAILED(ms_lpd3dDevice->CreateTexture(m_wShadowMapSize / 1.0, m_wShadowMapSize / 1.0, 1, D3DUSAGE_RENDERTARGET, D3DFMT_R32F, D3DPOOL_DEFAULT, &m_lpCharacterShadowMapTexture, NULL)))
		{
			TraceError("CMapOutdoor Unable to create Character Shadow render target texture\n");
			return;
		}
	}
	else
	{
		if (FAILED(ms_lpd3dDevice->CreateTexture(m_wShadowMapSize, m_wShadowMapSize, 1, D3DUSAGE_RENDERTARGET, D3DFMT_R32F, D3DPOOL_DEFAULT, &m_lpCharacterShadowMapTexture, NULL)))
		{
			TraceError("CMapOutdoor Unable to create Character Shadow render target texture\n");
			return;
		}
	}

	if (FAILED(m_lpCharacterShadowMapTexture->GetSurfaceLevel(0, &m_lpCharacterShadowMapRenderTargetSurface)))
	{
		TraceError("CMapOutdoor Unable to GetSurfaceLevel Character Shadow render target texture\n");
		return;
	}

	if (FAILED(ms_lpd3dDevice->CreateTexture(m_wShadowMapSize, m_wShadowMapSize, 1, D3DUSAGE_RENDERTARGET, D3DFMT_R32F, D3DPOOL_DEFAULT, &m_lpCharacterShadowMapTexture4, NULL)))
	{
		TraceError("CMapOutdoor Unable to create Character Shadow render target texture\n");
		return;
	}

	if (FAILED(m_lpCharacterShadowMapTexture4->GetSurfaceLevel(0, &m_lpCharacterShadowMapRenderTargetSurface4)))
	{
		TraceError("CMapOutdoor Unable to GetSurfaceLevel Character Shadow render target texture\n");
		return;
	}

	if (FAILED(ms_lpd3dDevice->CreateTexture(m_wShadowMapSize, m_wShadowMapSize, 1, D3DUSAGE_RENDERTARGET, D3DFMT_R5G6B5, D3DPOOL_DEFAULT, &m_lpCharacterShadowMapTexture2, NULL)))
	{
		TraceError("CMapOutdoor Unable to create Character Shadow render target texture\n");
		return;
	}

	if (FAILED(m_lpCharacterShadowMapTexture2->GetSurfaceLevel(0, &m_lpCharacterShadowMapRenderTargetSurface2)))
	{
		TraceError("CMapOutdoor Unable to GetSurfaceLevel Character Shadow render target texture\n");
		return;
	}


	if (FAILED(ms_lpd3dDevice->CreateDepthStencilSurface(m_wShadowMapSize, m_wShadowMapSize, D3DFMT_D16, D3DMULTISAMPLE_NONE, 0, true, &m_lpCharacterShadowMapDepthSurface, NULL)))
	{
		TraceError("CMapOutdoor Unable to create Character Shadow depth Surface\n");
		return;
	}

	if (loadTerrainTexture)
	{
		//JTX SPECULAR TEXTURE
		LoadDepthTerrainTexture("d:/ymir work/terrainmaps/temp/shadowhelper.png", &Frustum::Instance().shadowTexture);
		LoadDepthTerrainTexture("d:/ymir work/terrainmaps/temp/shadowhelperblack.png", &Frustum::Instance().shadowTextureBlack);
		LoadDepthTerrainTexture("d:/ymir work/terrainmaps/temp/spheremap.jpg", &Frustum::Instance().specularTexture);

		loadTerrainTexture = false;
	}


}

void CMapOutdoor::ReleaseCharacterShadowTexture()
{
	SAFE_RELEASE(m_lpCharacterShadowMapRenderTargetSurface);
	SAFE_RELEASE(m_lpCharacterShadowMapRenderTargetSurface2);
	SAFE_RELEASE(m_lpCharacterShadowMapRenderTargetSurface4);
	SAFE_RELEASE(m_lpCharacterShadowMapTexture);
	SAFE_RELEASE(m_lpCharacterShadowMapTexture2);
	SAFE_RELEASE(m_lpCharacterShadowMapTexture4);
	SAFE_RELEASE(m_lpCharacterShadowMapDepthSurface);
	SAFE_RELEASE(m_lpCharacterShadowMapDepthSurface);

}

DWORD dwLightEnable = FALSE;

void CMapOutdoor::SkyUtil()
{

	if (Frustum::Instance().renderShadow == 4)
	{
		ReleaseShaders();
		ReleaseCharacterShadowTexture();
	}

	if (Frustum::Instance().renderShadow != 4 && shadersReleased == true)
	{
		CreateShadowmapShaders();
		CreateCharacterShadowTexture();
	}

	if (recreate && Frustum::Instance().renderShadow != 4)
	{
		CreateCharacterShadowTexture();
		recreate = false;
	}


	const TEnvironmentData* envData = 0;
	CFlyingManager::Instance().GetMapManagerPtr()->GetCurrentEnvironmentData(&envData);
	D3DXVECTOR3 v3SunPosition = envData->DirLights[ENV_DIRLIGHT_BACKGROUND].Direction;
	v3SunPosition.z = v3SunPosition.z * 1.5f;

	Frustum::Instance().currentEnvironmentFace = envData->strSkyBoxFaceFileName[0].c_str();

	Frustum::Instance().waterSunDirection = { 0.0, 0.0, 0.0 };
	Frustum::Instance().characterSpecularLightPower = 0.8f;
	Frustum::Instance().isNightEnvironment = 0.0f;
	Frustum::Instance().isStormEnvironment = 0.0f;
	Frustum::Instance().AmbientLight = { 0.4f, 0.4f, 0.4f };
	isNoapteEnvironment = false;
	Frustum::Instance().treeNormalMap = 2.0f;
	Frustum::Instance().treeFinalColor = 65.0f;

	if (Frustum::Instance().currentEnvironmentFace.find("environment01_front") != std::string::npos)
	{
		Frustum::Instance().terrainLightPowerEnvBased = 7.0f;
		Frustum::Instance().isNightEnvironment = 1.0f;
		isNoapteEnvironment = true;
	}
	else if (Frustum::Instance().currentEnvironmentFace.find("environment02_front") != std::string::npos)
	{
		Frustum::Instance().terrainLightPowerEnvBased = 5.0f;
		Frustum::Instance().characterSpecularLightPower = 1.4f;
		Frustum::Instance().AmbientLight = { 1.2f, 1.2f, 1.2f };
	}
	else if (Frustum::Instance().currentEnvironmentFace.find("environment03_front") != std::string::npos)
	{
		Frustum::Instance().terrainLightPowerEnvBased = 4.5f;
		Frustum::Instance().characterSpecularLightPower = 1.0f;
	}
	else if (Frustum::Instance().currentEnvironmentFace.find("environment04_front") != std::string::npos)
	{
		Frustum::Instance().terrainLightPowerEnvBased = 7.0f;
	}
	else if (Frustum::Instance().currentEnvironmentFace.find("environment05_front") != std::string::npos)
	{
		Frustum::Instance().terrainLightPowerEnvBased = 4.0f;
		Frustum::Instance().characterSpecularLightPower = 1.1f;
		Frustum::Instance().AmbientLight = { 0.03f, 0.03f, 0.03f };
	}
	else if (Frustum::Instance().currentEnvironmentFace.find("environment06_front") != std::string::npos)
	{
		Frustum::Instance().terrainLightPowerEnvBased = 8.0f;
		Frustum::Instance().characterSpecularLightPower = 1.6f;
		Frustum::Instance().AmbientLight = { 0.8f, 0.8f, 0.8f };
	}
	else if (Frustum::Instance().currentEnvironmentFace.find("environment07_front") != std::string::npos)
	{
		Frustum::Instance().terrainLightPowerEnvBased = 7.0f;
		Frustum::Instance().characterSpecularLightPower = 1.1f;
		Frustum::Instance().AmbientLight = { 0.03f, 0.03f, 0.03f };
	}
	else if (Frustum::Instance().currentEnvironmentFace.find("environment08_front") != std::string::npos)
	{
		Frustum::Instance().terrainLightPowerEnvBased = 4.0f;
		Frustum::Instance().characterSpecularLightPower = 1.1f;
		Frustum::Instance().treeNormalMap = 2.0f;
		Frustum::Instance().treeFinalColor = 25.0f;
	}
	else if (Frustum::Instance().currentEnvironmentFace.find("environment09_front") != std::string::npos)
	{
		Frustum::Instance().terrainLightPowerEnvBased = 11.5f;
		Frustum::Instance().characterSpecularLightPower = 1.1f;
		Frustum::Instance().AmbientLight = { 10.0f, 10.0f, 10.0f };
	}
	else if (Frustum::Instance().currentEnvironmentFace.find("environment10_front") != std::string::npos)
	{
		Frustum::Instance().terrainLightPowerEnvBased = 7.5f;
		Frustum::Instance().characterSpecularLightPower = 1.45f;
		Frustum::Instance().AmbientLight = { 0.8f, 0.8f, 0.8f };
	}
	else if (Frustum::Instance().currentEnvironmentFace.find("environment11_front") != std::string::npos)
	{
		Frustum::Instance().terrainLightPowerEnvBased = 11.5f;
		Frustum::Instance().waterSunDirection = { -0.020000, -0.967921, -0.215650 };
		Frustum::Instance().characterSpecularLightPower = 2.1f;
		Frustum::Instance().AmbientLight = { 0.03f, 0.03f, 0.03f };
		Frustum::Instance().treeNormalMap = 0.0f;
		Frustum::Instance().treeFinalColor = 155.0f;
	}
	else if (Frustum::Instance().currentEnvironmentFace.find("rain") != std::string::npos)
	{
		Frustum::Instance().terrainLightPowerEnvBased = 7.0f;
		Frustum::Instance().characterSpecularLightPower = 2.0f;
		Frustum::Instance().isNightEnvironment = 1.0f;
		isNoapteEnvironment = true;
	}
	else if (Frustum::Instance().currentEnvironmentFace.find("storm") != std::string::npos)
	{
		Frustum::Instance().terrainLightPowerEnvBased = 10.0f;
		Frustum::Instance().characterSpecularLightPower = 2.0f;
		Frustum::Instance().isNightEnvironment = 1.0f;
		Frustum::Instance().isStormEnvironment = 1.0f;
		isNoapteEnvironment = true;
	}
	else
	{
		Frustum::Instance().terrainLightPowerEnvBased = 6.0f;
		Frustum::Instance().characterSpecularLightPower = 1.0f;
		Frustum::Instance().isNightEnvironment = 1.0f;
		Frustum::Instance().isStormEnvironment = 1.0f;
		isNoapteEnvironment = true;
	}

	if (current_v3SunPosition.x > 2.0f || current_v3SunPosition.x < -2.0f)
		current_v3SunPosition.x = 0.0f;
	if (current_v3SunPosition.y > 2.0f || current_v3SunPosition.y < -2.0f)
		current_v3SunPosition.y = 0.0f;
	if (current_v3SunPosition.z > 2.0f || current_v3SunPosition.z < -2.0f)
		current_v3SunPosition.z = 0.0f;

	Frustum::Instance().treeLightDirection = current_v3SunPosition;
	//TraceError("Water Opacity: %f", Frustum::Instance().waterOpacity);

}

int shadowstep = 10;
bool CMapOutdoor::BeginRenderCharacterShadowToTexture()
{
	time_t curr_time;
	curr_time = time(NULL);
	tm* tm_local = localtime(&curr_time);


	if (tm_local->tm_sec == 9 || tm_local->tm_sec == 29 || tm_local->tm_sec == 49)
	{
		shadowstep = 0;
	}

	D3DXMATRIX matLightView, matLightProj;

	CCamera* pCurrentCamera = CCameraManager::Instance().GetCurrentCamera();

	if (!pCurrentCamera)
		return false;

	if (recreate)
	{
		CreateCharacterShadowTexture();
		recreate = false;
	}

	const TEnvironmentData* envData = 0;
	CFlyingManager::Instance().GetMapManagerPtr()->GetCurrentEnvironmentData(&envData);
	D3DXVECTOR3 v3SunPosition = envData->DirLights[ENV_DIRLIGHT_BACKGROUND].Direction;
	v3SunPosition.z = v3SunPosition.z * 1.5f;

	//Frustum::Instance().currentEnvironmentFace = envData->strSkyBoxFaceFileName[0].c_str();

	if (std::strcmp(Frustum::Instance().currentMapNameSave.c_str(), "metin2_map_skipia_dungeon_02") != 0
		&& std::strcmp(Frustum::Instance().currentMapNameSave.c_str(), "metin2_map_skipia_dungeon_boss") != 0)
	{

		if ((Frustum::Instance().selectRainFrustun == 16 || Frustum::Instance().selectRainFrustun == 25 || Frustum::Instance().selectRainFrustun == 8) && std::strcmp(Frustum::Instance().currentMapNameSave.c_str(), "map_n_snowm_01") != 0)
		{
			if (shadowstep >= 0 && shadowstep <= 2 || shadowstep == 8)
			{
				if (tm_local->tm_sec == 10 || tm_local->tm_sec == 30 || tm_local->tm_sec == 50)
				{
					shadowstep += 1;

					std::random_device rd;
					std::mt19937 gen(rd());
					std::uniform_real_distribution<> distr(0.15, 0.2);

					float random_num;

					if (tm_local->tm_sec == 10)
					{
						random_num = distr(gen);
						current_v3SunPosition.z = current_v3SunPosition.z - random_num;
						current_v3SunPosition.x = current_v3SunPosition.x - random_num;
						current_v3SunPosition.y = current_v3SunPosition.y - random_num * 0.1108f;
					}
					if (tm_local->tm_sec == 30)
					{
						random_num = distr(gen);
						current_v3SunPosition.z = current_v3SunPosition.z - random_num;
						current_v3SunPosition.x = current_v3SunPosition.x - random_num;
						current_v3SunPosition.y = current_v3SunPosition.y - random_num * 0.1108f;
					}
					if (tm_local->tm_sec == 50)
					{
						random_num = distr(gen);
						current_v3SunPosition.z = current_v3SunPosition.z - random_num;
						current_v3SunPosition.x = current_v3SunPosition.x - random_num;
						current_v3SunPosition.y = current_v3SunPosition.y - random_num * 0.1108f;
					}

					//z = 0.0072f;
				}
			}
			if (tm_local->tm_sec == 12 || tm_local->tm_sec == 32 || tm_local->tm_sec == 52)
			{
				z = 0.015f;
			}
		}

	}

	D3DXVECTOR3 v3Target = pCurrentCamera->GetTarget();
	if (a)
	{
		current_v3SunPosition = v3SunPosition;
		a = false;
	}

	if (current_v3SunPosition.x > 2.0f || current_v3SunPosition.x < -2.0f)
		current_v3SunPosition.x = 0.0f;
	if (current_v3SunPosition.y > 2.0f || current_v3SunPosition.y < -2.0f)
		current_v3SunPosition.y = 0.0f;
	if (current_v3SunPosition.z > 2.0f || current_v3SunPosition.z < -2.0f)
		current_v3SunPosition.z = 0.0f;


	const auto vv = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
	D3DXMatrixLookAtRH(&matLightView,
		&shadow_eye,
		&v3Target,
		&vv);

	SnapShadowSpaceMatrix(matLightView, 10000.0f);
	D3DXMatrixOrthoRH(&matLightProj, 10000.0f, 10000.0f, 1.0f, 300000.0f);

	STATEMANAGER.SaveTransform(D3DTS_VIEW, &matLightView);
	STATEMANAGER.SaveTransform(D3DTS_PROJECTION, &matLightProj);
	CSpeedTreeForestDirectX8::Instance().UpdateCompundMatrix(((v3Target)-current_v3SunPosition * (200000.0f)), matLightView, matLightProj);


	dwLightEnable = STATEMANAGER.GetRenderState(D3DRS_LIGHTING);
	STATEMANAGER.SetRenderState(D3DRS_LIGHTING, FALSE);

	STATEMANAGER.SaveRenderState(D3DRS_TEXTUREFACTOR, 0xFF808080);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TFACTOR);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
	STATEMANAGER.SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);

	bool bSuccess = true;

	// Backup Device Context
#ifdef DIRECTX9
	if (FAILED(ms_lpd3dDevice->GetRenderTarget(0, &m_lpBackupRenderTargetSurface)))
#else
	if (FAILED(ms_lpd3dDevice->GetRenderTarget(&m_lpBackupRenderTargetSurface)))
#endif
	{
		TraceError("CMapOutdoor::BeginRenderCharacterShadowToTexture : Unable to Save Window Render Target\n");
		bSuccess = false;
	}

	if (FAILED(ms_lpd3dDevice->GetDepthStencilSurface(&m_lpBackupDepthSurface)))
	{
		TraceError("CMapOutdoor::BeginRenderCharacterShadowToTexture : Unable to Save Window Depth Surface\n");
		bSuccess = false;
	}

#ifdef DIRECTX9
	ms_lpd3dDevice->SetDepthStencilSurface(m_lpCharacterShadowMapDepthSurface);
	ms_lpd3dDevice->SetRenderTarget(0, m_lpCharacterShadowMapRenderTargetSurface);
#else
	if (FAILED(ms_lpd3dDevice->SetRenderTarget(m_lpCharacterShadowMapRenderTargetSurface, m_lpCharacterShadowMapDepthSurface)))
	{
		TraceError("CMapOutdoor::BeginRenderCharacterShadowToTexture : Unable to Set Shadow Map Render Target\n");
		bSuccess = false;
	}
#endif

	if (FAILED(ms_lpd3dDevice->Clear(0L, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0xFF, 0xFF, 0xFF), 1.0f, 0)))
	{
		TraceError("CMapOutdoor::BeginRenderCharacterShadowToTexture : Unable to Clear Render Target");
		bSuccess = false;
	}

	if (FAILED(ms_lpd3dDevice->GetViewport(&m_BackupViewport)))
	{
		TraceError("CMapOutdoor::BeginRenderCharacterShadowToTexture : Unable to Save Window Viewport\n");
		bSuccess = false;
	}

	if (FAILED(ms_lpd3dDevice->SetViewport(&m_ShadowMapViewport)))
	{
		TraceError("CMapOutdoor::BeginRenderCharacterShadowToTexture : Unable to Set Shadow Map viewport\n");
		bSuccess = false;
	}

	if (FAILED(ms_lpd3dDevice->SetVertexShader(m_lpShadowMapVertexShaderDepth)))
	{
		TraceError("CMapOutdoor::BeginRenderCharacterShadowToTexture : Failed to set depth pass vertex shader\n");
		bSuccess = false;
	}
	if (FAILED(ms_lpd3dDevice->SetPixelShader(m_lpShadowMapPixelShaderDepth)))
	{
		TraceError("CMapOutdoor::BeginRenderCharacterShadowToTexture : Failed to set depth pass pixel shader\n");
		bSuccess = false;
	}

	D3DXMatrixMultiplyTranspose(&m_MatLightViewProj, &matLightView, &matLightProj);
	ms_lpd3dDevice->SetVertexShaderConstantF(4, (float*)&m_MatLightViewProj, 4);

	return bSuccess;
}

void CMapOutdoor::EndRenderCharacterShadowToTexture()
{
	ms_lpd3dDevice->SetViewport(&m_BackupViewport);

#ifdef DIRECTX9
	ms_lpd3dDevice->SetDepthStencilSurface(m_lpBackupDepthSurface);
	ms_lpd3dDevice->SetRenderTarget(0, m_lpBackupRenderTargetSurface);
#else
	ms_lpd3dDevice->SetRenderTarget(m_lpBackupRenderTargetSurface, m_lpBackupDepthSurface);
#endif

	SAFE_RELEASE(m_lpBackupRenderTargetSurface);
	SAFE_RELEASE(m_lpBackupDepthSurface);

	STATEMANAGER.RestoreTransform(D3DTS_VIEW);
	STATEMANAGER.RestoreTransform(D3DTS_PROJECTION);

	// Restore Device Context
	STATEMANAGER.SetRenderState(D3DRS_LIGHTING, dwLightEnable);
	STATEMANAGER.RestoreRenderState(D3DRS_TEXTUREFACTOR);

	ms_lpd3dDevice->SetVertexShader(nullptr);
	ms_lpd3dDevice->SetPixelShader(nullptr);

	CSpeedTreeForestDirectX8::Instance().UpdateCompundMatrix(CCameraManager::Instance().GetCurrentCamera()->GetEye(), ms_matView, ms_matProj);
}

bool CMapOutdoor::BeginRenderCharacterShadowToTexture2()
{
	time_t curr_time;
	curr_time = time(NULL);
	tm* tm_local = localtime(&curr_time);


	if (tm_local->tm_sec == 9 || tm_local->tm_sec == 29 || tm_local->tm_sec == 49)
	{
		shadowstep = 0;
	}

	D3DXMATRIX matLightView, matLightProj;

	CCamera* pCurrentCamera = CCameraManager::Instance().GetCurrentCamera();

	if (!pCurrentCamera)
		return false;

	if (recreate)
	{
		CreateCharacterShadowTexture();
		recreate = false;
	}



	const TEnvironmentData* envData = 0;
	CFlyingManager::Instance().GetMapManagerPtr()->GetCurrentEnvironmentData(&envData);

	D3DXVECTOR3 v3SunPosition = envData->DirLights[ENV_DIRLIGHT_BACKGROUND].Direction;
	v3SunPosition.z = v3SunPosition.z * 1.5f;

	D3DXVECTOR3 v3Target = pCurrentCamera->GetTarget();
	if (a)
	{
		current_v3SunPosition = v3SunPosition;
		a = false;
	}


	if (current_v3SunPosition.x > 2.0f || current_v3SunPosition.x < -2.0f)
		current_v3SunPosition.x = 0.0f;
	if (current_v3SunPosition.y > 2.0f || current_v3SunPosition.y < -2.0f)
		current_v3SunPosition.y = 0.0f;
	if (current_v3SunPosition.z > 2.0f || current_v3SunPosition.z < -2.0f)
		current_v3SunPosition.z = 0.0f;





	float distanceX = v3SunPosition.x - current_v3SunPosition.x;
	float distanceY = v3SunPosition.y - current_v3SunPosition.y;
	float distanceZ = v3SunPosition.z - current_v3SunPosition.z;

	if (std::abs(distanceX) > tolerance)
	{
		float stepX = min(std::abs(distanceX), z);
		current_v3SunPosition.x += stepX * (distanceX > 0 ? 1 : -1);
	}

	if (std::abs(distanceY) > tolerance)
	{
		float stepY = min(std::abs(distanceY), z);
		current_v3SunPosition.y += stepY * (distanceY > 0 ? 1 : -1);
	}

	if (std::abs(distanceZ) > tolerance)
	{
		float stepZ = min(std::abs(distanceZ), z * zMultiplier);
		current_v3SunPosition.z += stepZ * (distanceZ > 0 ? 1 : -1);
	}

	if (std::abs(current_v3SunPosition.x - v3SunPosition.x) <= tolerance)
		current_v3SunPosition.x = v3SunPosition.x;
	if (std::abs(current_v3SunPosition.y - v3SunPosition.y) <= tolerance)
		current_v3SunPosition.y = v3SunPosition.y;
	if (std::abs(current_v3SunPosition.z - v3SunPosition.z) <= tolerance)
		current_v3SunPosition.z = v3SunPosition.z;

	if (std::strcmp(Frustum::Instance().currentMapNameSave.c_str(), "metin2_map_skipia_dungeon_02") != 0
		&& std::strcmp(Frustum::Instance().currentMapNameSave.c_str(), "metin2_map_skipia_dungeon_boss") != 0)
	{

		if ((Frustum::Instance().selectRainFrustun == 16 || Frustum::Instance().selectRainFrustun == 25 || Frustum::Instance().selectRainFrustun == 8) && std::strcmp(Frustum::Instance().currentMapNameSave.c_str(), "map_n_snowm_01") != 0)
		{
			if (shadowstep >= 0 && shadowstep <= 2 || shadowstep == 8)
			{
				if (tm_local->tm_sec == 10 || tm_local->tm_sec == 30 || tm_local->tm_sec == 50)
				{
					shadowstep += 1;
					if (tm_local->tm_sec == 10)
					{
						current_v3SunPosition.z = current_v3SunPosition.z - 0.065f;
						current_v3SunPosition.x = current_v3SunPosition.x - 0.065f;
					}
					if (tm_local->tm_sec == 30)
					{
						current_v3SunPosition.z = current_v3SunPosition.z - 0.065f;
						current_v3SunPosition.x = current_v3SunPosition.x - 0.065f;
					}
					if (tm_local->tm_sec == 50)
					{
						current_v3SunPosition.z = current_v3SunPosition.z - 0.065f;
						current_v3SunPosition.x = current_v3SunPosition.x - 0.065f;
					}
					z = 0.1f;
				}
			}
			if (tm_local->tm_sec == 12 || tm_local->tm_sec == 32 || tm_local->tm_sec == 52)
			{
				z = 0.015f;
			}
		}

	}

#include <unordered_map>

	const auto vv = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
	const auto shadow_eye = (v3Target)-current_v3SunPosition * (200000.0f);
	D3DXMatrixLookAtRH(&m_matLightView2,
		&shadow_eye,
		&v3Target,
		&vv);

	//TraceError("2 %f, %f, %f, ", current_v3SunPosition.x, current_v3SunPosition.y, current_v3SunPosition.z);
	SnapShadowSpaceMatrix(matLightView, 25000.0f);
	D3DXMatrixOrthoRH(&matLightProj, 25000.0f, 25000.0f, 1.0f, 300000.0f);

	STATEMANAGER.SaveTransform(D3DTS_VIEW, &m_matLightView2);
	STATEMANAGER.SaveTransform(D3DTS_PROJECTION, &matLightProj);


	dwLightEnable = STATEMANAGER.GetRenderState(D3DRS_LIGHTING);
	STATEMANAGER.SetRenderState(D3DRS_LIGHTING, FALSE);

	STATEMANAGER.SaveRenderState(D3DRS_TEXTUREFACTOR, 0xFF808080);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TFACTOR);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
	STATEMANAGER.SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);

	bool bSuccess = true;

	// Backup Device Context
#ifdef DIRECTX9
	if (FAILED(ms_lpd3dDevice->GetRenderTarget(0, &m_lpBackupRenderTargetSurface)))
#else
	if (FAILED(ms_lpd3dDevice->GetRenderTarget(&m_lpBackupRenderTargetSurface)))
#endif
	{
		TraceError("CMapOutdoor::BeginRenderCharacterShadowToTexture : Unable to Save Window Render Target\n");
		bSuccess = false;
	}

	if (FAILED(ms_lpd3dDevice->GetDepthStencilSurface(&m_lpBackupDepthSurface)))
	{
		TraceError("CMapOutdoor::BeginRenderCharacterShadowToTexture : Unable to Save Window Depth Surface\n");
		bSuccess = false;
	}

#ifdef DIRECTX9
	ms_lpd3dDevice->SetDepthStencilSurface(m_lpCharacterShadowMapDepthSurface);
	ms_lpd3dDevice->SetRenderTarget(0, m_lpCharacterShadowMapRenderTargetSurface2);
#else
	if (FAILED(ms_lpd3dDevice->SetRenderTarget(m_lpCharacterShadowMapRenderTargetSurface, m_lpCharacterShadowMapDepthSurface)))
	{
		TraceError("CMapOutdoor::BeginRenderCharacterShadowToTexture : Unable to Set Shadow Map Render Target\n");
		bSuccess = false;
	}
#endif

	if (FAILED(ms_lpd3dDevice->Clear(0L, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0xFF, 0xFF, 0xFF), 1.0f, 0)))
	{
		TraceError("CMapOutdoor::BeginRenderCharacterShadowToTexture : Unable to Clear Render Target");
		bSuccess = false;
	}

	if (FAILED(ms_lpd3dDevice->GetViewport(&m_BackupViewport)))
	{
		TraceError("CMapOutdoor::BeginRenderCharacterShadowToTexture : Unable to Save Window Viewport\n");
		bSuccess = false;
	}

	if (FAILED(ms_lpd3dDevice->SetViewport(&m_ShadowMapViewport)))
	{
		TraceError("CMapOutdoor::BeginRenderCharacterShadowToTexture : Unable to Set Shadow Map viewport\n");
		bSuccess = false;
	}

	return bSuccess;
}

void CMapOutdoor::EndRenderCharacterShadowToTexture2()
{
	ms_lpd3dDevice->SetViewport(&m_BackupViewport);

#ifdef DIRECTX9
	ms_lpd3dDevice->SetDepthStencilSurface(m_lpBackupDepthSurface);
	ms_lpd3dDevice->SetRenderTarget(0, m_lpBackupRenderTargetSurface);
#else
	ms_lpd3dDevice->SetRenderTarget(m_lpBackupRenderTargetSurface, m_lpBackupDepthSurface);
#endif

	SAFE_RELEASE(m_lpBackupRenderTargetSurface);
	SAFE_RELEASE(m_lpBackupDepthSurface);

	STATEMANAGER.RestoreTransform(D3DTS_VIEW);
	STATEMANAGER.RestoreTransform(D3DTS_PROJECTION);

	// Restore Device Context
	STATEMANAGER.SetRenderState(D3DRS_LIGHTING, dwLightEnable);
	STATEMANAGER.RestoreRenderState(D3DRS_TEXTUREFACTOR);
}


float currentValue = 500.0f;
float targetValue = 500.0f; 
float stepSize = 10.0f; 
float animationDuration = 2.0f; 
float elapsedTime = 0.0f;

bool CMapOutdoor::BeginRenderCharacterShadowToTexture3()
{
	time_t curr_time;
	curr_time = time(NULL);
	tm* tm_local = localtime(&curr_time);


	if (tm_local->tm_sec == 9 || tm_local->tm_sec == 29 || tm_local->tm_sec == 49)
	{
		shadowstep = 0;
	}

	D3DXMATRIX matLightView, matLightProj;

	CCamera* pCurrentCamera = CCameraManager::Instance().GetCurrentCamera();

	if (!pCurrentCamera)
		return false;

	if (recreate)
	{
		CreateCharacterShadowTexture();
		recreate = false;
	}



	const TEnvironmentData* envData = 0;
	CFlyingManager::Instance().GetMapManagerPtr()->GetCurrentEnvironmentData(&envData);
	D3DXVECTOR3 v3SunPosition = envData->DirLights[ENV_DIRLIGHT_BACKGROUND].Direction;
	v3SunPosition.z = v3SunPosition.z * 1.5f;

	if (v3SunPosition != Frustum::instance().previousSunPosition)
	{
		Frustum::instance().isChangingSky = true;
		Frustum::instance().previousSunPosition = v3SunPosition; // Update the previous value.
	}
	else
	{
		Frustum::instance().isChangingSky = false;
	}

	if (std::strcmp(Frustum::Instance().currentMapNameSave.c_str(), "metin2_map_skipia_dungeon_02") != 0
		&& std::strcmp(Frustum::Instance().currentMapNameSave.c_str(), "metin2_map_skipia_dungeon_boss") != 0)
	{

		if ((Frustum::Instance().selectRainFrustun == 16 || Frustum::Instance().selectRainFrustun == 25 || Frustum::Instance().selectRainFrustun == 8) && std::strcmp(Frustum::Instance().currentMapNameSave.c_str(), "map_n_snowm_01") != 0)
		{
			if (shadowstep >= 0 && shadowstep <= 2 || shadowstep == 8)
			{
				if (tm_local->tm_sec == 10 || tm_local->tm_sec == 30 || tm_local->tm_sec == 50)
				{
					shadowstep += 1;

					std::random_device rd;
					std::mt19937 gen(rd());
					std::uniform_real_distribution<> distr(0.15, 0.2);

					float random_num;

					if (tm_local->tm_sec == 10)
					{
						random_num = distr(gen);
						current_v3SunPosition.z = current_v3SunPosition.z - random_num;
						current_v3SunPosition.x = current_v3SunPosition.x - random_num;
						current_v3SunPosition.y = current_v3SunPosition.y - random_num * 0.1108f;
					}
					if (tm_local->tm_sec == 30)
					{
						random_num = distr(gen);
						current_v3SunPosition.z = current_v3SunPosition.z - random_num;
						current_v3SunPosition.x = current_v3SunPosition.x - random_num;
						current_v3SunPosition.y = current_v3SunPosition.y - random_num * 0.1108f;
					}
					if (tm_local->tm_sec == 50)
					{
						random_num = distr(gen);
						current_v3SunPosition.z = current_v3SunPosition.z - random_num;
						current_v3SunPosition.x = current_v3SunPosition.x - random_num;
						current_v3SunPosition.y = current_v3SunPosition.y - random_num * 0.1108f;
					}

					//z = 0.0072f;
				}
			}
			if (tm_local->tm_sec == 12 || tm_local->tm_sec == 32 || tm_local->tm_sec == 52)
			{
				z = 0.015f;
			}
		}

	}

	D3DXVECTOR3 v3Target = pCurrentCamera->GetTarget();
	if (a)
	{
		current_v3SunPosition = v3SunPosition;
		a = false;
	}


	if (current_v3SunPosition.x > 2.0f || current_v3SunPosition.x < -2.0f)
		current_v3SunPosition.x = 0.0f;
	if (current_v3SunPosition.y > 2.0f || current_v3SunPosition.y < -2.0f)
		current_v3SunPosition.y = 0.0f;
	if (current_v3SunPosition.z > 2.0f || current_v3SunPosition.z < -2.0f)
		current_v3SunPosition.z = 0.0f;





	float distanceX = v3SunPosition.x - current_v3SunPosition.x;
	float distanceY = v3SunPosition.y - current_v3SunPosition.y;
	float distanceZ = v3SunPosition.z - current_v3SunPosition.z;

	if (std::abs(distanceX) > tolerance)
	{
		float stepX = min(std::abs(distanceX), z);
		current_v3SunPosition.x += stepX * (distanceX > 0 ? 1 : -1);
	}

	if (std::abs(distanceY) > tolerance)
	{
		float stepY = min(std::abs(distanceY), z);
		current_v3SunPosition.y += stepY * (distanceY > 0 ? 1 : -1);
	}

	if (std::abs(distanceZ) > tolerance)
	{
		float stepZ = min(std::abs(distanceZ), z * zMultiplier);
		current_v3SunPosition.z += stepZ * (distanceZ > 0 ? 1 : -1);
	}

	if (std::abs(current_v3SunPosition.x - v3SunPosition.x) <= tolerance)
		current_v3SunPosition.x = v3SunPosition.x;
	if (std::abs(current_v3SunPosition.y - v3SunPosition.y) <= tolerance)
		current_v3SunPosition.y = v3SunPosition.y;
	if (std::abs(current_v3SunPosition.z - v3SunPosition.z) <= tolerance)
		current_v3SunPosition.z = v3SunPosition.z;




	const auto vv = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
	shadow_eye = (v3Target)-current_v3SunPosition * (200000.0f);
	D3DXMatrixLookAtRH(&matLightView,
		&shadow_eye,
		&v3Target,
		&vv);

	float newTargetValue;
	if (Frustum::Instance().shadowType == 2) {
		newTargetValue = 35000.0f;
	}
	else {
		newTargetValue = 5000.0f;
	}

	// If the target value has changed, reset the animation
	if (newTargetValue != targetValue) {
		targetValue = newTargetValue;
		elapsedTime = 0.0f;
		float totalDistance = std::abs(targetValue - currentValue);
		stepSize = totalDistance / (animationDuration * 60); // assuming 60 updates per second
	}

	// Update currentValue towards targetValue
	if (elapsedTime < animationDuration) {
		float distance = targetValue - currentValue;
		if (std::abs(distance) > 0) {
			float step = stepSize * 0.03f * 60; // Correct step for deltaTime
			currentValue += step * (distance > 0 ? 1 : -1);
			elapsedTime += 0.03f;
		}
	}
	else {
		currentValue = targetValue; // Snap to target if duration has elapsed
	}

	// Use currentValue in your functions
	SnapShadowSpaceMatrix(matLightView, currentValue);
	D3DXMatrixOrthoRH(&matLightProj, currentValue, currentValue, 1.0f, 300000.0f);

	STATEMANAGER.SaveTransform(D3DTS_VIEW, &matLightView);
	STATEMANAGER.SaveTransform(D3DTS_PROJECTION, &matLightProj);
	CSpeedTreeForestDirectX8::Instance().UpdateCompundMatrix(((v3Target)-current_v3SunPosition * (200000.0f)), matLightView, matLightProj);


	dwLightEnable = STATEMANAGER.GetRenderState(D3DRS_LIGHTING);
	STATEMANAGER.SetRenderState(D3DRS_LIGHTING, FALSE);

	STATEMANAGER.SaveRenderState(D3DRS_TEXTUREFACTOR, 0xFF808080);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TFACTOR);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
	STATEMANAGER.SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
	STATEMANAGER.SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
	STATEMANAGER.SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	bool bSuccess = true;

	// Backup Device Context
#ifdef DIRECTX9
	if (FAILED(ms_lpd3dDevice->GetRenderTarget(0, &m_lpBackupRenderTargetSurface)))
#else
	if (FAILED(ms_lpd3dDevice->GetRenderTarget(&m_lpBackupRenderTargetSurface)))
#endif
	{
		TraceError("CMapOutdoor::BeginRenderCharacterShadowToTexture : Unable to Save Window Render Target\n");
		bSuccess = false;
	}

	if (FAILED(ms_lpd3dDevice->GetDepthStencilSurface(&m_lpBackupDepthSurface)))
	{
		TraceError("CMapOutdoor::BeginRenderCharacterShadowToTexture : Unable to Save Window Depth Surface\n");
		bSuccess = false;
	}

#ifdef DIRECTX9
	ms_lpd3dDevice->SetDepthStencilSurface(m_lpCharacterShadowMapDepthSurface);
	ms_lpd3dDevice->SetRenderTarget(0, m_lpCharacterShadowMapRenderTargetSurface);
#else
	if (FAILED(ms_lpd3dDevice->SetRenderTarget(m_lpCharacterShadowMapRenderTargetSurface, m_lpCharacterShadowMapDepthSurface)))
	{
		TraceError("CMapOutdoor::BeginRenderCharacterShadowToTexture : Unable to Set Shadow Map Render Target\n");
		bSuccess = false;
	}
#endif

	if (FAILED(ms_lpd3dDevice->Clear(0L, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0xFF, 0xFF, 0xFF), 1.0f, 0)))
	{
		TraceError("CMapOutdoor::BeginRenderCharacterShadowToTexture : Unable to Clear Render Target");
		bSuccess = false;
	}

	if (FAILED(ms_lpd3dDevice->GetViewport(&m_BackupViewport)))
	{
		TraceError("CMapOutdoor::BeginRenderCharacterShadowToTexture : Unable to Save Window Viewport\n");
		bSuccess = false;
	}

	if (FAILED(ms_lpd3dDevice->SetViewport(&m_ShadowMapViewportTree)))
	{
		TraceError("CMapOutdoor::BeginRenderCharacterShadowToTexture : Unable to Set Shadow Map viewport\n");
		bSuccess = false;
	}

	if (FAILED(ms_lpd3dDevice->SetVertexShader(m_lpShadowMapVertexShaderDepth)))
	{
		TraceError("CMapOutdoor::BeginRenderCharacterShadowToTexture : Failed to set depth pass vertex shader\n");
		bSuccess = false;
	}
	if (FAILED(ms_lpd3dDevice->SetPixelShader(m_lpShadowMapPixelShaderDepth)))
	{
		TraceError("CMapOutdoor::BeginRenderCharacterShadowToTexture : Failed to set depth pass pixel shader\n");
		bSuccess = false;
	}

	D3DXMatrixMultiplyTranspose(&m_MatLightViewProj, &matLightView, &matLightProj);
	ms_lpd3dDevice->SetVertexShaderConstantF(4, (float*)&m_MatLightViewProj, 4);

	return bSuccess;
}

void CMapOutdoor::EndRenderCharacterShadowToTexture3()
{
	ms_lpd3dDevice->SetViewport(&m_BackupViewport);

#ifdef DIRECTX9
	ms_lpd3dDevice->SetDepthStencilSurface(m_lpBackupDepthSurface);
	ms_lpd3dDevice->SetRenderTarget(0, m_lpBackupRenderTargetSurface);
#else
	ms_lpd3dDevice->SetRenderTarget(m_lpBackupRenderTargetSurface, m_lpBackupDepthSurface);
#endif

	SAFE_RELEASE(m_lpBackupRenderTargetSurface);
	SAFE_RELEASE(m_lpBackupDepthSurface);

	STATEMANAGER.RestoreTransform(D3DTS_VIEW);
	STATEMANAGER.RestoreTransform(D3DTS_PROJECTION);

	// Restore Device Context
	STATEMANAGER.SetRenderState(D3DRS_LIGHTING, dwLightEnable);
	STATEMANAGER.RestoreRenderState(D3DRS_TEXTUREFACTOR);
	STATEMANAGER.SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

	ms_lpd3dDevice->SetVertexShader(nullptr);
	ms_lpd3dDevice->SetPixelShader(nullptr);

	CSpeedTreeForestDirectX8::Instance().UpdateCompundMatrix(CCameraManager::Instance().GetCurrentCamera()->GetEye(), ms_matView, ms_matProj);
}

void CMapOutdoor::SnapShadowSpaceMatrix(D3DXMATRIX& mat, float spaceSize)
{
	float step = spaceSize / 128.0;
	mat._41 = floor(mat._41 / step) * step;
	mat._42 = floor(mat._42 / step) * step;
	mat._43 = floor(mat._43 / step) * step;
}

bool CMapOutdoor::BeginRenderCharacterShadowToTexture4()
{
	D3DXMATRIX matLightView, matLightProj;

	CCamera* pCurrentCamera = CCameraManager::Instance().GetCurrentCamera();

	if (!pCurrentCamera)
		return false;

	if (recreate)
	{
		CreateCharacterShadowTexture();
		recreate = false;
	}

	const TEnvironmentData* envData = 0;
	CFlyingManager::Instance().GetMapManagerPtr()->GetCurrentEnvironmentData(&envData);
	D3DXVECTOR3 v3Target = pCurrentCamera->GetTarget();



	const auto vv = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
	D3DXMatrixLookAtRH(&matLightView,
		&shadow_eye,
		&v3Target,
		&vv);


	SnapShadowSpaceMatrix(matLightView, 3000.0f);
	D3DXMatrixOrthoRH(&matLightProj, 3000.0f, 3000.0f, 1.0f, 300000.0f);



	STATEMANAGER.SaveTransform(D3DTS_VIEW, &matLightView);
	STATEMANAGER.SaveTransform(D3DTS_PROJECTION, &matLightProj);
	CSpeedTreeForestDirectX8::Instance().UpdateCompundMatrix2(((v3Target)-current_v3SunPosition * (200000.0f)), matLightView, matLightProj);

	dwLightEnable = STATEMANAGER.GetRenderState(D3DRS_LIGHTING);
	STATEMANAGER.SetRenderState(D3DRS_LIGHTING, FALSE);

	STATEMANAGER.SaveRenderState(D3DRS_TEXTUREFACTOR, 0xFF808080);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TFACTOR);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
	STATEMANAGER.SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);

	bool bSuccess = true;

	// Backup Device Context
#ifdef DIRECTX9
	if (FAILED(ms_lpd3dDevice->GetRenderTarget(0, &m_lpBackupRenderTargetSurface)))
#else
	if (FAILED(ms_lpd3dDevice->GetRenderTarget(&m_lpBackupRenderTargetSurface)))
#endif
	{
		TraceError("CMapOutdoor::BeginRenderCharacterShadowToTexture : Unable to Save Window Render Target\n");
		bSuccess = false;
	}

	if (FAILED(ms_lpd3dDevice->GetDepthStencilSurface(&m_lpBackupDepthSurface)))
	{
		TraceError("CMapOutdoor::BeginRenderCharacterShadowToTexture : Unable to Save Window Depth Surface\n");
		bSuccess = false;
	}

#ifdef DIRECTX9
	ms_lpd3dDevice->SetDepthStencilSurface(m_lpCharacterShadowMapDepthSurface);
	ms_lpd3dDevice->SetRenderTarget(0, m_lpCharacterShadowMapRenderTargetSurface4);
#else
	if (FAILED(ms_lpd3dDevice->SetRenderTarget(m_lpCharacterShadowMapRenderTargetSurface, m_lpCharacterShadowMapDepthSurface)))
	{
		TraceError("CMapOutdoor::BeginRenderCharacterShadowToTexture : Unable to Set Shadow Map Render Target\n");
		bSuccess = false;
	}
#endif

	if (FAILED(ms_lpd3dDevice->Clear(0L, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0xFF, 0xFF, 0xFF), 1.0f, 0)))
	{
		TraceError("CMapOutdoor::BeginRenderCharacterShadowToTexture : Unable to Clear Render Target");
		bSuccess = false;
	}

	if (FAILED(ms_lpd3dDevice->GetViewport(&m_BackupViewport)))
	{
		TraceError("CMapOutdoor::BeginRenderCharacterShadowToTexture : Unable to Save Window Viewport\n");
		bSuccess = false;
	}

	if (FAILED(ms_lpd3dDevice->SetViewport(&m_ShadowMapViewport)))
	{
		TraceError("CMapOutdoor::BeginRenderCharacterShadowToTexture : Unable to Set Shadow Map viewport\n");
		bSuccess = false;
	}

	if (FAILED(ms_lpd3dDevice->SetVertexShader(m_lpShadowMapVertexShaderDepth)))
	{
		TraceError("CMapOutdoor::BeginRenderCharacterShadowToTexture : Failed to set depth pass vertex shader\n");
		bSuccess = false;
	}
	if (FAILED(ms_lpd3dDevice->SetPixelShader(m_lpShadowMapPixelShaderDepth)))
	{
		TraceError("CMapOutdoor::BeginRenderCharacterShadowToTexture : Failed to set depth pass pixel shader\n");
		bSuccess = false;
	}

	D3DXMatrixMultiplyTranspose(&m_MatLightViewProj2, &matLightView, &matLightProj);
	ms_lpd3dDevice->SetVertexShaderConstantF(4, (float*)&m_MatLightViewProj2, 4);

	return bSuccess;
}

void CMapOutdoor::EndRenderCharacterShadowToTexture4()
{
	ms_lpd3dDevice->SetViewport(&m_BackupViewport);

#ifdef DIRECTX9
	ms_lpd3dDevice->SetDepthStencilSurface(m_lpBackupDepthSurface);
	ms_lpd3dDevice->SetRenderTarget(0, m_lpBackupRenderTargetSurface);
#else
	ms_lpd3dDevice->SetRenderTarget(m_lpBackupRenderTargetSurface, m_lpBackupDepthSurface);
#endif

	SAFE_RELEASE(m_lpBackupRenderTargetSurface);
	SAFE_RELEASE(m_lpBackupDepthSurface);

	STATEMANAGER.RestoreTransform(D3DTS_VIEW);
	STATEMANAGER.RestoreTransform(D3DTS_PROJECTION);

	// Restore Device Context
	STATEMANAGER.SetRenderState(D3DRS_LIGHTING, dwLightEnable);
	STATEMANAGER.RestoreRenderState(D3DRS_TEXTUREFACTOR);

	ms_lpd3dDevice->SetVertexShader(nullptr);
	ms_lpd3dDevice->SetPixelShader(nullptr);

	CSpeedTreeForestDirectX8::Instance().UpdateCompundMatrix(CCameraManager::Instance().GetCurrentCamera()->GetEye(), ms_matView, ms_matProj);
}
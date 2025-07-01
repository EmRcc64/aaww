const char* terrainShadowsPS = R"(
struct PS_INPUT
{
    float4 Position : POSITION;
    float2 UV : TEXCOORD0;
    float3 Normal : TEXCOORD1;
    float4 LightViewPosition1 : TEXCOORD2;
    float4 LightViewPosition2 : TEXCOORD3;
    float4 WorldPos : TEXCOORD4;
    float3 Tangent : TEXCOORD5;
};

#define USE_SHADOWS
#define SHADOW_PCF_RADIUS 1
#define SHADOW_PCF_NUM_SAMPLES 10 //blur

float3 MaterialDiffuse : register(c0);
float3 LightDirection : register(c1);
float4 LightDiffuse : register(c2);
float4 AmbientColor : register(c3);
float3 CameraPos : register(c4);
float4 FogColor : register(c5);
float2 FogRange : register(c6);
float3 MaterialSpecular : register(c7);
float Shininess : register(c8);
float3 LightSpecular : register(c9);
int ShadowOpacity : register(c10); // 0..100?
float Time : register(c12); // Add this to the constants at the botom
float SpecularLight : register(c11);
float terrainLightMultiply : register(c14);

sampler2D DiffuseTextureTerrain : register(s0);
sampler2D ShadowMap1 : register(s1);   // big shadowmap
sampler2D ShadowMap2 : register(s2);   // local (small) shadowmap

static const float PI = 3.1416f;
static const float2 pcfOffsets[30] = {
    float2(0.0000, 0.0000),
    float2(0.5000, 0.0000),
    float2(0.3536, 0.3536),
    float2(0.0000, 0.5000),
    float2(-0.3536, 0.3536),
    float2(-0.5000, 0.0000),
    float2(-0.3536, -0.3536),
    float2(-0.0000, -0.5000),
    float2(0.3536, -0.3536),
    float2(1.0000, 0.0000),
    float2(0.8660, 0.5000),
    float2(0.5000, 0.8660),
    float2(0.0000, 1.0000),
    float2(-0.5000, 0.8660),
    float2(-0.8660, 0.5000),
    float2(-1.0000, 0.0000),
    float2(-0.8660, -0.5000),
    float2(-0.5000, -0.8660),
    float2(-0.0000, -1.0000),
    float2(0.5000, -0.8660),
    float2(0.8660, -0.5000),
    float2(1.5000, 0.0000),
    float2(1.1491, 0.9642),
    float2(0.2605, 1.4772),
    float2(-0.7500, 1.2990),
    float2(-1.4095, 0.5130),
    float2(-1.4095, -0.5130),
    float2(-0.7500, -1.2990),
    float2(0.2605, -1.4772),
    float2(1.1491, -0.9642)
};

float2 clipSpaceToTextureSpace(float4 clipSpace)
{
    float2 cs = clipSpace.xy;
    return float2(0.5f * cs.x, -0.5f * cs.y) + 0.5f;
}

float4 main(PS_INPUT input) : SV_TARGET
{
    float4 Color = tex2D(DiffuseTextureTerrain, input.UV);

    // Fetch and transform normal from the diffuse map (pretending it's a normal map)
    float3 NM = normalize((tex2D(DiffuseTextureTerrain, input.UV).rgb * 2.0f) - 1.0f);
    float3 N = normalize(input.Normal * NM);
    float3 L = -LightDirection;

    float OriginalLightIntensity = saturate(dot(N, L));
    float LightIntensity = 0.3f + 0.2f * OriginalLightIntensity;
    float3 LightResult = (AmbientColor.rgb + (MaterialDiffuse * LightDiffuse.rgb) * LightIntensity * 0.8f);



#ifdef USE_SHADOWS
    const float ShadowBias = 0.0001f;
    const float ShadowBias2 = 0.00004f;
    const float2 LightTexCoord1 = clipSpaceToTextureSpace(input.LightViewPosition1);
    const float2 LightTexCoord2 = clipSpaceToTextureSpace(input.LightViewPosition2);
    const float fCurrDepth1 = input.LightViewPosition1.z - ShadowBias;
    const float fCurrDepth2 = input.LightViewPosition2.z - ShadowBias2;
	 
    float bigShadow = 0.0f;
    float localShadow = 0.0f;	 
    const float texelScale1 = 1.0f / 1250.0f;
    const float texelScale2 = 1.0f / 600.0f;
	 const float increment = 1.0f / SHADOW_PCF_NUM_SAMPLES;
   
    bigShadow   += (fCurrDepth1 > tex2D(ShadowMap1, LightTexCoord1 + pcfOffsets[0] * texelScale1).r ? increment : 0.0f);
    localShadow += (fCurrDepth2 > tex2D(ShadowMap2, LightTexCoord2 + pcfOffsets[0] * texelScale2).r ? increment : 0.0f); 
    bigShadow   += (fCurrDepth1 > tex2D(ShadowMap1, LightTexCoord1 + pcfOffsets[1] * texelScale1).r ? increment : 0.0f);
    localShadow += (fCurrDepth2 > tex2D(ShadowMap2, LightTexCoord2 + pcfOffsets[1] * texelScale2).r ? increment : 0.0f);   
    bigShadow   += (fCurrDepth1 > tex2D(ShadowMap1, LightTexCoord1 + pcfOffsets[2] * texelScale1).r ? increment : 0.0f);
    localShadow += (fCurrDepth2 > tex2D(ShadowMap2, LightTexCoord2 + pcfOffsets[2] * texelScale2).r ? increment : 0.0f);     
    bigShadow   += (fCurrDepth1 > tex2D(ShadowMap1, LightTexCoord1 + pcfOffsets[3] * texelScale1).r ? increment : 0.0f);
    localShadow += (fCurrDepth2 > tex2D(ShadowMap2, LightTexCoord2 + pcfOffsets[3] * texelScale2).r ? increment : 0.0f);     
    bigShadow   += (fCurrDepth1 > tex2D(ShadowMap1, LightTexCoord1 + pcfOffsets[4] * texelScale1).r ? increment : 0.0f);
    localShadow += (fCurrDepth2 > tex2D(ShadowMap2, LightTexCoord2 + pcfOffsets[4] * texelScale2).r ? increment : 0.0f);     
    bigShadow   += (fCurrDepth1 > tex2D(ShadowMap1, LightTexCoord1 + pcfOffsets[5] * texelScale1).r ? increment : 0.0f);
    localShadow += (fCurrDepth2 > tex2D(ShadowMap2, LightTexCoord2 + pcfOffsets[5] * texelScale2).r ? increment : 0.0f);     
    bigShadow   += (fCurrDepth1 > tex2D(ShadowMap1, LightTexCoord1 + pcfOffsets[6] * texelScale1).r ? increment : 0.0f);
    localShadow += (fCurrDepth2 > tex2D(ShadowMap2, LightTexCoord2 + pcfOffsets[6] * texelScale2).r ? increment : 0.0f);     
    bigShadow   += (fCurrDepth1 > tex2D(ShadowMap1, LightTexCoord1 + pcfOffsets[7] * texelScale1).r ? increment : 0.0f);
    localShadow += (fCurrDepth2 > tex2D(ShadowMap2, LightTexCoord2 + pcfOffsets[7] * texelScale2).r ? increment : 0.0f);     
    bigShadow   += (fCurrDepth1 > tex2D(ShadowMap1, LightTexCoord1 + pcfOffsets[8] * texelScale1).r ? increment : 0.0f);
    localShadow += (fCurrDepth2 > tex2D(ShadowMap2, LightTexCoord2 + pcfOffsets[8] * texelScale2).r ? increment : 0.0f);     
    bigShadow   += (fCurrDepth1 > tex2D(ShadowMap1, LightTexCoord1 + pcfOffsets[9] * texelScale1).r ? increment : 0.0f);
    localShadow += (fCurrDepth2 > tex2D(ShadowMap2, LightTexCoord2 + pcfOffsets[9] * texelScale2).r ? increment : 0.0f);     
    bigShadow   += (fCurrDepth1 > tex2D(ShadowMap1, LightTexCoord1 + pcfOffsets[10] * texelScale1).r ? increment : 0.0f);
    localShadow += (fCurrDepth2 > tex2D(ShadowMap2, LightTexCoord2 + pcfOffsets[10] * texelScale2).r ? increment : 0.0f);     

    bigShadow   += (fCurrDepth1 > tex2D(ShadowMap1, LightTexCoord1 + pcfOffsets[11] * texelScale1).r ? increment : 0.0f);
    localShadow += (fCurrDepth2 > tex2D(ShadowMap2, LightTexCoord2 + pcfOffsets[11] * texelScale2).r ? increment : 0.0f);   
    bigShadow   += (fCurrDepth1 > tex2D(ShadowMap1, LightTexCoord1 + pcfOffsets[12] * texelScale1).r ? increment : 0.0f);
    localShadow += (fCurrDepth2 > tex2D(ShadowMap2, LightTexCoord2 + pcfOffsets[12] * texelScale2).r ? increment : 0.0f);     
    bigShadow   += (fCurrDepth1 > tex2D(ShadowMap1, LightTexCoord1 + pcfOffsets[13] * texelScale1).r ? increment : 0.0f);
    localShadow += (fCurrDepth2 > tex2D(ShadowMap2, LightTexCoord2 + pcfOffsets[13] * texelScale2).r ? increment : 0.0f);     
    bigShadow   += (fCurrDepth1 > tex2D(ShadowMap1, LightTexCoord1 + pcfOffsets[14] * texelScale1).r ? increment : 0.0f);
    localShadow += (fCurrDepth2 > tex2D(ShadowMap2, LightTexCoord2 + pcfOffsets[14] * texelScale2).r ? increment : 0.0f);     
    bigShadow   += (fCurrDepth1 > tex2D(ShadowMap1, LightTexCoord1 + pcfOffsets[15] * texelScale1).r ? increment : 0.0f);
    localShadow += (fCurrDepth2 > tex2D(ShadowMap2, LightTexCoord2 + pcfOffsets[15] * texelScale2).r ? increment : 0.0f);     
    bigShadow   += (fCurrDepth1 > tex2D(ShadowMap1, LightTexCoord1 + pcfOffsets[16] * texelScale1).r ? increment : 0.0f);
    localShadow += (fCurrDepth2 > tex2D(ShadowMap2, LightTexCoord2 + pcfOffsets[16] * texelScale2).r ? increment : 0.0f);     
    bigShadow   += (fCurrDepth1 > tex2D(ShadowMap1, LightTexCoord1 + pcfOffsets[17] * texelScale1).r ? increment : 0.0f);
    localShadow += (fCurrDepth2 > tex2D(ShadowMap2, LightTexCoord2 + pcfOffsets[17] * texelScale2).r ? increment : 0.0f);     
    bigShadow   += (fCurrDepth1 > tex2D(ShadowMap1, LightTexCoord1 + pcfOffsets[18] * texelScale1).r ? increment : 0.0f);
    localShadow += (fCurrDepth2 > tex2D(ShadowMap2, LightTexCoord2 + pcfOffsets[18] * texelScale2).r ? increment : 0.0f);     
    bigShadow   += (fCurrDepth1 > tex2D(ShadowMap1, LightTexCoord1 + pcfOffsets[19] * texelScale1).r ? increment : 0.0f);
    localShadow += (fCurrDepth2 > tex2D(ShadowMap2, LightTexCoord2 + pcfOffsets[19] * texelScale2).r ? increment : 0.0f);     
    bigShadow   += (fCurrDepth1 > tex2D(ShadowMap1, LightTexCoord1 + pcfOffsets[20] * texelScale1).r ? increment : 0.0f);
    localShadow += (fCurrDepth2 > tex2D(ShadowMap2, LightTexCoord2 + pcfOffsets[20] * texelScale2).r ? increment : 0.0f); 

    bigShadow   += (fCurrDepth1 > tex2D(ShadowMap1, LightTexCoord1 + pcfOffsets[21] * texelScale1).r ? increment : 0.0f);
    localShadow += (fCurrDepth2 > tex2D(ShadowMap2, LightTexCoord2 + pcfOffsets[21] * texelScale2).r ? increment : 0.0f);   
    bigShadow   += (fCurrDepth1 > tex2D(ShadowMap1, LightTexCoord1 + pcfOffsets[22] * texelScale1).r ? increment : 0.0f);
    localShadow += (fCurrDepth2 > tex2D(ShadowMap2, LightTexCoord2 + pcfOffsets[22] * texelScale2).r ? increment : 0.0f);     
    bigShadow   += (fCurrDepth1 > tex2D(ShadowMap1, LightTexCoord1 + pcfOffsets[23] * texelScale1).r ? increment : 0.0f);
    localShadow += (fCurrDepth2 > tex2D(ShadowMap2, LightTexCoord2 + pcfOffsets[23] * texelScale2).r ? increment : 0.0f);     
    bigShadow   += (fCurrDepth1 > tex2D(ShadowMap1, LightTexCoord1 + pcfOffsets[24] * texelScale1).r ? increment : 0.0f);
    localShadow += (fCurrDepth2 > tex2D(ShadowMap2, LightTexCoord2 + pcfOffsets[24] * texelScale2).r ? increment : 0.0f);     
    bigShadow   += (fCurrDepth1 > tex2D(ShadowMap1, LightTexCoord1 + pcfOffsets[25] * texelScale1).r ? increment : 0.0f);
    localShadow += (fCurrDepth2 > tex2D(ShadowMap2, LightTexCoord2 + pcfOffsets[25] * texelScale2).r ? increment : 0.0f);     
    bigShadow   += (fCurrDepth1 > tex2D(ShadowMap1, LightTexCoord1 + pcfOffsets[26] * texelScale1).r ? increment : 0.0f);
    localShadow += (fCurrDepth2 > tex2D(ShadowMap2, LightTexCoord2 + pcfOffsets[26] * texelScale2).r ? increment : 0.0f);     
    bigShadow   += (fCurrDepth1 > tex2D(ShadowMap1, LightTexCoord1 + pcfOffsets[27] * texelScale1).r ? increment : 0.0f);
    localShadow += (fCurrDepth2 > tex2D(ShadowMap2, LightTexCoord2 + pcfOffsets[27] * texelScale2).r ? increment : 0.0f);     
    bigShadow   += (fCurrDepth1 > tex2D(ShadowMap1, LightTexCoord1 + pcfOffsets[28] * texelScale1).r ? increment : 0.0f);
    localShadow += (fCurrDepth2 > tex2D(ShadowMap2, LightTexCoord2 + pcfOffsets[28] * texelScale2).r ? increment : 0.0f);     
    bigShadow   += (fCurrDepth1 > tex2D(ShadowMap1, LightTexCoord1 + pcfOffsets[29] * texelScale1).r ? increment : 0.0f);
    localShadow += (fCurrDepth2 > tex2D(ShadowMap2, LightTexCoord2 + pcfOffsets[29] * texelScale2).r ? increment : 0.0f);     

	 // Distance to the inner shadow border (0.0 .. 0.5)
    float borderDist = 0.5f - max(abs(0.5f - LightTexCoord2.x), abs(0.5f - LightTexCoord2.y));
    float fade = smoothstep(0.0f, 0.2f, borderDist);
	 
    float finalShadow = lerp(bigShadow, localShadow, fade);//(shadow2 + (shadow1 * fade)) / 2.0f;
	 //finalShadow = bigShadow + gridVal * 0.8f;
	 float InvertedOpacity = 1.0f - (95.0f / 120.0f);

	finalShadow = lerp(bigShadow, localShadow, fade); 
	finalShadow *= InvertedOpacity;

	LightResult *= (1.0f - finalShadow);

    if (finalShadow < 0.30)
    {
    
        // Compute view direction
        float3 V = normalize(CameraPos - input.WorldPos.xyz);
    
        // Compute reflection vector
        float3 R = reflect(-L, N);
    
        // Compute specular reflection based on alpha intensity
        float SpecularIntensity = pow(saturate(dot(V, R)), Shininess) * 1.75f;
    
        // Accumulate specular lighting result
        LightResult += (SpecularIntensity * MaterialSpecular * LightSpecular.rgb)/1.5f * Color.r;
    }
   
#endif

    Color.rgb *= LightResult;

    // Fog
    const float fogStart = FogRange.x;
    const float fogEnd = FogRange.y;
    float distance = length(input.WorldPos.xyz - CameraPos);
    float fogFactor = saturate((distance - fogStart) / (fogEnd - fogStart));
    return lerp(Color, FogColor, fogFactor);
}

)";
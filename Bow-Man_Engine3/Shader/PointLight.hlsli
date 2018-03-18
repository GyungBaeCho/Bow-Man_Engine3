#include "ConstantBuffer.hlsli"
#include "Function.hlsli"
#include "GBuffer.hlsli"

static const float3 HemiDir[2] = { float3(1.0, 1.0, 1.0), float3(-1.0, 1.0, -1.0) };

struct VS_IN
{
    float4 position : POSITION;
};

struct VS_OUT
{
    float4 position	 : SV_POSITION;
    uint uInstanceID : INSTANCE_ID;
};

struct HS_CONSTANT_DATA_OUTPUT
{
    float Edges[4]	: SV_TessFactor;
    float Inside[2]	: SV_InsideTessFactor;
};

struct HS_OUT
{
    float3 HemiDir	 : POSITION;
    uint uInstanceID : INSTANCE_ID;
};

struct DS_OUT
{
    float4 position	 : SV_POSITION;
    uint uInstanceID : INSTANCE_ID;
    float2 cpPos	 : CLIP_POSITION;
};

float4 CalDeferredPointLight(SurfaceElement surface, uint uInstanceID)
{
//	float4 materialColor = gcMaterial.m_vDiffuseSpec;
	float4 materialColor = float4(1, 1, 1, 10);

	float3 vToLight = gArrPointLight[uInstanceID].m_vPosition - surface.vPosition;
	float fDistToLight = length(vToLight);
	vToLight /= fDistToLight;
	float fDotNL = saturate(dot(vToLight, surface.vNormal));

	//Ambient
    float3 vAmbientColor = gArrPointLight[uInstanceID].m_vColorSpecular.rgb;

	//Diffuse
    float3 vDiffuseColor = gArrPointLight[uInstanceID].m_vColorSpecular.rgb * fDotNL;

	//Specular
    float3 vToEye = normalize(gvCameraPosition.xyz - surface.vPosition);
	float3 vHalf = normalize(vToEye + vToLight);
	float fDotNH = saturate(dot(surface.vNormal, vHalf));
    float3 vSpecualarColor = gArrPointLight[uInstanceID].m_vColorSpecular.rgb * pow(fDotNH, materialColor.a) * gArrPointLight[uInstanceID].m_vColorSpecular.a * surface.fSpecularPower * fDotNL;

	//Distance Attenuation
	float fDistToLightNorm = 1.0 - saturate(fDistToLight * (1 / gArrPointLight[uInstanceID].m_fRange));
	float Attn = fDistToLightNorm * fDistToLightNorm;

	//factor
	float3 vFactor = materialColor.rgb * Attn * gArrPointLight[uInstanceID].m_fPower;

	//Result	
    return float4(((surface.vAlbedo.rgb * (vDiffuseColor + vAmbientColor)) + vSpecualarColor) * vFactor, 1);
}
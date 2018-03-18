#include "ConstantBuffer.hlsli"
#include "Function.hlsli"
#include "GBuffer.hlsli"

static const float3 HemiDir[2] = { float3(1.0, 1.0, 1.0), float3(-1.0, 1.0, -1.0), };

#define CylinderPortion 0.2
#define ExpendAmount (1.0 + CylinderPortion)

struct VS_IN
{
	float4 position : POSITION;
};

struct VS_OUT
{
	float4 position : SV_POSITION;
	uint uInstanceID : INSTANCE_ID;
};

struct HS_CONSTANT_DATA_OUTPUT
{
	float Edges[4] : SV_TessFactor;
	float Inside[2] : SV_InsideTessFactor;
};

struct HS_OUT
{
	float3 HemiDir : POSITION;
	uint uInstanceID : INSTANCE_ID;
};

struct DS_OUT
{
	float4 position : SV_POSITION;
	uint uInstanceID : INSTANCE_ID;
	float2 cpPos : CLIP_POSITION;
};


float3 CalDeferredSpotLight(SurfaceElement surface, uint uInstanceID)
{
    //(gArrSpotLight[uInstanceID].m_vColorSpecular.rgb * vFactor)
//	float4 materialColor = gcMaterial.m_vDiffuseSpec;
    float4 materialColor = float4(1, 1, 1, 10);

    float3 vToLight = gArrSpotLight[uInstanceID].m_vPosition - surface.vPosition;
    float fDistToLight = length(vToLight);
    vToLight /= fDistToLight; //¡§±‘»≠

	//Ambient
    float3 vAmbientColor = gArrSpotLight[uInstanceID].m_vColorSpecular.rgb;

	//Diffuse
    float fDotNL = saturate(dot(vToLight, surface.vNormal));
    float3 vDiffuseColor = gArrSpotLight[uInstanceID].m_vColorSpecular.rgb * fDotNL;

	//Specular
    float3 vToEye = normalize(gvCameraPosition.xyz - surface.vPosition);
    float3 vHalf = normalize(vToEye + vToLight);
    float fDotNH = saturate(dot(surface.vNormal, vHalf));
	 
    float3 vSpecualarColor = gArrSpotLight[uInstanceID].m_vColorSpecular.rgb * pow(fDotNH, materialColor.a) * gArrSpotLight[uInstanceID].m_vColorSpecular.a * surface.fSpecularPower * fDotNL;

	//Cone Attenuation
    float cosAng = dot(gArrSpotLight[uInstanceID].m_vDirection, -vToLight);
    float conAtt = saturate((cosAng - gArrSpotLight[uInstanceID].m_fCosOuter) * gArrSpotLight[uInstanceID].m_fCosAttRcp);
    conAtt *= conAtt;

	//Distant Attenuation
    float fDistToLightNorm = 1.0 - saturate(fDistToLight * (1.0 / gArrSpotLight[uInstanceID].m_fRange));
    float Attn = fDistToLightNorm * fDistToLightNorm;

	//¡∂∏Ì ∞®º‚ ∆—≈Õ
    float3 vFactor = materialColor.rgb * conAtt * Attn * gArrSpotLight[uInstanceID].m_fPower;

    return float4(((surface.vAlbedo.rgb * (vDiffuseColor + vAmbientColor)) + vSpecualarColor) * vFactor, 1);
}


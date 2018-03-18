#include "GBuffer.hlsli"
#include "Function.hlsli"
#include "ConstantBuffer.hlsli"

struct VS_IN
{
    float4 position : POSITION;
};

struct VS_OUT
{
	float4 position		: SV_Position;
	float2 texCoord		: TEXCOORD0;
	float2 cpPos		: CLIP_POSITION;
	uint uInstanceID	: INSTANCE_ID;
};

float4 CalDeferredDirLight(SurfaceElement surface, uint uInstanceID)
{
    float3 vToCamera = normalize(gvCameraPosition.xyz - surface.vPosition);

//	float4 materialColor = gcMaterial.m_vDiffuseSpec;
    float4 materialColor = float4(1, 1, 1, 10);

    float3 vAmbientColor = gArrDirLight[uInstanceID].m_vColorSpecular.rgb;

	//Semi Globe Light
    float fFactor = ((surface.vNormal.y + 1) * 0.5);
    float3 vDiffuseColor = ((1 - fFactor) * gArrDirLight[uInstanceID].m_vUpperColor) + (fFactor * gArrDirLight[uInstanceID].m_vDownColor);

	//Diffuse
    float fDotNL = dot(surface.vNormal, -gArrDirLight[uInstanceID].m_vDirection);
    fDotNL = saturate(fDotNL);
    float3 vDiffuse = gArrDirLight[uInstanceID].m_vColorSpecular.rgb * fDotNL;

	//Specular
    float3 vHalf1 = normalize(vToCamera + (-gArrDirLight[uInstanceID].m_vDirection));
    float3 vHalf2 = normalize(-gvCameraDirection.xyz + (-gArrDirLight[uInstanceID].m_vDirection));
    float fDotNH = saturate(dot(surface.vNormal, vHalf1));
    float3 vSpecualarColor = gArrDirLight[uInstanceID].m_vColorSpecular.rgb * pow(fDotNH, materialColor.a) * gArrDirLight[uInstanceID].m_vColorSpecular.a * surface.fSpecularPower * fDotNL;

	//Attenuation Factor
    float3 vFactor = materialColor.rgb * gArrDirLight[uInstanceID].m_fPower;

	//Resul
    return float4(((surface.vAlbedo.rgb * (vAmbientColor + vDiffuseColor)) + vSpecualarColor) * vFactor, 1);
}
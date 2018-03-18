#ifndef __FUNCTION__
	#define __FUNCTION__

#include "ConstantBuffer.hlsli"
#include "GBuffer.hlsli"
#include "Texture.hlsli"

struct SurfaceElement
{
	float3 vAlbedo;
	float3 vPosition;
	float3 vNormal;
	float fSpecularPower;
};

//--------------------------------------------------------------------------
float CalLinerDepth(float fDepth)
{
	float fLinerDepth = gvPerspectiveValue.z / (fDepth + gvPerspectiveValue.w);
	return fLinerDepth;
}

float3 CalWorldPos(float2 vClipPos, float fDepth)
{
	float fLinerDepth = CalLinerDepth(fDepth);

	float4 vWorldPos;
	vWorldPos.xy = vClipPos * gvPerspectiveValue.xy * fLinerDepth;
	vWorldPos.z = fLinerDepth;
	vWorldPos.w = 1.0f;
    vWorldPos = mul(vWorldPos, gmtxViewInversed);
	return vWorldPos.xyz;
}

SurfaceElement UnpackSurfaceElement(float2 cpCoord, float2 position)
{
	SurfaceElement surfaceElement;

	float3 cpPos = float3(position, 0);
    float fDepth = gtexDepth.Load(cpPos).r;

	surfaceElement.vAlbedo = gtexAlbedo.Load(cpPos).rgb;
//	surfaceElement.vNormal = (gtexNormalDepth.Load(cpPos) * 2) - 1;
	surfaceElement.vNormal = gtexNormalDepth.Load(cpPos).xyz;
	surfaceElement.vPosition = gtexPositionSpec.Load(cpPos).xyz;
//	surfaceElement.vPosition = CalWorldPos(cpCoord, fDepth);
//	surfaceElement.fSpecularPower = gtexPositionSpec.Load(cpPos).w;
    surfaceElement.fSpecularPower = gtexNormalDepth.Load(cpPos).a;
	return surfaceElement;
}

float4 Fog(float4 cColor, float3 vPosition)
{
	float3 vCameraPosition = gvCameraPosition.xyz;
	float3 vPositionToCamera = vCameraPosition - vPosition;
	float fDistanceToCamera = length(vPositionToCamera);

    float fNear = gfFarPlaneDistant*0.1;
    float fFar = gfFarPlaneDistant - 500;
//	float4 f4FogColor = float4(0.0, 0.0, 0, cColor.a);
    float4 f4FogColor = float4(0.3, 0.2, 0.4, cColor.a);
//	float4 f4FogColor = float4(0.7, 0.9, 1, cColor.a);

	float fFogRange = fFar - fNear;
	float fFogFactor = saturate((fDistanceToCamera - fNear) / fFogRange);

	return lerp(cColor, f4FogColor, fFogFactor);
}

float4x4 QuaternionToMatrix(float4 quat)
{
	float4x4 output;
	float yy2 = 2.0f * quat.y * quat.y;
	float xy2 = 2.0f * quat.x * quat.y;
	float xz2 = 2.0f * quat.x * quat.z;
	float yz2 = 2.0f * quat.y * quat.z;
	float zz2 = 2.0f * quat.z * quat.z;
	float wz2 = 2.0f * quat.w * quat.z;
	float wy2 = 2.0f * quat.w * quat.y;
	float wx2 = 2.0f * quat.w * quat.x;
	float xx2 = 2.0f * quat.x * quat.x;
	output[0][0] = -yy2 - zz2 + 1.0f;
	output[0][1] = xy2 + wz2;
	output[0][2] = xz2 - wy2;
	output[0][3] = 0;
	output[1][0] = xy2 - wz2;
	output[1][1] = -xx2 - zz2 + 1.0f;
	output[1][2] = yz2 + wx2;
	output[1][3] = 0;
	output[2][0] = xz2 + wy2;
	output[2][1] = yz2 - wx2;
	output[2][2] = -xx2 - yy2 + 1.0f;
	output[2][3] = 0;
	output[3][0] = output[3][1] = output[3][2] = 0;
	output[3][3] = 1;
	return output;
}

//--------------------------------------------------------------------------
float4 CalFowardLight(float4 vAlbedo, float3 vPosition, float3 vNormal, float fSpecualr)
{
	float3 vToCamera = normalize(gvCameraPosition.xyz - vPosition);

	float3 vDiffuseColor = float3(0, 0, 0);
	float3 vSpecualarColor = float3(0, 0, 0);
	float4 materialColor = gcMaterial.m_vDiffuseSpec;

    int i;
	//Directianal Light
	//	[unroll]
	for (i = 0; i < gnLight.x; ++i)
	{
	//	if (gArrDirLight[i].m_fPower == 0) continue;

		//Semi Globe Light
		float fFactor = ((vNormal.y + 1) * 0.5);
		vDiffuseColor += ((1 - fFactor) * gArrDirLight[i].m_vUpperColor.rgb) + (fFactor * gArrDirLight[i].m_vDownColor.rgb);

		//Diffuse
		float fDotNL = dot(vNormal, -gArrDirLight[i].m_vDirection);
		fDotNL = saturate(fDotNL);
		float3 vDiffuse = gArrDirLight[i].m_vColorSpecular.rgb * fDotNL;

		//Specular
		float3 vHalf1 = normalize(vToCamera + (-gArrDirLight[i].m_vDirection));
		float3 vHalf2 = normalize(-gvCameraDirection.xyz + (-gArrDirLight[i].m_vDirection));
		float fDotNH = saturate(dot(vNormal, vHalf1));
		float3 vSpecular = gArrDirLight[i].m_vColorSpecular.rgb * pow(fDotNH, materialColor.a) * gArrDirLight[i].m_vColorSpecular.a * fSpecualr * fDotNL;

		float3 vFactor = materialColor.rgb * gArrDirLight[i].m_fPower;

		//Result
		vDiffuseColor += (vDiffuse * vFactor);
		vSpecualarColor += (vSpecular * vFactor);
	}

	//Point Light
	//[unroll]
	for (i = 0; i < gnLight.y; ++i)
	{
	//	if (gArrPointLight[i].m_fPower == 0) continue;

		float3 vToLight = gArrPointLight[i].m_vPosition - vPosition;
		float fDistToLight = length(vToLight);
		vToLight /= fDistToLight;
		float fDotNL = saturate(dot(vToLight, vNormal));

		//Diffuse
		float3 vDiffuse = gArrPointLight[i].m_vColorSpecular.rgb * fDotNL;

		//Specular
		float3 vToEye = gvCameraPosition.xyz - vPosition;
		vToEye = normalize(vToEye);
		float3 vHalf = normalize(vToEye + vToLight);
		float fDotNH = saturate(dot(vNormal, vHalf));
		float3 vSpecular = gArrPointLight[i].m_vColorSpecular.rgb * pow(fDotNH, materialColor.a) * gArrPointLight[i].m_vColorSpecular.a * fSpecualr * fDotNL;

		float fDistToLightNorm = 1.0 - saturate(fDistToLight * (1 / gArrPointLight[i].m_fRange));
		float Attn = fDistToLightNorm * fDistToLightNorm;
		float3 vFactor = materialColor.rgb * Attn * gArrPointLight[i].m_fPower;

		//Result	
		vDiffuseColor += (vDiffuse * vFactor);
		vSpecualarColor += (vSpecular * vFactor);
	}

	//Spot Light
	//	[unroll]
	for (i = 0; i < gnLight.z; ++i)
	{
	//	if (gArrSpotLight[i].m_fPower == 0) continue;

		float3 vToLight = gArrSpotLight[i].m_vPosition - vPosition;
		float fDistToLight = length(vToLight);
		vToLight /= fDistToLight; //정규화

		float3 vToEye = gvCameraPosition.xyz - vPosition;

		//퐁 디퓨즈
		float fDotNL = saturate(dot(vToLight, vNormal));
		float3 vDiffuse = gArrSpotLight[i].m_vColorSpecular.rgb * fDotNL;

		//블린 스펙큘러
		vToEye = normalize(vToEye);
		float3 vHalf = normalize(vToEye + vToLight);
		float fDotNH = saturate(dot(vNormal, vHalf));
		float3 vSpecular = gArrSpotLight[i].m_vColorSpecular.rgb * pow(fDotNH, materialColor.a) * gArrSpotLight[i].m_vColorSpecular.a * fSpecualr * fDotNL;

		//원뿔 감쇄
		float cosAng = dot(gArrSpotLight[i].m_vDirection, -vToLight);
		float conAtt = saturate((cosAng - gArrSpotLight[i].m_fCosOuter) * gArrSpotLight[i].m_fCosAttRcp);
		conAtt *= conAtt;

		//거리 감쇄
		float fDistToLightNorm = 1.0 - saturate(fDistToLight * (1.0 / gArrSpotLight[i].m_fRange));
		float Attn = fDistToLightNorm * fDistToLightNorm;

        float3 vFactor = materialColor.rgb * conAtt * Attn * gArrSpotLight[i].m_fPower;

		vDiffuseColor += (vDiffuse * vFactor);
		vSpecualarColor += (vSpecular * vFactor);
	}

	return float4((vAlbedo.rgb * vDiffuseColor) + vSpecualarColor, vAlbedo.a);
}


//------------------------------------------------------------------------------------------------------------------------------------------------------------
//float4 Fog2(float4 cColor, float fDencity) {
//	if (gvFogParameter.x == LINEAR_FOG){
//		fFogFactor = fDencity * gvFogParameter.w;
//	}//saturate 0이하면 0으로 1이상이라면 1로 변환시켜준다.
//
//	return lerp(cColor, gcFogColor, fFogFactor);
//}

//float4 Fog(float4 cColor, float3 vPosition)
//{
//	float3 vCameraPosition = gvCameraPosition.xyz;
//	float3 vPositionToCamera = vCameraPosition - vPosition;
//	float fDistanceToCamera = length(vPositionToCamera);
//	float fFogFactor = 0.0f;

//	if (gvFogParameter.x == LINEAR_FOG)
//	{
//		float fFogRange = gvFogParameter.z - gvFogParameter.y;
//		fFogFactor = saturate((fDistanceToCamera - gvFogParameter.y) / fFogRange * gvFogParameter.w);
//	} //saturate 0이하면 0으로 1이상이라면 1로 변환시켜준다.

////	cColor = lerp(cColor, gcFogColor, fFogFactor);

//	//if (cColor.a == 1) {
//	//	cColor.r = 1;
//	//	cColor.g = 0;
//	//	cColor.b = 0;
//	//}

//	return lerp(cColor, gcFogColor, fFogFactor);
//}

//------------------------------------------------------------------------------------------------------------------------------------------------------------
//float4 CartoonRender(float4 color){
//	return color;

//	float data = color.r*100;
//	data -= data%10;
//	data /= 100;
//	color.r = data;

//	data = color.g*100;
//	data -= data%10;
//	data /= 100;
//	color.g = data;

//	data = color.b*100;
//	data -= data%10;
//	data /= 100;
//	color.b = data;

////	color = float4(smoothstep(0.1, 0.9, color.r), smoothstep(0.3, 0.7, color.g), smoothstep(0.3, 0.7, color.b), 1);

////	color.a = 10;

//	return color;
//}


//float4 GreyTone(float4 color){
//	return float4(color.r + (color.g-color.r)*gcGreyTone.r, color.g, color.b + (color.g-color.b)*gcGreyTone.r, 1);
////	return float4(color.r, color.g + (color.r-color.g)*gcGreyTone.r, color.b + (color.r-color.b)*gcGreyTone.r, 1);

//	//	float sum = (color.r*gcGreyTone.r + color.g*gcGreyTone.r + color.b*gcGreyTone.r)/3;
////	float sum = color.r;
////	color = float4(sum, sum, sum, 1);

////	return color;
//}


#endif
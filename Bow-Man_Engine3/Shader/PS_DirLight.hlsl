#include "DirLight.hlsli"

//SurfaceElement UnpackSurfaceElement(float2 cpCoord, float2 position)
//{
//    SurfaceElement surfaceElement;

//    float3 vPosition = float3(position, 0);
//    float fDepth = gtexDepth.Load(vPosition);

//    surfaceElement.vAlbedo = gtexAlbedo.Load(vPosition);
//    surfaceElement.vPosition = CalWorldPos(cpCoord, fDepth);

//    surfaceElement.vNormal = gtexNormalDepth.Load(vPosition);
//    surfaceElement.fSpecularPower = gtexPositionSpec.Load(vPosition).w;

//    return surfaceElement;
//}


float4 main(VS_OUT input) : SV_TARGET
{
	SurfaceElement surface = UnpackSurfaceElement(input.cpPos, input.position.xy);
	return CalDeferredDirLight(surface, input.uInstanceID);
}
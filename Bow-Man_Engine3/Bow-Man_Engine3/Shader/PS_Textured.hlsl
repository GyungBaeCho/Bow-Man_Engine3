#include "Function.hlsli"
#include "Texture.hlsli"
#include "GBuffer.hlsli"

struct VS_OUTPUT
{
	float4 position		: SV_POSITION;
	float3 positionW	: POSITION;
	float2 texCoordBase : TEXCOORD0;
};

float4 main(VS_OUTPUT input) : SV_Target
{
	float4 cColor = gtexAlbedo.Sample(gSSWarp, input.texCoordBase);
	return cColor;
//	return Fog(cColor, input.positionW);
}

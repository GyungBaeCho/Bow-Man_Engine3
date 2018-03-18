#include "Function.hlsli"
#include "GBuffer.hlsli"
#include "Texture.hlsli"

struct VS_OUTPUT
{
	float4 position : SV_POSITION;
	float3 positionW : POSITION;
	float2 texCoordBase : TEXCOORD0;
	float3 normalW : NORMAL;
};


float4 main(VS_OUTPUT input) : SV_Target
{
	float4 cColor;
	cColor = gtexAlbedo.Sample(gSSWarp, input.texCoordBase);
	cColor = CalFowardLight(cColor, input.positionW, input.normalW, 1);

//	cColor = gtexAlbedo.Sample(gSSWarp, input.texCoordBase) * input.color;
//	cColor = gtexAlbedo.Sample(gSSWarp, input.texCoordBase) * Lighting(input.positionW, input.normalW);
//	cColor = Fog(cColor, input.positionW);

//	if (cColor.a < 0.6)	discard;

    return cColor;
//	return Fog(cColor, input.positionW);
}

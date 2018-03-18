#include "ConstantBuffer.hlsli"
#include "Texture.hlsli"

Texture2D				gtexHDR : register(t0);
StructuredBuffer<float> gtexAverageLum : register(t1);
Texture2D<float4>		gtexBloom : register(t2);

struct VS_OUT
{
	float4 position : SV_Position;
	float2 texCoord : TEXCOORD0;
};

float4 main(VS_OUT input) : SV_TARGET
{
	float3 vHdrColor = gtexHDR.Sample(gSSWarp, input.texCoord).rgb;
	vHdrColor += gfBloomScale * gtexBloom.Sample(gSSWarp, input.texCoord).rgb;

	float fLScale = dot(vHdrColor, LUM_FACTOR.rgb);
	fLScale *= gMiddleGrey / gtexAverageLum[0];
	fLScale = (fLScale + (fLScale * fLScale) / gLumWhiteSqr) / (1.0 + fLScale);

    float4 cFinalColor = float4(vHdrColor * fLScale, 1);
//    cFinalColor.rgb = gtexPositionSpec.Sample(gSSClamp, input.texCoord).rgb;

	return cFinalColor;
}
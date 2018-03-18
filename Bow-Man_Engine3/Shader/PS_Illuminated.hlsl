#include "GBuffer.hlsli"
#include "Function.hlsli"

struct VS_OUTPUT
{
	float4 position		: SV_POSITION;
	float3 positionW	: POSITION;
	float3 normalW		: NORMAL;
};

float4 main(VS_OUTPUT input) : SV_Target
{
	float4 cColor = CalFowardLight(float4(1, 1, 1, 1), input.positionW, input.normalW, 1);
    return Fog(cColor, input.positionW);
}
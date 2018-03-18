#include "Function.hlsli"
#include "GBuffer.hlsli"

struct VS_OUTPUT
{
	float4 position		: SV_POSITION;
	float3 positionW	: POSITION0;
	float4 color		: COLOR0;
};

float4 main(VS_OUTPUT input) : SV_Target
{
	return Fog(input.color, input.positionW);
//	return input.color;
}

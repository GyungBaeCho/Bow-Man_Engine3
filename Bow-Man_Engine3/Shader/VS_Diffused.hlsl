//POSITION
//COLOR
//END

#include "ConstantBuffer.hlsli"

struct VS_INPUT
{
	float3 position : POSITION0;
	float4 color : COLOR0;
};

struct VS_OUTPUT
{
	float4 position : SV_POSITION;
	float3 positionW : POSITION0;
	float4 color : COLOR0;
};

//VS_OUTPUT main(VS_INPUT input) : SV_POSITION
VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	output.positionW = mul(float4(input.position, 1.0f), gmtxWorld).xyz;
	output.position = mul(float4(output.positionW, 1), gmtxVP);
	output.color = input.color;

	return output;
}

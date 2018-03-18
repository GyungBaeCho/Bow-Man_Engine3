//POSITION
//TEXCOORD
//END

#include "ConstantBuffer.hlsli"

struct VS_INPUT
{
	float3 position		: POSITION;
	float2 texCoordBase : TEXCOORD0;
};

struct VS_OUTPUT
{
	float4 position		: SV_POSITION;
	float3 positionW	: POSITION;
	float2 texCoordBase : TEXCOORD0;
};


VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output = (VS_OUTPUT)0;
	output.positionW = mul(float4(input.position, 1), gmtxWorld).xyz;
	output.position = mul(float4(output.positionW, 1), gmtxVP);
	output.texCoordBase = input.texCoordBase;

	return(output);
}
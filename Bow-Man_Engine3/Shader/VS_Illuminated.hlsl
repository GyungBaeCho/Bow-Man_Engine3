//POSITION
//NORMAL
//END

#include "ConstantBuffer.hlsli"
#include "Function.hlsli"

struct VS_INPUT
{
	float3 position		: POSITION;
	float3 normal		: NORMAL;
};

struct VS_OUTPUT
{
	float4 position		: SV_POSITION;
	float3 positionW	: POSITION;
	float3 normalW		: NORMAL;
};

//VS_OUTPUT main(VS_INPUT input, uint vtxID : SV_VertexID) : SV_POSITION
VS_OUTPUT main(VS_INPUT input, uint vtxID : SV_VertexID)
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	output.positionW = mul(float4(input.position, 1.0f), gmtxWorld).xyz;
	output.position = mul(float4(output.positionW, 1.0f), gmtxVP);
	output.normalW = mul(input.normal, (float3x3) gmtxWorld);
//	output.normalW = normalize(mul(input.normal, (float3x3)gmtxWorld));

	return output;
}
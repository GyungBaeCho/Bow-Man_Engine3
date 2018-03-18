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
	VS_OUTPUT output = (VS_OUTPUT) 0;
	matrix mtxWorld = gmtxWorld;
	mtxWorld._41 = 0;
	mtxWorld._42 = 0;
	mtxWorld._43 = 0;
	output.positionW = mul(float4(input.position, 1.0f), mtxWorld).xyz;
	
	matrix mtxView = gmtxView;
	mtxView._41 = 0;
	mtxView._42 = 0;
	mtxView._43 = 0;

	matrix mtxVP = mul(mtxView, gmtxProjection);
	output.position = mul(float4(output.positionW, 1), mtxVP);

	output.texCoordBase = input.texCoordBase;
	return (output);
}


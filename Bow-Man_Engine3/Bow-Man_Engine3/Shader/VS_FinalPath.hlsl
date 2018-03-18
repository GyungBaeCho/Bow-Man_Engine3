
static const float2 arrScreenPos[4] =
{
	float2(-1.0, 1.0),
	float2(1.0, 1.0),
	float2(-1.0, -1.0),
	float2(1.0, -1.0),
};

static const float2 arrScreenTex[4] =
{
	float2(0, 0),
	float2(1, 0),
	float2(0, 1),
	float2(1, 1),
};

struct VS_OUT
{
	float4 position : SV_Position;
	float2 texCoord : TEXCOORD0;
};

VS_OUT main(uint VertexID : SV_VertexID)
{
	VS_OUT output;
	output.position = float4(arrScreenPos[VertexID], 0.0, 1.0);
	output.texCoord = arrScreenTex[VertexID];
	return output;
}
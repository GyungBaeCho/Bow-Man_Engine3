#include "DirLight.hlsli"

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

VS_OUT main(VS_IN input, uint VertexID : SV_VertexID, uint InstanceID: SV_InstanceID)
{
    VS_OUT output;
    output.position = float4(arrScreenPos[VertexID], 0.0, 1.0);
    output.texCoord = arrScreenTex[VertexID];
    output.cpPos = output.position.xy;
    output.uInstanceID = InstanceID;
    return output;
}
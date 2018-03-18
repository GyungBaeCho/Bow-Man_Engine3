#include "GBuffer.hlsli"
#include "Function.hlsli"

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float3 positionW : POSITION;
    float3 normalW : NORMAL;
};

G_BUFFER main(VS_OUTPUT input) : SV_Target
{
	G_BUFFER output = (G_BUFFER) 0;
	output.color = float4(1,1,1,1);
    output.color = Fog(output.color, input.positionW);

	output.normal = float4(input.normalW, 0);
//	output.normal = float4((input.normalW + 1) * 0.5, 0);

	//float fSpecular = gMaterial.m_cDiffuseSpec.a;
	output.position = float4(input.positionW, 1);

    return output;
}
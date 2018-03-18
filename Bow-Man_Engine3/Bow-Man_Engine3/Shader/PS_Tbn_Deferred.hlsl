#include "Function.hlsli"
#include "GBuffer.hlsli"
#include "Texture.hlsli"

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float3 positionW : POSITION;
    float2 texCoordBase : TEXCOORD0;
    float3x3 tbn : TBN;
};

G_BUFFER main(VS_OUTPUT input) : SV_Target
{
	G_BUFFER output = (G_BUFFER) 0;
	float4 cColor = gtexAlbedo.Sample(gSSWarp, input.texCoordBase);
	
	output.color = cColor;
	output.color = Fog(output.color, input.positionW);

    output.normal.xyz = (gtexNormalMap.Sample(gSSWarp, input.texCoordBase) * 2) - 1;
	output.normal.xyz = normalize(mul(output.normal.xyz, input.tbn));

//	float fSpecular = gMaterial.m_cDiffuseSpec.a;
	output.position = float4(input.positionW, 1);

	return output;
}

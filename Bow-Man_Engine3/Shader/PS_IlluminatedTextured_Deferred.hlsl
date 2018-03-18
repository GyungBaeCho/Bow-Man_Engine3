#include "Function.hlsli"
#include "GBuffer.hlsli"
#include "Texture.hlsli"

struct VS_OUTPUT
{
    float4 position		: SV_POSITION;
    float3 positionW	: POSITION;
    float2 texCoordBase : TEXCOORD0;
    float3 normalW		: NORMAL;
};

G_BUFFER main(VS_OUTPUT input) : SV_Target
{
	G_BUFFER output = (G_BUFFER) 0;
	float4 cColor = gtexAlbedo.Sample(gSSWarp, input.texCoordBase);
   // if (cColor.a < 0.6) discard;
	
	output.color = cColor;
	output.color = Fog(output.color, input.positionW);
//	output.color = float4(1,0,0,1);

    output.normal = float4(input.normalW, 0);
//	output.normal = float4((input.normalW + 1) * 0.5, 0);

//	float fSpecular = gMaterial.m_cDiffuseSpec.a;
    output.position = float4(input.positionW, 1);

	return output;
}

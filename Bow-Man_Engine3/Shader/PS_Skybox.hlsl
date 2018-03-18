#include "Function.hlsli"
#include "Texture.hlsli"
#include "GBuffer.hlsli"

struct VS_OUTPUT
{
    float4 position		: SV_POSITION;
    float3 positionW	: POSITION;
    float2 texCoordBase : TEXCOORD0;
};

float4 main(VS_OUTPUT input) : SV_Target
{
	float4 cColor = gtexAlbedo.Sample(gSSWarp, input.texCoordBase);
	float2 f2cpPos = float2(input.position.x/1920, input.position.y/1080);

    float fDepth = gtexDepth.Load(float3(input.position.xy, 0)).x;
    
    //if (int(fDepth) == 1)
    //    return cColor;
    //else
    //    discard;

	
	//return float4(fLinerDepth, 0, 0, 1);

	//return float4(fLinerDepth, 0, 0, 1);
	//return float4(CalWorldPos(f2cpPos, fLinerDepth), 1);

	//float fLinerDepth = CalLinerDepth(fDepth) / gfFarPlaneDistant;

    //float4 vWorldPos;
    //vWorldPos.xy = f2cpPos * gvPerspectiveValue.xy * fLinerDepth;
    //vWorldPos.z = fLinerDepth;
    //vWorldPos.w = 1.0f;
    //vWorldPos = mul(vWorldPos, gmtxViewInversed);
    
    //return vWorldPos;

    float fLinerDepth = gvPerspectiveValue.z / (fDepth + gvPerspectiveValue.w) / gfFarPlaneDistant;

   // return float4(fLinerDepth,0,0, 1);

    if ((fLinerDepth) > 0.99)
        return cColor;
    else
        discard;

    return float4(0,0,0,0);
}

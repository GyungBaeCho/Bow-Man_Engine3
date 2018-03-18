#include "Terrain.hlsli"

VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output = (VS_OUTPUT) 0;
	//output.positionW = input.position;
	output.positionW = mul(float4(input.position, 1.0f), gmtxWorld).xyz;
    output.positionW.y = gtexHeightMap.SampleLevel(gSSClamp, float2(input.texCoordBase.x, input.texCoordBase.y), 0).r;
    output.texCoordBase = input.texCoordBase;
    output.texCoordDetail = input.texCoordDetail;
    return output;
}
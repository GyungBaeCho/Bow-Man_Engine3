#include "Terrain.hlsli"

G_BUFFER main(DS_OUTPUT input) : SV_Target
{
    G_BUFFER output = (G_BUFFER) 0;
	
	float4 vBaseColor = gtexAlbedo.Sample(gSSWarp, input.texCoordBase);
    float4 vDetailColor1 = gtexDetail1.Sample(gSSWarp, input.texCoordDetail);
    output.color = (vBaseColor + vDetailColor1)/2;
    output.color.a = 1;
    output.color = Fog(output.color, input.positionW);
//	output.color = float4(0,0, 1, 1);

    float fSpecular = gtexSpecular.Sample(gSSWarp, input.texCoordDetail).r;

    float fTexCellSpaceU = 1/256.0f;
    float fTexCellSpaceV = 1/256.0f;
    float fWorldCellSpace = 1;

    float2 vLeftTex = input.texCoordBase + float2(-fTexCellSpaceU, 0.0f);
    float2 vRightTex = input.texCoordBase + float2(fTexCellSpaceU, 0.0f);
    float2 vBottomTex = input.texCoordBase + float2(0.0f, -fTexCellSpaceV);
    float2 vTopTex = input.texCoordBase + float2(0.0f, fTexCellSpaceV);

    float fLeftY = gtexHeightMap.SampleLevel(gSSWarp, vLeftTex, 0).r;
    float fRightY = gtexHeightMap.SampleLevel(gSSWarp, vRightTex, 0).r;
    float fBottomY = gtexHeightMap.SampleLevel(gSSWarp, vBottomTex, 0).r;
    float fTopY = gtexHeightMap.SampleLevel(gSSWarp, vTopTex, 0).r;

    float3 vTangent = normalize(float3(2.0f * fWorldCellSpace, fRightY - fLeftY, 0.0f));
    float3 vBitangent = normalize(float3(0.0f, fBottomY-fTopY, -2.0f*fWorldCellSpace));
    float3 vNormal = cross(vTangent, vBitangent);

    float3x3 TBN = float3x3(vTangent, vBitangent, vNormal);
    vNormal = gtexNormalMap.Sample(gSSWarp, input.texCoordDetail).xyz;
    vNormal = (vNormal*2)-1;
    vNormal = normalize(mul(vNormal, TBN));

    output.normal = float4(vNormal, fSpecular);
    output.position = float4(input.positionW, 1);

    return output;
}
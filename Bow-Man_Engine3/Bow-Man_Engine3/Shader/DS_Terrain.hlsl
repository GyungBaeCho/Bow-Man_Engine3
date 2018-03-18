#include "Terrain.hlsli"


//fxc /E main /T ds_5_0 /Od /Zi /Fo CompiledGs.fxo DS_Terrain.hlsl
[domain("quad")]
DS_OUTPUT main(HS_CONSTANT_DATA_OUTPUT input, float2 domain : SV_DomainLocation, const OutputPatch<HS_OUTPUT, NUM_CONTROL_POINTS> patch)
{
	float3 vPos1 = lerp(patch[0].positionW, patch[1].positionW, domain.x);
	float3 vPos2 = lerp(patch[2].positionW, patch[3].positionW, domain.x);
	float3 positionW = lerp(vPos1, vPos2, domain.y);

	float2 vTexCoordBase1 = lerp(patch[0].texCoordBase, patch[1].texCoordBase, domain.x);
	float2 vTexCoordBase2 = lerp(patch[2].texCoordBase, patch[3].texCoordBase, domain.x);
	float2 vTexCoordBase = lerp(vTexCoordBase1, vTexCoordBase2, domain.y);

	float2 vTexCoordDetail1 = lerp(patch[0].texCoordDetail, patch[1].texCoordDetail, domain.x);
	float2 vTexCoordDetail2 = lerp(patch[2].texCoordDetail, patch[3].texCoordDetail, domain.x);
	float2 vTexCoordDetail = lerp(vTexCoordDetail1, vTexCoordDetail2, domain.y);
    
//	float fTexCellSpace = -(1.0f / 36*64);
    positionW.y = gtexHeightMap.SampleLevel(gSSClamp, float2(vTexCoordBase.x, vTexCoordBase.y), 0).r;


	//float fTexCellSpaceU = 1 / 256.0f;
	//float fTexCellSpaceV = 1 / 256.0f;
	//float fWorldCellSpace = 1;

	//float2 vLeftTex = vTexCoordBase + float2(-fTexCellSpaceU, 0.0f);
	//float2 vRightTex = vTexCoordBase + float2(fTexCellSpaceU, 0.0f);
	//float2 vBottomTex = vTexCoordBase + float2(0.0f, -fTexCellSpaceV);
	//float2 vTopTex = vTexCoordBase + float2(0.0f, fTexCellSpaceV);

	//float fLeftY = gtexHeightMap.SampleLevel(gSSWarp, vLeftTex, 0).r;
	//float fRightY = gtexHeightMap.SampleLevel(gSSWarp, vRightTex, 0).r;
	//float fBottomY = gtexHeightMap.SampleLevel(gSSWarp, vBottomTex, 0).r;
	//float fTopY = gtexHeightMap.SampleLevel(gSSWarp, vTopTex, 0).r;

	//float3 vTangent = normalize(float3(2.0f * fWorldCellSpace, fRightY - fLeftY, 0.0f));
	//float3 vBitangent = normalize(float3(0.0f, fBottomY - fTopY, -2.0f * fWorldCellSpace));
	//float3 vNormal = cross(vTangent, vBitangent);

	//float3x3 TBN = float3x3(vTangent, vBitangent, vNormal);
	//vNormal = gtexNormalMap.SampleLevel(gSSWarp, vTexCoordDetail, 0).xyz;
	//vNormal = (vNormal * 2) - 1;
	//vNormal = normalize(mul(vNormal, TBN));
	//float fDisplacement = gtexSpecular.SampleLevel(gSSWarp, vTexCoordDetail, 0).x * 0.1;

	DS_OUTPUT output;
//	output.positionW = positionW + fDisplacement*vNormal;
    output.positionW = positionW;
	output.position = mul(float4(output.positionW, 1), gmtxVP);
	output.texCoordBase = vTexCoordBase;
	output.texCoordDetail = vTexCoordDetail;
	return output;
}

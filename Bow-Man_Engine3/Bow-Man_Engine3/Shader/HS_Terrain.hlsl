#include "Terrain.hlsli"

bool IsAabbBehindPlane(float3 center, float3 extents, float4 plane)
{
	float3 n = abs(plane.xyz);
	float r = dot(extents, n);
	float s = dot(float4(center, 1.0f), plane);

	//평면 뒤에 있다는 것은 카메라 프러스텀에 보인다는 것.
	return (s - r) < 0.0f;
}

bool ISAabbInsideFrustum(float3 center, float3 extents, float4 frustumPlanes[6])
{
    for (int i = 0; i < 6; ++i)
    {
        if (!IsAabbBehindPlane(center, extents, frustumPlanes[i]))
            return false;
    }
    return true;
}

float CalculateTessFactor(InputPatch<VS_OUTPUT, 4> patch, uint patchID : SV_PrimitiveID, float3 position)
{
    float minY = 0;
    float maxX = 100000;
    float3 vMin = float3(patch[0].positionW.x, minY, patch[0].positionW.z);
    float3 vMax = float3(patch[3].positionW.x, maxX, patch[3].positionW.z);

    float3 boxCenter = 0.5f * (vMin + vMax);
    float3 boxExtents = 0.5f * (vMax - vMin);
    if (ISAabbInsideFrustum(boxCenter, boxExtents, garrFrustum))
    {
        float fDistToCamera = distance(position, gvCameraPosition.xyz);
        float fMaxDistance = 350;
        float fThreshold = 200;

        float fFactor = 1 - saturate((fDistToCamera - fThreshold) / (fMaxDistance));
        return lerp(1, 64.0f, fFactor);
    }
	else
        return 0;
}

HS_CONSTANT_DATA_OUTPUT CalcHSPatchConstants(InputPatch<VS_OUTPUT, 4> input, uint nPatchID : SV_PrimitiveID)
{
    HS_CONSTANT_DATA_OUTPUT output;

    float3 edge0 = 0.5f * (input[0].positionW + input[2].positionW);
    float3 edge1 = 0.5f * (input[0].positionW + input[1].positionW);
    float3 edge2 = 0.5f * (input[1].positionW + input[3].positionW);
    float3 edge3 = 0.5f * (input[2].positionW + input[3].positionW);
    float3 center = 0.25f * (input[0].positionW + input[1].positionW + input[2].positionW + input[3].positionW);

    output.EdgeTessFactor[0] = CalculateTessFactor(input, nPatchID, edge0);
    output.EdgeTessFactor[1] = CalculateTessFactor(input, nPatchID, edge1);
    output.EdgeTessFactor[2] = CalculateTessFactor(input, nPatchID, edge2);
    output.EdgeTessFactor[3] = CalculateTessFactor(input, nPatchID, edge3);
    output.InsideTessFactor[0] = CalculateTessFactor(input, nPatchID, center);
    output.InsideTessFactor[1] = output.InsideTessFactor[0];
    return output;
}

//fxc /E main /T hs_5_0 /Od /Zi /Fo CompiledGs.fxo HS_Terrain.hlsl
[domain("quad")]
[partitioning("fractional_even")]
[outputtopology("triangle_ccw")]
[outputcontrolpoints(NUM_CONTROL_POINTS)]
[patchconstantfunc("CalcHSPatchConstants")]
[maxtessfactor(64.0f)]
HS_OUTPUT main(InputPatch<VS_OUTPUT, NUM_CONTROL_POINTS> input, uint i : SV_OutputControlPointID, uint PatchID : SV_PrimitiveID)
{
    HS_OUTPUT output;
    output.positionW = input[i].positionW;
    output.texCoordBase = input[i].texCoordBase;
    output.texCoordDetail = input[i].texCoordDetail;
    return output;
}

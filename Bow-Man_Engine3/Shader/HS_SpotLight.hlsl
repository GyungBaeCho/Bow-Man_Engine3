#include "SpotLight.hlsli"

//fxc /E CalcHSPatchConstants /T hs_5_0 /Od /Zi /Fo CompiledGs.fxo PointLightInstance.fx
HS_CONSTANT_DATA_OUTPUT CalcHSPatchConstants()
{
    HS_CONSTANT_DATA_OUTPUT Output;

    float tessFactor = 12.0;
    Output.Edges[0] = Output.Edges[1] = Output.Edges[2] = Output.Edges[3] = tessFactor;
    Output.Inside[0] = Output.Inside[1] = tessFactor;
    return Output;
}

//fxc /E PointLightHS /T hs_5_0 /Od /Zi /Fo CompiledGs.fxo PointLight.fx
[domain("quad")]
[partitioning("integer")]
[outputtopology("triangle_ccw")]
[outputcontrolpoints(4)]
[patchconstantfunc("CalcHSPatchConstants")]
HS_OUT main(InputPatch<VS_OUT, 1> input, uint index : SV_OutputControlPointID, uint PatchID : SV_PrimitiveID)
{
    HS_OUT output;
    output.HemiDir = HemiDir[PatchID];
    output.uInstanceID = input[index].uInstanceID;
    return output;
}

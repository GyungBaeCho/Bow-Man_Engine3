#include "PointLight.hlsli"

//fxc /E PointLightPS /T ps_5_0 /Od /Zi /Fo CompiledGs.fxo PointLight.fx
float4 main(DS_OUT input) : SV_TARGET
{
    SurfaceElement surface = UnpackSurfaceElement(input.cpPos, input.position.xy);
    return CalDeferredPointLight(surface, input.uInstanceID);
}

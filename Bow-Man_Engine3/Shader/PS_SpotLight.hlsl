#include "SpotLight.hlsli"

float4 main(DS_OUT input) : SV_TARGET
{
    SurfaceElement surface = UnpackSurfaceElement(input.cpPos, input.position.xy);
	return float4(CalDeferredSpotLight(surface, input.uInstanceID), 1);
}
#include "Bloom.hlsli"

[numthreads(1024, 1, 1)]
void main(int3 svDispatchThreadID : SV_DispatchTHreadID)
{
    uint2 CurPixel = uint2(svDispatchThreadID.x % gRes.x, svDispatchThreadID.x / gRes.x);

    if (CurPixel.y < gRes.y)
    {
        float4 vColor = gtxtHdrDownScale.Load(int3(CurPixel, 0));
        float Lum = dot(vColor, LUM_FACTOR);
        float avgLum = gbuffAverageLum[0];

        float fColorScale = saturate(Lum - avgLum * gfBloomThreshold);
        grwBloom[CurPixel.xy] = vColor * fColorScale;
    }
}





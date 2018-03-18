#include "Bloom.hlsli"

[numthreads(GROUP_THREAD, 1, 1)]
void main(uint3 svGroupId : SV_GroupID, uint svGroupIndex : SV_GroupIndex)
{
    int2 coord = int2(svGroupId.x, svGroupIndex - KERNEL_HALF + (GROUP_THREAD - KERNEL_HALF * 2) * svGroupId.y);
    coord = clamp(coord, int2(0, 0), int2(gRes.x - 1, gRes.y - 1));

    SharedInput[svGroupIndex] = gtxtInput.Load(int3(coord, 0));

    GroupMemoryBarrierWithGroupSync();

    if (
		svGroupIndex >= KERNEL_HALF //
		&& svGroupIndex < (GROUP_THREAD - KERNEL_HALF) //
		&& (svGroupIndex - KERNEL_HALF + (GROUP_THREAD - KERNEL_HALF * 2) * svGroupId.y < gRes.y) //계산쉐이더에서 잡은 픽셀 좌표가 텍스쳐를 벗어나지 않도록
	)
    {
        float4 vColor = 0;
		[unroll]
        for (int i = -KERNEL_HALF; i <= KERNEL_HALF; ++i)
        {
            vColor += SharedInput[svGroupIndex + i] * SampleWeights[i + KERNEL_HALF];
        }
        grwOutput[coord] = float4(vColor.rgb, 1.0f);
    }
}
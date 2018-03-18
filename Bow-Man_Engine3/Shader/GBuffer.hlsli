#ifndef __GBUFFER__
	#define __GBUFFER__

//-----------------------------------------------------------------------
//여러 렌더 타겟에 렌더링 하기 위한 구조체
struct G_BUFFER
{
	float4 color	: SV_TARGET0;
	float4 normal	: SV_TARGET1;
	float4 position : SV_TARGET2;
};

#endif
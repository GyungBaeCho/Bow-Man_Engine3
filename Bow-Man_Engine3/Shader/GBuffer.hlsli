#ifndef __GBUFFER__
	#define __GBUFFER__

//-----------------------------------------------------------------------
//���� ���� Ÿ�ٿ� ������ �ϱ� ���� ����ü
struct G_BUFFER
{
	float4 color	: SV_TARGET0;
	float4 normal	: SV_TARGET1;
	float4 position : SV_TARGET2;
};

#endif
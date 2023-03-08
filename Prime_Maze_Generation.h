#pragma once

struct Prime_Point
{
	long x, y;
};

//��ȡ��ǰ����Ϣ����
typedef void (*SearchCallBack)
(
	const Prime_Point &stCurrent,
	void *UserDefineValue
);

//�������������
typedef size_t(*Random)
(
	size_t szMin,//��������
	size_t szMax,//����������
	void *UserDefineValue
);

//Ĭ�����������
size_t DefaultRandom
(
	size_t szMin,
	size_t szMax,
	void *UserDefineValue
);

long Prime//�ɹ�����1������-1Ϊ��������
(
	bool *bpMap,//��ͼ
	long lMapWide,//��ͼ�������Ϊ����
	long lMapHigh,

	const Prime_Point &stBegPoint,//����յ����Ϊ��������
	const Prime_Point &stEndPoint,//��ʹ�ô˲������ǻ���м��

	bool bOptimizeStrategy,//�Ƿ�ʹ���Ż�����

	Random pfuncRandom = DefaultRandom,//���������
	void *pRandomValue = nullptr,//�û��Զ������

	SearchCallBack pfuncCurrentSearch = nullptr,//��ȡ��ǰ�򶴵ĵ�
	void *pCurrentSearchValue = nullptr//�û��Զ��������CurrentSearch����ʹ�ã�)//֮���һ���Ƿ�����б�ߴ�,��һ���������,��һ����ȡ��ǰ״̬�ĺ���
);
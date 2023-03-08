#include "Prime_Maze_Generation.h"
#include <vector>

struct Wall_Point
{
	long lWallX, lWallY;//ǽ����
	long lRearX, lRearY;//ǽ��������
};

void RemoveWall(std::vector<Wall_Point> &stWallList, const Wall_Point &stNewWall, size_t *szpVctMap, long lVctMapWide, long lVctMapHigh);
void RemoveWall(std::vector<Wall_Point> &stWallList, size_t szRemoveWallIndex, size_t *szpVctMap, long lVctMapWide, long lVctMapHigh);

long Prime(//�ɹ�����1������-1Ϊ��������
	bool *bpMap,//��ͼ
	long lMapWide,//��ͼ�������Ϊ����
	long lMapHigh,

	const Prime_Point &stBegPoint,//����յ����Ϊ��������
	const Prime_Point &stEndPoint,//��ʹ�ô˲������ǻ���м��

	bool bOptimizeStrategy,//�Ƿ�ʹ���Ż�����

	Random pfuncRandom,//���������
	void *pRandomValue,//�û��Զ������

	SearchCallBack pfuncCurrentSearch,//��ȡ��ǰ�򶴵ĵ�
	void *pCurrentSearchValue)//�û��Զ��������CurrentSearch����ʹ�ã�)//֮���һ���Ƿ�����б�ߴ�,��һ���������,��һ����ȡ��ǰ״̬�ĺ���
{
	if (bpMap == nullptr)
	{
		return -1;
	}

	if (lMapWide % 2 == 0 || lMapHigh % 2 == 0 ||
		stBegPoint.x % 2 == 0 || stBegPoint.y % 2 == 0 ||
		stEndPoint.x % 2 == 0 || stEndPoint.y % 2 == 0)
	{
		return -1;//��������
	}

	if (stBegPoint.x <= 0 || stBegPoint.x >= lMapWide - 1 ||
		stBegPoint.y <= 0 || stBegPoint.y >= lMapHigh - 1 ||
		stEndPoint.x <= 0 || stEndPoint.x >= lMapWide - 1 ||
		stEndPoint.y <= 0 || stEndPoint.y >= lMapWide - 1)
	{
		return -1;//�����߽�
	}

	if (pfuncRandom == nullptr)
	{
		return -1;//����Ϊnullptr
	}

	//��������ͼ���ó�ǽ
	std::fill<bool *, bool>(bpMap, bpMap + lMapWide * lMapHigh, 1);

	//ӳ����Ż����ԣ�
	size_t *szpVctMap = nullptr;
	long lVctMapWide = 0;
	long lVctMapHigh = 0;
	if (bOptimizeStrategy)
	{
		//����ӳ����С
		lVctMapWide = lMapWide;
		lVctMapHigh = lMapHigh;
		//ǽ��ǽ�б��е�ӳ��
		szpVctMap = new(std::nothrow) size_t[lVctMapWide * lVctMapHigh];
		//�˴������飬������szpVctMap������м�飬���Ϊnullptrֻ�����������1.�û���ʹ���Ż����ԣ�2.����ʧ�ܣ��͵����û���ʹ���Ż����ԣ�
	}

	//ǽ�б�
	std::vector<Wall_Point> stWallList;
	//���õ�ǰ��Ϊ���
	Prime_Point stCurrent = stBegPoint;
	//����
	bpMap[stCurrent.y * lMapWide + stCurrent.x] = 0;
	//����״̬����
	if (pfuncCurrentSearch != nullptr)
	{
		pfuncCurrentSearch(stCurrent, pCurrentSearchValue);
	}
	
	const struct
	{
		long lXconver;//x�任
		long lYconver;//y�任
	}stConver[8] =
	{
		{ 0,-1 },	//��
		{ 0, 1 },	//��
		{-1, 0 },	//��
		{ 1, 0 },	//��
	};

	while (true)
	{
		//�ѵ�ǰ�㸽����ǽ�����ǽ�б�
		for (long i = 0; i < 4; ++i)
		{
			//�ӵ�ǰ�㹹��&�任
			Wall_Point stNewWall;

			//���쵱ǰ�㣬����ǽ����
			stNewWall.lWallX = stCurrent.x + stConver[i].lXconver;
			stNewWall.lWallY = stCurrent.y + stConver[i].lYconver;

			//Խ���ж�
			if (stNewWall.lWallX < 0 || stNewWall.lWallX >= lMapWide ||
				stNewWall.lWallY < 0 || stNewWall.lWallY >= lMapHigh)
			{
				continue;//������Ҫ��
			}

			//�������ǽ�Ѿ���·������
			if (bpMap[stNewWall.lWallY * lMapWide + stNewWall.lWallX] == 0)
			{
				continue;
			}

			//����ǽ��������
			stNewWall.lRearX = stNewWall.lWallX + stConver[i].lXconver;
			stNewWall.lRearY = stNewWall.lWallY + stConver[i].lYconver;

			//Խ���ж�
			if (stNewWall.lRearX < 0 || stNewWall.lRearX >= lMapWide ||
				stNewWall.lRearY < 0 || stNewWall.lRearY >= lMapHigh)//����
			{
				continue;//������Ҫ��
			}

			//����ǽ�����ǲ���·�����������һ��ѭ��
			if (bpMap[stNewWall.lRearY * lMapWide + stNewWall.lRearX] == 0)
			{
				//�Ż�����(���������ϵ�ǽֱ��ɾ��)
				if (szpVctMap != nullptr)
				{
					RemoveWall(stWallList, stNewWall, szpVctMap, lVctMapWide, lVctMapHigh);
				}

				continue;//������Ҫ��
			}

			//�µ�ǽ�����б�
			stWallList.push_back(stNewWall);
			if (szpVctMap != nullptr)
			{
				szpVctMap[stNewWall.lWallY * lVctMapWide + stNewWall.lWallX] = stWallList.size() - 1;//����wall��x��y�����µ�ǽ�б�����
			}
		}

		//û�ж����ǽ�ˣ��㷨����
		if (stWallList.empty())
		{
			return 1;//����
		}

		//�����ǽ��
		size_t szRand = pfuncRandom(0, stWallList.size(), pRandomValue);
		Wall_Point *stRandWall = &stWallList[szRand];

		if (szpVctMap == nullptr)//ûʹ���Ż�����
		{
			while (bpMap[stRandWall->lRearY * lMapWide + stRandWall->lRearX] == 0)//����ǽ�����ǲ���·���������������ǽ
			{
				//ɾ��ǽ
				RemoveWall(stWallList, szRand, szpVctMap, lVctMapWide, lVctMapHigh);

				if (stWallList.empty())//�㷨����
				{
					return 1;//����
				}

				//�����һ��
				szRand = pfuncRandom(0, stWallList.size(), pRandomValue);
				stRandWall = &stWallList[szRand];
			}
		}
		//����ʹ���Ż��������ڱ����ɴ�ǽ�Ĺ��̾��Ѿ�ɾ�����Ϸ���ǽ��

		//��
		bpMap[stRandWall->lRearY * lMapWide + stRandWall->lRearX] = 0;
		bpMap[stRandWall->lWallY * lMapWide + stRandWall->lWallX] = 0;

		//���õ�ǰ��
		stCurrent.x = stRandWall->lRearX;
		stCurrent.y = stRandWall->lRearY;

		//����״̬����
		if (pfuncCurrentSearch != nullptr)
		{
			pfuncCurrentSearch({stRandWall->lWallX,stRandWall->lWallY}, pCurrentSearchValue);
			pfuncCurrentSearch({stRandWall->lRearX,stRandWall->lRearY}, pCurrentSearchValue);
		}

		//�򶴽�����������ǽɾ��
		RemoveWall(stWallList, szRand, szpVctMap, lVctMapWide, lVctMapHigh);

		//static long i = 0;
		//printf("%ld ", i++);
	}

	//�ͷ�
	if (szpVctMap != nullptr)
	{
		delete[] szpVctMap;
	}

	return 1;
}

void RemoveWall(std::vector<Wall_Point> &stWallList, const Wall_Point &stNewWall, size_t *szpVctMap, long lVctMapWide, long lVctMapHigh)
{
	if (szpVctMap == nullptr)
	{
		return;
	}

	//ɾ�������Ѿ���·��ǽ
	//������ǽɾ�ˣ�����β��Ԫ�ؽ���Ȼ��pop_back������ͬʱ����ӳ���
	size_t szRemoveWallIndex = szpVctMap[stNewWall.lWallY * lVctMapWide + stNewWall.lWallX];
	size_t szLastWallIndex = stWallList.size() - 1;

	//���Ҫɾ���Ĳ������һ��Ԫ��
	if (szRemoveWallIndex != szLastWallIndex)
	{
		//�������һ��ǽ������Ϊ��ǰҪɾ��ǽ������
		Prime_Point stLastWall =
		{
			stWallList[szLastWallIndex].lWallX,
			stWallList[szLastWallIndex].lWallY,
		};

		szpVctMap[stLastWall.y * lVctMapWide + stLastWall.x] = szRemoveWallIndex;
		//����
		std::swap(stWallList[szRemoveWallIndex], stWallList[szLastWallIndex]);
	}
	//ɾ��
	stWallList.pop_back();
}

void RemoveWall(std::vector<Wall_Point> &stWallList, size_t szRemoveWallIndex, size_t *szpVctMap, long lVctMapWide, long lVctMapHigh)
{
	//������Ԫ������
	size_t szLastWallIndex = stWallList.size() - 1;
	//�������һ��Ԫ���򽻻�
	if (szRemoveWallIndex != szLastWallIndex)
	{
		if (szpVctMap != nullptr)
		{
			//�������һ��ǽ������Ϊ��ǰҪɾ��ǽ������
			Prime_Point stLastWall =
			{
				stWallList[szLastWallIndex].lWallX,
				stWallList[szLastWallIndex].lWallY,
			};

			szpVctMap[stLastWall.y * lVctMapWide + stLastWall.x] = szRemoveWallIndex;
		}
		//����
		std::swap(stWallList[szRemoveWallIndex], stWallList[szLastWallIndex]);
	}
	//ɾ��
	stWallList.pop_back();
}


//Ĭ�����������
size_t DefaultRandom(
	size_t szMin,
	size_t szMax,
	void *UserDefineValue)
{
	return (rand() % szMax) + szMin;
}
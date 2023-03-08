#include "Prime_Maze_Generation.h"
#include <vector>

struct Wall_Point
{
	long lWallX, lWallY;//墙坐标
	long lRearX, lRearY;//墙后面坐标
};

void RemoveWall(std::vector<Wall_Point> &stWallList, const Wall_Point &stNewWall, size_t *szpVctMap, long lVctMapWide, long lVctMapHigh);
void RemoveWall(std::vector<Wall_Point> &stWallList, size_t szRemoveWallIndex, size_t *szpVctMap, long lVctMapWide, long lVctMapHigh);

long Prime(//成功返回1，返回-1为参数错误
	bool *bpMap,//地图
	long lMapWide,//地图长宽必须为奇数
	long lMapHigh,

	const Prime_Point &stBegPoint,//起点终点必须为奇数坐标
	const Prime_Point &stEndPoint,//不使用此参数但是会进行检查

	bool bOptimizeStrategy,//是否使用优化策略

	Random pfuncRandom,//生成随机数
	void *pRandomValue,//用户自定义参数

	SearchCallBack pfuncCurrentSearch,//获取当前打洞的点
	void *pCurrentSearchValue)//用户自定义参数（CurrentSearch函数使用）)//之后加一个是否允许斜线打洞,加一个随机函数,加一个获取当前状态的函数
{
	if (bpMap == nullptr)
	{
		return -1;
	}

	if (lMapWide % 2 == 0 || lMapHigh % 2 == 0 ||
		stBegPoint.x % 2 == 0 || stBegPoint.y % 2 == 0 ||
		stEndPoint.x % 2 == 0 || stEndPoint.y % 2 == 0)
	{
		return -1;//不是奇数
	}

	if (stBegPoint.x <= 0 || stBegPoint.x >= lMapWide - 1 ||
		stBegPoint.y <= 0 || stBegPoint.y >= lMapHigh - 1 ||
		stEndPoint.x <= 0 || stEndPoint.x >= lMapWide - 1 ||
		stEndPoint.y <= 0 || stEndPoint.y >= lMapWide - 1)
	{
		return -1;//超出边界
	}

	if (pfuncRandom == nullptr)
	{
		return -1;//函数为nullptr
	}

	//把整个地图设置成墙
	std::fill<bool *, bool>(bpMap, bpMap + lMapWide * lMapHigh, 1);

	//映射表（优化策略）
	size_t *szpVctMap = nullptr;
	long lVctMapWide = 0;
	long lVctMapHigh = 0;
	if (bOptimizeStrategy)
	{
		//计算映射表大小
		lVctMapWide = lMapWide;
		lVctMapHigh = lMapHigh;
		//墙在墙列表中的映射
		szpVctMap = new(std::nothrow) size_t[lVctMapWide * lVctMapHigh];
		//此处无需检查，后续对szpVctMap都会进行检查，如果为nullptr只有两种情况：1.用户不使用优化策略，2.分配失败（就当成用户不使用优化策略）
	}

	//墙列表
	std::vector<Wall_Point> stWallList;
	//设置当前点为起点
	Prime_Point stCurrent = stBegPoint;
	//起点打洞
	bpMap[stCurrent.y * lMapWide + stCurrent.x] = 0;
	//调用状态函数
	if (pfuncCurrentSearch != nullptr)
	{
		pfuncCurrentSearch(stCurrent, pCurrentSearchValue);
	}
	
	const struct
	{
		long lXconver;//x变换
		long lYconver;//y变换
	}stConver[8] =
	{
		{ 0,-1 },	//上
		{ 0, 1 },	//下
		{-1, 0 },	//左
		{ 1, 0 },	//右
	};

	while (true)
	{
		//把当前点附近的墙添加入墙列表
		for (long i = 0; i < 4; ++i)
		{
			//从当前点构造&变换
			Wall_Point stNewWall;

			//构造当前点，计算墙坐标
			stNewWall.lWallX = stCurrent.x + stConver[i].lXconver;
			stNewWall.lWallY = stCurrent.y + stConver[i].lYconver;

			//越界判断
			if (stNewWall.lWallX < 0 || stNewWall.lWallX >= lMapWide ||
				stNewWall.lWallY < 0 || stNewWall.lWallY >= lMapHigh)
			{
				continue;//不符合要求
			}

			//如果这面墙已经是路则跳过
			if (bpMap[stNewWall.lWallY * lMapWide + stNewWall.lWallX] == 0)
			{
				continue;
			}

			//计算墙背后坐标
			stNewWall.lRearX = stNewWall.lWallX + stConver[i].lXconver;
			stNewWall.lRearY = stNewWall.lWallY + stConver[i].lYconver;

			//越界判断
			if (stNewWall.lRearX < 0 || stNewWall.lRearX >= lMapWide ||
				stNewWall.lRearY < 0 || stNewWall.lRearY >= lMapHigh)//超界
			{
				continue;//不符合要求
			}

			//测试墙背后是不是路，如果是则下一轮循环
			if (bpMap[stNewWall.lRearY * lMapWide + stNewWall.lRearX] == 0)
			{
				//优化策略(遇到不符合的墙直接删除)
				if (szpVctMap != nullptr)
				{
					RemoveWall(stWallList, stNewWall, szpVctMap, lVctMapWide, lVctMapHigh);
				}

				continue;//不符合要求
			}

			//新的墙加入列表
			stWallList.push_back(stNewWall);
			if (szpVctMap != nullptr)
			{
				szpVctMap[stNewWall.lWallY * lVctMapWide + stNewWall.lWallX] = stWallList.size() - 1;//设置wall在x、y坐标下的墙列表索引
			}
		}

		//没有多余的墙了，算法结束
		if (stWallList.empty())
		{
			return 1;//结束
		}

		//随机挑墙打洞
		size_t szRand = pfuncRandom(0, stWallList.size(), pRandomValue);
		Wall_Point *stRandWall = &stWallList[szRand];

		if (szpVctMap == nullptr)//没使用优化策略
		{
			while (bpMap[stRandWall->lRearY * lMapWide + stRandWall->lRearX] == 0)//测试墙背后是不是路，如果是则重新挑墙
			{
				//删除墙
				RemoveWall(stWallList, szRand, szpVctMap, lVctMapWide, lVctMapHigh);

				if (stWallList.empty())//算法结束
				{
					return 1;//结束
				}

				//随机下一个
				szRand = pfuncRandom(0, stWallList.size(), pRandomValue);
				stRandWall = &stWallList[szRand];
			}
		}
		//否则使用优化策略则在遍历可打洞墙的过程就已经删除不合法的墙了

		//打洞
		bpMap[stRandWall->lRearY * lMapWide + stRandWall->lRearX] = 0;
		bpMap[stRandWall->lWallY * lMapWide + stRandWall->lWallX] = 0;

		//设置当前点
		stCurrent.x = stRandWall->lRearX;
		stCurrent.y = stRandWall->lRearY;

		//调用状态函数
		if (pfuncCurrentSearch != nullptr)
		{
			pfuncCurrentSearch({stRandWall->lWallX,stRandWall->lWallY}, pCurrentSearchValue);
			pfuncCurrentSearch({stRandWall->lRearX,stRandWall->lRearY}, pCurrentSearchValue);
		}

		//打洞结束，把这面墙删了
		RemoveWall(stWallList, szRand, szpVctMap, lVctMapWide, lVctMapHigh);

		//static long i = 0;
		//printf("%ld ", i++);
	}

	//释放
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

	//删除后面已经是路的墙
	//把这面墙删了：和最尾部元素交换然后pop_back弹出，同时设置映射表
	size_t szRemoveWallIndex = szpVctMap[stNewWall.lWallY * lVctMapWide + stNewWall.lWallX];
	size_t szLastWallIndex = stWallList.size() - 1;

	//如果要删除的不是最后一个元素
	if (szRemoveWallIndex != szLastWallIndex)
	{
		//设置最后一个墙的索引为当前要删除墙的索引
		Prime_Point stLastWall =
		{
			stWallList[szLastWallIndex].lWallX,
			stWallList[szLastWallIndex].lWallY,
		};

		szpVctMap[stLastWall.y * lVctMapWide + stLastWall.x] = szRemoveWallIndex;
		//交换
		std::swap(stWallList[szRemoveWallIndex], stWallList[szLastWallIndex]);
	}
	//删除
	stWallList.pop_back();
}

void RemoveWall(std::vector<Wall_Point> &stWallList, size_t szRemoveWallIndex, size_t *szpVctMap, long lVctMapWide, long lVctMapHigh)
{
	//求出最后元素索引
	size_t szLastWallIndex = stWallList.size() - 1;
	//不是最后一个元素则交换
	if (szRemoveWallIndex != szLastWallIndex)
	{
		if (szpVctMap != nullptr)
		{
			//设置最后一个墙的索引为当前要删除墙的索引
			Prime_Point stLastWall =
			{
				stWallList[szLastWallIndex].lWallX,
				stWallList[szLastWallIndex].lWallY,
			};

			szpVctMap[stLastWall.y * lVctMapWide + stLastWall.x] = szRemoveWallIndex;
		}
		//交换
		std::swap(stWallList[szRemoveWallIndex], stWallList[szLastWallIndex]);
	}
	//删除
	stWallList.pop_back();
}


//默认随机数函数
size_t DefaultRandom(
	size_t szMin,
	size_t szMax,
	void *UserDefineValue)
{
	return (rand() % szMax) + szMin;
}
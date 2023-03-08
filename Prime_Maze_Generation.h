#pragma once

struct Prime_Point
{
	long x, y;
};

//获取当前点信息函数
typedef void (*SearchCallBack)
(
	const Prime_Point &stCurrent,
	void *UserDefineValue
);

//生成随机数函数
typedef size_t(*Random)
(
	size_t szMin,//包含自身
	size_t szMax,//不包含自身
	void *UserDefineValue
);

//默认随机数函数
size_t DefaultRandom
(
	size_t szMin,
	size_t szMax,
	void *UserDefineValue
);

long Prime//成功返回1，返回-1为参数错误
(
	bool *bpMap,//地图
	long lMapWide,//地图长宽必须为奇数
	long lMapHigh,

	const Prime_Point &stBegPoint,//起点终点必须为奇数坐标
	const Prime_Point &stEndPoint,//不使用此参数但是会进行检查

	bool bOptimizeStrategy,//是否使用优化策略

	Random pfuncRandom = DefaultRandom,//生成随机数
	void *pRandomValue = nullptr,//用户自定义参数

	SearchCallBack pfuncCurrentSearch = nullptr,//获取当前打洞的点
	void *pCurrentSearchValue = nullptr//用户自定义参数（CurrentSearch函数使用）)//之后加一个是否允许斜线打洞,加一个随机函数,加一个获取当前状态的函数
);
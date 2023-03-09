#include <chrono>
#include <stdio.h>
#include <thread>

#include <Windows.h>
#include <Bcrypt.h>
#pragma comment(lib,"bcrypt.lib")

#define MAPX 20001
#define MAPY 20001

#include "Prime_Maze_Generation.h"

size_t RandomFunc(
	size_t szMin,
	size_t szMax,
	void *UserDefineValue)
{
	size_t szRand;
	BCryptGenRandom(*((BCRYPT_ALG_HANDLE *)UserDefineValue), (PUCHAR)&szRand, sizeof(szRand), 0);
	return (szRand % szMax) + szMin;
}

int main(void)
{
	//srand(1);
	BCRYPT_ALG_HANDLE stBAH;
	BCryptOpenAlgorithmProvider(&stBAH, L"3DES", nullptr, 0);

	std::thread thread1(
		[&]()
		{
			bool *bpMap = new(std::nothrow) bool[MAPX * MAPY];
			if (bpMap == nullptr)
			{
				printf("Out of memory!\n");
				return;
			}
			{
				auto start_time = std::chrono::steady_clock::now();

				long ret = Prime(bpMap, MAPX, MAPY, Prime_Point{1,1}, Prime_Point{1,1}, true, RandomFunc, &stBAH);

				auto end_time = std::chrono::steady_clock::now();
				auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count() / 1000.00;

				printf("use:%lfs,OptimizeStrategy:true,return:%ld\n", duration, ret);
			}
			delete[] bpMap;
		});
	
	std::thread thread2(
		[&]()
		{
			bool *bpMap = new(std::nothrow) bool[MAPX * MAPY];
			if (bpMap == nullptr)
			{
				printf("Out of memory!\n");
				return;
			}
			{
				auto start_time = std::chrono::steady_clock::now();

				long ret = Prime(bpMap, MAPX, MAPY, Prime_Point{1,1}, Prime_Point{1,1}, false, RandomFunc, &stBAH);

				auto end_time = std::chrono::steady_clock::now();
				auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count() / 1000.00;

				printf("use:%lfs,OptimizeStrategy:false,return:%ld\n", duration, ret);
			}
			delete[] bpMap;
		});
	
	thread1.detach();
	thread2.join();

	/*
	for (long i = 0; i < MAPX * MAPY; ++i)
	{
		if (i % MAPX == 0)
		{
			putchar('\n');
		}
		//printf("%s", bpMap[i] == 1 ? "¡ö" : "  ");
		printf("%s", bpMap[i] == 1 ? "1," : "0,");
	}
	*/

	BCryptCloseAlgorithmProvider(stBAH, 0);
	return 0;
}
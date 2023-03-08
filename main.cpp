#include <chrono>
#include <stdio.h>
#include <thread>

#include <Windows.h>
#include <Bcrypt.h>
#pragma comment(lib,"bcrypt.lib")

#define MAPX 27981
#define MAPY 27981

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
	
	

	srand(1);
	BCRYPT_ALG_HANDLE stBAH;
	BCryptOpenAlgorithmProvider(&stBAH, L"3DES", nullptr, 0);

	std::thread thread1(
		[&]()
		{
			bool *bpMap = new(std::nothrow) bool[MAPX * MAPY];
			if (bpMap == nullptr)
			{
				return -2;
			}
			{
				auto start_time = std::chrono::steady_clock::now();

				long ret = Prime(bpMap, MAPX, MAPY, Prime_Point{1,1}, Prime_Point{1,1}, true);

				auto end_time = std::chrono::steady_clock::now();
				auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count() / 1000.00;

				printf("return:%ld,use:%lfs\n", ret, duration);
			}
			delete[] bpMap;
		});
	
	std::thread thread2(
		[&]()
		{
			bool *bpMap = new(std::nothrow) bool[MAPX * MAPY];
			if (bpMap == nullptr)
			{
				return -2;
			}
			{
				auto start_time = std::chrono::steady_clock::now();

				long ret = Prime(bpMap, MAPX, MAPY, Prime_Point{1,1}, Prime_Point{1,1}, false);

				auto end_time = std::chrono::steady_clock::now();
				auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count() / 1000.00;

				printf("return:%ld,use:%lfs\n", ret, duration);
			}
			delete[] bpMap;
		});
	
	thread1.detach();
	thread2.join();

	//for (long i = 0; i < MAPX * MAPY; ++i)
	//{
	//	if (i % MAPX == 0)
	//	{
	//		putchar('\n');
	//	}
	//	//printf("%s", bpMap[i] == 1 ? "¡ö" : "  ");
	//	printf("%s", bpMap[i] == 1 ? "1," : "0,");
	//}


	BCryptCloseAlgorithmProvider(stBAH, 0);

	return 0;
}



//#include <chrono>
//#include <cstring>
//#include <iostream>
//#include <algorithm>
//
//#define BLOCK_SIZE 8
//
//void manual_fill(char *ptr, size_t size)
//{
//	for (char *i = ptr; i < ptr + size; i += BLOCK_SIZE)
//	{
//		//std::memset(i, 0xff, BLOCK_SIZE);
//		*((char *)(i + (BLOCK_SIZE - 1))) = 0x01;
//	}
//}
//
//void test_manual_fill(char *ptr, size_t size)
//{
//	auto start_time = std::chrono::steady_clock::now();
//	manual_fill(ptr, size);
//	auto end_time = std::chrono::steady_clock::now();
//	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count();
//	std::cout << "Manual fill: " << duration << " us\n";
//}
//
//void std_fill(char *ptr, size_t size)
//{
//	std::fill(ptr, ptr + size, 0x01);
//}
//
//void test_std_fill(char *ptr, size_t size)
//{
//	auto start_time = std::chrono::steady_clock::now();
//	std_fill(ptr, size);
//	auto end_time = std::chrono::steady_clock::now();
//	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count();
//	std::cout << "std::fill: " << duration << " us\n";
//}
//
//int main()
//{
//	const size_t size = 1024 * 1024 * 10; // 10 MB
//	char *mem = new char[size];
//
//	test_manual_fill(mem, size);
//	test_std_fill(mem, size);
//
//	delete[] mem;
//	return 0;
//}

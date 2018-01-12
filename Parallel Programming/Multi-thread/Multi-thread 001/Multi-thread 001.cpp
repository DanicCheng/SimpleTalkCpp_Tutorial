// Multi-thread 001.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Windows.h>
#include <conio.h>

const int N = 100000;

DWORD WINAPI my_thread_func(void* param) {
	for (int i = 0; i < N; i++) {
		printf("+");
	}

	return 0;
}

void my_example1() {
	CreateThread(nullptr, 0, &my_thread_func, nullptr, 0, nullptr);
	for (int i = 0; i < N; i++) {
		printf(".");
	}
}

int main()
{
	my_example1();


	printf("\n======= Program Ended ===========\n");
	_getch();
    return 0;
}


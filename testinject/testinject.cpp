// testinject.cpp: 定义控制台应用程序的入口点。
//
#define Unicode
#include "stdafx.h"

#include<iostream>
#include <windows.h>
using namespace std;
int main()
{
	LoadLibrary(L"kernel32.dll");
	MessageBox(NULL, L"Inject Success", L"Inject Success", MB_OKCANCEL);
	while (1);
	return 0;
}


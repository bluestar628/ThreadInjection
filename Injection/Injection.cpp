// 线程注入.cpp : 定义控制台应用程序的入口点。
//
#define UNICODE
#include "stdafx.h"
#include<stdio.h>
#include <windows.h>
#include<tlhelp32.h>
#include<iostream>
#include<string.h>
#include <cstring>
#include <comdef.h> 
using namespace std;

void GetLastErrorAsString()
{
	//Get the error message, if any.
	DWORD errorMessageID = ::GetLastError();
	cout <<"Error number:"<< errorMessageID << endl;
	if (errorMessageID == 0)
		return; //No error message has been recorded

	char* messageBuffer = nullptr;
	size_t size = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPWSTR)messageBuffer, 0, NULL);
	printf("Error:%s\n", messageBuffer);

	//Free the buffer.
	LocalFree(messageBuffer);
}
int EnableDebugPriv(const TCHAR* name)
{
	HANDLE hToken;
	TOKEN_PRIVILEGES tp;
	LUID luid;
	OpenProcessToken(GetCurrentProcess(),
		TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY,
		&hToken);
	LookupPrivilegeValue(NULL, name, &luid);
	tp.PrivilegeCount = 1;
	tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	tp.Privileges[0].Luid = luid;
	AdjustTokenPrivileges(hToken,0,&tp,sizeof(TOKEN_PRIVILEGES),NULL,NULL);
	return 0;
}
BOOL InjectDll(LPCTSTR DllFullPath, const DWORD dwRemoteProcessId)
{
	HANDLE hRemoteProcess;
	EnableDebugPriv(SE_DEBUG_NAME);
	hRemoteProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwRemoteProcessId);
	if (!hRemoteProcess)
	{
		cout << "打开远程进程失败" << endl;
		GetLastErrorAsString();
		return false;
	}
	LPTSTR pszLibFileRemote;
	pszLibFileRemote = (LPTSTR)VirtualAllocEx(hRemoteProcess, NULL,
		1000 + 1, MEM_COMMIT, PAGE_READWRITE);
	WriteProcessMemory(hRemoteProcess, pszLibFileRemote, (LPVOID)DllFullPath, lstrlen(DllFullPath) + 1, NULL);
	HMODULE kernelHandle = GetModuleHandle(TEXT("kernel32.dll"));
	if (!kernelHandle)
	{
		cout << "Get Kernel32 Handle Failed!" << endl;
		return false;
	}
	PTHREAD_START_ROUTINE pfnStartAddr =
		(PTHREAD_START_ROUTINE)GetProcAddress(kernelHandle, "LoadLibraryA");
	if (!pfnStartAddr)
	{
		cout << "Get LoadLibrary Address Failed" << endl;
	 GetLastErrorAsString();
		return false;
	}
	HANDLE hRemoteThread;
	if ((hRemoteThread = CreateRemoteThread(hRemoteProcess, NULL, 0, pfnStartAddr, (LPTSTR)pszLibFileRemote, 0, NULL))==NULL)
	{
		GetLastErrorAsString();
		return FALSE;
	}
	cout <<"Thread Id:"<<hex <<GetThreadId(hRemoteThread) << endl;
	DWORD code = -823;
	WaitForSingleObject(hRemoteThread, INFINITE);
	DWORD ExitCode;
	GetExitCodeThread(hRemoteThread, &ExitCode);
	cout << "Exit code:" <<ExitCode<< endl;
	CloseHandle(hRemoteProcess);
	CloseHandle(hRemoteThread);
	return TRUE;
}
DWORD GetProcessId(LPCTSTR pName)
{
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (INVALID_HANDLE_VALUE == hSnapshot) {
		return NULL;
	}
	PROCESSENTRY32 pe = { sizeof(pe) };
	for (BOOL ret = Process32First(hSnapshot, &pe); ret; ret = Process32Next(hSnapshot, &pe)) {

		//printf("%ws\n", pe.szExeFile);

		if (_tcscmp(pe.szExeFile, pName) == 0) {
			CloseHandle(hSnapshot);
			printf("%d\n", pe.th32ProcessID);
			return pe.th32ProcessID;
		}
		//printf("%-6d %s\n", pe.th32ProcessID, pe.szExeFile);
	}
	CloseHandle(hSnapshot);
	return 0;

}

int EnableDebugPrivilege(const LPTSTR name)
{
	HANDLE token;
	TOKEN_PRIVILEGES tp;
	//打开进程令牌环   
	if (!OpenProcessToken(GetCurrentProcess(),
		TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &token))
	{
		cout << "open process token error!\n";
		return 0;
	}
	//获得进程本地唯一ID   
	LUID luid;
	if (!LookupPrivilegeValue(NULL, name, &luid))
	{
		cout << "lookup privilege value error!\n";
		return 0;
	}
	tp.PrivilegeCount = 1;
	tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	tp.Privileges[0].Luid = luid;
	//调整进程权限   
	if (!AdjustTokenPrivileges(token, 0, &tp, sizeof(TOKEN_PRIVILEGES), NULL, NULL))
	{
		cout << "adjust token privilege error!\n";
		return 0;
	}
	return 1;
}
int main()
{	
	TCHAR myFILE[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, myFILE);
	_tcscat_s(myFILE,L"\\myfirstdll.dll");
	printf("%ws\n", myFILE);
	DWORD Pid=GetProcessId(L"testinject.exe");
	EnableDebugPrivilege(SE_DEBUG_NAME);
	if(Pid) InjectDll(L"D:\ProgramC++\ThreadInjection\Release\myfirstdll.dll", Pid);
	else cout << "Process Not Found" << endl;
	system("pause");
    return 0;
}


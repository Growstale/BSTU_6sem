#include "pch.h"
#include "DefineTableService.h"
#include "EchoServer.h"
#include <ctime>

static clock_t Cs = 0;  // Начальное время таймера

double GetTimeValue() {
	return static_cast<double>(clock() - Cs);
}


#define _CRT_SECURE_NO_WARNINGS

BEGIN_TABLESERVICE
	ENTRYSERVICE("echo", EchoServer),
	ENTRYSERVICE("time", TimeServer),
	ENTRYSERVICE("rand", RandServer),
	ENTRYSERVICE("sync", SynchroServer)
END_TABLESERVICE;

extern "C" __declspec(dllexport) HANDLE SSS(char* id, LPVOID prm)
{
	HANDLE rc = NULL;
	int  i = 0;
	while (i < SIZETS && strcmp(TABLESERVICE_ID(i), id) != 0) i++;
	if (i < SIZETS) {
		rc = CreateThread(NULL, NULL, TABLESERVICE_FN(i), prm, NULL, NULL);
	}
	return rc;
};

BOOL APIENTRY DllMain(HANDLE hinst, DWORD  rcall, LPVOID wres)
//			   функция обозначает точку входа в программный
//             модуль динамически подключаемой библиотеки,  
//             функции получает управление от операционной 
//             системы в момент загрузки.

{
	if (rcall == DLL_PROCESS_ATTACH) {
		Cs = clock();  // Запоминаем время загрузки DLL
	}
	return TRUE;
}



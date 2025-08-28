#include "ConcurrentServer.h"

DWORD WINAPI GarbageCleaner(LPVOID pPrm) {
	try {
        cout << "GarbageCleaner started\n";

		while (*((TalkersCommand*)pPrm) != EXIT) {
            EnterCriticalSection(&ListContactCriticalSection);
            if (!Contacts.empty()) {
                for (auto it = Contacts.begin(); it != Contacts.end();) {
                    if (it->sthread == Contact::FINISH || it->sthread == Contact::TIMEOUT || it->sthread == Contact::ABORT) {
                        if (!CancelWaitableTimer(it->htimer)) {
                            cout << SetErrorMsgText("CancelWaitableTimer failed", GetLastError()) << endl;
                            it->htimer = NULL;
                        }

                        closesocket(it->s);
                        if (it->hthread != NULL) {
                            if (!CloseHandle(it->hthread)) {
                                cout << SetErrorMsgText("CloseHandle failed: ", GetLastError()) << endl;
                            }
                        }
                        it = Contacts.erase(it);
                        InterlockedDecrement(&CurrentClients);
                    }
                    else {
                        ++it; 
                    }
                }
            }

            LeaveCriticalSection(&ListContactCriticalSection);
            Sleep(1000);
		}
	}
	catch (string errorMsgText)
	{
		cout << endl << errorMsgText;
	}
	DWORD rc = 0;
	ExitThread(rc);
}

#include "ConcurrentServer.h"

DWORD WINAPI ConsolePipe(LPVOID pPrm) {
	cout << "ConsolePipe started\n";
	HANDLE hPipe;
	try 
	{
		wchar_t name[512];
		swprintf_s(name, sizeof(name) / sizeof(wchar_t), L"\\\\.\\pipe\\%s", pipename.c_str());
		while (true) {

			hPipe = CreateNamedPipe(name, PIPE_ACCESS_DUPLEX, PIPE_TYPE_MESSAGE | PIPE_WAIT, 1, NULL, NULL, INFINITE, NULL);
			if (hPipe == INVALID_HANDLE_VALUE) {
				throw SetErrorMsgText("Create:", GetLastError());
			}

			while (*((TalkersCommand*)pPrm) != EXIT) {
				if (!ConnectNamedPipe(hPipe, NULL)) {
					DWORD error = GetLastError();
					if (error == ERROR_BROKEN_PIPE || error == ERROR_NO_DATA) {
						cout << "RConsole is disconnected" << endl;
						break;
					}
					else {
						cout << "NamedPipe error: " << error << endl;
						break;
					}
				}
				char readBuffer[400], writeBuffer[400];
				DWORD bytesRead = 0, bytesWrite = 0;
				TalkersCommand SetCommand;
				bool serverCommand;

				while (*((TalkersCommand*)pPrm) != EXIT) {
					if (*((TalkersCommand*)pPrm) == GETCOMMAND) {
						if (!ReadFile(hPipe, readBuffer, sizeof(readBuffer), &bytesRead, NULL)) {
							break;
						}
						if (bytesRead > 0) {
							string command(readBuffer, bytesRead);
							int port;
							if (sscanf_s(command.c_str(), "OPEN_ACCEPT %d", &port) == 1) {
								switch (OpenPort(port)) {
								case 0:
									sprintf_s(writeBuffer, "Port %d opened", port);
									break;
								case 1:
									sprintf_s(writeBuffer, "Port %d is already open", port);
									break;
								default:
									sprintf_s(writeBuffer, "Error when opening the port %d", port);
									break;
								}
							}
							else if (sscanf_s(command.c_str(), "CLOSE_ACCEPT %d", &port) == 1) {
								switch (ClosePort(port)) {
								case 0:
									sprintf_s(writeBuffer, "Port %d closed", port);
									break;
								case 1:
									sprintf_s(writeBuffer, "Port %d is not open", port);
									break;
								case 2:
									sprintf_s(writeBuffer, "Cannot close the only open port %d", port);
									break;
								default:
									sprintf_s(writeBuffer, "Error when closing the port %d", port);
									break;
								}
							}
							else if (sscanf_s(command.c_str(), "OPEN_ACCEPT_BROADCAST %d", &port) == 1) {
								switch (OpenBroadcastPort(port)) {
								case 0:
									sprintf_s(writeBuffer, "Port %d opened (broadcast)", port);
									break;
								case 1:
									sprintf_s(writeBuffer, "Port %d is already open (broadcast)", port);
									break;
								default:
									sprintf_s(writeBuffer, "Error when opening the port %d (broadcast)", port);
									break;
								}
							}
							else if (sscanf_s(command.c_str(), "CLOSE_ACCEPT_BROADCAST %d", &port) == 1) {
								switch (CloseBroadcastPort(port)) {
								case 0:
									sprintf_s(writeBuffer, "Port %d closed (broadcast)", port);
									break;
								case 1:
									sprintf_s(writeBuffer, "Port %d is not open (broadcast)", port);
									break;
								case 2:
									sprintf_s(writeBuffer, "Cannot close the only open port %d (broadcast)", port);
									break;
								default:
									sprintf_s(writeBuffer, "Error when closing the port %d (broadcast)", port);
									break;
								}
							}

							else {
								int n = atoi(readBuffer);
								switch (n) {
								case 1:
									sprintf_s(writeBuffer, "%s", "START");
									SetCommand = TalkersCommand::START;
									break;
								case 2:
									sprintf_s(writeBuffer, "%s", "STOP");
									SetCommand = TalkersCommand::STOP;
									break;
								case 3:
									sprintf_s(writeBuffer, "%s", "EXIT");
									SetCommand = TalkersCommand::EXIT;
									break;
								case 4:
									sprintf_s(writeBuffer, "\nTotal:   \t%i\nActive:  \t%i\nRejected:\t%i\nTimeout:   \t%i\n", TotalClients, CurrentClients, RejectedClients, TimeoutClients);
									//sprintf_s(writeBuffer, "\nTotal:   \t%i\nActive:  \t%i\nRejected:\t%i\n", TotalClients, CurrentClients, RejectedClients);
									SetCommand = TalkersCommand::GETCOMMAND;
									break;
								case 5:
									sprintf_s(writeBuffer, "%s", "WAIT");
									SetCommand = TalkersCommand::WAIT;
									break;
								case 6:
									sprintf_s(writeBuffer, "%s", "SHUTDOWN");
									SetCommand = TalkersCommand::SHUTDOWN;
									break;
								default:
									sprintf_s(writeBuffer, "%s", "error");
									SetCommand = TalkersCommand::GETCOMMAND;
									break;
								}
								*((TalkersCommand*)pPrm) = SetCommand;
							}

							if (!WriteFile(hPipe, writeBuffer, strlen(writeBuffer) + 1, &bytesWrite, NULL)) {
								DWORD error = GetLastError();
								if (error == ERROR_NO_DATA || error == ERROR_BROKEN_PIPE) {
									cout << "Client disconnected during write" << endl;

								}
								else {
									cout << "WriteFile error: " << error << endl;
								}
								break; 
							}

						}
					}
					Sleep(100);
				}
			}
			DisconnectNamedPipe(hPipe);
			CloseHandle(hPipe);
		}
	}
	catch (string ErrorPipeText) {
		cout << ErrorPipeText << endl;
	}
	DWORD rc = 0;
	ExitThread(rc);
}
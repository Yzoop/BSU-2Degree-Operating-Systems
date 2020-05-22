#include <stdio.h>
#include <windows.h>
#include <string>
#include <iostream>
#include "employee.h"
#include <vector>

#define MESSAGE_SIZE 16
#define MESSAGE_READ "r"
#define MESSAGE_WRITE "w"

void PrintFile(HANDLE hFile);
DWORD WINAPI ProcessClient(LPVOID);
DWORD WINAPI ConnectClients(LPVOID);
template<typename T> T input(std::string message);
void writeEmployeesToFile(HANDLE hFile, int numOfEmployees);
HANDLE createFileWithName(const std::string& file_name);

int clientCount = 0;
HANDLE hAllClientsLeaveEvent;
CRITICAL_SECTION csClientCount;

std::vector<int> readerCounts;
std::vector<HANDLE> hWriteSems;
std::vector<CRITICAL_SECTION> csRecords;

struct Employee* recordData;


int main() {
	std::string file_name;

	file_name = input<std::string>("Please, write a file name: ");

	HANDLE hFile = createFileWithName(file_name);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		printf("CreateFile failed, %d\n", GetLastError());
		return -1;
	}

	int num_of_records = input<int>("Please, write the number of records: ");

	writeEmployeesToFile(hFile, num_of_records);

	DWORD dwFileSize = GetFileSize(hFile, NULL);
	//check if we've got file size for futher calculations
	if (dwFileSize == INVALID_FILE_SIZE) {
		printf("GetFileSize failed, %d\n", GetLastError());
		return -1;
	}
	//check if we've successfully created file mapping for reading records
	HANDLE hMapping = CreateFileMapping(hFile, NULL, PAGE_READWRITE, 0, 0, NULL);
	if (hMapping == NULL) {
		printf("CreateFileMapping failed, %d\n", GetLastError());
		return -1;
	}

	recordData = (struct Employee*)MapViewOfFile(hMapping, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, dwFileSize); 
	if (recordData == NULL)
	{
		printf("MapViewOfFile failed, %d\n", GetLastError());
		return -1;
	}

	readerCounts.resize(num_of_records);
	csRecords.resize(num_of_records);
	hWriteSems.resize(num_of_records);

	for (int i = 0; i < num_of_records; i++)
	{
		InitializeCriticalSection(&csRecords[i]);
		hWriteSems[i] = CreateSemaphore(NULL, 1, 1, NULL);
		if (hWriteSems[i] == NULL)
		{
			printf("CreateSemaphore failed, %d\n", GetLastError());
			return -1;
		}
	}

	InitializeCriticalSection(&csClientCount);
	hAllClientsLeaveEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	HANDLE hConnectClientsThread = CreateThread(NULL, 0, ConnectClients, NULL, 0, NULL);
	if (hConnectClientsThread == NULL)
	{
		printf("CreateThread failed, %d\n", GetLastError());
		return -1;
	}

	WaitForSingleObject(hAllClientsLeaveEvent, INFINITE);

	UnmapViewOfFile(recordData);
	CloseHandle(hMapping);

	PrintFile(hFile);

	system("pause");

	TerminateThread(hConnectClientsThread, 1);

	CloseHandle(hConnectClientsThread);

	CloseHandle(hAllClientsLeaveEvent);

	DeleteCriticalSection(&csClientCount);

	for (int i = 0; i < num_of_records; i++) {
		CloseHandle(hWriteSems[i]);
		DeleteCriticalSection(&csRecords[i]);
	}

	CloseHandle(hFile);

	return 0;
}

DWORD WINAPI ConnectClients(LPVOID lpvParam)
{
	BOOL fConnected = FALSE;
	HANDLE hPipe = INVALID_HANDLE_VALUE, hThread = NULL;
	LPCWSTR lpszPipename = TEXT("\\\\.\\pipe\\my_pipe");

	while(true)
	{
		hPipe = CreateNamedPipe(
			lpszPipename,
			PIPE_ACCESS_DUPLEX,
			PIPE_TYPE_MESSAGE |
			PIPE_READMODE_MESSAGE |
			PIPE_WAIT,
			PIPE_UNLIMITED_INSTANCES,
			0,
			0,
			0,
			NULL
		);

		if (hPipe == INVALID_HANDLE_VALUE) {
			printf("CreateNamedFile failed, %d\n", GetLastError());
			return -1;
		}

		fConnected = ConnectNamedPipe(hPipe, NULL) ?
			TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);

		if (fConnected)
		{
			hThread = CreateThread(
				NULL,
				0,
				ProcessClient,
				(LPVOID)hPipe,
				0,
				NULL
			);

			if (hThread == NULL) {
				printf("CreateThread failed, %d\n", GetLastError());
				return -1;
			}

			EnterCriticalSection(&csClientCount);
			clientCount++;
			LeaveCriticalSection(&csClientCount);
		}
	}
}

DWORD WINAPI ProcessClient(LPVOID lpvParam) {
	if (lpvParam == NULL)
	{
		printf("Unexpected null parameter lpvParam in function ProcessClient\n");

		return -1;
	}

	char message[MESSAGE_SIZE];

	BOOL fSuccess = FALSE;
	HANDLE hPipe = (HANDLE)lpvParam;

	int numOfRecord = 0;

	while(true)
	{
		if (!ReadFile(hPipe, message, MESSAGE_SIZE * sizeof(char), NULL, NULL))
		{
			printf("ReadFile failed, %d\n", GetLastError());
			break;
		}

		sscanf_s(message, "%*s %d", &numOfRecord);
		--numOfRecord;

		if (strcmp(message, MESSAGE_READ) == 0)
		{
			EnterCriticalSection(&csRecords[numOfRecord]);

			readerCounts[numOfRecord]++;
			if (readerCounts[numOfRecord] == 1)
				WaitForSingleObject(hWriteSems[numOfRecord], INFINITE);

			LeaveCriticalSection(&csRecords[numOfRecord]);

			if (!ReadFile(hPipe, message, MESSAGE_SIZE * sizeof(char), NULL, NULL))
			{
				printf("ReadFile failed, %d\n", GetLastError());
				break;
			}

			EnterCriticalSection(&csRecords[numOfRecord]);

			readerCounts[numOfRecord]--;
			if (readerCounts[numOfRecord] == 0)
				ReleaseSemaphore(hWriteSems[numOfRecord], 1, NULL);

			LeaveCriticalSection(&csRecords[numOfRecord]);
		}
		else if (strcmp(message, MESSAGE_WRITE) == 0) {
			WaitForSingleObject(hWriteSems[numOfRecord], INFINITE);

			if (!WriteFile(hPipe, recordData + numOfRecord, sizeof(struct Employee),
				NULL, NULL)) {
				printf("WriteFile failed, %d\n", GetLastError());
				break;
			}

			if (!ReadFile(hPipe, recordData + numOfRecord, sizeof(struct Employee),
				NULL, NULL)) {
				printf("ReadFile failed, %d\n", GetLastError());
				break;
			}

			if (!ReadFile(hPipe, message, MESSAGE_SIZE * sizeof(char), NULL, NULL)) {
				printf("ReadFile failed, %d\n", GetLastError());
				break;
			}

			ReleaseSemaphore(hWriteSems[numOfRecord], 1, NULL);
		}
		else {
			break;
		}
	}

	// decrement number of clients and set event if no clients
	EnterCriticalSection(&csClientCount);

	clientCount--;
	if (clientCount == 0)
		SetEvent(hAllClientsLeaveEvent);

	LeaveCriticalSection(&csClientCount);

	FlushFileBuffers(hPipe);
	DisconnectNamedPipe(hPipe);
	CloseHandle(hPipe);

	return 1;
}

void PrintFile(HANDLE hFile) {

	SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
	int i = 1;
	struct Employee tp;
	DWORD numOfBytesRead;

	printf("\nbinary file data:\n");

	while (ReadFile(hFile, &tp, sizeof(struct Employee), &numOfBytesRead, NULL))
	{
		if (numOfBytesRead == 0)
			break;

		printf("Employee N%d { %d, %s, %lf }\n", i, tp.num, tp.name, tp.hours);

		i++;
	}
}

void writeEmployeesToFile(HANDLE hFile, int numOfEmployees)
{
	for (int i = 0; i < numOfEmployees; i++) {
		struct Employee tp;
		std::cout << "Please, write an id of " << i << "-th" << "employee: ";
		std::cin >> tp.num;

		std::cout << "Please, write a name of " << i << "-th" << "employee: ";
		std::cin >> tp.name;

		std::cout << "Please, write number of working hours of" << i << "-th" << "employee: ";
		std::cin >> tp.hours;

		WriteFile(hFile, &tp, sizeof(struct Employee), NULL, NULL);
	}

	PrintFile(hFile);
}

HANDLE createFileWithName(const std::string& file_name)
{
	HANDLE hFile = CreateFile(std::wstring(file_name.begin(), file_name.end()).c_str(),
		GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL, NULL);

	return hFile;
}

template<typename T>
T input(std::string message)
{
	T read_data;
	std::cout << message;
	std::cin >> read_data;
	return read_data;
}

#include <stdio.h>
#include <windows.h>
#include "employee.h"
#include <string>

#define MESSAGE_SIZE 50
#define PIPE_NAME_SIZE 100
#define MESSAGE_READ "r"
#define MESSAGE_WRITE "w"
int main(int argc, char* argv[]) {
	HANDLE hPipe;
	char message[MESSAGE_SIZE];
	BOOL fSuccess = FALSE;

	const char* lpszPipenameFormat = "\\\\%s\\pipe\\my_pipe";
	char pipename[PIPE_NAME_SIZE];

	if (argc > 1) {
		sprintf_s(pipename, PIPE_NAME_SIZE, lpszPipenameFormat, argv[1]);
	}
	else {
		sprintf_s(pipename, PIPE_NAME_SIZE, lpszPipenameFormat, ".");
	}

	if (!WaitNamedPipe((LPCWSTR)pipename, NMPWAIT_WAIT_FOREVER)) {
		printf("WaitNamedPipe failed, %d\n", GetLastError());
		return -1;
	}

	hPipe = CreateFile(
		(LPCWSTR)pipename,
		GENERIC_READ |
		GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		0,
		NULL
	);

	if (hPipe == INVALID_HANDLE_VALUE)
	{
		printf("CreateFile failed, %d\n", GetLastError());
		return -1;
	}

	while(true) {
		char operation;

		printf("Enter 'w' to modify record, 'r' to read record, 'q' to quit: ");
		scanf_s("%c", &operation);
		getchar();

		if (operation == 'q') {
			if (!WriteFile(hPipe, "QUIT", 5, NULL, NULL)) {
				printf("WriteFile failed, %d\n", GetLastError());
				return -1;
			}

			break;
		}

		int numOfRecord;

		printf("Enter number of record: ");
		scanf_s("%d", &numOfRecord);
		getchar();

		if (operation == 'w')
		{
			sprintf_s(message, MESSAGE_SIZE, "w %d", numOfRecord);
		}
		else if (operation == 'r')
		{
			sprintf_s(message, MESSAGE_SIZE, "r %d", numOfRecord);
		}

		if (!WriteFile(hPipe, message, MESSAGE_SIZE * sizeof(char), NULL, NULL))
		{
			printf("WriteFile failed, %d\n", GetLastError());
			return -1;
		}

		struct Employee tp;

		if (!ReadFile(hPipe, &tp, sizeof(struct Employee), NULL, NULL))
		{
			printf("ReadFile failed, %d\n", GetLastError());
			return -1;
		}

		printf("Employee N%d { %d, %s, %lf }\n", numOfRecord, tp.num, tp.name, tp.hours);

		if (operation == 'w')
		{
			printf("Enter employee id: ");
			scanf_s("%d", &tp.num);
			getchar();

			char name[11];

			printf("Enter employee name: ");
			fgets(name, 11, stdin);
			name[strlen(name) - 1] = 0;
			sprintf_s(tp.name, EMPLOYEE_NAME_SIZE, "%s", name);

			printf("Enter count of hours worked: "); // double
			scanf_s("%lf", &tp.hours);
			getchar();

			printf("Enter 's' to send new record: ");
			getchar();
			getchar();

			if (!WriteFile(hPipe, &tp, sizeof(struct Employee), NULL, NULL)) {
				printf("WriteFile failed, %d\n", GetLastError());
				return -1;
			}

			sprintf_s(message, MESSAGE_SIZE, "RELEASE WRITE");
		}
		else if (operation == 'r')
		{
			sprintf_s(message, MESSAGE_SIZE, "RELEASE READ");
		}

		printf("Enter 'e' to end access to record: ");
		getchar();
		getchar();

		if (!WriteFile(hPipe, message, MESSAGE_SIZE * sizeof(char), NULL, NULL))
		{
			printf("WriteFile failed, %d\n", GetLastError());
			return -1;
		}
	}

	CloseHandle(hPipe);

	return 0;
}
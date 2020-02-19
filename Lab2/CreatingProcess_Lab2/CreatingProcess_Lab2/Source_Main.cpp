#define _CRT_SECURE_NO_WARNINGS

#define PATH_PLUS_BINARYNAME_PLUS_NUMREPORTS "D:/BSU/2Degree/4SEM/OS/LABS/Lab2/CreatingProcess_Lab2/x64/Debug/Creator.exe %s %d"
#define PATH_PLUS_REPORTER_PLUS_HOURS "D:/BSU/2Degree/4SEM/OS/LABS/Lab2/CreatingProcess_Lab2/x64/Debug/Reporter.exe %s %s %lf"
#define MESSAGE_ASK_FOR_CREATOR_NAME "Please, enter suitable name for a binary file for creator: "
#define MESSAGE_ASK_FOR_REPORTER_NAME "Enter suitable name for a file for reporter: "


#include <windows.h>
#include <stdio.h>
#include "Employee.h"
#include <tchar.h>
#include <iostream>


char creatorBinaryFileName[256], reporterFileName[256];
int numReports = 0;

void getFileName(char*, const char*);
void getNumReports(int&);
void startCreatorJob();
void startReporterJob();
void getPaymentPerOur(double&);
void printDataFromFile(const char* fileName, const char* format);
void printDataFromReporter(const char* fileName);


void main()
{
    startCreatorJob();

    
    printf("Creator job has successfully finished!\nNow have a look on what is in the binary file:\n");
    printDataFromFile(creatorBinaryFileName, "rb");

    printf("\n\nTrying to start reporter...\n");

    startReporterJob();

    printDataFromReporter(reporterFileName);
}

void getFileName(char* fileName, const char* messageForUser)
{
    printf(messageForUser);
    scanf("%s", fileName);
}

void getNumReports(int& numReports)
{
    printf("Please, enter number (> 0) of reports: ");
    scanf("%d", &numReports);
}


void startCreatorJob()
{
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    getFileName(creatorBinaryFileName, MESSAGE_ASK_FOR_CREATOR_NAME);
    getNumReports(numReports);

    char arguments[256];

    sprintf(arguments, PATH_PLUS_BINARYNAME_PLUS_NUMREPORTS, creatorBinaryFileName, numReports);

    // Start the child process. 
    if (!CreateProcessA(
        NULL,                   // the path
        arguments,              // Command line
        NULL,                   // Process handle not inheritable
        NULL,                   // Thread handle not inheritable
        FALSE,                  // Set handle inheritance to FALSE
        CREATE_NEW_CONSOLE,     // Opens file in a separate console
        NULL,                   // Use parent's environment block
        NULL,                   // Use parent's starting directory 
        &si,                    // Pointer to STARTUPINFO structure
        &pi                     // Pointer to PROCESS_INFORMATION structure
    ))
    {
        printf("Creator process failed (%d).\n", GetLastError());
        return;
    }

    // Wait until child process exits.
    WaitForSingleObject(pi.hProcess, INFINITE);

    // Close process and thread handles. 
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

void startReporterJob()
{
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));
    
    double paymentPerHour = 0;
    
    getFileName(reporterFileName, MESSAGE_ASK_FOR_REPORTER_NAME);
    getPaymentPerOur(paymentPerHour);

    char arguments[256];

    sprintf(arguments, PATH_PLUS_REPORTER_PLUS_HOURS, creatorBinaryFileName, reporterFileName,paymentPerHour);
    
    // Start the child process. 
    if (!CreateProcessA(
        NULL,                   // the path
        arguments,              // Command line
        NULL,                   // Process handle not inheritable
        NULL,                   // Thread handle not inheritable
        FALSE,                  // Set handle inheritance to FALSE
        CREATE_NEW_CONSOLE,     // Opens file in a separate console
        NULL,                   // Use parent's environment block
        NULL,                   // Use parent's starting directory 
        &si,                    // Pointer to STARTUPINFO structure
        &pi                     // Pointer to PROCESS_INFORMATION structure
    ))
    {
        printf("Reporter process failed (%d).\n", GetLastError());
        return;
    }

    // Wait until child process exits.
    WaitForSingleObject(pi.hProcess, INFINITE);

    // Close process and thread handles. 
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

void getPaymentPerOur(double& paymentPerHour)
{
    printf("Please, enter payment per hour: ");
    scanf("%lf", &paymentPerHour);
}

void printDataFromFile(const char* fileName, const char* format)
{
    FILE* binFileFromCreator = fopen(creatorBinaryFileName, format);

    fscanf(binFileFromCreator, "%d", &numReports);

    printf("there are %d reports in the file...\n", numReports);

    for (int repId = 0; repId < numReports; ++repId) {
        struct employee emp;
        fread(&emp, sizeof(struct employee), 1, binFileFromCreator);

        printf("report #%d:\nNum:\t%d\nName:\t%s\nHours:\t%lf\n", repId, emp.num, emp.name, emp.hours);
    }

    fclose(binFileFromCreator);
}

void printDataFromReporter(const char* fileName)
{
    FILE* reporter = fopen(fileName, "r");

    char line[256];
    while (fgets(line, 100, reporter) != NULL)
        printf("%s", line);
}

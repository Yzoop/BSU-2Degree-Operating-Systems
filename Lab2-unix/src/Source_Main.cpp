#define CREATOR_STR "Creator"
#define REPORTER_STR "Reporter"
#define MESSAGE_ASK_FOR_CREATOR_NAME "Please, enter suitable name for a binary file for creator: "
#define MESSAGE_ASK_FOR_REPORTER_NAME "Enter suitable name for a file for reporter: "

#include "unix_process_manager.h"
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
	getFileName(creatorBinaryFileName, MESSAGE_ASK_FOR_CREATOR_NAME);
	getNumReports(numReports);

	char arguments[500];

	sprintf(arguments, creatorBinaryFileName, numReports);
	unix_process_st creator_unix_proc = { CREATOR_STR, arguments };

	// Start the child process. 
	if (create_unix_process(&creator_unix_proc) == UNIX_PROCESS_CREATE_ERROR)
	{
		printf("Error while creating %s unix process\n", creator_unix_proc.name);
	}
}

void startReporterJob()
{
	double paymentPerHour = 0;

	getFileName(reporterFileName, MESSAGE_ASK_FOR_REPORTER_NAME);
	getPaymentPerOur(paymentPerHour);

	char arguments[500];
	sprintf(arguments, creatorBinaryFileName, reporterFileName, paymentPerHour);

	// Start the child process. 
	unix_process_st reporter_unix_proc = { REPORTER_STR, arguments };
	if (create_unix_process(&reporter_unix_proc) == UNIX_PROCESS_CREATE_ERROR)
	{
		printf("Error while creating %s unix process\n", reporter_unix_proc.name);
	}
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

	char line[500];
	while (fgets(line, 500, reporter) != NULL)
		printf("%s", line);
}

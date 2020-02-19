#define _CRT_SECURE_NO_WARNINGS


#include <iostream>
#include "../CreatingProcess_Lab2/Employee.h"

using namespace std;

void writeDataToFile(char*, char*, double);

int main(int argc, char* argv[])
{
	if (argc == 4) {
		char* creatorBinFileName = argv[1];
		char* reporterFileName = argv[2];
		double paymentPerHour = strtod(argv[3], NULL);

		writeDataToFile(creatorBinFileName, reporterFileName, paymentPerHour);
		cout << "Data from Creator has succesfully been written!\n";
	}
	else {
		cout << "If you want to use reporter, you have to give it 3 additional args:\n" << "binary file name from creator, for reporter and payment per hour";
	}

	system("pause");
}

void writeDataToFile(char* creatorBinFileName, char* reporterFileName, double paymentPerHour)
{
	char reporterFirstLine[256];
	int numReports = 0;
	FILE* creatorBinaryFile = fopen(creatorBinFileName, "rb");
	FILE* reporterFile = fopen(reporterFileName, "w");

	sprintf(reporterFirstLine, "'%s' file report\n", creatorBinFileName);
	fwrite(reporterFirstLine, sizeof(char), strlen(reporterFirstLine), reporterFile);
	fscanf(creatorBinaryFile, "%d", &numReports);

	cout << "Writing " << numReports << " reports...";

	for (int repId = 0; repId < numReports; ++repId) {
		struct employee emp;
		fread(&emp, sizeof(struct employee), 1, creatorBinaryFile);
		fprintf(reporterFile, "----------%d report-----------\nEmployee's number:\t%d\nEmployee's name:\t%s\nEmployee's hours:\t%lf\nEmployee's salary:\t%lf\n", 
							 repId + 1, emp.num, emp.name, emp.hours, (double)emp.hours * paymentPerHour);
	}

}

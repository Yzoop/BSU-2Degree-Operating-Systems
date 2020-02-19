#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <string>
#include "../CreatingProcess_Lab2/Employee.h"


using namespace std;


employee* getReports(int);
bool isReportWriteToFileSuccess(int numreps, employee* reports, char* fName);

int main(int argc, char* argv[]) {
	if (argc == 3) {
		char* binaryFileName = argv[1];
		int numberOfReports = atoi(argv[2]);

		cout << numberOfReports << " - argv2" << "; str arg: " << argv[2];

		employee* reports = getReports(numberOfReports);

		if (isReportWriteToFileSuccess(numberOfReports, reports, binaryFileName)) {
			cout << "Reports have just been successfully written to file " << binaryFileName << endl;
		}
		else {
			cout << "Error while writing reports to file " << binaryFileName << endl;
		}

		delete[] reports;
	}
	else {
		cout << "Error!\n You must have given 2 additional arguments: binary file name and a number of reports" << endl;
	}

	system("pause");
	return 0;
}

employee* getReports(int nReports)
{
	employee* reports = new employee[nReports];
	cout << "\n--------Now you have to enter " << nReports << " reports--------\n\n";

	for (int repId = 0; repId < nReports; ++repId) {
		int id, numHours;
		char* name;

		cout << "Report " << repId + 1 << "/" << nReports << "...\n";

		cout << "Enter employee's Id: ";
		cin >> reports[repId].num;

		cout << "Enter employee's name: ";
		cin >> reports[repId].name;

		cout << "Enter employee's number of hours spent on work: ";
		cin >> reports[repId].hours;

		cout << "---Written---\n";
	}

	return reports;
}

bool isReportWriteToFileSuccess(int numreps, employee* reports, char* fName)
{
	try {
		FILE* binFile = fopen(fName, "wb");

		fprintf(binFile, "%d", numreps);
		fwrite(reports, sizeof(struct employee), numreps, binFile);

		fclose(binFile);

		return true;
	}
	catch (...) {
		return false;
	}
}

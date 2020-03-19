#include <iostream>

using namespace std;

int main(int argc, char* argv[]) {

	cout << "Process 'Creator' has started\n";

	cout << "-----\nThere are " << argc - 1 << " additional arguments: ";
	for (int i = 1; i < argc; i++) {
		cout << i << ". " << argv[i] << "\n";
	}

	return 0;
}

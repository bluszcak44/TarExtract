#include <iostream>
#include <cstdlib>
#include "test.h"
#include <unistd.h>
using namespace std;

void cleanup ();

int main (int argCount, char * argValues[]) {
	atexit (cleanup);
	cout << "Testing" << endl;
	
	return EXIT_SUCCESS;

}


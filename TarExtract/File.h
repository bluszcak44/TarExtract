#ifndef TAREXTRACT_FILE_H
#define TAREXTRACT_FILE_H


#include <cstdlib>
#include <vector>
#include <string>
//Simple header file for the file object

using namespace std;

class File {

public:
	File();
	~File();

	vector<string> fileContents;

private:

};
#endif
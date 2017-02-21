//Written by: Blaine Luszcak
#include <string>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include "File.h"

using namespace std;

//header struct
struct tarHdr {
	char name[100];
	char mode[8];
	char uid[8];
	char gid[8];
	char size[12];
	char mtime[12];
	char checksum[8];
	char linkflag[1];
	char linkname[100];
	char pad[255];
};

//actual file struct
struct contentsFile {
	//char contents[];
	char test[700];
	vector<string> contents;
	vector<char> v;
	vector<int> headerLoc;
	vector<int> totalFileSize;
	//int s = 5;
	//char* contents = new char[s];
	//vector<char> contents[1000];
};

//Prototyping functions
void readHdr(tarHdr &myHdr, ifstream &tarF, contentsFile &file, int currentPos); //function prototype w/ a struct
void readFile(string fileName, const char * size, contentsFile &file, ifstream &tarF);

int main() {

	ifstream infile, nextFile;
	string fileN;
	int size, currentPos, fileLength;

	currentPos = 0;
	//if (fileIN successful)

	//currentPos += 512;

	tarHdr myHdr; //header struct for each file in the .tar
	contentsFile file; //filecontents struct

	infile.open("example.tar");
	if (infile.fail()) {
		cerr << "Couldn't open input file -- exitting" << endl;
	} else {
		//infile.read((char *)&myHdr, sizeof(myHdr));
		cout << ".tar opened" << endl;
	}
	
	char test;
	int si;
	si = 150;

	infile.seekg(0, infile.end);
	fileLength = infile.tellg();
	infile.seekg(1024);
	//infile.read((char *)&myHdr, sizeof(myHdr));
	//infile.get(test);

	readHdr(myHdr, infile, file, currentPos); //get your header struct
	readFile(myHdr.name, myHdr.size, file, infile); //take the header info and find the corresponding file in .tar

	//loop though the rest of the file?
	while (file.headerLoc < file.totalFileSize) {
		//get length of the files when stream is open
		infile.seekg(0, infile.end);
		fileLength = infile.tellg();
		infile.seekg(0, 512);

		readHdr(myHdr, infile, file, currentPos);
		readFile(myHdr.name, myHdr.size, file, infile);

	}

	cout << "test" << endl;

	//File *prgFile = new File(); //each file should be an object no? al;dfj;dskfja

	infile.close(); //close the filereader after it's done

	cout << "We made it to the end of the program. " << "\n" << endl;
	printf("Press enter to continue...\n");
	getchar();
}

//open .tar file and read in the header
void readHdr(tarHdr &myHdr, ifstream &tarF, contentsFile &file, int currentPos) {

	//ifstream infile;
	string fileName;
	int ownersID, groupID, filesize, modifyTime, checksum, length, headerPos;
	unsigned int computedChecksum;
	char * block;

	//get length of the files when stream is open
	//tarF.seekg(0, tarF.end);
	//length = tarF.tellg();
	//tarF.seekg(0, tarF.beg);

	tarF.read((char *)&myHdr, sizeof(myHdr)); //read header into struct

	//all items are stored as c strings and can be converted to C++ strings through assignment.
	fileName = myHdr.name; //isn't it .name instead of .fileName? (idk I changed it)
	cout << "Filename: " << fileName << endl;

	/*strtol will parse a string and convert the value to an integer. The 8 in the last argument says to assume it is an octal value
	* stored in the string. */
	ownersID = strtol(myHdr.uid, NULL, 8);
	groupID = strtol(myHdr.gid, NULL, 8);
	cout << "Owner ID = " << ownersID << "\n" << "Group ID = " << groupID << endl;

	filesize = strtol(myHdr.size, NULL, 8);
	cout << "Filesize = " << filesize << endl;
	file.headerLoc.push_back(filesize); //pushes current file size into vector
	headerPos = 512 + filesize; 
	file.headerLoc.push_back(headerPos);

	checksum = strtol(myHdr.checksum, NULL, 8);
	cout << "Checksum = " << checksum << endl;

	//size_t skip = filesize % 512 ? filesize + 512 - (filesize % 512) : filesize;

	// Put spaces in the checksum area before we compute it.
	for (int i = 0; i < 8; i++) {
		myHdr.checksum[i] = ' ';
	}

	computedChecksum = 0;   // initially zero, get each character in the header block and sum them together.
	block = (char *)&myHdr;
	for (int i = 0; i < sizeof(myHdr); i++) {
		computedChecksum += block[i];
	}
	cout << "Computed Checksum = " << computedChecksum << endl;
}

//read the first file based on the header
void readFile(string fileName, const char * size, contentsFile &file, ifstream &tarF) {

	int fileSize;
	ifstream currFile;
	string fileN;
	const char *f;

	//converting it to a C string
	fileN = fileName; 
	f = fileN.c_str();

	fileSize = strtol(size, NULL, 8); //converting octal to base 10 int

	currFile.open(f);
	//infile.read((char*)&file.test, sizeof(int)*size);
	if (tarF.fail()) {
		cerr << "Couldn't open input file -- exitting" << endl;
	}
	file.v.resize(fileSize);
	tarF.read(&file.v[0], fileSize);
	//tarF.read((char *)&file.test, fileSize);
	
	string s = string(file.v.begin(), file.v.end());
	file.contents.push_back(s);

	currFile.close();
}
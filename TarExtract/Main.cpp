//Written by: Blaine Luszcak
#include <string>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <sys/types.h> //linux
#include "File.h"

#ifdef _WIN32
#include <direct.h>
#elif defined __linux__
#include <sys/stat.h>
#endif

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
	char test[700];
	vector<string> contents;
	vector<char> v;
	vector<int> headerLoc, totalFileSize, currFileSize, trackFileSize, outputFiles;
};

//Prototyping functions
void readHdr(tarHdr &myHdr, ifstream &tarF, contentsFile &file);
void readFile(string fileName, const char * size, contentsFile &file, ifstream &tarF);
void remainingFilesLoop(tarHdr &myHdr, contentsFile &file, ifstream &tarF);
void outputFiles(tarHdr &myHdr, contentsFile &file);
void make_directory(string name);

int main() {

	ifstream infile, nextFile;
	string fileN;
	int size, currentPos, fileLength;

	currentPos = 0;

	tarHdr myHdr; //header struct for each file in the .tar
	contentsFile file; //filecontents struct

	infile.open("example.tar");
	if (infile.fail()) {
		cerr << "Couldn't open input file -- exitting" << endl;
	} else {
		//infile.read((char *)&myHdr, sizeof(myHdr));
		cout << ".tar opened" << endl;
	}

	make_directory("example");
	cout << "test" << endl;
	//char test;
	//int si;
	//si = 150;

	//get the size of the .tar and then read from the 0th line
	infile.seekg(0, infile.end);
	fileLength = infile.tellg();
	file.totalFileSize.push_back(fileLength); //pushes size of .tar file into vector
	infile.seekg(0, infile.beg); //start reading at the beginning of the file so line 0
	
	//infile.read((char *)&myHdr, sizeof(myHdr));
	//infile.get(test);

	readHdr(myHdr, infile, file); //get your header struct
	readFile(myHdr.name, myHdr.size, file, infile); //take the header info and find the corresponding file in .tar
	outputFiles(myHdr, file);
	//Functin call to save first file into created directory, after this the call will only be used in "remainingFilesLoop"
	remainingFilesLoop(myHdr, file, infile); //loop through the remaining files

	infile.close(); //close the filereader after it's done

	cout << "We made it to the end of the program. " << "\n" << endl;
	printf("Press enter to continue...\n");
	getchar();
}

//open .tar file and read in the header
void readHdr(tarHdr &myHdr, ifstream &tarF, contentsFile &file) {

	string fileName;
	int ownersID, groupID, filesize, modifyTime, checksum, length, headerPos, currSize;
	unsigned int computedChecksum;
	char * block;

	currSize = 0;

	tarF.read((char *)&myHdr, sizeof(myHdr)); //read header into struct

	if (strtol(myHdr.size,NULL,8) != 0) { 
		
		fileName = myHdr.name; 
		cout << "Filename: " << fileName << endl;

		/*strtol will parse a string and convert the value to an integer. The 8 in the last argument says to assume it is an octal value
		* stored in the string. */
		ownersID = strtol(myHdr.uid, NULL, 8);
		groupID = strtol(myHdr.gid, NULL, 8);
		cout << "Owner ID = " << ownersID << "\n" << "Group ID = " << groupID << endl;

		filesize = strtol(myHdr.size, NULL, 8);
		cout << "Filesize = " << filesize << endl;
		file.currFileSize.push_back(filesize); //pushes size of the current file we are looking at into vector
		headerPos = 512 + filesize;
		file.headerLoc.push_back(headerPos); //pushes total header location into vector

		//keeps track of how far we are in the vector
		for (auto& n : file.headerLoc) {
			currSize += n;
		}
		file.trackFileSize.push_back(currSize); //actually tracking the size accurately

		checksum = strtol(myHdr.checksum, NULL, 8);
		cout << "Checksum = " << checksum << endl;

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
	} else {
		cout << "We have read all the files" << endl;
	}	
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
	
	string s = string(file.v.begin(), file.v.end());
	file.contents.push_back(s);

	currFile.close();
}

//Loops through the remainingfiles after the first initial one
void remainingFilesLoop(tarHdr &myHdr, contentsFile &file, ifstream &tarFile) {
	int currentPos;

	currentPos = 0;

	//loop though the rest of the file
	while (myHdr.name != " " && myHdr.mode != " ") {
		int temp;

		//if file <= 512 multiply by 1024 to account for header and data block
		if (file.currFileSize.back() <= 512) {
			currentPos = (file.contents.size() * 1024); //only 1 data block long
		} else {
			temp = file.headerLoc.back() / 512; //bigger than 512 has not been tested
			currentPos = temp * 512;
		}

		//read at the correct byte location
		tarFile.seekg(currentPos);
		readHdr(myHdr, tarFile, file);

		//make sure that we have not reached the end of the file
		if (strtol(myHdr.size, NULL, 8) != 0) {
			readFile(myHdr.name, myHdr.size, file, tarFile);
			outputFiles(myHdr, file);
		} else {
			break;
		}
	}
}

//output all of the files that we got from the .tar into the folder that we created as .txts
void outputFiles(tarHdr &myHdr, contentsFile &file) {
	ofstream fileOut;
	int fileCnt;
	fileCnt = 0;
	fileOut.open(("example/" + (string)myHdr.name).c_str());

	if (fileOut.is_open()) {
		cout << endl << "Beginning to write to the file" << endl;
		file.outputFiles.push_back(1); //need global variable to count files 
		fileCnt = file.outputFiles.size();
		fileOut << file.contents.at(fileCnt -1);
	} else {
		cout << "File could not be opened." << endl << endl;
	}
} 

//Make a directory in both windows and linux --WORKS
void make_directory(string name) {
	string mk;
	string path;

	mk = "mkdir ";
	path = mk.append(name);
	const char *c = path.c_str();

	system(c);
	cout << "test" << endl;
}
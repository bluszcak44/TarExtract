//Written by: Blaine Luszcak
#include <string>
#include <cstdlib>
#include <stdio.h>
#include <cstring>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <sys/types.h> 
#include "File.h"

#ifdef _WIN32
#include <direct.h>
#define GetCurrentDir getcwd
#elif defined __linux__
#include <unistd.h>
#include <sys/stat.h>
#define GetCurrentDir _getcwd
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
	vector<string> contents, tarHeaders, tarFileContents;
	vector<char> v;
	vector<int> headerLoc, totalFileSize, currFileSize, trackFileSize,
		outputFiles, tarFileSizes, tarFileLink, headerPos, dataSize, dataBlockCnt, hdrNxt, hdrPrev;
};

//Prototyping functions
void readHdr(tarHdr &myHdr, ifstream &tarF, contentsFile &file);
void readFile(string fileName, contentsFile &file, ifstream &tarF);
//void remainingFilesLoop(tarHdr &myHdr, contentsFile &file, ifstream &tarF);
void outputFiles(tarHdr &myHdr, contentsFile &file);
void make_directory(string name);
//char curPth();
int main() {

	ifstream infile, nextFile;
	string fileN, usrTarFile, usrFolderChng;
	int currentPos;

	currentPos = 0;

	tarHdr myHdr; //header struct for each file in the .tar
	contentsFile file; //filecontents struct

	cout << "Which .tar file would you like to extract? (i.e. 'example')" << endl;
	cin >> usrTarFile;
	infile.open(usrTarFile + ".tar");
	if (infile.fail()) {
		cerr << "Couldn't open input file -- exitting" << endl;
	}
	else {
		cout << ".tar opened" << endl;
	}

	//-----------------------
	//cout << "Y or N: Would you like to change the name of the extraction folder?" << endl;
	//cin >> usrFolderChng;
	//if (usrFolderChng == "Y") {
	//	cout << "Please enter the name you would like to give the folder " << endl;
	//	cin >> usrTarFile;
	//} else if (usrFolderChng == "N") {
	//	cout << "Using same name as the .tar file: " << usrTarFile;
	//} else {
	//	//
	//}

	//curPth(); not 100% convinced that I need this

	make_directory("open");
	//make_directory("testFolder");
	//make_directory("srcTest");

	//--------------------

	readHdr(myHdr, infile, file); //get your header struct
	readFile(myHdr.name, file, infile); //take the header info and find the corresponding file in .tar
	outputFiles(myHdr, file); //Functin call to save first file into created directory, after this the call will only be used in "remainingFilesLoop"

	infile.close(); //close the filereader after it's done

	cout << "We made it to the end of the program. " << "\n" << endl;
	printf("Press enter to continue...\n");
	getchar();
	getchar(); //need 2 so it eats the CR and actually pauses
}

//open .tar file and read in the header
void readHdr(tarHdr &myHdr, ifstream &tarF, contentsFile &file) {

	string fileName, folder;
	int ownersID, groupID, filesize, modifyTime, checksum, headerPos, currSize,
		hdrCnt, hdrStrt, prev;
	unsigned int computedChecksum;
	char * block;
	char folderC;
	streampos pos;

	currSize = 0;
	hdrCnt = 0;
	hdrStrt = 0;
	prev = 0;

	file.hdrPrev.push_back(0);
	//file.headerPos.push_back(0);
		while (myHdr.linkflag != "" && (string)myHdr.name != "") {
		//reading the headerst
		tarF.read((char*)&myHdr, sizeof(myHdr));
		int temp = strtol(myHdr.size, NULL, 8);
		int p = ((temp + 511) / 512); //how many data block there are, I need this number for the data files
		file.dataBlockCnt.push_back(p); //push # of data blocks in
		tarF.seekg(p * 512, (std::ios_base::cur));

		prev = tarF.tellg();
		file.hdrPrev.push_back(prev);

		if ((string)myHdr.name != "" && (string)myHdr.name != "") {

			fileName = myHdr.name;
			cout << "Filename: " << fileName << endl;
			folderC = fileName.back(); //trying to find slash

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


			if ((string)myHdr.linkflag == "5" || folderC == '/') {

				//put 0 as the first header because it has to start there
				if (hdrCnt == 0) {
					temp = tarF.tellg();
					file.headerPos.push_back(0);
					file.headerPos.push_back(temp);
					hdrCnt += 2;
				} else if (hdrCnt == 1)  {
					int t = *(file.hdrPrev.rbegin() + 1);
					//int temp = *(file.headerPos.rbegin());
					file.headerPos.push_back(t); //folder headers just move 512
					hdrCnt++;
				} else {
					int t = *(file.hdrPrev.rbegin());
					//int temp = *(file.headerPos.rbegin());
					file.headerPos.push_back(t); //folder headers just move 512
					hdrCnt++;
				}

			} else if ((string)myHdr.linkflag == "\0" || (string)myHdr.linkflag == "0") {
				
				//header pos has to start at 0
				if (hdrCnt == 0) {
					temp = tarF.tellg(); //next header position
					file.headerPos.push_back(0); //first header always starts at 0
					hdrCnt++;
				} else if (hdrCnt == 1) {
					//temp = tarF.tellg(); //next header position
					int previous = *(file.hdrPrev.rbegin() + 1);
					file.headerPos.push_back(previous);
					//file.headerPos.push_back(temp);
					hdrCnt++;
				} else { 

					if (file.hdrPrev.size() == file.headerPos.size()) {
						if (p == 1) {
							//temp = tarF.tellg(); //next header position
							int previous = *(file.hdrPrev.rbegin());
							//int previous = *(file.hdrPrev.rbegin() + 1);
							file.headerPos.push_back(previous);
							hdrCnt++;
						} else { //bigger files case not tested
							int previous = *(file.hdrPrev.rbegin());
							//temp = tarF.tellg();
							file.headerPos.push_back(previous);
							hdrCnt++;
						}
					} if (p == 1) {
						//if the one before and the current are equivalent don't push in the same thing
						if (*(file.hdrPrev.rbegin() + 1) == file.headerPos.back()) {
							int previous = *(file.hdrPrev.rbegin());
							file.headerPos.push_back(previous);
							hdrCnt++;
						} else {
							//temp = tarF.tellg(); //next header position
							int previous = *(file.hdrPrev.rbegin() + 1); //-------------------------------------------------------------Changed this to +1 if it stop working
						    //int previous = *(file.hdrPrev.rbegin() + 1);
							file.headerPos.push_back(previous);
							hdrCnt++;
						}
					} else { //bigger files case not tested
						int previous = *(file.hdrPrev.rbegin());
						//temp = tarF.tellg();
						file.headerPos.push_back(previous);
						hdrCnt++;
					}
				}
			} else {
				cout << "We broke down! " << endl;
			}

			file.tarHeaders.push_back(myHdr.name);
			file.tarFileSizes.push_back(strtol(myHdr.size, NULL, 8));
			file.tarFileLink.push_back(strtol(myHdr.linkflag, NULL, 8));
		} else {
			cout << "We have read all the files" << endl;
			break;
		}
	}
}

//read the first file based on the header
void readFile(string fileName, contentsFile &file, ifstream &tarF) {

	int fileSize, pos;
	ifstream currFile;
	string fileN, s;
	//const char *f;
	char folderC;


	file.dataBlockCnt.pop_back();
	for (int i = 0; i < file.tarHeaders.size(); i++) {
		fileN = file.tarHeaders[i];
		//f = fileN.c_str();
		folderC = fileN.back();
		fileSize = file.tarFileSizes[i];

		currFile.open(("open/" + (string)fileN.c_str()));
		if (tarF.fail()) {
			cerr << "Couldn't open input file -- exitting" << endl;
		}


		if (file.tarFileLink[i] != 5 && folderC != '/') { //means its a file
			if (file.headerPos[i] == 0) {
				pos = file.headerPos[i] + (file.dataBlockCnt[i] * 512);
				//pos = file.headerPos[i] + 512; //start reading
				tarF.seekg(pos);
				file.v.resize(file.tarFileSizes[i]);
				tarF.read(&file.v[0], file.tarFileSizes[i]);
			} else {
				//pos = file.headerPos[i] + (file.dataBlockCnt[i] * 512);
				pos = file.headerPos[i] + 512; //start reading
				tarF.seekg(pos);
				file.v.resize(file.tarFileSizes[i]);
				tarF.read(&file.v[0], file.tarFileSizes[i]);
			}
		}
		else {
			cout << "Folder: " << fileN << endl;
		}

		string s = string(file.v.begin(), file.v.end());
		file.tarFileContents.push_back(s);
		file.v.clear();
		//currFile.close();
	}
	currFile.close();
}

//output all of the files that we got from the .tar into the folder that we created
void outputFiles(tarHdr &myHdr, contentsFile &file) {
	ofstream fileOut;
	int fileCnt;
	string test;
	fileCnt = 0;

	test = "open/";
	for (int i = 0; i < file.tarHeaders.size(); i++) {
		fileOut.open(("open/" + (string)file.tarHeaders[i]).c_str());
		//fileOut.open(("srcTest/" + (string)file.tarHeaders[i]).c_str());

		if ((char)file.tarHeaders[i].back() == '/') {
			string temp = file.tarHeaders[i].c_str();
			string temptemp = temp.erase(temp.size() - 1);
			make_directory(test + temptemp);
		} else {
			if (fileOut.is_open()) {
				fileOut << file.tarFileContents[i];
			} else {
				cout << "File could not be opened." << endl << endl;
			}
		}
		fileOut.close();
	}
	cout << "All of the files have been output." << endl;
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

//char curPth() {
//	char cCurrentPath[FILENAME_MAX];
//
//	if (!GetCurrentDir(cCurrentPath, sizeof(cCurrentPath)))
//	{
//		return errno;
//	}
//
//	cCurrentPath[sizeof(cCurrentPath) - 1] = '\0'; /* not really required */
//
//	printf("The current working directory is %s", cCurrentPath);
//}
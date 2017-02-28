//Written by: Blaine Luszcak
#include <cstring>
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
	vector<string> contents, tarHeaders, tarFileContents;
	vector<char> v;
	vector<int> headerLoc, totalFileSize, currFileSize, trackFileSize, outputFiles,
				hdrPrev, hdrPos, dataBlocks, tarFileSizes, tarFileLink;
};

//Prototyping functions
void readHdr(tarHdr &myHdr, ifstream &tarF, contentsFile &file);
void readFile(string fileName, const char * size, contentsFile &file, ifstream &tarF);
void remainingFilesLoop(tarHdr &myHdr, contentsFile &file, ifstream &tarF);
void outputFiles(tarHdr &myHdr, contentsFile &file);
void make_directory(string name);

int main() {

	ifstream infile, nextFile;
	string fileN, usrTarFile, usrFolderChng;
	int size, currentPos, fileLength;

	currentPos = 0;

	tarHdr myHdr; //header struct for each file in the .tar
	contentsFile file; //filecontents struct

	cout << "Which .tar file would you like to extract? (i.e. 'example')" << endl;
	cin >> usrTarFile;
	infile.open(usrTarFile + ".tar");
	if (infile.fail()) {
		cerr << "Couldn't open input file -- exitting" << endl;
	} else {
		cout << ".tar opened" << endl;
	}

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

	make_directory("testFile");

	//get the size of the .tar and then read from the 0th line
	infile.seekg(0, infile.end);
	fileLength = infile.tellg();
	file.totalFileSize.push_back(fileLength); //pushes size of .tar file into vector
	infile.seekg(0, infile.beg); //start reading at the beginning of the file so line 0

	readHdr(myHdr, infile, file); //get your header struct
//readFile(myHdr.name, myHdr.size, file, infile); //take the header info and find the corresponding file in .tar
//	outputFiles(myHdr, file); //Functin call to save first file into created directory, after this the call will only be used in "remainingFilesLoop"
//	remainingFilesLoop(myHdr, file, infile); //loop through the remaining files

	infile.close(); //close the filereader after it's done

	cout << "We made it to the end of the program. " << "\n" << endl;
	printf("Press enter to continue...\n");
	getchar();
	getchar(); //need 2 so it eats the CR and actually pauses
}

//open .tar file and read in the header
void readHdr(tarHdr &myHdr, ifstream &tarF, contentsFile &file) {

	string fileName, folder, hdrLnkflag;
	int ownersID, groupID, modifyTime, checksum, length, hdrCheckSum, headerPos, currSize,
		hdrCnt, hdrStrt, hdrPrev, fileSize, p, pntLoc;
	unsigned int hdrComputedChecksum;
	char * block;
	char folderC, slash;
	bool headers, firstHeader;
	//streampos pos;

	headers = true;
	firstHeader = true;
	currSize = 0;
	hdrCnt = 0;
	hdrStrt = 0;
	hdrPrev = 0;

	file.hdrPrev.push_back(0); //start headerPrevious at 0
//	file.hdrPos.push_back(0); //starting real header vec @ 0 b/c it's always starting @ 0
	while (headers == true) { //can't start "" so need bool

		//Loop to read header and find the next one || starts @ 0 but doesn't auto push it
		tarF.read((char*)&myHdr, sizeof(myHdr));
		fileSize = strtol(myHdr.size, NULL, 8);
		p = ((fileSize + 511) / 512); //algo for skip
		file.dataBlocks.push_back(p); //push # of 512 data blocks before next header
		tarF.seekg(p * 512, (std::ios_base::cur)); //Goes to the NEXT header

		fileName = myHdr.name;
		hdrLnkflag = myHdr.linkflag;
		if (fileName != "") { //apparently you can't do two && !='s
			hdrPrev = tarF.tellg();
			file.hdrPrev.push_back(hdrPrev); //keep track of all header positions
			
			slash = fileName.back(); //get slash in fileName if present
			cout << "Filename: " << fileName << endl;

			/*strtol will parse a string and convert the value to an integer. The 8 in the last argument says to assume it is an octal value * stored in the string. */
			ownersID = strtol(myHdr.uid, NULL, 8);
			groupID = strtol(myHdr.gid, NULL, 8);
			cout << "Owner ID = " << ownersID << "\n" << "Group ID = " << groupID << endl;

			cout << "Fileize = " << fileSize << endl;

			hdrCheckSum = strtol(myHdr.checksum, NULL, 8);
			cout << "Checksum = " << hdrCheckSum << endl;

			//Put spaces in the checksum are before we compute
			for (int i = 0; i < 8; i++) {
				myHdr.checksum[i] = ' ';
			}
			block = (char*)&myHdr;
			hdrComputedChecksum = 0;
			for (int i = 0; i < sizeof(myHdr); i++) {
				hdrComputedChecksum += block[i];
			}
			cout << "Computed Checksum " << hdrComputedChecksum << endl;

			//---------------------basic header information read---------------------------//

			//sorting out dataPositions based on files and folders
			if (hdrLnkflag == "5" || slash == '/') { //if either of these statements true

				//if first thing is a FOLDER push 0 into first element of vector
				if (firstHeader == true) {
					pntLoc = tarF.tellg();
					file.hdrPos.push_back(0);
					file.hdrPos.push_back(pntLoc);
					firstHeader = false;
					hdrCnt += 2;
				} else if (hdrCnt == 1) {
					//int t = *(file.hdrPos.rbegin());
					int t = *(file.hdrPrev.rbegin() + 1);
					file.hdrPos.push_back(t);
					hdrCnt++;
				} else {
					int t = *(file.hdrPrev.rbegin());
					//int t = *(file.hdrPos.rbegin());
					file.hdrPos.push_back(t); //folder headers just move 512
					hdrCnt++;
				}
			}
			//actual file
			else if (hdrLnkflag == "\0" || hdrLnkflag == "0") {

				//if first thing is a FILE ---- header pos has to start at 0
				if (firstHeader == true) {
					pntLoc = tarF.tellg();
					file.hdrPos.push_back(0);
					file.hdrPos.push_back(pntLoc); //--originally commented out //shouldn't we push both in like the top?
					firstHeader = false;
					hdrCnt += 2;
				} else {
					if (p == 1) {
						//if the one before and the current are equivalent don't push in the same thing
						if (*(file.hdrPrev.rbegin() + 1) == file.hdrPos.back()) {
							pntLoc = *(file.hdrPrev.rbegin());
							file.hdrPos.push_back(pntLoc);
							hdrCnt++;
						} else { //not sure we ever hit this case
							pntLoc = *(file.hdrPrev.rbegin() + 1);
							file.hdrPos.push_back(pntLoc);
							hdrCnt++;
						}
					}
					//bigger files > 512
					else {
						pntLoc = *(file.hdrPrev.rbegin()); //previous
						file.hdrPos.push_back(pntLoc);
						hdrCnt++;
					}
				}
			} else {
				cout << "Something went wrong. " << endl;
			}

			file.tarHeaders.push_back(fileName);
			file.tarFileSizes.push_back(fileSize);
			file.tarFileLink.push_back(strtol(myHdr.linkflag, NULL, 8));
		} else {
			cout << "No more headers" << endl;
			file.hdrPrev.pop_back(); //one extra to get rid of now prev = pos = #headers
			file.hdrPos.pop_back();
			headers = false; //break me out of the almighy for loop that starts this shindig
		}	
	}
	cout << "We have read all of the files " << endl;
	cout << "-------------------" << endl;

	for (int i = 0; i < file.hdrPos.size(); i++) {
		cout << "Header: " << file.tarHeaders[i] << endl;
		cout << "File size " << file.tarFileSizes[i] << endl;
		cout << "Starting position: " << file.hdrPos[i] << endl;
		cout << "LinkFlag: " <<  file.tarFileLink[i] << endl;
		cout << "-------------------" << endl;
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
		}
		else {
			temp = file.headerLoc.back() / 512; //bigger than 512 has not been tested
			currentPos = temp * 512;
		}

		//reading
		//infile.read((char*))&myheader, sizeof(myheader));
		//int p = strtol(myheader.filesize,null,8) + 511) /512;
		//infile.seekg(p * 512, std::(ios_base::cur);
		//infile.read((char*)&myheader, sizeof(myheader));

		//read at the correct byte location
		tarFile.seekg(currentPos);
		readHdr(myHdr, tarFile, file);

		//make sure that we have not reached the end of the file
		if (strtol(myHdr.size, NULL, 8) != 0) {
			readFile(myHdr.name, myHdr.size, file, tarFile);
			outputFiles(myHdr, file);
		}
		else {
			break;
		}
	}
}

//output all of the files that we got from the .tar into the folder that we created
void outputFiles(tarHdr &myHdr, contentsFile &file) {
	ofstream fileOut;
	int fileCnt;
	string directoryStart, hdrTitle, dirNoSlash;
	char slash;
	fileCnt = 0;

	directoryStart = "open/";

	for (int i = 0; i < file.tarHeaders.size(); i++) {
		hdrTitle = file.tarHeaders[i];
		slash = hdrTitle.back();

		fileOut.open((directoryStart + hdrTitle.c_str()));
		
		if (slash == '/') {
			dirNoSlash = hdrTitle.erase(hdrTitle.size() - 1);
			make_directory(directoryStart + dirNoSlash);
		} else {
			if (fileOut.is_open()) {
				fileOut << file.tarFileContents[i];
			} else {
				cout << "File could not be opened." << endl;
			}
		}
		fileOut.close(); //not sure where the best place to call this is
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
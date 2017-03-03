//Written by: Blaine Luszcak
#include <cstdlib>
#include <cstring>
#include <string>
#include <fstream>
#include <iostream>
#include <vector> 
//#include <unistd.h> //-------------------------------NEED FOR LINUX-----------
#include <stdio.h>
#include <signal.h>
#include <sys/types.h>

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
	vector<string> contents, tarHeaders, tarFileContents;
	vector<char> v;
	vector<int> hdrPrev, hdrPos, dataBlocks, tarFileSizes, tarFileLink;
};

//Prototyping functions
void readHdr(tarHdr &myHdr, ifstream &tarF, contentsFile &file);
void readFile(contentsFile &file, ifstream &tarF, string usrTarFile);
void outputFiles(tarHdr &myHdr, contentsFile &file, string usrTarFile);
void make_directory(string usrTarFile);

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

	make_directory(usrTarFile);

	readHdr(myHdr, infile, file); 
	readFile(file, infile, usrTarFile);
	outputFiles(myHdr, file, usrTarFile);

	infile.close();

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

	headers = true;
	firstHeader = true;
	currSize = 0;
	hdrCnt = 0;
	hdrStrt = 0;
	hdrPrev = 0;

	file.hdrPrev.push_back(0); //start headerPrevious at 0
	while (headers == true) { 

		//Loop to read header and find the next one || starts @ 0 but doesn't auto push it
		tarF.read((char*)&myHdr, sizeof(myHdr));
		fileSize = strtol(myHdr.size, NULL, 8);
		p = ((fileSize + 511) / 512); //algo for skip
		file.dataBlocks.push_back(p); //push # of 512 data blocks before next header
		tarF.seekg(p * 512, (std::ios_base::cur)); //Goes to the NEXT header

		fileName = myHdr.name;
		hdrLnkflag = myHdr.linkflag;
		if (fileName != "") { 
			hdrPrev = tarF.tellg();
			file.hdrPrev.push_back(hdrPrev);
			
			slash = fileName.back(); //get slash in fileName if present
			cout << "Filename: " << fileName << endl;

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
					int t = *(file.hdrPrev.rbegin() + 1);
					file.hdrPos.push_back(t);
					hdrCnt++;
				} else {
					int t = *(file.hdrPrev.rbegin());
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
			file.dataBlocks.pop_back();
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
void readFile(contentsFile &file, ifstream &tarF, string usrTarFile) {

	int fileSize, pos;
	ifstream currFile;
	string fileName, s;
	const char *f;
	char slash;

	for (int i = 0; i < file.tarHeaders.size(); i++) {
		fileName = file.tarHeaders[i];
		slash = fileName.back();
		fileSize = file.tarFileSizes[i];

		currFile.open(usrTarFile + "/" + (string)fileName);
		if (tarF.fail()) {
			cerr << "Couldn't open input file -- exiting" << endl;
		}  

		//if we have folder, do nothing -- if we have file seek and read
		if (file.tarFileLink[i] == 5 || slash == '/') {
			cout << "Folder: " << fileName << endl;
			pos = file.hdrPos[i];
			tarF.seekg(pos); 
		} else { 
			//
			if (file.hdrPos[i] == 0) {
				pos = file.hdrPos[i] + (file.dataBlocks[i] * 512);
				tarF.seekg(pos);
				file.v.resize(file.tarFileSizes[i]);
				tarF.read(&file.v[0], file.tarFileSizes[i]);
			} else {
				pos = file.hdrPos[i] + 512;
				tarF.seekg(pos);
				file.v.resize(file.tarFileSizes[i]);
				tarF.read(&file.v[0], file.tarFileSizes[i]);
			}
		}

		s = string(file.v.begin(), file.v.end()); //read contents of v we just got into a string
		file.tarFileContents.push_back(s);
		file.v.clear();
	}
	currFile.close();
}

//output all of the files that we got from the .tar into the folder that we created
void outputFiles(tarHdr &myHdr, contentsFile &file, string usrTarFile) {
	ofstream fileOut;
	int fileCnt;
	string directoryStart, hdrTitle, dirNoSlash;
	char slash;
	fileCnt = 0;
	//pid_t pid; -----------------------NEED FOR LINUX----------------

	directoryStart = usrTarFile + "/";

	for (int i = 0; i < file.tarHeaders.size(); i++) {
		//pid = fork(); -----------------------------NEED FOR LINUX-----------
		
		hdrTitle = file.tarHeaders[i];
		slash = hdrTitle.back();

		fileOut.open((directoryStart + hdrTitle.c_str()));
		
		if (slash == '/') {
			dirNoSlash = hdrTitle.erase(hdrTitle.size() - 1);
			make_directory(directoryStart + dirNoSlash + "/");
		} else {
			if (fileOut.is_open()) {
				fileOut << file.tarFileContents[i];
				//sleep(3); //-----------------------NEED FOR LINUX----------
				//kill(pid, SIGKILL);---------------NEED FOR LINUX
			} else {
				cout << "File could not be opened." << endl;
			}
		}
		fileOut.close();
	}
}

//Make a directory in both windows and linux
void make_directory(string usrTarFile) {
	string mk;
	string path;

	mk = "mkdir ";
	//mk = "mkdir -p "; // -----------NEED THIS ONE FOR LINUX-----------
	path = mk.append(usrTarFile);
	const char *c = path.c_str();

	system(c); 
}
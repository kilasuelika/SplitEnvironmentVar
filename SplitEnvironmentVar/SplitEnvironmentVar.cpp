// SplitEnvironmentVar.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include <cstdlib>
#include <stdlib.h>
#include<string>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/algorithm/string/replace.hpp>
#include<fstream>
using namespace std;
namespace fs = boost::filesystem;

string AssembleSetPath(string &name, string &path) {
	string temppath = path;
	//boost::replace_all(temppath,"%","^%");
	return "SETX /m " + name + " \"" + temppath +"\"";
};
vector<string> SplitVars(string &Varstr) {
	string tempvar;
	vector<string> varvec;
	for (auto ch : Varstr) {
		tempvar.push_back(ch);
		if (ch == ';') {
			varvec.push_back(tempvar);
			tempvar.clear();
		}
	};
	if (tempvar.size() > 1) {
		varvec.push_back(tempvar);
	}
	if (varvec.back().back() != ';')varvec.back().push_back(';');
	return varvec;
};

int main()
{
	char * env_p;
	size_t len;
	_dupenv_s(&env_p, &len, "path");
	//cout << "Your PATH is: " << env_p << '\n';
	//Backup;
	string BackupEnv{ env_p };
	
	BackupEnv = "pathbkp=" + BackupEnv;
	_putenv(BackupEnv.c_str());
	cout << "Backup: EnvBackup.txt..." << endl;
	ofstream Bkpfs("EnvBackup.txt");
	Bkpfs << BackupEnv;
	cout << "Finished." << endl;

	//Clean empty path.
	string tempvar;
	string env_ps{ env_p };
	string newenv;
	for (auto ch:env_ps) {
		if (ch != ';') {
			tempvar.push_back(ch);
		}
		else {
			if (fs::exists((fs::path)tempvar) || tempvar[0] == '%') {
				newenv = newenv + tempvar+";";
			};
			tempvar.clear();
		};
	};
	newenv=newenv+tempvar;
	if (newenv.back() != ';')newenv += ';';
	
	//Write the new env to a file.
	cout << "Writing cleaned environment variables file: CleanedEnv.txt..." << endl;
	ofstream envfs("CleanedEnv.txt");
	envfs << newenv;
	cout << "Finished." << endl;

	//Generate a bat file.
	cout << "Generating batch file: RunCleanEnv.bat..." << endl;
	ofstream batfile("RunCleanEnv.bat");
	tempvar.clear();
	int npathfile = 0;
	string name;
	vector<string> varvec = SplitVars(newenv);
	cout << "There are " << varvec.size() << " paths." << endl;
	for (int i = 0; i < varvec.size();) {
		if (tempvar.size() + varvec[i].size() > 1000) {
			name = "path" + to_string(npathfile);
			npathfile += 1;
			batfile<<AssembleSetPath(name , tempvar)<<endl<<endl;
			tempvar.clear();
		}
		else {
			tempvar += varvec[i];
			i++;
		};
	};

	name = "path" + to_string(npathfile);
	batfile << AssembleSetPath(name, tempvar) << endl << endl;
	//Write main path.
	batfile << "SETX /m path \"%SystemRoot%;%SystemRoot%\\system32;";
	for (int i = 0; i <= npathfile; i++) {
		batfile << "%%path" << i << "%%;";
	};
	batfile << "\"";
	cout << "The path has been splitted into " << npathfile + 1 << " files." << endl;
	cout << "Finished." << endl;
	return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file

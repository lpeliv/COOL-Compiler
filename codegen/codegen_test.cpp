#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>
#include <algorithm>
#include <dirent.h>
#include <chrono>
#include <thread>

using namespace std;


// opis: provjeravam je li datoteka postoji
// pre:		name je ime datotekexstr1
// post:	vraća istinu ako postoji
bool doesFileExists(const std::string& name) {

    ifstream f(name.c_str());
    bool exists = f.good();
	f.close();
	return exists;
}

// opis: provjeravam je li xstr1 ima filename odnosno znak ':'
// pre:		xstr1
// post:	ako je u xstr1 ':' onda makni sve livo od najdesnije ':0 i nju
void checkFilenameDoubleDot(string& xstr1) {
	size_t pos1;
	string xstr2;
	pos1 = xstr1.find(".cl:");
	if(pos1 != string::npos) {
// ako je onda trazi desnu ':'
		size_t pos2;
		xstr1 = xstr1.substr(pos1+4, xstr1.length());
		for(auto it : xstr1) {
			if( !(it >= '0' && it <= '9')) 
				xstr2.push_back(it);				
		}
		xstr1 = xstr2;
	}
}


int main() {
// za citanje sadrzaja direktorija
	DIR* dir;
	struct dirent* ent;
// vektor sa imenima datoteka	
	vector<string> files;
// stringovi za obradu imena datotkea	
	string filename, filename_results, comm;
// za trazenje tocke	
	size_t pos, pos1, pos2;
	
// brisem i ponovo stvaram poddirektorij results	
	system("rm -r ./test_files/res");
	system("mkdir ./test_files/res");
// ako postoji folder test_files ulazim u njega	
	if ((dir = opendir ("test_files")) != NULL) {
// petlja koja prolazi kroz sve datoteke iz foldera	
		while ((ent = readdir (dir)) != NULL) {
			filename = string(ent->d_name);
// filtriram ono sta nisu datoteke			
			pos = filename.find(".");
			if(pos != string::npos && pos!=0) {
				filename = filename.substr(0,pos);
				if(find(files.begin(), files.end(), filename) == files.end()) { 
					files.push_back(filename);
				}
			}
		}
		closedir (dir);
	} else {
		/* could not open directory */
		perror ("");
		return EXIT_FAILURE;
	}
	
	int nofiles=1;
	int nok=0;
	string parser_ok;
	cout << endl << "ERRORS IN:" << endl;
// za svaku testnu datoteku	
	sort(files.begin(), files.end());
	for(auto it : files) {
		nofiles++;
// ime testne datoteke		
		filename = ".//test_files//" + it;
// ime izlazne datoteke u folderu results		
		filename_results = ".//test_files//res//" + it;

		do {
			do {
// string sa naredbom za sve faze i izlazom i stderr u datoteku
				comm = "./lexer "+filename+         ".cl >" + filename_results + ".lx";
				system(comm.c_str());
				comm = "./parser <"+filename_results+".lx >" + filename_results + ".ps";
				system(comm.c_str());
				comm = "./semant <"+filename_results+".ps >" + filename_results + ".sm";
				system(comm.c_str());
				ifstream pf(filename_results + ".sm");
				getline(pf, parser_ok);
				pf.close();
			} while( parser_ok == "-" );
			comm = "./codegen <"+filename_results+".sm >" + filename_results + ".s";
			system(comm.c_str());
// proveravam je li došlo do greške u parsiranju AST-a
		} while(!doesFileExists(filename_results+".s"));

// string sa naredbom za spim i izlazom i stderr u datoteku
		comm = "spim -file "+filename_results+".s >"
		+filename_results+".spim.myout 2>&1";
		system(comm.c_str());
		
// otvaram za usporedbu datoteku sa ispravnim izlazom i sa izlazom koji je gornja naredba stvorila
		ifstream out(filename+".spim.out");
		ifstream myout(filename_results+".spim.myout");
// otvaram datoteku u kojoj cu spremiti razlike		
		ofstream diff(filename_results+".diff");
		if( !out || !myout || !diff ) {
			cout << "Error opening test files" << endl;
			return EXIT_FAILURE;
		}

// zapocinjem sa usporedbom		
		string mystr1, str1;
		string mystr2, str2;
		string mystr3, str3;
		string mystr4, str4;
		int lineno=1;
// preskacem prvih 5 linija za out
		for(int i = 0 ; i<5 ; ++i) 
			getline(out, str1);
// preskacem prvih 7 linija za myout
		for(int i = 0 ; i<8 ; ++i) 
			getline(myout, mystr1);
// je li datoteka ispravna		
		bool ok = true;
		getline(out, str1);
		if(str1.find("Increasing heap...") != std::string::npos ) {
			getline(out, str1);
		}
		getline(myout, mystr1);
		if(mystr1.find("Increasing heap...") != std::string::npos ) {
			getline(myout, mystr1);
		}
		do {
// provjeravam da nema ime datoteke u stringu
			checkFilenameDoubleDot(mystr1);
			checkFilenameDoubleDot(str1);
// usporedjujem			
			if(mystr1!=str1) {
				ok = false;
				diff << "error in line# " << lineno << endl;
				diff << "    parsed as: " << mystr1 << endl;
				diff << "    should be: " << str1 << endl;
				cout << "error in line# " << lineno << endl;
				cout << "    parsed as: " << mystr1 << endl;
				cout << "    should be: " << str1 << endl;
			} else 
		
			lineno++;

// citam novi red
		getline(out, str1);
		if(str1.find("Increasing heap...") != std::string::npos ) {
			getline(out, str1);
		}
		getline(myout, mystr1);
		if(mystr1.find("Increasing heap...") != std::string::npos ) {
			getline(myout, mystr1);
		}

		} while(myout || !mystr1.empty());
// je li izlazna datoteka ispravna		
		if(ok) {
			nok++;
			cout << "       ok #"<< nofiles <<" : " << it << endl;
			using namespace std::chrono_literals; // ns, us, ms, s, h, etc.
			// std::this_thread::sleep_for(std::chrono::milliseconds(500));
		} else {
// ispisi redni broj i ime datoteke	
			cout << "    error #"<< nofiles <<" : " << it << endl;
		} 
		
		myout.close();
		out.close();
		diff.close();
	}	
	cout << "_____________________________" << endl << "Grade: " << nok << "/" << --nofiles << endl;
	return 0;
}
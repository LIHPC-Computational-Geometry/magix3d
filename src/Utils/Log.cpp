/*
 *  Eric B le 23/5/2019
 */
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <sys/utsname.h>
#include "TkUtil/Log.h"
/*----------------------------------------------------------------------------*/
namespace Mgx3D {
//---------------------------------------------------------------------------------------
namespace Utils {
//---------------------------------------------------------------------------------------

extern "C" void maillage_log(int argc, char *argv[], const char* nom_proj)
{
	// qui lance cette application
	std::string nom_lanceur;
	if (argc == 0)
		nom_lanceur = std::string("python");
	else {
		std::string full_nom_proj(argv[0]);
		int pos = full_nom_proj.find_last_of('/');
		if (pos >= full_nom_proj.size())
			return;
		nom_lanceur = full_nom_proj.substr(pos+1,full_nom_proj.size()-1);
	}

	// version du projet
	const char* version = MAGIX3D_VERSION;	// GSCC_PROJECT_VERSION;

	// où il faut trouver le fichier de logues
	std::string log_file;
	const char* c_log_file = getenv("STDENV_PATH_LOG");
	if (c_log_file == 0){
		// on tente une reconstruction à partir du HOME
		const char* home = getenv("HOME");
		if (home == 0)
			return;

		std::string str_home(home);
		int pos = str_home.find("/home/");
		if (pos >= str_home.size())
			return;
		log_file = str_home.substr(0,pos+5) + std::string("/maillage/maillage");
	}
	else
		log_file = std::string(c_log_file);

	// l'OS
	const char* c_os_name = getenv("STDENV_FULLOS");
	std::string os_name;
	if (c_os_name)
		os_name = std::string(c_os_name);
	else {
		struct utsname unameData;
		int ret =  uname(&unameData);
		if (ret)
			os_name = std::string("OS_UNKNOWN");
		else
			os_name = std::string(unameData.sysname) + std::string("-") + std::string(unameData.release);
	}

	// date et heure
	char begin_date[128];
	time_t timeval;
	time(&timeval);
	strftime(begin_date,128,"%d/%m/%y %H:%M",localtime(&timeval));

	// le user
	char user_name[128];
	int ret = getlogin_r(user_name, 128);
	if (ret || !user_name)
		strcpy(user_name,"USER_UNKNOWN");

	// le fichier de logue avec son path
	std::string log_file_path = log_file + std::string("/loglist/") + nom_proj + std::string(".log");
	//std::cout<<"log_file_path = "<<log_file_path<<std::endl;
	std::ofstream file(log_file_path.c_str(),std::ios::app);
	if (!file)
		return;

	// on ajoute au fichier
	file << user_name << ' ' << version << ' ' << os_name << ' ' << begin_date << ' ' << nom_lanceur << "\n";

}
//---------------------------------------------------------------------------------------
} // end namespace Utils
/*----------------------------------------------------------------------------*/
} // end namespace Mgx3D
//---------------------------------------------------------------------------------------


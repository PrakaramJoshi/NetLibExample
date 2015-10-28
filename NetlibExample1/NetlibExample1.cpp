// NetlibExample1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <boost/network/protocol/http/client.hpp>
#include <iostream>
#include "Logger.h"
#include "Crawler.h"
#include <Geography\Cities.h>
#include <Datastructures\parallel.h>
#include "app_settings.h"



bool read_app_settings(app_settings &_apps){
	std::string app_setting_file = R"(B:\Workspace\NetlibExample1\app_settings.txt)";
	AceLogger::Log("Reading app settings file at : " + app_setting_file);
	return _apps.read_settings(app_setting_file);
}

using namespace AceLogger;
int main(int argc, char *argv[]) {

	REGISTER_LOGGER("B:\\Logs", "NetLibExample", "0.0.0.1");
	app_settings apps;
	if (!read_app_settings(apps)){
		Log("Cannot continue as unable to read settings file.", LOG_ERROR);
		exit(0);
	}
	using namespace boost::network;
	Crawler crawler;
	
	std::string file_name = AceLogger::time_stamp_file_name();
	std::string file_dir = apps.p_output_dir;
	std::string file_path = file_dir+"\\"+file_name;
	if (!AceLogger::create_dir(file_dir)){
		Log("unable to create output dir", LOG_ERROR);
		file_path = "";
	}
	if (!apps.p_save_data)
		file_path = "";
	//crawler.Run("http://en.wikipedia.org/wiki/Main_Page",".*wikipedia\.org.*", file_path,false);
	//crawler.Run("http://www.cnn.com/", ".*wsj\.com.*", file_path,false);
	//crawler.Run("http://online.wsj.com", ".*wsj\.com.*", file_path,true);
	//crawler.Run("http://timesofindia.indiatimes.com/international-home", ".*indiatimes\.com.*", file_path, true);

	crawler.Run(apps.p_starting_web_address, apps.p_stay_within_pattern, file_path, apps.p_run_as_drain);

	return 0;
}



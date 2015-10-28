#ifndef APP_SETTINGS_H
#define APP_SETTINGS_H
#include <string>
#include <vector>
#include <boost\program_options.hpp>
#include <Logger\Logger.h>
#include <boost\filesystem.hpp>
#include <boost\algorithm\string.hpp>

struct app_settings{
	std::string p_starting_web_address;
	std::string p_stay_within_pattern;
	std::string p_output_dir;
	bool p_run_as_drain;
	bool p_save_data;

	app_settings(){
		p_starting_web_address;
		p_stay_within_pattern;
		p_output_dir;
		p_run_as_drain;
		p_save_data;
	};

	bool read_settings(const std::string &_settings_file){
		try
		{
			std::vector<std::string> options;
			if (!read_file(_settings_file, options)){
				AceLogger::Log("Cannot read settings as the settings file not found.", AceLogger::LOG_ERROR);
				return false;
			}

			namespace po = boost::program_options;
			po::options_description desc("Options");

			std::string run_as_drain;
			std::string save_output;

			desc.add_options()
				("help", "Print help messages")
				("start_web_address", po::value<std::string>(&p_starting_web_address)->default_value("http://online.wsj.com"),"starting web address(seed address)")
				("stay_within", po::value<std::string>(&p_stay_within_pattern)->default_value(".*wsj\.com.*"), "pattern of web address to stay within")
				("output_dir", po::value<std::string>(&p_output_dir)->default_value("B:\\TestingData"), "output dir to save the downloaded data")
				("run_as_drain", po::value<std::string>(&run_as_drain)->default_value("No"), "do not send data to the ipc Q")
				("save_output", po::value<std::string>(&save_output)->default_value("No"), "save the output data in the output dir");

			po::variables_map vm;
			try
			{
				po::store(po::command_line_parser(options).options(desc).run(),
					vm); 
				if (vm.count("help"))
				{
					AceLogger::Log("Help not available", AceLogger::LOG_WARNING);
					return false;
				}

				po::notify(vm); 

				if (!convert_to_bool(run_as_drain, p_run_as_drain))
					return false;
				if (!convert_to_bool(save_output, p_save_data))
					return false;
				return validate_settings();
			}
			catch (po::error& e)
			{
				AceLogger::Log(e.what(), AceLogger::LOG_ERROR);
				return false;
			}


		}
		catch (std::exception& e)
		{
			AceLogger::Log("Unhandled Exception", AceLogger::LOG_ERROR);
			AceLogger::Log(e.what(), AceLogger::LOG_ERROR);
			return false;

		}
	}
private:
	bool read_file(const std::string &_file_path,
		std::vector<std::string> &_data)const{
		if (!boost::filesystem::exists(boost::filesystem::path(_file_path))){
			AceLogger::Log("file doesn't exist: " + _file_path, AceLogger::LOG_ERROR);
			return false;
		}
		std::ifstream ifs(_file_path);
		std::string temp;
		while (std::getline(ifs, temp)){
			if (!temp.empty()){
				if (temp[0] != '#')
					_data.push_back(temp);
			}
			
		}
		ifs.close();
		return true;
	};

	bool convert_to_bool(std::string _str, bool &_value){
		boost::trim(_str);
		boost::to_lower(_str);
		if (boost::iequals("no", _str)){
			_value = false;
			return true;
		}
		else if (boost::iequals("yes", _str)){
			_value = true;
			return true;
		}
		AceLogger::Log("Invalid value provided, should be just No or Yes", AceLogger::LOG_ERROR);
		return false;
	}

	bool validate_settings()const {
		bool returnVal = true;
		if (returnVal)
			AceLogger::Log("input data validated");

		return returnVal;
	}
};
#endif
#include "stdafx.h"
#include "PostParser.h"
#include <Logger.h>

using namespace AceLogger;

PostParser::PostParser(BlockingQueue<std::string> &_datain,
		const std::string &_out_file_path) :m_datain(_datain), m_outfile_path(_out_file_path){
	AceLogger::Log("Post Parser stage created");
};

PostParser::~PostParser(){
}

void PostParser::Run(bool _drain){
	if (_drain){
		Log("Running as a drain....");
		std::string *nextdata;
		while (m_datain.Remove(&nextdata)){
			delete nextdata;
		}
	}
	else{
		Log("NOT running as a drain....");
		std::string *nextdata;
		std::ofstream ofs;
		bool save_data = true;
		if (m_outfile_path == "")
			save_data = false;
		else
			ofs.open(m_outfile_path);

		//IPCMessageQ<std::string> ipcQ("Q1", true);
		IPCBufferSocketTCP<std::string> ipcQ("127.0.0.1",13, true);
		//IPCBufferSocketUDP<std::string> ipcQ("127.0.0.1", 1200, true);
		while (m_datain.Remove(&nextdata)){
			if (nextdata){
				m_html.set_text(*nextdata);
				std::vector<std::string > plain_text;
				m_html.plain_text(plain_text);
				(*nextdata).clear();
				(*nextdata) = "";
				for (auto s : plain_text)
					(*nextdata) += s;
				if (save_data)
					ofs << *nextdata << std::endl;
				ipcQ.Send(nextdata);
			}

		}
		if (ofs.is_open())
			ofs.close();
	}

}

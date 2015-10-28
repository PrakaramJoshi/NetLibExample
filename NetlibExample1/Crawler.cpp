#include "stdafx.h"
#include "Crawler.h"
#include <boost\thread.hpp>
#include <boost\bind\bind.hpp>
#include <Logger.h>
#include "Schedular.h"

using namespace AceLogger;

Crawler::Crawler(){
	AceLogger::Log("Crawler created");
}

Crawler::~Crawler()
{
}

void Crawler::Run(const std::string &_sourceUrl,
				const std::string &_stayWithin,
				const std::string &_out_file_path,
				bool _post_parser_drain){

	Schedular schedulr(m_urls);
	boost::thread_group schedularThread;
	schedularThread.create_thread(boost::bind(boost::mem_fn(&Schedular::Run), &schedulr, boost::ref(_sourceUrl), boost::ref(_stayWithin), boost::ref(_out_file_path),_post_parser_drain));
	//std::thread schedularThread(&Schedular::Run, &schedulr, _sourceUrl);
	std::string *nextUrl;
	unsigned long long count = 0;
	while (m_urls.Remove(&nextUrl)){
		if (nextUrl){
			count++;
			//Log(*nextUrl,AceLogger::LOG_STATUS,LOG_TYPE::DISPLAY);
			if (count % 10000==0)
				std::cout << "Identified " << count <<"..."<< std::endl;
			delete nextUrl;
		}
	}
	schedularThread.join_all();
}
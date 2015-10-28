#include "stdafx.h"
#include "Schedular.h"
#include <Logger.h>
#include<vector>
#include<unordered_set>
#include <chrono>
#include <boost\bind\bind.hpp>
#include "Parser.h"
#include "Fetcher.h"
#include "RobotsTXT.h"
#include "PostParser.h"

using namespace AceLogger;
Schedular::Schedular(BlockingQueue<std::string> &_urlsUnique) :m_unqiueUrls(_urlsUnique){
	AceLogger::Log("Schedular created");
};

Schedular::~Schedular(){

};

void Schedular::Run(const std::string &_source,
	const std::string &_stayWithin,
	const std::string &_out_file_path,
	bool _post_parser_drain){
	BlockingQueue<std::string> urlsForFetcher(300);
	BlockingQueue<std::string> urlsFromParser;
	BlockingQueue<std::string> urlsFromRobotsTXT;
	BlockingQueue<std::string> data;
	BlockingQueue<std::string> data_from_parser;

	Counter<unsigned long> pages_visited;
	//SYSTEM_INFO sysinfo;
	//GetSystemInfo(&sysinfo);

	int numCPU = 30; //sysinfo.dwNumberOfProcessors;
	urlsForFetcher.Insert(new std::string(_source));
	std::vector<Fetcher*> fetchers;
	std::vector<Parser*> parsers;
	for (int i = 0; i < numCPU; i++){
		Fetcher *fetcher = new Fetcher(urlsForFetcher, data, pages_visited);
		Parser *parser = new Parser(data, urlsFromParser, data_from_parser, _stayWithin);
		fetchers.push_back(fetcher);
		parsers.push_back(parser);
	}

	boost::thread_group fetchParserThreads;
	for (int i = 0; i < numCPU; i++){
		fetchParserThreads.create_thread(boost::bind(boost::mem_fn(&Parser::Run), parsers[i]));
		fetchParserThreads.create_thread(boost::bind(boost::mem_fn(&Fetcher::Run), fetchers[i]));
	}

	RobotsTXT robotsTXT(urlsFromParser, urlsFromRobotsTXT);
	fetchParserThreads.create_thread(boost::bind(boost::mem_fn(&RobotsTXT::Run), &robotsTXT));

	PostParser postPraser(data_from_parser, _out_file_path);
	fetchParserThreads.create_thread(boost::bind(boost::mem_fn(&PostParser::Run), &postPraser, _post_parser_drain));

	auto t_start = std::chrono::high_resolution_clock::now();

	std::unordered_set<std::string> founduri;
	std::string *nexturi;
	std::string msg = "Downloaded pages: ";
	auto count = pages_visited.val();
	char rate[256];
	rate[255] = '\0';
	while (urlsFromRobotsTXT.Remove(&nexturi)){
		if (nexturi){
			if (founduri.find(*nexturi) == founduri.end()){
				founduri.insert(*nexturi);
				m_unqiueUrls.Insert(new std::string(*nexturi));
				auto qsize = urlsForFetcher.Insert(nexturi);
				if (qsize>300){
					Log("Fetch queue size " + boost::lexical_cast<std::string>(qsize));
				}
				auto count2 = pages_visited.val();
				if (count2 != count&& count2 % 100 == 0){
					count = count2;
					auto t_end = std::chrono::high_resolution_clock::now();
					auto duration = std::chrono::duration<double, std::milli>(t_end - t_start).count();
					auto pageRate = (count*1000.0) / duration;
					sprintf_s(rate, 255, " [PPS: %3.2f]", pageRate);
					Log(msg + boost::lexical_cast<std::string>(count)+std::string(rate));

				}
			}
			else
				delete nexturi;
		}
	}
	fetchParserThreads.join_all();
	for (auto p : parsers)
		delete p;
	for (auto f : fetchers)
		delete f;
	Log("Schedular stops...");
}
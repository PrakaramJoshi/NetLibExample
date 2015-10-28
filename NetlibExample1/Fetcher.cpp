#include "stdafx.h"
#include "Fetcher.h"
#include <Logger.h>
#include <boost/network/protocol/http/client.hpp>

using namespace AceLogger;
using namespace boost::network;

Fetcher::Fetcher(BlockingQueue<std::string> &_urlsin,
				BlockingQueue<std::string> &_dataout,
				Counter<unsigned long>&_counter) :m_urlsin(_urlsin), m_dataout(_dataout), m_counter(_counter){
	AceLogger::Log("Fetcher created");
}

Fetcher::~Fetcher(){
}

void Fetcher::Run(){
	// fetches data for the uris in the queue
	std::string *nexturi;
	http::client client;

	//http::basic_client<http::tags::http_default_8bit_udp_resolve,1U,1U> client;
	while (m_urlsin.Remove(&nexturi)){
		if (nexturi){

			if (boost::contains(*nexturi, "http:")){

				try{

					http::client::request request(*nexturi);
					request << header("Connection", "close");
					http::client::response response = client.get(request);
					std::string prefix = boost::lexical_cast<std::string>((*nexturi).length()) + ":" + (*nexturi) + ":";
					std::string *msgVal = new std::string(prefix + std::string(body(response)));
					m_counter++;
					m_dataout.Insert(msgVal);
				}
				catch (...){
				}

			}

			delete nexturi;
		}
	}
	Log("Fetcher takes a break...");
}
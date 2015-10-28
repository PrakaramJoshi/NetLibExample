#include "stdafx.h"
#include "Parser.h"
#include <Logger.h>
#include <StringUtils.h>
#include <regex>

using namespace AceLogger;

Parser::Parser(BlockingQueue<std::string> &_datain,
				BlockingQueue<std::string> &_urlsout,
				BlockingQueue<std::string> &_dataout,
				const std::string &_stayWithin) :m_urlsout(_urlsout), m_datain(_datain), m_stayWithin(_stayWithin), m_dataout(_dataout) {
	AceLogger::Log("Parser created");
}

Parser::~Parser(){
}

void Parser::get_parent(const std::string &_data,
	std::string &_parent){
	std::string parentLength = "";
	int index = 0;
	for (auto c : _data){
		if (c != ':'){
			parentLength += c;
		}
		else
			break;
		index++;
	}
	int parent_length = boost::lexical_cast<int>(parentLength);
	index += 1;
	std::string parent;
	for (int i = index; i < index + parent_length; ++i){
		if (_data[i] == '/' && (i + 1)<(index + parent_length) && _data[i + 1] != '/'&&index >0 && _data[i - 1] != '/')
			break;
		_parent += _data[i];
	}
}

void Parser::Run(){
	//The work horse which finds the uris from text

	std::string *nextdata;
	std::smatch m;
	std::regex e(R"(\<a\shref\s*=\s*"[^"]*")");
	std::regex estayWithin(m_stayWithin);
	while (m_datain.Remove(&nextdata)){
		if (nextdata){
			std::string parent = "";
			std::string str = *nextdata;
			m_dataout.Insert(nextdata);
			nextdata = nullptr;
			get_parent(str, parent);
			while (std::regex_search(str, m, e)) {
				for (auto x : m) {
					std::string str(x);
					std::vector<std::string> tokens;
					StringUtils::split(str, tokens, '"');
					if (tokens.size() >= 3){
						if (tokens[1][0] == '/'){
							//relative address
							if (tokens[1][1] == '/'&&tokens[1][0] == '/')
								tokens[1] = "http:" + tokens[1];
							else
								tokens[1] = parent + tokens[1];

						}
						if (tokens[1][0] != '#'){

							std::smatch m2;
							if (std::regex_search(tokens[1], m2, estayWithin)) {
								m_urlsout.Insert(new std::string(tokens[1]));
							}

						}
					}

				}
				str = m.suffix().str();
			}
		}
	}
	Log("Work horse sleeps...");
}
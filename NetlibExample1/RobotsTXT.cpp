#include "stdafx.h"
#include "RobotsTXT.h"
#include <Logger.h>
#include <StringUtils.h>
#include <boost/network/protocol/http/client.hpp>
#include <sstream>

using namespace AceLogger;
using namespace boost::network;

RobotsTXT::RobotsTXT(BlockingQueue<std::string> &_unqiueUrlsIn,
	BlockingQueue<std::string> &_unqiueUrlsOut) :m_unqiueUrlsIn(_unqiueUrlsIn), m_unqiueUrlsOut(_unqiueUrlsOut){
	AceLogger::Log("Robots Txt reader created");
	m_badUrls.insert("http://games.cnn.com/");
};

RobotsTXT::~RobotsTXT(){
};

std::string RobotsTXT::GetRelativeAddress(const std::string &_website)const{
	std::string website = "";

	int count = 3;
	for (auto iter = _website.begin(); iter != _website.end(); ++iter){
		if ((*iter) == '/' || (*iter) == '?'){
			count--;
		}
		if (count <= 0)
			website += (*iter);
	}
	return website;
}

std::string RobotsTXT::GetParentWebSite(const std::string &_website)const{
	std::string website = "";

	int count = 3;
	for (auto iter = _website.begin(); iter != _website.end() && count; ++iter){
		if ((*iter) == '/' || (*iter) == '?'){
			count--;
		}
		if (count)
			website += (*iter);
	}
	if (website != "")
		website += "/";
	return website;
}

bool RobotsTXT::IsNewWebsite(const std::string &_website)const{
	return m_disAllowed.find(_website) == m_disAllowed.end();
}

void RobotsTXT::FetchRobotsTXT(const std::string &_website,
	std::string &_parentWebsite){
	if (boost::contains(_website, "http:")){
		_parentWebsite = GetParentWebSite(_website);
		boost::to_lower(_parentWebsite);
		if (!IsNewWebsite(_parentWebsite))
			return;
		m_disAllowed[_parentWebsite].insert("------------------------------");

		std::string robotsTxtAddress = _parentWebsite + "robots.txt";
		Log("Fetching robots.txt for " + robotsTxtAddress);
		if (m_badUrls.find(_parentWebsite) != m_badUrls.end())
			return;
		http::client client;
		try{

			http::client::request request(robotsTxtAddress);
			//request << header("Connection", "close");
			http::client::response response = client.get(request);
			std::stringstream str;
			str << body(response);
			std::vector<std::string> robots_msg;
			std::string tmp;
			bool first = true;
			while (std::getline(str, tmp)){
				if (first){
					first = false;
				}
				else
					robots_msg.push_back(tmp);
			}
			if (robots_msg.empty())
				return;
			robots_msg.pop_back();
			ParseRobotsTXT(robots_msg, _parentWebsite);
		}
		catch (...){
			Log("Unable to fetch the robots.txt from " + robotsTxtAddress, AceLogger::LOG_ERROR);
		}

	}
}

OPNAME RobotsTXT::GetOpName(const std::string &_str)const{
	std::string trimmed = _str;
	boost::algorithm::trim(trimmed);
	for (int i = 0; i < OPNAME::UNKNOWN_OP; i++){
		if (boost::iequals(trimmed, op[i]))
			return OPNAME(i);
	}
	return OPNAME::UNKNOWN_OP;
}

void RobotsTXT::ParseLine(const std::string &_line,
	OPNAME &_opname,
	std::string &_value)const{
	std::vector<std::string> tokens;
	StringUtils::split(_line, tokens, ':');
	if (tokens.size() == 2){
		_opname = GetOpName(tokens[0]);
		_value = tokens[1];
		boost::algorithm::trim(_value);
	}
	else
		_opname = OPNAME::NO_OP;
}

void RobotsTXT::ParseRobotsTXT(std::vector<std::string> &_robots_msg,
	const std::string &_parentWebsite){
	//TODO
	bool iscurrentUserAgent = false;
	for (auto s : _robots_msg){
		if (!boost::contains(s, "#")){
			OPNAME opname = OPNAME::UNKNOWN_OP;
			std::string opValue = "";
			ParseLine(s, opname, opValue);
			switch (opname){
			case OPNAME::USER_AGENT:
				if (opValue == "*")
					iscurrentUserAgent = true;
				break;
			case OPNAME::DISALLOW:
				if (iscurrentUserAgent){
					m_disAllowed[_parentWebsite].insert(opValue);
					//Log("Dis Allowed: " + _parentWebsite +" --- "+ opValue, AceLogger::LOG_WARNING);
				}
				break;
			case OPNAME::ALLOW:
				//TODO not currently implemented
				Log("The following might be not visted " + s);
				break;
			case OPNAME::SITEMAP:
				//TODO
				break;
			case OPNAME::CRAWL_DELAY:
				//TODO
				break;
			case OPNAME::UNKNOWN_OP:
				//Log("Unknown robots.txt token " + s);
				break;
			case OPNAME::NO_OP:
				//TODO
				//Probably no robots.txt found

				break;
			}
		}
	}
}

void RobotsTXT::Run(){
	std::string *nextdata;
	while (m_unqiueUrlsIn.Remove(&nextdata)){
		if (nextdata){
			boost::algorithm::trim(*nextdata);
			std::string parent_website;
			FetchRobotsTXT(*nextdata, parent_website);
			auto iter = m_disAllowed.find(parent_website);
			bool can_scan_page = true;
			if (iter != m_disAllowed.end()){
				std::string relative_address = GetRelativeAddress(*nextdata);
				for (auto innerIter = iter->second.begin(); innerIter != iter->second.end() && can_scan_page; ++innerIter){
					if (boost::contains(relative_address, *innerIter))
						can_scan_page = false;
				}
			}
			if (can_scan_page)
				m_unqiueUrlsOut.Insert(nextdata);
			else{
				//Log("Cannot SCAN " + *nextdata);
				delete nextdata;
			}
		}
	}
}
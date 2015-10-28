#ifndef ROBOTSTXT_H
#define ROBOTSTXT_H
#include <DataStructure.h>
#include <string>
#include<map>
#include <set>

enum OPNAME{ USER_AGENT, DISALLOW, ALLOW, SITEMAP, CRAWL_DELAY, UNKNOWN_OP, NO_OP };
static const std::string op[UNKNOWN_OP] = { "User-agent", "Disallow", "Allow", "Sitemap", "Crawl-delay" };

class RobotsTXT{

	BlockingQueue<std::string> &m_unqiueUrlsIn;
	BlockingQueue<std::string> &m_unqiueUrlsOut;
	std::map<std::string, std::set<std::string> > m_disAllowed;
	std::set<std::string> m_badUrls;

	void FetchRobotsTXT(const std::string &_website,
		std::string &_parentWebsite);

	void ParseRobotsTXT(std::vector<std::string> &_robots_msg,
		const std::string &_parentWebsite);

	std::string GetParentWebSite(const std::string &_website)const;

	std::string GetRelativeAddress(const std::string &_website)const;

	bool IsNewWebsite(const std::string &_website)const;

	OPNAME GetOpName(const std::string &_str)const;

	void ParseLine(const std::string &_line,
		OPNAME &_opname,
		std::string &_value)const;

public:
	RobotsTXT(BlockingQueue<std::string> &_unqiueUrlsIn,
			BlockingQueue<std::string> &_unqiueUrlsOut);

	void Run();

	~RobotsTXT();
};
#endif
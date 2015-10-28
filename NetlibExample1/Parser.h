#ifndef PARSER_H
#define PARSER_H
#include <DataStructure.h>
#include <string>

class Parser{
	BlockingQueue<std::string> &m_urlsout;
	BlockingQueue<std::string> &m_datain;
	BlockingQueue<std::string> &m_dataout;
	std::string m_stayWithin;

	void get_parent(const std::string &_data,
					std::string &_parent);
public:
	Parser(BlockingQueue<std::string> &_datain,
		BlockingQueue<std::string> &_urlsout,
		BlockingQueue<std::string> &_dataout,
		const std::string &_stayWithin);
	void Run();

	~Parser();
};

#endif

#ifndef POST_PARSER_H
#define POST_PARSER_H
#include <DataStructure.h>
#include <string>
#include <html/Chtml.h>

class PostParser{
	BlockingQueue<std::string> &m_datain;
	std::string m_outfile_path;
	HTML::Chtml m_html;

public:
	PostParser(BlockingQueue<std::string> &_datain,
		const std::string &_out_file_path);

	// if drain is true, then the post processor drains all the input data
	// hence doesnt apply any logic on the input data
	void Run(bool _drain);

	~PostParser();
};
#endif
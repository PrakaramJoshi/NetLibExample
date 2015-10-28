#pragma once
#include <DataStructure.h>

class Crawler
{
	BlockingQueue<std::string> m_urls;

public:
	Crawler();

	void Run(const std::string &_sourceUrl,
			const std::string &_stayWithin,
			const std::string &_out_file_path,
			bool _post_parser_drain);

	~Crawler();
};


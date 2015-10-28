#ifndef SCHEDULAR_H
#define SCHEDULAR_H
#include <DataStructure.h>
#include <string>

class Schedular{
	BlockingQueue<std::string> &m_unqiueUrls;

public:
	Schedular(BlockingQueue<std::string> &_urlsUnique);

	void Run(const std::string &_source,
		const std::string &_stayWithin,
		const std::string &_out_file_path,
		bool _post_parser_drain);

	~Schedular();
};
#endif
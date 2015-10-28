#ifndef FETCHER_H
#define FETCHER_H
#include <DataStructure.h>
#include <string>
class Fetcher{

	BlockingQueue<std::string> &m_urlsin;
	BlockingQueue<std::string> &m_dataout;
	Counter<unsigned long> &m_counter;
public:
	Fetcher(BlockingQueue<std::string> &_urlsin,
		BlockingQueue<std::string> &_dataout,
		Counter<unsigned long>&_counter);

	void Run();

	~Fetcher();

};
#endif
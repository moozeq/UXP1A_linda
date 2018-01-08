#ifndef INCLUDE_GUARDEDLIST_H_
#define INCLUDE_GUARDEDLIST_H_

#include <mutex>
#include <list>
#include <Request.h>

class GuardedList
{
public:
	virtual ~GuardedList();

	std::list<Request *> & getList(void);
	std::mutex & getMutex(void);
	void push_back(Request * r);
	void erase(std::list<Request *>::const_iterator it);

private:
	std::list<Request *> list;
	std::mutex mtx;
};

#endif /* INCLUDE_GUARDEDLIST_H_ */

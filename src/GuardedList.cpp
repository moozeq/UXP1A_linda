#include "GuardedList.h"

GuardedList::~GuardedList()
{
	std::unique_lock<std::mutex> uLock(mtx);
	for(auto r : list){
		delete r;
	}
}

std::list<Request *> & GuardedList::getList(void)
{
	return list;
}

std::mutex & GuardedList::getMutex(void)
{
	return mtx;
}

void GuardedList::push_back(Request * r)
{
	std::unique_lock<std::mutex> uLock(mtx);
	list.push_back(r);
}

void GuardedList::erase(std::list<Request *>::const_iterator it)
{
	std::unique_lock<std::mutex> uLock(mtx);
	list.erase(it);
}

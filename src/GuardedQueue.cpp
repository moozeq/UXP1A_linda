#include "GuardedQueue.h"
#include <iostream>

GuardedQueue::~GuardedQueue()
{
	std::unique_lock<std::mutex> uLock(mtx);
	for(unsigned i = 0; i < queue.size(); ++i)
		delete queue[i];
}

void GuardedQueue::producerEnter(Request * request)
{
	std::unique_lock<std::mutex> uLock(mtx);
	queue.push_back(request);

	if(queue.size() == 1)
		emptyCond.notify_one();
}

Request * GuardedQueue::consumerEnter()
{
	std::unique_lock<std::mutex> uLock(mtx);
	emptyCond.wait(uLock, [this]{return queue.size() > 0;});

	Request * poppedRequest = queue.front();
	queue.pop_front();

	uLock.unlock();
	return poppedRequest;
}

size_t GuardedQueue::size(void)
{
	std::unique_lock<std::mutex> uLock(mtx);
	return queue.size();
}

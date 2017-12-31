#ifndef INCLUDE_GUARDEDQUEUE_H_
#define INCLUDE_GUARDEDQUEUE_H_

#include <mutex>
#include <condition_variable>
#include <deque>
#include "Request.h"

class GuardedQueue
{
public:
	void producerEnter(Request * request);
	Request * consumerEnter();
	size_t size(void);
	~GuardedQueue();

private:
	std::deque<Request *> queue;
 	std::condition_variable emptyCond;
	std::mutex mtx;
};

#endif /* INCLUDE_GUARDEDQUEUE_H_ */

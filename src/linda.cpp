#include "linda.h"

const char* serverPath = "/tmp/fifo.server";
const char * inFifoSemaphoreName = "/serverInFifoSemaphore";

int init(sem_t ** serverFifoSem)
{
	if(access(serverPath, F_OK) == -1) {
		std::cout<<"Server doesn't exist"<<std::endl;
		return 1;
	}
	// Open server fifo semaphore
	(*serverFifoSem) = sem_open(inFifoSemaphoreName, O_RDWR);
	if (*serverFifoSem == SEM_FAILED) {
		perror("sem_open(3) failed");
		exit(EXIT_FAILURE);
	}
	return 0;
}

void sendRequest(std::ofstream * outStream, sem_t * semaphore, const Request * req)
{
	if (sem_wait(semaphore) < 0)
		perror("sem_wait(3) failed on child");
	(*outStream)<<(*req);
	if (sem_post(semaphore) < 0)
		perror("sem_post(3) error on child");
}

void input(const Request * req)
{
	sem_t * serverFifoSemaphore;
	if (init(&serverFifoSemaphore) != 0)
		return;
	std::ofstream outFIFO(serverPath, std::ofstream::binary);
	std::string pipePath = "/tmp/fifo.";
	pipePath.append(std::to_string(getpid()));
	umask(0);
	mkfifo(pipePath.c_str(), 0666);

	sendRequest(&outFIFO, serverFifoSemaphore, req);

	std::cout<<"Request for tuple has been sent"<<std::endl;

	// Get reply from server
	Reply* rep = new Reply();
	std::ifstream inFIFO(pipePath.c_str(), std::ifstream::binary);
	std::ofstream outClientTmpFifo(pipePath, std::ofstream::binary);
	std::cout<<"Waiting for reply..."<<std::endl;
	inFIFO >> *rep;

	if (rep->isFound) {
		std::cout<<"Replied tuple: "<<std::endl;
		std::cout<<*(rep->tuple);
	}
	else
		std::cout<<"Didn't find demanded tuple"<<std::endl;
	delete rep;
	unlink(pipePath.c_str());
	if (sem_close(serverFifoSemaphore) < 0)
		perror("sem_close(3) failed");
}

void output(const Request * req)
{
	sem_t * serverFifoSemaphore;
	if (init(&serverFifoSemaphore) != 0)
		return;
	std::ofstream outFIFO(serverPath, std::ofstream::binary);

	sendRequest(&outFIFO, serverFifoSemaphore,req);

	std::cout<<"Tuple's been sent"<<std::endl;
	if (sem_close(serverFifoSemaphore) < 0)
		perror("sem_close(3) failed");
}

void read(const Request * req)
{
	sem_t * serverFifoSemaphore;
	if (init(&serverFifoSemaphore) != 0)
		return;
	std::ofstream outFIFO(serverPath, std::ofstream::binary);
	std::string pipePath = "/tmp/fifo.";
	pipePath.append(std::to_string(getpid()));
	umask(0);
	mkfifo(pipePath.c_str(), 0666);

	sendRequest(&outFIFO, serverFifoSemaphore,req);

	std::cout<<"Request for tuple has been sent"<<std::endl;

	// Get reply from server
	Reply* rep = new Reply();
	std::ifstream inFIFO(pipePath.c_str(), std::ifstream::binary);
	std::ofstream outClientTmpFifo(pipePath, std::ofstream::binary);
	std::cout<<"Waiting for reply..."<<std::endl;
	inFIFO >> *rep;

	if (rep->isFound) {
		std::cout<<"Replied tuple: "<<std::endl;
		std::cout<<*(rep->tuple);
	}
	else
		std::cout<<"Didn't find demanded tuple"<<std::endl;
	delete rep;
	unlink(pipePath.c_str());
	if (sem_close(serverFifoSemaphore) < 0)
		perror("sem_close(3) failed");
}

void linda_input(const Request * req)
{
	input(req);
}

void linda_input(Tuple tuple, std::time_t timeout)
{
	Request * req = new Request(getpid(), timeout, Request::Input, tuple);
	input(req);
	delete req;
}

void linda_input(const std::initializer_list<Elem> & init, std::time_t timeout)
{
	Tuple * t = new Tuple(init);
	Request * req = new Request(t, Request::Input, timeout);
	input(req);
	delete req;
}

void linda_output(const Request * req)
{
	output(req);
}

void linda_output(Tuple tuple)
{
	Request * req = new Request(getpid(), 0, Request::Output, tuple);
	output(req);
	delete req;
}

void linda_output(const std::initializer_list<Elem> & init)
{
	Tuple * t = new Tuple(init);
	Request * req = new Request(t, Request::Output, 0);
	output(req);
	delete req;
}

void linda_read(Request * req)
{
	input(req);
}

void linda_read(Tuple tuple, std::time_t timeout)
{
	Request * req = new Request(getpid(), timeout, Request::Read, tuple);
	read(req);
	delete req;
}

void linda_read(const std::initializer_list<Elem> & init, std::time_t timeout)
{
	Tuple * t = new Tuple(init);
	Request * req = new Request(t, Request::Read, timeout);
	read(req);
	delete req;
}

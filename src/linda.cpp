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

const Reply * input(const Request * req)
{
	sem_t * serverFifoSemaphore;
	if (init(&serverFifoSemaphore) != 0)
		return nullptr;
	std::ofstream outFIFO(serverPath, std::ofstream::binary);
	std::string pipePath = "/tmp/fifo.";
	pipePath.append(std::to_string(getpid()));
	umask(0);
	mkfifo(pipePath.c_str(), 0666);

	sendRequest(&outFIFO, serverFifoSemaphore, req);

	// Get reply from server
	Reply* rep = new Reply();
	std::ifstream inFIFO(pipePath.c_str(), std::ifstream::binary);
	std::ofstream outClientTmpFifo(pipePath, std::ofstream::binary);
	inFIFO >> *rep;

	unlink(pipePath.c_str());
	if (sem_close(serverFifoSemaphore) < 0)
		perror("sem_close(3) failed");
	return rep;
}

int output(const Request * req)
{
	sem_t * serverFifoSemaphore;
	if (init(&serverFifoSemaphore) != 0)
		return -1;
	std::ofstream outFIFO(serverPath, std::ofstream::binary);

	sendRequest(&outFIFO, serverFifoSemaphore,req);

	if (sem_close(serverFifoSemaphore) < 0)
		perror("sem_close(3) failed");
	return 0;
}

const Reply * read(const Request * req)
{
	sem_t * serverFifoSemaphore;
	if (init(&serverFifoSemaphore) != 0)
		return nullptr;
	std::ofstream outFIFO(serverPath, std::ofstream::binary);
	std::string pipePath = "/tmp/fifo.";
	pipePath.append(std::to_string(getpid()));
	umask(0);
	mkfifo(pipePath.c_str(), 0666);

	sendRequest(&outFIFO, serverFifoSemaphore,req);

	// Get reply from server
	Reply* rep = new Reply();
	std::ifstream inFIFO(pipePath.c_str(), std::ifstream::binary);
	std::ofstream outClientTmpFifo(pipePath, std::ofstream::binary);
	inFIFO >> *rep;

	unlink(pipePath.c_str());
	if (sem_close(serverFifoSemaphore) < 0)
		perror("sem_close(3) failed");
	return rep;
}

Reply linda_input(const Request * req)
{
	const Reply * rPtr = input(req);
	Reply rep;
	if(rPtr != nullptr)
		rep = *rPtr;
	delete rPtr;
	return rep;
}

Reply linda_input(Tuple tuple, std::time_t timeout)
{
	Request * req = new Request(getpid(), timeout, Request::Input, tuple);
	Reply rep;
	const Reply * rPtr = input(req);
	if(rPtr != nullptr)
		rep = *rPtr;
	delete rPtr;
	delete req;
	return rep;
}

Reply linda_input(const std::initializer_list<Elem> & init, std::time_t timeout)
{
	Tuple * t = new Tuple(init);
	Request * req = new Request(t, Request::Input, timeout);
	Reply rep;
	const Reply * rPtr = input(req);
	if(rPtr != nullptr)
		rep = *rPtr;
	delete rPtr;
	delete req;
	return rep;
}

int linda_output(const Request * req)
{
	return output(req);
}

int linda_output(Tuple tuple)
{
	Request * req = new Request(getpid(), 0, Request::Output, tuple);
	int status = output(req);
	delete req;
	return status;
}

int linda_output(const std::initializer_list<Elem> & init)
{
	Tuple * t = new Tuple(init);
	Request * req = new Request(t, Request::Output, 0);
	int status = output(req);
	delete req;
	return status;
}

Reply linda_read(Request * req)
{
	const Reply * rPtr = read(req);
	Reply rep;
	if(rPtr != nullptr)
		rep = *rPtr;
	delete rPtr;
	return rep;
}

Reply linda_read(Tuple tuple, std::time_t timeout)
{
	Request * req = new Request(getpid(), timeout, Request::Read, tuple);
	Reply rep;
	const Reply * rPtr = read(req);
	if(rPtr != nullptr)
		rep = *rPtr;
	delete rPtr;
	delete req;
	return rep;
}

Reply linda_read(const std::initializer_list<Elem> & init, std::time_t timeout)
{
	Tuple * t = new Tuple(init);
	Request * req = new Request(t, Request::Read, timeout);
	Reply rep;
	const Reply * rPtr = read(req);
	if(rPtr != nullptr)
		rep = *rPtr;
	delete rPtr;
	delete req;
	return rep;
}

int linda_terminate_server()
{
	Request req(getpid(), 0, Request::Stop, Tuple());
	return linda_output(&req);
}

void linda_unlink_client_fifo()
{
	std::string pipePath = "/tmp/fifo.";
	pipePath.append(std::to_string(getpid()));

	if(access(pipePath.c_str(), F_OK) != -1)
		unlink(pipePath.c_str());
}

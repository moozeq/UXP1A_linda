#ifndef INCLUDE_GUARDEDCOUT_H_
#define INCLUDE_GUARDEDCOUT_H_

#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <wait.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <fstream>
#include <stdlib.h>
#include <semaphore.h>

struct GuardedCout{
	// based on https://stackoverflow.com/questions/18277304/using-stdcout-in-multiple-threads
	GuardedCout(std::string semaphoreName)
	{
		semName = semaphoreName;
		init();
	}

	~GuardedCout()
	{
		if (sem_unlink(semName.c_str()) < 0)
			perror("sem_unlink(3) failed");
	}

	std::ostream&
	print_one(std::ostream& os)
	{
	    return os;
	}

	template <class A0, class ...Args>
	std::ostream&
	print_one(std::ostream& os, const A0& a0, const Args& ...args)
	{
	    os << a0;
	    return print_one(os, args...);
	}

	template <class ...Args>
	std::ostream&
	print(std::ostream& os, const Args& ...args)
	{
	    return print_one(os, args...);
	}

	template <class ...Args>
	std::ostream&
	print(const Args& ...args)
	{
		if (sem_wait(coutSemaphore) < 0)
			perror("sem_wait(3) failed on child");
	    std::ostream & os = print(std::cout, "[", getpid(), "]", args...);
	    if (sem_post(coutSemaphore) < 0)
	    			perror("sem_post(3) error on child");
	    return os;
	}

	void init(void)
	{
		coutSemaphore = sem_open(
				semName.c_str(), O_CREAT | O_RDWR, (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP), 1);
		if (coutSemaphore == SEM_FAILED)
		{
			coutSemaphore = sem_open(semName.c_str(), O_RDWR);
			if (coutSemaphore == SEM_FAILED) {
				perror("sem_open(3) failed");
				exit(EXIT_FAILURE);
			}
			return;
		}
	}

private:
	sem_t * coutSemaphore;
	std::string semName;
};


#endif /* INCLUDE_GUARDEDCOUT_H_ */

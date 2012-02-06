#include "MyThread.h"

#include <iostream>

using namespace std;

// a global variable holding the address of the real
// working procedure
void* proc;

// startProcedure for Linux platform
void* linuxThreadProc(void *param)
{
  	((void(*)(void*))proc)(param);
		return 0;
}

// the default constructor, does nothing
MyThread::MyThread()
{
}

// the destructor, does nothing
MyThread::~MyThread()
{
  pthread_exit(NULL);
}

// call this method to create a thread
void MyThread::createThread(void *threadProc,void *param)
{
  	// save the address of the working procedure to the global variable
  	proc=threadProc;

	// start the thread, NULL => Default Thread
	pthread_create(&the_thread,NULL,linuxThreadProc,param);
}

// call this method to exit this thread.
void MyThread::exitThread()
{
  	pthread_exit(NULL);
}

void MyThread::cancelThread()
{
  	pthread_cancel(the_thread);
}

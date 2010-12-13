#include "thread.h"

#include <stdio.h>

static void *thread_func(void *data)
{
	Thread *obj = static_cast<Thread *>(data);
	while (obj->Run());
	pthread_exit(NULL);

	return NULL;
}


Thread::Thread()
	: bRunning(true)
	, tHandler()
{
	int rc = 0;

	rc = pthread_create(&tHandler, NULL, thread_func, static_cast<void *>(this));
	if (rc)
	{
		fprintf(stderr, "error: pthread_create error %d.\n", rc);
		bRunning = false;
	}

	this->Run();
}

Thread::~Thread()
{
	bRunning = false;
	pthread_join(tHandler, NULL);
}

bool Thread::Run()
{
	return bRunning;
}


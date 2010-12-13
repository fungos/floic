#ifndef __THREAD_H__
#define __THREAD_H__

#include <pthread.h>

class Thread
{
	public:
		Thread();
		virtual ~Thread();

		virtual bool Run();

	protected:
		bool bRunning;

	private:
		pthread_t tHandler;
		
		Thread(const Thread &);
		Thread operator=(const Thread &);
};

#endif // __THREAD_H__

#ifndef __RUNNER_H__
#define __RUNNER_H__

class IFlooder;

class Runner
{
	public:
		Runner();
		~Runner();

		void Execute(const char *command);
		void Stop();

	private:
		IFlooder **arThreads;

		char *pIp;
		char *pHost;
		char *pURLChain;
		char *pMessage;
		char *pMethod;
		char *pSrcHost;

		int iTimeout;
		int iPort;
		int iSrcPort;
		int iThreads;
		int iSpeed;

		bool bWait;
		bool bRandom;
		bool bDefault;

		Runner(const Runner &);
		Runner operator=(const Runner &);
};

#endif // __RUNNER_H__


#ifndef __HTTPFLOODER_H__
#define __HTTPFLOODER_H__

#include "iflooder.h"

#include <netinet/in.h>

class HTTPFlooder : public IFlooder
{
	public:
		HTTPFlooder(int fid, const char *ip, int port, int delay, const char *host, const char *suburl, bool random, bool wait);
		virtual ~HTTPFlooder();

		// Thread
		virtual bool Run();

	private:
		char *pHost;
		const char *pURLChain;

		struct sockaddr_in sAddress;

		bool bRandom;
		bool bWait;

		HTTPFlooder(const HTTPFlooder&);
		HTTPFlooder operator=(const HTTPFlooder&);
};

#endif // __HTTPFLOODER_H__

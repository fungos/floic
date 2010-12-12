#ifndef __XXPFLOODER_H__
#define __XXPFLOODER_H__

#include "iflooder.h"

#include <netinet/in.h>

#define XXP_RANDOM_DATA_SIZE_MAX 32

class XXPFlooder : public IFlooder
{
	public:
		XXPFlooder(int fid, const char *ip, int port, int delay, const char *host, int protocol, const char *data, bool random);
		virtual ~XXPFlooder();

		// Thread
		virtual bool Run();

	private:
		const char *pData;
		const char *pHost;
		char pRandomData[XXP_RANDOM_DATA_SIZE_MAX];

		int iDataSize;
		int iProtocol;

		struct sockaddr_in sAddress;

		bool bRandom;

		XXPFlooder(const XXPFlooder &);
		XXPFlooder operator=(const XXPFlooder &);
};

#endif // __XXPFLOODER_H__

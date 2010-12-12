#include "iflooder.h"

IFlooder::IFlooder(int fid, const char *ip, int port, int delay)
	: pIp(ip)
	, iId(fid)
	, iPort(port)
	, iDelay(delay)
	, iCount(0)
	, iError(0)
{
}

IFlooder::~IFlooder()
{
}


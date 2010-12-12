#ifndef __IFLOODER_H__
#define __IFLOODER_H__

#include "thread.h"

class IFlooder : public Thread
{
	public:
		IFlooder(int fid, const char *ip, int port, int delay);
		virtual ~IFlooder();

	protected:
		const char *pIp;
		int iId;
		int iPort;
		int iDelay;
		int iCount;
		int iError;

	private:
		IFlooder(const IFlooder &);
		IFlooder operator=(const IFlooder &);
};

#endif // __IFLOODER_H__

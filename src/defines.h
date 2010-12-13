#ifndef __DEFINES_H__
#define __DEFINES_H__

extern int verbosity;

#define LOG(x, ...)	if (verbosity) fprintf(stdout, x, ##__VA_ARGS__);
#define ERROR(x, ...)	if (verbosity) fprintf(stderr, x, ##__VA_ARGS__);
#define DBG(x, ...)	if (verbosity > 2) fprintf(stdout, x, ##__VA_ARGS__);
#define INFO(x, ...)	if (verbosity > 1) fprintf(stdout, x, ##__VA_ARGS__);

#endif // __DEFINES_H__

#include "util.h"

#include <stdlib.h>
#include <string.h>

//const char _valid[] = "0123456789abcdefghijklmnopqrstuvwxyz_";
const char _valid[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

char get_random_valid_char()
{
	return _valid[rand() % strlen(_valid)];
}

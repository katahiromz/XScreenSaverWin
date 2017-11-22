#undef usleep
#define usleep(usec) Sleep(usec / 1000)

#include <unistd.h>

int msleep(unsigned int tms) {
  return usleep(tms * 1000);
}

int main() {
	msleep(200);
	return 0;
}

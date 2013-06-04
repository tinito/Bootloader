#include "ch.h"
#include "hal.h"

#include "print.h"

void print(char *p) {

	while (*p) {
		sdPut(&SERIAL_DRIVER, *p++);
	}
}

void printn(uint32_t n) {
	char buf[16], *p;

	if (!n)
		sdPut(&SERIAL_DRIVER, '0');
	else {
		p = buf;
		while (n)
			*p++ = (n % 10) + '0', n /= 10;
		while (p > buf)
			sdPut(&SERIAL_DRIVER, *--p);
	}
}

void read(char * p) {
	char c;

	while ((c = sdGet(&SERIAL_DRIVER))!= 0){
		if ((c == '\r') || (c == '\n')) {
			break;
		}
		*p++ = c;
	}
}

int readn(void) {
	int n = 0;
	char c;

	while ((c = sdGet(&SERIAL_DRIVER))!= 0){
		if ((c == '\r') || (c == '\n')) {
			break;
		}

		if ((c >= '0') && (c <= '9')) {
			n = n * 10 + (c - '0');
		}
	}
	return n;
}

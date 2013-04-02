#include "ch.h"
#include "hal.h"

#include "print.h"

void printn(uint32_t n) {
  char buf[16], *p;

  if (!n)
    chSequentialStreamPut(&SERIAL_DRIVER, '0');
  else {
    p = buf;
    while (n)
      *p++ = (n % 10) + '0', n /= 10;
    while (p > buf)
      chSequentialStreamPut(&SERIAL_DRIVER, *--p);
  }
}

void print(char *p) {

  while (*p) {
    chSequentialStreamPut(&SERIAL_DRIVER, *p++);
  }
}

void printnl(void) {

  chSequentialStreamWrite(&SERIAL_DRIVER, (uint8_t *)"\r\n", 2);
}

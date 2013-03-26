#include "ch.h"
#include "hal.h"

#include "println.h"

void println(char *p) {

  while (*p) {
    chSequentialStreamPut(&SERIAL_DRIVER, *p++);
  }
  chSequentialStreamWrite(&SERIAL_DRIVER, (uint8_t *)"\r\n", 2);
}

#include "ch.h"
#include "hal.h"

#include "println.h"

msg_t AppThread(void *arg) __attribute__((section ("app_section")));

msg_t AppThread(void *arg) {

	(void) arg;

	println("Hello from AppThread()\r\n");

	while (TRUE) {
		palTogglePad(LED_GPIO, LED2);
		chThdSleepMilliseconds(100);
	}
	return 0;
}


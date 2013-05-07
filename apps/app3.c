#include "ch.h"
#include "hal.h"
#include "appcfg.h"

#include "../print.h"

static void test(int n) {
	systime_t time = chTimeNow();
	print("APP3: ");
	printn(n);
	print(" - ");
	printn(time);
	print("\r\n");
}


msg_t AppThread3(void *arg) {
	uint32_t cnt;

	(void) arg;

	cnt = 0xFFFFFFFF;
	print("Hello from APP3\r\n");

	while (TRUE) {
		palTogglePad(LED_GPIO, LED4);
		cnt--;
		test(cnt);
		chThdSleepMilliseconds(1000);
	}
	return 0;
}

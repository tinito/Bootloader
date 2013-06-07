#include "ch.h"
#include "hal.h"

#include "../print.h"

static void test(int n) {
	systime_t time = chTimeNow();
	print("APP3: ");
	printn(n);
	print(" - ");
	printn(time);
	print("\r\n");
}


msg_t app_thread(void *arg) {
	uint32_t cnt;

	(void) arg;

	cnt = 0xFFFFFFFF;
#if 0
	print("Hello from APP3\r\n");
#endif
	while (TRUE) {
		palTogglePad(LED_GPIO, LED4);
		cnt--;
#if 0
		test(cnt);
#endif
		chThdSleepMilliseconds(1000);
	}
	return 0;
}

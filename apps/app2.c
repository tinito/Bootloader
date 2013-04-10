#include "ch.h"
#include "hal.h"
#include "appcfg.h"

#include "../print.h"

static void test(int n) {
	systime_t time = chTimeNow();
	print("APP2: ");
	printn(n);
	print(" - ");
	printn(time);
	printnl();
}

msg_t AppThread2(void *arg) {
	uint32_t cnt;

	(void) arg;

	cnt = 0xFFFFFFFF;
	print("Hello from APP2");
	printnl();

	while (TRUE) {
		palTogglePad(LED_GPIO, LED3);
		cnt--;
		test(cnt);
		chThdSleepMilliseconds(1000);
	}
	return 0;
}

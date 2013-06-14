#include "ch.h"
#include "hal.h"

#include "../print.h"

int biessess;

uint8_t datavec[] = { 0xCC, 0xCC, 0xCC, 0xCC };
char datastr[] = "ABC:)\r\n";

static void test(int n) {

	systime_t time = chTimeNow();
	printn(n);
	print("\r\n");
	printn(biessess);
	print("\r\n");
	print(datastr);
	print("\r\n");
	print("\r\n");
	
	(void)datavec;
}

msg_t app_thread(void *arg) {
	uint32_t cnt;

	(void)arg;

	while (TRUE) {
		palTogglePad(LED_GPIO, LED3);
		cnt--;
		biessess++;
		test(biessess);
		chThdSleepMilliseconds(1000);
	}
	return 0;
}

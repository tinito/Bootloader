#include "ch.h"
#include "hal.h"
#include "appcfg.h"

#include "../print.h"

int biessess;

uint8_t datavec[] = { 0xCC, 0xCC, 0xCC, 0xCC };
char datastr[] = "ABC:)\r\n";

static void test(int n) {

	systime_t time = chTimeNow();
	printn(n);
	print(datastr);
	
	(void)datavec;
}

msg_t app_thread(void *arg) {
	uint32_t cnt;

	(void)arg;
/*
	cnt = 0xFFFFFFFF;
	print("Hello from APP2\r\n");
*/
	while (TRUE) {
		palTogglePad(LED_GPIO, LED3);
		cnt--;
		biessess++;
		test(biessess);
		chThdSleepMilliseconds(1000);
	}
	return 0;
}

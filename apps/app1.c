#include "ch.h"
#include "hal.h"
#include "appcfg.h"

#include "../print.h"

/*
 * GPT3 callback.
 */
static void gpt3cb(GPTDriver *gptp) {

  (void)gptp;
  palTogglePad(LED_GPIO, LED2);
}

/*
 * GPT3 configuration.
 */
static const GPTConfig gpt3cfg = {
  10000,    /* 10kHz timer clock.*/
  gpt3cb    /* Timer callback.*/
};

void test(int n) {
	print("APP1: ");
	printn(n);
	print("\r\n");
}

msg_t AppThread1(void *arg) {
	uint32_t cnt;

	(void) arg;

	cnt = 0;
	print("Hello from APP1\r\n");

	gptStart(&GPTD3, &gpt3cfg);
	gptStartContinuous(&GPTD3, 500);
	print("GPTD3 started\r\n");

	while (TRUE) {
		cnt++;
		test(cnt);
		chThdSleepMilliseconds(5000);
	}
	return 0;
}

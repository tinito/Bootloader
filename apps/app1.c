#include "ch.h"
#include "hal.h"
#include "appcfg.h"

#include "../print.h"

/*
 * GPT3 callback.
 */
static void gpt3cb(GPTDriver *gptp) {

  (void)gptp;
  palTogglePad(LED_GPIO, LED3);
}

/*
 * GPT3 configuration.
 */
static const GPTConfig gpt3cfg = {
  10000,    /* 10kHz timer clock.*/
  gpt3cb    /* Timer callback.*/
};

static msg_t AppThread1(void *arg) {
	uint32_t cnt;

	(void) arg;

	cnt = 0;
	print("Hello from APP1");
	printnl();

	gptStart(&GPTD3, &gpt3cfg);
	gptStartContinuous(&GPTD3, 5000);
	print("GPTD3 started");
	printnl();

	while (TRUE) {
		palTogglePad(LED_GPIO, LED2);
		cnt++;
		print("APP1: ");
		printn(cnt);
		printnl();
		chThdSleepMilliseconds(1000);
	}
	return 0;
}

appcfg_t appcfg[2] __attribute__ ((section (".appcfg"))) = {{"APP1", AppThread1}, {"APP2", 0x080110d1}};

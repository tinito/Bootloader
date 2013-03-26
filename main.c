#include "ch.h"
#include "hal.h"

#include "println.h"

static WORKING_AREA(waThread1, 128);
static WORKING_AREA(waAppThread, 128);

#define APP_ADDRESS 0x08010001

/*===========================================================================*/
/* Application threads.                                                      */
/*===========================================================================*/

/*
 * Red LED blinker thread, times are in milliseconds.
 */
static msg_t Thread1(void *arg) {

	(void) arg;

	println("Hello from Thread1()\r\n");

	while (TRUE) {
		palTogglePad(LED_GPIO, LED3);
		chThdSleepMilliseconds(200);
	}
	return 0;
}

/*
 * Application entry point.
 */
int main(void) {

	/*
	 * System initializations.
	 * - HAL initialization, this also initializes the configured device drivers
	 *   and performs the board-specific initializations.
	 * - Kernel initialization, the main() function becomes a thread and the
	 *   RTOS is active.
	 */
	halInit();
	chSysInit();

	/*
	 * Activates the serial driver 1 using the driver default configuration.
	 */
	sdStart(&SERIAL_DRIVER, NULL);

	println("Hello from main()\r\n");

	chThdSleepMilliseconds(500);

	/*
	 * Creates the blinker thread.
	 */
	chThdCreateStatic(waThread1, sizeof(waThread1), NORMALPRIO, Thread1, NULL);

	chThdSleepMilliseconds(2000);

	/*
	 * Creates the application thread.
	 */
	chThdCreateStatic(waAppThread, sizeof(waAppThread), NORMALPRIO, APP_ADDRESS, NULL);

	/*
	 * Normal main() thread activity, in this demo it does nothing except
	 * sleeping in a loop and check the button state.
	 */
	while (TRUE) {
		chThdSleepMilliseconds(200);
	}
}

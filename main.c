#include "ch.h"
#include "hal.h"

#include "print.h"

static WORKING_AREA(waThread1, 128);
static WORKING_AREA(waAppThread, 128);
static WORKING_AREA(waAppThread2, 128);

#define APPTHREAD_ADDRESS	0x08011031
#define APPCFG_ADDRESS		0x08010000

typedef struct {
	char name[16];
	void * address;
} appcfg_t;

/*===========================================================================*/
/* Application threads.                                                      */
/*===========================================================================*/

/*
 * Red LED blinker thread, times are in milliseconds.
 */
static msg_t Thread1(void *arg) {

	(void) arg;

	print("Hello from Thread1()");
	printnl();

	while (TRUE) {
		palTogglePad(LED_GPIO, LED1);
		chThdSleepMilliseconds(200);
	}
	return 0;
}

/*
 * Application entry point.
 */
int main(void) {
	appcfg_t * appcfg = (appcfg_t *) APPCFG_ADDRESS;

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

	print("Hello from main()");
	printnl();

	chThdSleepMilliseconds(500);

	/*
	 * Creates the blinker thread.
	 */
	chThdCreateStatic(waThread1, sizeof(waThread1), NORMALPRIO, Thread1, NULL);

	chThdSleepMilliseconds(2000);

	/*
	 * Creates the application thread.
	 */
	print("Starting ");
	print(appcfg->name);
	print(" at address ");
	printn((int)appcfg->address);
	print(" ...");
	printnl();
	chThdSleepMilliseconds(500);

	chThdCreateStatic(waAppThread, sizeof(waAppThread), NORMALPRIO, appcfg->address, NULL);

	chThdSleepMilliseconds(5000);

	appcfg++;

	print("Starting ");
	print(appcfg->name);
	print(" at address ");
	printn((int)appcfg->address);
	print(" ...");
	printnl();
	chThdSleepMilliseconds(500);

	chThdCreateStatic(waAppThread2, sizeof(waAppThread2), NORMALPRIO, appcfg->address, NULL);

	/*
	 * Normal main() thread activity, in this demo it does nothing except
	 * sleeping in a loop and check the button state.
	 */
	while (TRUE) {
		chThdSleepMilliseconds(500);
	}
}

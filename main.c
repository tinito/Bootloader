#include "ch.h"
#include "hal.h"

#include "flash/flash.h"
#include "flash/helper.h"
#include "flash/ihex.h"
#include "flash/flashconfig.h"

#include "print.h"

#define BOOTLOADER_ERROR_MMC_NOCARD     1
#define BOOTLOADER_ERROR_MMC_BADFS      2
#define BOOTLOADER_ERROR_MMC_NOFILE     3
#define BOOTLOADER_ERROR_BAD_HEX        4
#define BOOTLOADER_ERROR_BAD_FLASH      5

static WORKING_AREA(waThread1, 128);
static WORKING_AREA(waFlashThread, 4096);
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
 * Flash programming thread.
 */
static struct LinearFlashing flashPage;
static uint8_t buffer[4096];

static msg_t FlashThread(void *arg) {

	(void) arg;

	while (!chThdShouldTerminate()) {
		int size = 0;
		int offset = 0;
		int n = 0;
		int err;
		IHexRecord irec;
		uint16_t addressOffset = 0x00;
		uint32_t address = 0x0;
		char * p = buffer;
		uint32_t tp = 0;

		print("SIZE: ");
		size = readn();
		printn(size);
		printnl();

		if (size == 0) {
			chThdExit(0);
			return 0;
		}

		print("OFFSET: ");
		offset = readn();
		printn(offset);
		printnl();

		print("receiving ");
		printn(size);
		print("bytes..");

		sdRead(&SERIAL_DRIVER, buffer, size);
		p = buffer;
		buffer[size] = '\0';

		/*
		 * Here comes the flashing magic (pun intended).
		 */
		linearFlashProgramStart(&flashPage);

		chSysLock();

		while ((n = Read_IHexRecord(&irec, p)) >= 0) {
			switch (irec.type) {
			case IHEX_TYPE_00: /**< Data Record */
				address = (((uint32_t) addressOffset) << 16) + irec.address + offset;

				err = linearFlashProgram(&flashPage, address,
						(flashdata_t*) irec.data, irec.dataLen);

				if (err) {
					print("BLERR: ");
					printn(BOOTLOADER_ERROR_BAD_FLASH);
					printnl();
				}
/*
				print("data: ");
				printn(n);
				print(" bytes\r\n");
*/
				break;

			case IHEX_TYPE_04: /**< Extended Linear Address Record */
				addressOffset = (((uint16_t) irec.data[0]) << 8) + irec.data[1];
/*
				print("offset: ");
				printn(addressOffset);
				printnl();
*/
				break;

			case IHEX_TYPE_01: /**< End of File Record */
			case IHEX_TYPE_05: /**< Start Linear Address Record */
				break;

			case IHEX_TYPE_02: /**< Extended Segment Address Record */
			case IHEX_TYPE_03: /**< Start Segment Address Record */
				print("BLERR: ");
				printn(BOOTLOADER_ERROR_BAD_HEX);
				printnl();
				break;
			}

			p += n;
		}

		err = linearFlashProgramFinish(&flashPage);

		chSysUnlock();

		print("...");
		printn(n);
		print(" bytes wrote to Flash.\r\n");

		print("Enter AppThread() offset: ");
		tp = readn();
		printnl();

		tp += FLASH_USER_BASE + 1 + offset;
		print("Starting...");

		chThdSleepMilliseconds(100);

		chThdCreateStatic(waAppThread, sizeof(waAppThread), NORMALPRIO + 1,
					tp, NULL);

		chThdSleepMilliseconds(100);
	}

	chThdExit(0);
	return 0;
}

/*
 * Application entry point.
 */
int main(void) {
	Thread * tp;
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

	/*
	 * Creates the Flash thread.
	 */
	tp = chThdCreateStatic(waFlashThread, sizeof(waFlashThread), NORMALPRIO,
			FlashThread, NULL);

	chThdWait(tp);
	chThdSleepMilliseconds(500);

/*	chThdCreateStatic(waAppThread, sizeof(waAppThread), NORMALPRIO,
			APPTHREAD_ADDRESS, NULL);
*/
	while (TRUE) {
		chThdSleepMilliseconds(500);
	}

	/*
	 * Creates the application thread.
	 */
	print("Starting ");
	print(appcfg->name);
	print(" at address ");
	printn((int) appcfg->address);
	print(" ...");
	printnl();
	chThdSleepMilliseconds(500);

	chThdCreateStatic(waAppThread, sizeof(waAppThread), NORMALPRIO,
			appcfg->address, NULL);

	chThdSleepMilliseconds(5000);

	appcfg++;

	print("Starting ");
	print(appcfg->name);
	print(" at address ");
	printn((int) appcfg->address);
	print(" ...");
	printnl();
	chThdSleepMilliseconds(500);

	chThdCreateStatic(waAppThread2, sizeof(waAppThread2), NORMALPRIO,
			appcfg->address, NULL);

	/*
	 * Normal main() thread activity, in this demo it does nothing except
	 * sleeping in a loop and check the button state.
	 */
	while (TRUE) {
		chThdSleepMilliseconds(500);
	}
}

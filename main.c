#include "ch.h"
#include "hal.h"
#include "shell.h"
#include "chprintf.h"

#include "flash/flash.h"
#include "flash/helper.h"
#include "flash/ihex.h"
#include "flash/flashconfig.h"

//#include "symbols.h"

#include "print.h"

#define BOOTLOADER_ERROR_MMC_NOCARD     1
#define BOOTLOADER_ERROR_MMC_BADFS      2
#define BOOTLOADER_ERROR_MMC_NOFILE     3
#define BOOTLOADER_ERROR_BAD_HEX        4
#define BOOTLOADER_ERROR_BAD_FLASH      5

static WORKING_AREA(waThread1, 128);
static WORKING_AREA(waFlashThread, 4096);

#define WA_SIZE_256B      THD_WA_SIZE(256)

#define APPTHREAD_ADDRESS	0x08011031
#define APPCFG_ADDRESS		0x08010000

typedef struct {
	char name[16];
	void * address;
} appcfg_t;

/*===========================================================================*/
/* Command line related.                                                     */
/*===========================================================================*/

#define SHELL_WA_SIZE   THD_WA_SIZE(4096)
#define TEST_WA_SIZE    THD_WA_SIZE(1024)

static void cmd_mem(BaseSequentialStream *chp, int argc, char *argv[]) {
	size_t n, size;

	(void) argv;
	if (argc > 0) {
		chprintf(chp, "Usage: mem\r\n");
		return;
	}
	n = chHeapStatus(NULL, &size);
	chprintf(chp, "core free memory : %u bytes\r\n", chCoreStatus());
	chprintf(chp, "heap fragments   : %u\r\n", n);
	chprintf(chp, "heap free total  : %u bytes\r\n", size);
}

static void cmd_threads(BaseSequentialStream *chp, int argc, char *argv[]) {
	static const char *states[] = { THD_STATE_NAMES };
	Thread *tp;

	(void) argv;
	if (argc > 0) {
		chprintf(chp, "Usage: threads\r\n");
		return;
	}
	chprintf(chp, "    addr    stack prio refs     state time\r\n");
	tp = chRegFirstThread();
	do {
		chprintf(chp, "%.8lx %.8lx %4lu %4lu %9s %lu\r\n", (uint32_t) tp,
				(uint32_t) tp->p_ctx.r13, (uint32_t) tp->p_prio,
				(uint32_t)(tp->p_refs - 1), states[tp->p_state],
				(uint32_t) tp->p_time);
		tp = chRegNextThread(tp);
	} while (tp != NULL);
}
/*
static void cmd_sym(BaseSequentialStream *chp, int argc, char *argv[]) {
	int i;

	(void) argv;
	if (argc > 0) {
		chprintf(chp, "Usage: sym\r\n");
		return;
	}

	for (i = 0; i < symbols_nelts; i++) {
		chprintf(chp, "%s %x\r\n", symbols[i].name, symbols[i].value);
	}
}
*/
static const ShellCommand commands[] = { { "mem", cmd_mem }, { "threads",
		cmd_threads }, /*{ "sym", cmd_sym },*/ { NULL, NULL } };

static const ShellConfig shell_cfg1 = { (BaseSequentialStream *) &SERIAL_DRIVER,
		commands };

/*===========================================================================*/
/* Application threads.                                                      */
/*===========================================================================*/

/*
 * Red LED blinker thread, times are in milliseconds.
 */
static msg_t Thread1(void *arg) {

	(void) arg;

	print("Hello from Thread1()\r\n");

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
	int offset = FLASH_USER_BASE;

	(void) arg;

	while (!chThdShouldTerminate()) {
		int size = 0;
		int n = 0;
		int err;
		IHexRecord irec;
		uint16_t addressOffset = 0x00;
		uint32_t address = 0x0;
		char * p = buffer;
		uint32_t tp = 0;

		chprintf((BaseSequentialStream *)&SERIAL_DRIVER, "OFFSET: %x\r\n", offset);

		chprintf((BaseSequentialStream *)&SERIAL_DRIVER, "SIZE: ");
		size = readn();
		chprintf((BaseSequentialStream *)&SERIAL_DRIVER, "%d\r\n", size);

		if (size == 0) {
			chThdExit(0);
			return 0;
		}

		print("receiving ");
		printn(size);
		print("bytes..\r\n");

		sdRead(&SERIAL_DRIVER, buffer, size);
		p = buffer;
		buffer[size] = '\0';

		/*
		 * Here comes the flashing magic (pun intended).
		 */
		linearFlashProgramStart(&flashPage);

		chSysLock()
		;

		while ((n = Read_IHexRecord(&irec, p)) >= 0) {
			switch (irec.type) {
			case IHEX_TYPE_00: /**< Data Record */
				address = (((uint32_t) addressOffset) << 16) + irec.address;

				err = linearFlashProgram(&flashPage, address,
						(flashdata_t*) irec.data, irec.dataLen);

				if (err) {
					print("BLERR: ");
					printn(BOOTLOADER_ERROR_BAD_FLASH);
					print("\r\n");
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
				 print("\r\n");
				 */
				break;

			case IHEX_TYPE_01: /**< End of File Record */
			case IHEX_TYPE_05: /**< Start Linear Address Record */
				break;

			case IHEX_TYPE_02: /**< Extended Segment Address Record */
			case IHEX_TYPE_03: /**< Start Segment Address Record */
				print("BLERR: ");
				printn(BOOTLOADER_ERROR_BAD_HEX);
				print("\r\n");
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
		print("\r\n");

		tp += offset + 1;
		print("Starting...");

		offset = FLASH_ADDRESS_OF_PAGE(flashPage.currentPage + 1) - FLASH_BASE + FLASH_USER_BASE;

		chThdSleepMilliseconds(100);

		chThdCreateFromHeap(NULL, WA_SIZE_256B, NORMALPRIO + 1, tp, NULL);

		chThdSleepMilliseconds(100);
	}

	chThdExit(0);
	return 0;
}

/*
 * Application entry point.
 */
int main(void) {
	Thread *shelltp = NULL;
	Thread * tp;
//	appcfg_t * appcfg = (appcfg_t *) APPCFG_ADDRESS;

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

	/*
	 * Shell manager initialization.
	 */
	shellInit();

	print("Hello from main()");
	print("\r\n");

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

	while (TRUE) {
		if (!shelltp)
			shelltp = shellCreate(&shell_cfg1, SHELL_WA_SIZE, NORMALPRIO);
		else if (chThdTerminated(shelltp)) {
			chThdRelease(shelltp);
			shelltp = NULL;
		}
		chThdSleepMilliseconds(200);
	}
}

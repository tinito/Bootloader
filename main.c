#include "ch.h"
#include "hal.h"
#include "shell.h"
#include "chprintf.h"

#include "flash/flash.h"
#include "flash/helper.h"
#include "flash/ihex.h"
#include "flash/flashconfig.h"

#include "apploader.h"

#include "print.h"

static WORKING_AREA(waBlinkerThread, 128);

AppLoader app_loader;
AppLoaderConfig app_loadercfg;
Thread *app_threads[LDR_MAX_APPS] = { NULL };

static void init_loader_config(AppLoaderConfig *cfgp) {

  extern const uint8_t __ram_start__[], __ram_end__[];
  extern const uint8_t __apppgm_base__[], __apppgm_end__[];
  extern const uint8_t __appcfg_start__[], __appcfg_end__[];

  static uint8_t pagebuf[FLASH_PAGE_SIZE];
  static uint8_t ihexbuf[2 * (1 + 1 + 2 + 1 + 255 + 1)];

  cfgp->chp = (BaseChannel *)&SERIAL_DRIVER;
#if LDR_ENABLE_DEBUG
  cfgp->dbgchp = (BaseChannel *)&SERIAL_DRIVER;
#endif
  cfgp->pagebufp = pagebuf;
  cfgp->pagebuflen = sizeof(pagebuf);
  cfgp->ihexbufp = ihexbuf;
  cfgp->ihexbuflen = sizeof(ihexbuf);
  cfgp->ramstart = (uint32_t)__ram_start__;
  cfgp->ramend   = (uint32_t)__ram_end__;
  cfgp->pgmstart = (uint32_t)__apppgm_base__;
  cfgp->pgmend   = (uint32_t)__apppgm_end__;
  cfgp->cfgstart = (uint32_t)__appcfg_start__;
  cfgp->cfgend   = (uint32_t)__appcfg_end__;
}

/*===========================================================================*/
/* Command line related.                                                     */
/*===========================================================================*/

#define SHELL_WA_SIZE   THD_WA_SIZE(4096)

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

static void cmd_reboot(BaseSequentialStream *chp, int argc, char *argv[]) {

  (void)argc; (void)argv;

  chprintf(chp, "\r\n\r\nBOARD IS REBOOTING\r\n\r\n");
  boardReset();
}

static void cmd_app_list(BaseSequentialStream *chp, int argc, char *argv[]) {

  uint32_t i;
  (void)argc; (void)argv;

  chprintf(chp, "ID @.text   #.text   @.bss    #.bss    @.data   #.data   "
                "@datapgm #stack   @thread  Name\r\n");
  ldrLock(&app_loader);
  for (i = 0; i < flash_apps.numapps; ++i) {
    const app_info_t *const infop = (const app_info_t *)&flash_apps.infos[i];
    chprintf(chp, "%.2x ", (int)i);
    chprintf(chp, "%.8lx %.8lx ", infop->pgmadr, infop->pgmlen);
    chprintf(chp, "%.8lx %.8lx ", infop->bssadr, infop->bsslen);
    chprintf(chp, "%.8lx %.8lx ", infop->dataadr, infop->datalen);
    chprintf(chp, "%.8lx %.8lx ", infop->datapgmadr, infop->stacklen);
    chprintf(chp, "%.8lx %s\r\n", (uint32_t)app_threads[i], infop->name);
  }
  ldrUnlock(&app_loader);
}

static void cmd_app_install(BaseSequentialStream *chp, int argc, char *argv[]) {

  (void)chp; (void)argc, (void)argv;

  chSequentialStreamGet(chp);   /* Remove the buffered '\n'.*/
  ldrLock(&app_loader);
  ldrInstall(&app_loader);
  ldrUnlock(&app_loader);
}

static void cmd_app_run(BaseSequentialStream *chp, int argc, char *argv[]) {

  if (argc != 1) {
    chprintf(chp, "Usage:\r\n  run <appname>\r\n");
    return;
  }

  ldrLock(&app_loader);
  ldrRun(&app_loader, argv[0], app_threads);
  ldrUnlock(&app_loader);
}

static void cmd_app_remove_last(BaseSequentialStream *chp,
                                int argc, char *argv[]) {

  (void)chp; (void)argc, (void)argv;

  ldrLock(&app_loader);
  ldrRemoveLast(&app_loader);
  ldrUnlock(&app_loader);
}

static void cmd_app_remove_all(BaseSequentialStream *chp,
                               int argc, char *argv[]) {

  (void)chp; (void)argc, (void)argv;

  ldrLock(&app_loader);
  ldrRemoveAll(&app_loader);
  ldrUnlock(&app_loader);
}

static const ShellCommand commands[] = {
  { "mem",              cmd_mem },
  { "threads",          cmd_threads },
  { "reboot",           cmd_reboot },
  { "app_install",      cmd_app_install },
  { "app_run",          cmd_app_run },
  { "app_list",         cmd_app_list },
  { "app_remove_last",  cmd_app_remove_last },
  { "app_remove_all",   cmd_app_remove_all },
  { NULL, NULL }
};

static const ShellConfig shell_cfg1 = {
  (BaseSequentialStream *)&SERIAL_DRIVER,
  commands
};

/*===========================================================================*/
/* Application threads.                                                      */
/*===========================================================================*/

/*
 * Red LED blinker thread, times are in milliseconds.
 */
static msg_t BlinkerThread(void *arg) {

	(void) arg;

	while (TRUE) {
		palTogglePad(LED_GPIO, LED1);
		chThdSleepMilliseconds(200);
	}
	return CH_SUCCESS;
}

/*
 * Application entry point.
 */
int main(void) {

  Thread *shelltp = NULL;

  /*
   * System initializations.
   * - HAL initialization, this also initializes the configured device drivers
   *   and performs the board-specific initializations.
   * - Kernel initialization, the main() function becomes a thread and the
   *   RTOS is active.
   */
  halInit();
  chSysInit();

  /* Activates the serial drivers using the default configuration.*/
  sdStart(&SD1, NULL);
  sdStart(&SD3, NULL);

  /* Creates the blinker thread.*/
  chThdCreateStatic(waBlinkerThread, sizeof(waBlinkerThread), NORMALPRIO,
                    BlinkerThread, NULL);

  /* Initializes the app loader.*/
  init_loader_config(&app_loadercfg);
  ldrInit(&app_loader, &app_loadercfg);

  /* Shell manager initialization.*/
  shellInit();

  /* Background loop.*/
  while (TRUE) {
    if (shelltp == NULL) {
      shelltp = shellCreate(&shell_cfg1, SHELL_WA_SIZE, NORMALPRIO);
    } else if (chThdTerminated(shelltp)) {
      chThdRelease(shelltp);
      shelltp = NULL;
    }
    chThdSleepMilliseconds(200);
  }

  return CH_SUCCESS;
}

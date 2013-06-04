/*
 * helper.c
 *
 *  Created on: May 19, 2012
 *      Author: mabl
 */

#include "ch.h"
#include "hal.h"

#include "flash.h"
#include "helper.h"

#include <string.h>

void linearFlashProgramStart(struct LinearFlashing* flash) {

  chDbgCheck(flash->pageBuffer != NULL, "linearFlashProgramStart");
  chDbgCheck(flash->pageSize > 0, "linearFlashProgramStart");
  chDbgCheck((flash->pageSize & (sizeof(flashdata_t) - 1)) == 0,
             "linearFlashProgramStart");

  flash->pageBufferTainted = FALSE;
  flash->currentPage = 0;
}

int linearFlashProgramFinish(struct LinearFlashing* flash) {
  int err = 0;

  /* Write back last buffer if it is tainted */
  if (flash->pageBufferTainted) {
    err = flashPageWriteIfNeeded(flash->currentPage, flash->pageBuffer);
  }

  return err;
}

int linearFlashProgram(struct LinearFlashing* flash, uint32_t address,
                       const flashdata_t* buffer, int length) {
  flashpage_t oldPage;
  int pagePosition;
  int processLen;
  bool_t writeback = FALSE;
  int err;

  /* Process all given words */
  while (length > 0) {
    oldPage = flash->currentPage;
    flash->currentPage = FLASH_PAGE_OF_ADDRESS(address);
    pagePosition = address - FLASH_ADDRESS_OF_PAGE(flash->currentPage);
    processLen = (FLASH_PAGE_SIZE - pagePosition);

    /* Read back new page if page has changed. */
    if (oldPage != flash->currentPage) {
      err = flashPageRead(flash->currentPage, flash->pageBuffer);
      if (err == CH_FAILED) return -1;
      flash->pageBufferTainted = FALSE;
    }

    /* Process no more bytes than remaining */
    if (processLen > length) {
      processLen = length;
    } else if (processLen <= length) {
      writeback = TRUE;
    }

    /* Copu buffer into page buffer and mark as tainted*/
    memcpy(&flash->pageBuffer[pagePosition / sizeof(flashdata_t)], buffer,
           processLen);
    flash->pageBufferTainted = TRUE;

    /* Decrease handled bytes from total length. */
    length -= processLen;

    /* Writeback buffer if needed */
    if (writeback) {
      err = flashPageWriteIfNeeded(flash->currentPage, flash->pageBuffer);
      if (err) return err;
      writeback = FALSE;
    }
  }
  return CH_SUCCESS;
}


void flashJumpApplication(uint32_t address) {

  typedef void (*proc_f)(void);

  proc_f jumpf;
  unsigned i;

  /* Load jump address into function pointer.*/
  jumpf = (proc_f)((uint32_t *)address)[1];

  /* Reset all interrupts to default */
  chSysDisable();

  /* Clear pending interrupts just to be on the safe side.*/
  SCB_ICSR = ICSR_PENDSVCLR;

  /* Disable all interrupts.*/
  for (i = 0; i < 8; ++i) {
    NVIC->ICER[i] = NVIC->IABR[i];
  }

  /* Set stack pointer as in application's vector table.*/
  __set_MSP(((uint32_t *)address)[0]);
  jumpf();
}

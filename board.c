/*
    ChibiOS/RT - Copyright (C) 2006,2007,2008,2009,2010,
                 2011 Giovanni Di Sirio.

    This file is part of ChibiOS/RT.

    ChibiOS/RT is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    ChibiOS/RT is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "ch.h"
#include "hal.h"

/**
 * @brief   PAL setup.
 * @details Digital I/O ports static configuration as defined in @p board.h.
 *          This variable is used by the HAL when initializing the PAL driver.
 */
#if HAL_USE_PAL || defined(__DOXYGEN__)
const PALConfig pal_default_config =
{
  {VAL_GPIOAODR, VAL_GPIOACRL, VAL_GPIOACRH},
  {VAL_GPIOBODR, VAL_GPIOBCRL, VAL_GPIOBCRH},
  {VAL_GPIOCODR, VAL_GPIOCCRL, VAL_GPIOCCRH},
};
#endif

/*
 * Early initialization code.
 * This initialization must be performed just after stack setup and before
 * any other initialization.
 */
void __early_init(void) {

  stm32_clock_init();
}

/*
 * Board-specific initialization code.
 */
void boardInit(void) {
  AFIO->MAPR |= AFIO_MAPR_TIM1_REMAP_PARTIALREMAP;
  AFIO->MAPR |= AFIO_MAPR_CAN_REMAP_REMAP2;
  AFIO->MAPR |= AFIO_MAPR_USART1_REMAP;
}

void boardReset(void) {

    chThdSleep(MS2ST(10));

    /* Ensure completion of memory access. */
    __DSB();

    /* Generate reset by setting VECTRESETK and SYSRESETREQ, keeping priority group unchanged.
     * If only SYSRESETREQ used, no reset is triggered, discovered while debugging.
     * If only VECTRESETK is used, if you want to read the source of the reset afterwards
     * from (RCC->CSR & RCC_CSR_SFTRSTF),
     * it won't be possible to see that it was a software-triggered reset.
     * */
    SCB->AIRCR = (0x5FA << SCB_AIRCR_VECTKEY_Pos)
               | (SCB ->AIRCR & SCB_AIRCR_PRIGROUP_Msk)
               | SCB_AIRCR_VECTRESET_Msk
               | SCB_AIRCR_SYSRESETREQ_Msk;

    /* Ensure completion of memory access. */
    __DSB();

    /* Wait for reset. */
    for (;;) {}
}

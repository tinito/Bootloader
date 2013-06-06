/*
 * AppLoader.h
 *
 *  Created on: 08/mag/2013
 *      Author: texzk
 */

#ifndef LOADER_H_
#define LOADER_H_

#include "ch.h"
#include "io_channel.h"

#include <stdint.h>

#if !defined(LDR_ENABLE_DEBUG) || defined(__DOXYGEN__)
#define LDR_ENABLE_DEBUG        TRUE
#endif

#if !defined(LDR_CHN_READ_TIMEOUT) || defined(__DOXYGEN__)
#define LDR_CHN_READ_TIMEOUT    TIME_INFINITE
#endif

#if !defined(LDR_CHN_WRITE_TIMEOUT) || defined(__DOXYGEN__)
#define LDR_CHN_WRITE_TIMEOUT   TIME_INFINITE
#endif

#if !defined(LDR_BUF_SIZE) || defined(__DOXYGEN__)
#define LDR_BUF_SIZE            (1 << 10)
#endif
#if LDR_BUF_SIZE < 1
#error "LDR_BUF_SIZE must be >= 1"
#endif

#if !defined(LDR_MAX_APPS) || defined(__DOXYGEN__)
#define LDR_MAX_APPS            16
#endif
#if LDR_MAX_APPS < 1
#error "LDR_MAX_APPS must be >= 1"
#endif

#if !defined(LDR_MAX_NAME_LENGTH) || defined(__DOXYGEN__)
#define LDR_MAX_NAME_LENGTH     ((1 << 4) - 1)
#endif
#if LDR_MAX_NAME_LENGTH < 1
#error "LDR_MAX_NAME_LENGTH must be >= 1"
#endif

#if !defined(LDR_FLASH_ALIGN) || defined(__DOXYGEN__)
#define LDR_FLASH_ALIGN         2
#endif

#if !defined(LDR_PGM_ALIGN) || defined(__DOXYGEN__)
#define LDR_PGM_ALIGN_SIZE      16
#endif
#if LDR_PGM_ALIGN_SIZE < 1
#error "LDR_PGM_ALIGN_SIZE must be >= 1"
#endif

#define LDR_PGM_ALIGN_MASK  (LDR_PGM_ALIGN_SIZE - 1)

#define LDR_PGM_ALIGN_PREV(p) \
  ((uint32_t)(p) & ~LDR_PGM_ALIGN_MASK)

#define LDR_PGM_ALIGN_NEXT(p) \
  (LDR_PGM_ALIGN_PREV((uint32_t)(p) + LDR_PGM_ALIGN_MASK))

#define LDR_PGM_IS_ALIGNED(p) \
  (((uint32_t)(p) & LDR_PGM_ALIGN_MASK) == 0)

typedef enum ldr_err_t {
  LDR_OK            = 0,
  LDR_ERR_NOMEM     = -1,
  LDR_ERR_TIMEOUT   = -2,
  LDR_ERR_BADVAL    = -3,
  LDR_ERR_EOF       = -4,
} ldr_err_t;

typedef struct app_info_t {
  char      name[LDR_MAX_NAME_LENGTH + 1]
            __attribute__((aligned(LDR_FLASH_ALIGN)));
  uint32_t  pgmlen;
  uint32_t  pgmadr;
  uint32_t  bsslen;
  uint32_t  bssadr;
  uint32_t  datalen;
  uint32_t  dataadr;
  uint32_t  datapgmadr;
  uint32_t  stacklen;
  uint32_t  threadadr;
} app_info_t;

typedef struct app_flash_t {
  uint32_t      numapps __attribute__((aligned(LDR_FLASH_ALIGN)));
  uint32_t      freeadr __attribute__((aligned(LDR_FLASH_ALIGN)));
  app_info_t    infos[LDR_MAX_APPS] __attribute__((aligned(LDR_FLASH_ALIGN)));
} __attribute__((packed)) app_flash_t;

typedef struct AppLoaderConfig {
  BaseChannel   *chp;
#if LDR_ENABLE_DEBUG
  BaseChannel   *dbgchp;
#endif
  uint8_t       *pagebufp;
  size_t        pagebuflen;
  uint8_t       *ihexbufp;
  size_t        ihexbuflen;
  uint32_t      ramstart;
  uint32_t      ramend;
  uint32_t      pgmstart;
  uint32_t      pgmend;
  uint32_t      cfgstart;
  uint32_t      cfgend;
} AppLoaderConfig;

typedef struct AppLoader {
  const AppLoaderConfig *cfgp;
  Mutex                 lock;
  uint32_t              freeadr;
  app_info_t            info;
  uint32_t              numapps;
} AppLoader;

extern const volatile app_flash_t flash_apps;

void ldrInit(AppLoader *ldrp, const AppLoaderConfig *cfgp);
void ldrLock(AppLoader *ldrp);
void ldrUnlock(AppLoader *ldrp);
ldr_err_t ldrInstall(AppLoader *ldrp);
ldr_err_t ldrRemoveLast(AppLoader *ldrp);
ldr_err_t ldrRemoveAll(AppLoader *ldrp);
ldr_err_t ldrRun(AppLoader *ldrp, const char *namep, Thread **threadsp);
ldr_err_t ldrGetInfo(AppLoader *ldrp, const char *namep, app_info_t *infop);

#endif /* LOADER_H_ */

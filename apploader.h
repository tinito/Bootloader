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

/**
 * @brief   Enables debug messages.
 */
#if !defined(LDR_ENABLE_DEBUG) || defined(__DOXYGEN__)
#define LDR_ENABLE_DEBUG        TRUE
#endif

/**
 * @brief   Read timeout.
 */
#if !defined(LDR_CHN_READ_TIMEOUT) || defined(__DOXYGEN__)
#define LDR_CHN_READ_TIMEOUT    TIME_INFINITE
#endif

/**
 * @brief   Write timeout.
 */
#if !defined(LDR_CHN_WRITE_TIMEOUT) || defined(__DOXYGEN__)
#define LDR_CHN_WRITE_TIMEOUT   TIME_INFINITE
#endif

/**
 * @brief   Size of the page buffer.
 */
#if !defined(LDR_BUF_SIZE) || defined(__DOXYGEN__)
#define LDR_BUF_SIZE            (1 << 10)
#endif
#if LDR_BUF_SIZE < 1
#error "LDR_BUF_SIZE must be >= 1"
#endif

/**
 * @brief   Maximum number of user applications.
 */
#if !defined(LDR_MAX_APPS) || defined(__DOXYGEN__)
#define LDR_MAX_APPS            16
#endif
#if LDR_MAX_APPS < 1
#error "LDR_MAX_APPS must be >= 1"
#endif

/**
 * @brief   Maximum application name length.
 * @details Best if in the form: @f$ 2^n - 1 @f$.
 */
#if !defined(LDR_MAX_NAME_LENGTH) || defined(__DOXYGEN__)
#define LDR_MAX_NAME_LENGTH     ((1 << 4) - 1)
#endif
#if LDR_MAX_NAME_LENGTH < 1
#error "LDR_MAX_NAME_LENGTH must be >= 1"
#endif

/**
 * @brief   Flash memory word alignment.
 */
#if !defined(LDR_FLASH_ALIGN) || defined(__DOXYGEN__)
#define LDR_FLASH_ALIGN         2
#endif

/**
 * @brief   Program code alignment.
 * @details Architecture-dependent. Must be in the form: @f$ 2^n - 1 @f$.
 */
#if !defined(LDR_PGM_ALIGN) || defined(__DOXYGEN__)
#define LDR_PGM_ALIGN_SIZE      16
#endif
#if LDR_PGM_ALIGN_SIZE < 1
#error "LDR_PGM_ALIGN_SIZE must be >= 1"
#endif

/**
 * @brief   Program code alignment offset mask.
 */
#define LDR_PGM_ALIGN_MASK  (LDR_PGM_ALIGN_SIZE - 1)

/**
 * @brief   Aligns to the nearest previous program code word.
 * @details Behaves like @p floor() but for program code word alignment.
 * @param[in] p
 *          Reference address.
 */
#define LDR_PGM_ALIGN_PREV(p) \
  ((uint32_t)(p) & ~LDR_PGM_ALIGN_MASK)

/**
 * @brief   Aligns to the nearest next program code word.
 * @details Behaves like @p ceil() but for program code word alignment.
 * @param[in] p
 *          Reference address.
 */
#define LDR_PGM_ALIGN_NEXT(p) \
  (LDR_PGM_ALIGN_PREV((uint32_t)(p) + LDR_PGM_ALIGN_MASK))

/**
 * @brief   Checks if an address is aligned to the program code.
 * @param[in] p
 *          Reference address.
 */
#define LDR_PGM_IS_ALIGNED(p) \
  (((uint32_t)(p) & LDR_PGM_ALIGN_MASK) == 0)

/**
 * @brief   App loader error codes.
 */
typedef enum ldr_err_t {
  LDR_OK            = 0,    /**< @brief No error.*/
  LDR_ERR_NOMEM     = -1,   /**< @brief Not enough memory.*/
  LDR_ERR_TIMEOUT   = -2,   /**< @brief Timed out.*/
  LDR_ERR_BADVAL    = -3,   /**< @brief Wrong parameter or stream value.*/
  LDR_ERR_EOF       = -4,   /**< @brief Stream reset or closed.*/
} ldr_err_t;

/**
 * @brief   App informations.
 */
typedef struct app_info_t {
  /** @brief App name string, null-terminated.*/
  char      name[LDR_MAX_NAME_LENGTH + 1]
            __attribute__((aligned(LDR_FLASH_ALIGN)));
  uint32_t  pgmlen;         /**< @brief <tt>.text</tt> length.*/
  uint32_t  pgmadr;         /**< @brief <tt>.text</tt> flash address.*/
  uint32_t  bsslen;         /**< @brief <tt>.bss</tt> length.*/
  uint32_t  bssadr;         /**< @brief <tt>.bss</tt> RAM address.*/
  uint32_t  datalen;        /**< @brief <tt>.data</tt> length.*/
  uint32_t  dataadr;        /**< @brief <tt>.data</tt> RAM address.*/
  uint32_t  datapgmadr;     /**< @brief <tt>.data</tt> flash address.*/
  uint32_t  stacklen;       /**< @brief Entry point thread stack length.*/
  uint32_t  threadadr;      /**< @brief Entry point thread address.*/
} app_info_t;

/**
 * @brief   Flash layout register for apps.
 */
typedef struct app_flash_t {
  /** @brief Number of installed apps.*/
  uint32_t      numapps __attribute__((aligned(LDR_FLASH_ALIGN)));
  /** @brief First free program memory address.*/
  uint32_t      freeadr __attribute__((aligned(LDR_FLASH_ALIGN)));
  /** @brief App information register.*/
  app_info_t    infos[LDR_MAX_APPS] __attribute__((aligned(LDR_FLASH_ALIGN)));
} __attribute__((packed)) app_flash_t;

/**
 * @brief   App loader configuration.
 */
typedef struct AppLoaderConfig {
  /* Communication channels.*/
  BaseChannel   *chp;       /**< @brief Data channel.*/
#if LDR_ENABLE_DEBUG
  BaseChannel   *dbgchp;    /**< @brief Debug channel.*/
#endif

  /* Buffers.*/
  uint8_t       *pagebufp;  /**< @brief Flash page buffer address.*/
  size_t        pagebuflen; /**< @brief Flash page buffer length.*/
  uint8_t       *ihexbufp;  /**< @brief IHEX buffer address.*/
  size_t        ihexbuflen; /**< @brief IHEX buffer length.*/

  /* Memory limits.*/
  uint32_t      ramstart;   /**< @brief RAM start address.*/
  uint32_t      ramend;     /**< @brief RAM end address.*/
  uint32_t      pgmstart;   /**< @brief Program code start address.*/
  uint32_t      pgmend;     /**< @brief Program code end address.*/
  uint32_t      cfgstart;   /**< @brief App configuration start address.*/
  uint32_t      cfgend;     /**< @brief App configuration end address.*/
} AppLoaderConfig;

/**
 * @brief   App loader status.
 */
typedef struct AppLoader {
  const AppLoaderConfig *cfgp;      /**< @brief Configuration pointer.*/
  Mutex                 lock;       /**< @brief Lock word.*/
  uint32_t              numapps;    /**< @brief Number of installed apps.*/
  uint32_t              freeadr;    /**< @brief Free program memory address.*/
  app_info_t            info;       /**< @brief Current app info.*/
} AppLoader;

extern const volatile app_flash_t flash_apps;

#ifdef __cplusplus
extern "C" {
#endif

void ldrInit(AppLoader *ldrp, const AppLoaderConfig *cfgp);
void ldrLock(AppLoader *ldrp);
void ldrUnlock(AppLoader *ldrp);
ldr_err_t ldrInstall(AppLoader *ldrp);
ldr_err_t ldrRemoveLast(AppLoader *ldrp);
ldr_err_t ldrRemoveAll(AppLoader *ldrp);
ldr_err_t ldrRun(AppLoader *ldrp, const char *namep, Thread *threadsp[]);
ldr_err_t ldrGetInfo(AppLoader *ldrp, const char *namep, app_info_t *infop);

#ifdef __cplusplus
}
#endif
#endif /* LOADER_H_ */

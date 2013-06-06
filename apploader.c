

#include "apploader.h"

#include "ch.h"
#include "hal.h"
#include "io_channel.h"

#include "flash/flash.h"
#include "flash/helper.h"

#include <string.h>


#define CHNGET(chn) \
  chnGetTimeout((BaseChannel *)(chn), LDR_CHN_READ_TIMEOUT)

#define CHNPUT(chn, c) \
  chnPutTimeout((BaseChannel *)(chn), (const uint8_t)(c), \
                LDR_CHN_WRITE_TIMEOUT)

#define CHNREAD(chn, bufp, buflen) \
  chnReadTimeout((BaseChannel *)(chn), (const uint8_t *)(bufp), \
                 (size_t)(buflen), LDR_CHN_READ_TIMEOUT)

#define CHNWRITE(chn, bufp, buflen) \
  chnWriteTimeout((BaseChannel *)(chn), (const uint8_t *)(bufp), \
                  (size_t)(buflen), LDR_CHN_WRITE_TIMEOUT)

#if LDR_ENABLE_DEBUG
#define DBG(expr)       expr
#else
#define DBG(expr)
#endif

#define DBGCH           (ldrp->cfgp->dbgchp)

#define DBGPRINT(textp) \
  DBG(CHNWRITE(DBGCH, (const uint8_t *)(textp), strlen((const char *)(textp))))

enum ldr_ihex_type_t {
  IHEX_TYPE_DATA    = 0x00, /**< Data record.*/
  IHEX_TYPE_EOF     = 0x01, /**< End of file record.*/
  IHEX_TYPE_ESA     = 0x02, /**< Extended segment address record.*/
  IHEX_TYPE_SSA     = 0x03, /**< Start segment address record.*/
  IHEX_TYPE_ELA     = 0x04, /**< Extended linear address record.*/
  IHEX_TYPE_SLA     = 0x05, /**< Start linear address record.*/

  IHEX_TYPE__LENGTH
};

typedef struct ihex_record_t {
  uint8_t   count;
  uint16_t  offset;
  uint8_t   type;
  uint8_t   *datap;
  uint8_t   checksum;
} IhexRecord;

static const char hex2char[16] = {
  '0', '1', '2', '3', '4', '5', '6', '7',
  '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
};

const volatile app_flash_t flash_apps
__attribute__ ((section(".appcfg"), aligned(2))) = {
  0, 0, { { "", 0, 0, 0, 0, 0, 0, 0, 0, 0 } }
};

static ldr_err_t expect_char(BaseChannel *chp, const char c) {

  if (CHNGET(chp) != (msg_t)c) return LDR_ERR_BADVAL;
  return LDR_OK;
}

static ldr_err_t expect_char_ws(BaseChannel *chp, const char c) {

  register msg_t r;
  while ((r = CHNGET(chp)) != (const msg_t)c) {
    if (r == Q_OK) return LDR_ERR_EOF;
    if (r != ' ' && r != '\r' && r != '\n' && r != '\t' && r != '\v')
      return LDR_ERR_BADVAL;
  }
  return LDR_OK;
}

static ldr_err_t expect_nl(BaseChannel *chp) {

  if (CHNGET(chp) != '\r') return LDR_ERR_BADVAL;
  if (CHNGET(chp) != '\n') return LDR_ERR_BADVAL;
  return LDR_OK;
}

static ldr_err_t read_char(BaseChannel *chp, char *cp) {

  msg_t c = CHNGET(chp);
  if (c == Q_RESET) return LDR_ERR_EOF;
  *cp = (char)c;
  return LDR_OK;
}

static ldr_err_t write_char(BaseChannel *chp, const char c) {

  if (CHNPUT(chp, c) != Q_OK) return LDR_ERR_EOF;
  return LDR_OK;
}

static ldr_err_t write_nl(BaseChannel *chp) {

  if (CHNPUT(chp, '\r') != Q_OK) return LDR_ERR_EOF;
  if (CHNPUT(chp, '\n') != Q_OK) return LDR_ERR_EOF;
  return LDR_OK;
}

static ldr_err_t read_byte(BaseChannel *chp, uint8_t *bytep) {

  register msg_t c;

  /* High nibble.*/
  c = CHNGET(chp);
  if (c == Q_RESET) return LDR_ERR_EOF;
  if      (c >= '0' && c <= '9') c -= '0';
  else if (c >= 'a' && c <= 'f') c += 10 - 'a';
  else if (c >= 'A' && c <= 'F') c += 10 - 'A';
  else return LDR_ERR_BADVAL;
  *bytep = (uint8_t)c << 4;

  /* Low nibble.*/
  c = CHNGET(chp);
  if (c == Q_RESET) return LDR_ERR_EOF;
  if      (c >= '0' && c <= '9') c -= '0';
  else if (c >= 'a' && c <= 'f') c += 10 - 'a';
  else if (c >= 'A' && c <= 'F') c += 10 - 'A';
  else return LDR_ERR_BADVAL;
  *bytep |= (uint8_t)c & 0x0F;

  return LDR_OK;
}

static ldr_err_t read_dword(BaseChannel *chp, uint32_t *ptrp) {

  register msg_t c;
  register uint_fast8_t i;
  *ptrp = 0;
  for (i = 2 * sizeof(uint32_t); i--;) {
    c = CHNGET(chp);
    if (c == Q_RESET) return LDR_ERR_EOF;
    if      (c >= '0' && c <= '9') c -= '0';
    else if (c >= 'a' && c <= 'f') c += 10 - 'a';
    else if (c >= 'A' && c <= 'F') c += 10 - 'A';
    else return LDR_ERR_BADVAL;
    *ptrp = (*ptrp << 4) | (uint32_t)(c & 0x0F);
  }
  return LDR_OK;
}

static ldr_err_t write_byte(BaseChannel *chp, uint8_t byte) {

  if (CHNPUT(chp, hex2char[byte >> 4]) != Q_OK)
    return LDR_ERR_EOF;
  if (CHNPUT(chp, hex2char[byte & 0x0F]) != Q_OK)
    return LDR_ERR_EOF;
  return LDR_OK;
}

static ldr_err_t write_dword(BaseChannel *chp, uint32_t ptr) {

  register uint_fast8_t i;
  for (i = 2 * sizeof(uint32_t); i--; ptr <<= 4) {
    if (CHNPUT(chp, hex2char[(ptr >> (sizeof(uint32_t) * 8 - 4)) & 0x0F])
        != Q_OK)
      return LDR_ERR_EOF;
  }
  return LDR_OK;
}

static ldr_err_t read_ihex_record(AppLoader *ldrp, IhexRecord *recordp) {

  BaseChannel *const chp = ldrp->cfgp->chp;
  uint8_t b, i;

#define CHECK(x) { register ldr_err_t e = (x); if (e != LDR_OK) return e; }

  recordp->count = 0;
  recordp->type = IHEX_TYPE__LENGTH;
  recordp->checksum = 0;

  /* Skip after the start code.*/
  CHECK(expect_char_ws(chp, ':')); DBGPRINT(":");

  /* Byte count.*/
  CHECK(read_byte(chp, &b)); DBG(write_byte(DBGCH, b)); DBGPRINT(" ");
  recordp->count = b;
  recordp->checksum = b;

  /* Address.*/
  CHECK(read_byte(chp, &b)); DBG(write_byte(DBGCH, b));
  recordp->offset = (uint16_t)b << 8;
  recordp->checksum += b;
  CHECK(read_byte(chp, &b)); DBG(write_byte(DBGCH, b)); DBGPRINT(" ");
  recordp->offset |= (uint16_t)b;
  recordp->checksum += b;

  /* Record type.*/
  CHECK(read_byte(chp, &b)); DBG(write_byte(DBGCH, b)); DBGPRINT(" ");
  if (b >= IHEX_TYPE__LENGTH) return LDR_ERR_BADVAL;
  recordp->type = b;
  recordp->checksum += b;

  /* Data.*/
  for (i = 0; i < recordp->count; ++i) {
    CHECK(read_byte(chp, &b)); DBG(write_byte(DBGCH, b));
    recordp->datap[i] = b;
    recordp->checksum += b;
  }
  DBGPRINT(" ");

  /* Checksum.*/
  CHECK(read_byte(chp, &b)); DBG(write_byte(DBGCH, b)); DBGPRINT(" ");
  if (((recordp->checksum + b) & 0xFF) != 0) {
    recordp->checksum = b;
    return LDR_ERR_BADVAL;
  } else {
    recordp->checksum = b;
  }
  DBGPRINT("\r\n");
  return LDR_OK;

#undef CHECK
}

static ldr_err_t process_ihex(AppLoader *ldrp, const AppLoaderConfig *cfgp) {

  IhexRecord record;
  struct LinearFlashing lf;

  uint32_t base = 0;

#define CHECK(x) { register ldr_err_t e = (x); if (e != LDR_OK) return e; }

  lf.pageBuffer = (flashdata_t *)cfgp->pagebufp;
  lf.pageSize = cfgp->pagebuflen;
  linearFlashProgramStart(&lf);
  record.datap = cfgp->ihexbufp;

  for (;;) {
    CHNWRITE(cfgp->chp, (const uint8_t *)",\r\n", 3);
    CHECK(read_ihex_record(ldrp, &record));
    switch (record.type) {
    case IHEX_TYPE_DATA: {
      /* Write to the correct memory area.*/
      uint32_t start, endx;
      start = base + record.offset;
      endx = start + record.count;
      if (endx <= ldrp->info.pgmadr + ldrp->info.pgmlen &&
          start >= ldrp->info.pgmadr) {
        /* ".text" segment.*/
        DBGPRINT(".text <- 0x"); DBG(write_dword(DBGCH, start));
        DBGPRINT(" - 0x"); DBG(write_dword(DBGCH, endx - 1)); DBGPRINT("\r\n");
        if (0 != linearFlashProgram(&lf, base + record.offset,
                                    (const flashdata_t *)record.datap,
                                    record.count)) {
          DBGPRINT("ERROR: Cannot write some \".text\" into FLASH\r\n");
          return LDR_ERR_NOMEM;
        }
      }
      else if (endx <= ldrp->info.datapgmadr + ldrp->info.datalen &&
               start >= ldrp->info.datapgmadr) {
        /* ".data" segment.*/
        DBGPRINT(".data <- 0x"); DBG(write_dword(DBGCH, start));
        DBGPRINT(" - 0x"); DBG(write_dword(DBGCH, endx - 1)); DBGPRINT("\r\n");
        if (0 != linearFlashProgram(&lf, base + record.offset,
                                    (const flashdata_t *)record.datap,
                                    record.count)) {
          DBGPRINT("ERROR: Cannot write some \".data\" into FLASH\r\n");
          return LDR_ERR_NOMEM;
        }
      }
      else {
        DBGPRINT("ERROR: Invalid range: 0x"); DBG(write_dword(DBGCH, start));
        DBGPRINT(" - 0x"); DBG(write_dword(DBGCH, endx - 1)); DBGPRINT("\r\n");
        return LDR_ERR_BADVAL;
      }
      break;
    }
    case IHEX_TYPE_EOF: {
      if (record.count  != 0) return LDR_ERR_BADVAL;
      if (record.offset != 0) return LDR_ERR_BADVAL;
      linearFlashProgramFinish(&lf);
      DBGPRINT("Reached IHEX EOF\r\n");
      return LDR_OK;
    }
    case IHEX_TYPE_ESA: {
      if (record.count  != 2) return LDR_ERR_BADVAL;
      if (record.offset != 0) return LDR_ERR_BADVAL;
      base  = (uint32_t)record.datap[0] << 12;
      base |= (uint32_t)record.datap[1] <<  4;
      DBGPRINT("base = 0x"); DBG(write_dword(DBGCH, base)); DBGPRINT("\r\n");
      break;
    }
    case IHEX_TYPE_SSA: {
      if (record.count  != 4) return LDR_ERR_BADVAL;
      if (record.offset != 0) return LDR_ERR_BADVAL;
      break;
    }
    case IHEX_TYPE_ELA: {
      if (record.count  != 2) return LDR_ERR_BADVAL;
      if (record.offset != 0) return LDR_ERR_BADVAL;
      base  = (uint32_t)record.datap[0] << 24;
      base |= (uint32_t)record.datap[1] << 16;
      DBGPRINT("base = 0x"); DBG(write_dword(DBGCH, base)); DBGPRINT("\r\n");
      break;
    }
    case IHEX_TYPE_SLA: {
      uint32_t addr;
      if (record.count  != 4) return LDR_ERR_BADVAL;
      if (record.offset != 0) return LDR_ERR_BADVAL;
      addr  = (uint32_t)record.datap[0] << 24;
      addr |= (uint32_t)record.datap[1] << 16;
      addr |= (uint32_t)record.datap[2] <<  8;
      addr |= (uint32_t)record.datap[3];
      ldrp->info.threadadr = addr;
      DBGPRINT("app_thread = 0x"); DBG(write_dword(DBGCH, addr));
      DBGPRINT("\r\n");
      break;
    }
    default: return LDR_ERR_BADVAL;
    }
  }
  chDbgPanic("Unreachable code");
  return LDR_ERR_BADVAL;

#undef CHECK
}

static ldr_err_t recv_lengths(AppLoader *ldrp, app_info_t *infop) {

  BaseChannel *const chp = ldrp->cfgp->chp;
  uint8_t namelen, trunclen, i;

#define CHECK(x)    { register ldr_err_t e = (x); \
                      if (e != LDR_OK) return e; }

  DBGPRINT("  pgmlen   = 0x");
  CHECK(read_dword(chp, &infop->pgmlen));
  DBG(write_dword(DBGCH, infop->pgmlen));
  CHECK(expect_char(chp, ','));

  DBGPRINT("\r\n  bsslen   = 0x");
  CHECK(read_dword(chp, &infop->bsslen));
  DBG(write_dword(DBGCH, infop->bsslen));
  CHECK(expect_char(chp, ','));

  DBGPRINT("\r\n  datalen  = 0x");
  CHECK(read_dword(chp, &infop->datalen));
  DBG(write_dword(DBGCH, infop->datalen));
  CHECK(expect_char(chp, ','));

  DBGPRINT("\r\n  stacklen = 0x");
  CHECK(read_dword(chp, &infop->stacklen));
  DBG(write_dword(DBGCH, infop->stacklen));
  CHECK(expect_char(chp, ','));

  DBGPRINT("\r\n  name     = \"");
  CHECK(read_byte(chp, &namelen));
  trunclen = (namelen > LDR_MAX_NAME_LENGTH) ? LDR_MAX_NAME_LENGTH : namelen;
  for (i = 0; i < trunclen; ++i)
    CHECK(read_char(chp, &infop->name[i]));
  for (; i < namelen; ++i)
    CHECK(read_char(chp, &infop->name[trunclen]));
  infop->name[trunclen] = 0;
  DBGPRINT("\"\r\n");
  CHECK(expect_nl(chp));

  return LDR_OK;
#undef CHECK
}

static ldr_err_t compute_addresses(AppLoader *ldrp) {

    /* Update the whole stack size.*/
  if (ldrp->info.stacklen == 0) {
    DBGPRINT("ERROR: Stack length for app_thread() cannot be zero\r\n");
    return LDR_ERR_BADVAL;
  }
  ldrp->info.stacklen = THD_WA_SIZE(ldrp->info.stacklen);

  /* Allocate the program memory.*/
  ldrp->info.pgmadr = ldrp->freeadr = LDR_PGM_ALIGN_NEXT(ldrp->freeadr);
  ldrp->info.datapgmadr = ldrp->info.pgmadr +
                          LDR_PGM_ALIGN_NEXT(ldrp->info.pgmlen);
  if (ldrp->info.datapgmadr + LDR_PGM_ALIGN_NEXT(ldrp->info.datalen) >
      LDR_PGM_ALIGN_PREV(ldrp->cfgp->pgmend)) {
    DBGPRINT("ERROR: Not enough FLASH to store program and data\r\n");
    return LDR_ERR_NOMEM;
  }
  DBGPRINT("  pgmadr     = 0x");
  DBG(write_dword(DBGCH, ldrp->info.pgmadr)); DBGPRINT("\r\n");
  if (ldrp->info.datalen > 0) {
    DBGPRINT("  datapgmadr = 0x");
    DBG(write_dword(DBGCH, ldrp->info.datapgmadr)); DBGPRINT("\r\n");
  } else {
    ldrp->info.datapgmadr = 0;
  }

  /* Allocate the ".bss" segment.*/
  ldrp->info.bssadr = 0;
  if (ldrp->info.bsslen > 0) {
    ldrp->info.bssadr = (uint32_t)chCoreTrim(ldrp->info.bsslen);
    if (ldrp->info.bssadr == 0) {
      DBGPRINT("ERROR: Cannot allocate \".bss\"\r\n");
      return LDR_ERR_NOMEM;
    }
    DBGPRINT("  bssadr     = 0x");
    DBG(write_dword(DBGCH, ldrp->info.bssadr)); DBGPRINT("\r\n");
    memset((void *)ldrp->info.bssadr, 0x00, ldrp->info.bsslen);
  }

  /* Allocate the ".data" segment.*/
  ldrp->info.dataadr = 0;
  if (ldrp->info.datalen > 0) {
    ldrp->info.dataadr = (uint32_t)chCoreTrim(ldrp->info.datalen);
    if (ldrp->info.dataadr == 0) {
      DBGPRINT("ERROR: Cannot allocate \".data\"\r\n");
      return LDR_ERR_NOMEM;
    }
    DBGPRINT("  dataadr    = 0x");
    DBG(write_dword(DBGCH, ldrp->info.dataadr)); DBGPRINT("\r\n");
    memset((void *)ldrp->info.dataadr, 0xCC, ldrp->info.datalen);
  }
  return LDR_OK;
}

static ldr_err_t send_addresses(AppLoader *ldrp, app_info_t *infop) {

  BaseChannel *const chp = ldrp->cfgp->chp;

#define CHECK(x)    { register ldr_err_t e = (x); \
                      if (e != LDR_OK) return e; }

  CHECK(write_dword(chp, infop->pgmadr));
  CHECK(write_char(chp, ','));
  CHECK(write_dword(chp, infop->bssadr));
  CHECK(write_char(chp, ','));
  CHECK(write_dword(chp, infop->dataadr));
  CHECK(write_char(chp, ','));
  CHECK(write_dword(chp, infop->datapgmadr));
  CHECK(write_nl(chp));

  return LDR_OK;
#undef CHECK
}

static ldr_err_t update_flash(AppLoader *ldrp, app_info_t *lastinfop) {

  struct LinearFlashing lf;

  lf.pageBuffer = (flashdata_t *)ldrp->cfgp->pagebufp;
  lf.pageSize = ldrp->cfgp->pagebuflen;
  linearFlashProgramStart(&lf);

  /* Number of stored applications.*/
  linearFlashProgram(&lf, (uint32_t)&flash_apps.numapps,
                     (const flashdata_t *)&ldrp->numapps,
                     (int)sizeof(flash_apps.numapps));

  /* Free app program memory address.*/
  linearFlashProgram(&lf, (uint32_t)&flash_apps.freeadr,
                     (const flashdata_t *)&ldrp->freeadr,
                     (int)sizeof(flash_apps.freeadr));

  /* Last application information.*/
  if (ldrp->numapps > 0 && lastinfop != NULL) {
    linearFlashProgram(&lf, (uint32_t)&flash_apps.infos[ldrp->numapps - 1],
                       (const flashdata_t *)lastinfop,
                       (int)sizeof(app_info_t));
  }

  linearFlashProgramFinish(&lf);
  return LDR_OK;
}

void ldrInit(AppLoader *ldrp, const AppLoaderConfig *cfgp) {

  extern const uint8_t __apppgm_base__[];

  chDbgCheck(ldrp != NULL, "ldrInit");
  chDbgCheck(cfgp != NULL, "ldrInit");

  chMtxInit(&ldrp->lock);

  ldrp->cfgp = cfgp;
  ldrp->numapps = flash_apps.numapps;
  if (ldrp->numapps > 0 && ldrp->numapps <= LDR_MAX_APPS)
    ldrp->freeadr = LDR_PGM_ALIGN_NEXT(flash_apps.freeadr);
  else
    ldrp->freeadr = LDR_PGM_ALIGN_NEXT((uint32_t)__apppgm_base__);
}

void ldrLock(AppLoader *ldrp) {

  chMtxLock(&ldrp->lock);
}

void ldrUnlock(AppLoader *ldrp) {

  Mutex *lockp = chMtxUnlock(); (void)lockp;
  chDbgCheck(lockp == &ldrp->lock, "ldrUnlock");
}

ldr_err_t ldrInstall(AppLoader *ldrp) {

  const AppLoaderConfig *cfgp;
  ldr_err_t err;

  chDbgCheck(ldrp != NULL, "ldrInstall");
  cfgp = ldrp->cfgp;
  chDbgCheck(cfgp != NULL, "ldrInstall");
  chDbgCheck(cfgp->chp != NULL, "ldrInstall");
  chDbgCheck(cfgp->pagebufp != NULL, "ldrInstall");
  chDbgCheck(cfgp->pagebuflen > 0, "ldrInstall");
  chDbgCheck(cfgp->ihexbufp != NULL, "ldrInstall");
  chDbgCheck(cfgp->ihexbuflen > 0, "ldrInstall");
#define CHECK(x) { if ((err = (x)) != LDR_OK) goto error_dealloc; }

  memset((void *)&ldrp->info, 0, sizeof(app_info_t));

  /* Receive the desired segment lengths.*/
  DBGPRINT("Receiving segment lengths\r\n");
  CHECK(recv_lengths(ldrp, &ldrp->info));

  /* Compute offsets and allocate RAM chunks.*/
  DBGPRINT("Computing offsets\r\n");
  CHECK(compute_addresses(ldrp));

  /* Send back the computed section offsets.*/
  DBGPRINT("\r\nSending computed offsets\r\n");
  CHECK(send_addresses(ldrp, &ldrp->info));

  /* Receive and store the IHEX file.*/
  DBGPRINT("Processing IHEX file\r\n");
  CHECK(process_ihex(ldrp, cfgp));

  /* Terminate the installation stream.*/
  write_char(cfgp->chp, '$'); write_nl(cfgp->chp);

  /* Update the next base pointer.*/
  ++ldrp->numapps;
  ldrp->freeadr += LDR_PGM_ALIGN_NEXT(ldrp->info.pgmlen) +
                    LDR_PGM_ALIGN_NEXT(ldrp->info.datalen);
  DBGPRINT("Next program base: 0x");
  DBG(write_dword(DBGCH, (uint32_t)ldrp->freeadr)); DBGPRINT("\r\n");

  /* Update the flash app layout.*/
  DBGPRINT("Updating the flash app layout\r\n");
  update_flash(ldrp, &ldrp->info);

  DBGPRINT("Installation done successfully\r\n");
  return LDR_OK;

error_dealloc:
  /* Restore the core memory.*/
  if (ldrp->info.bssadr != 0) {
    chCoreGrow(ldrp->info.bsslen);
    ldrp->info.bssadr = 0;
  }
  if (ldrp->info.dataadr != 0) {
    chCoreGrow(ldrp->info.datalen);
    ldrp->info.dataadr = 0;
  }

  /* Consume incoming data and send the error string.*/
  while (chnGetTimeout(cfgp->chp, TIME_IMMEDIATE) >= 0) {}
  write_char(cfgp->chp, '!'); write_nl(cfgp->chp);
  DBGPRINT("\r\nSome errors occurred during installation\r\n");
  return err;

#undef CHECK
}

ldr_err_t ldrRemoveLast(AppLoader *ldrp) {

  uint32_t oldstart;
  ldr_err_t err;

  chDbgCheck(ldrp != NULL, "ldrRemoveLast");

  if (ldrp->numapps == 0) {
    DBGPRINT("ERROR: There are no installed apps\r\n");
    return LDR_ERR_NOMEM;
  }

  --ldrp->numapps;
  ldrp->info = flash_apps.infos[ldrp->numapps];
  DBGPRINT("Removing app \""); DBGPRINT(ldrp->info.name); DBGPRINT("\"\r\n");

  oldstart = ldrp->freeadr;
  ldrp->freeadr -= LDR_PGM_ALIGN_NEXT(ldrp->info.pgmlen) +
                    LDR_PGM_ALIGN_NEXT(ldrp->info.datalen);
  err = update_flash(ldrp, NULL);
  if (err != LDR_OK) {
    ldrp->freeadr = oldstart;
    ++ldrp->numapps;
    DBGPRINT("ERROR: Cannot update FLASH app layout\r\n");
    return err;
  }
  DBGPRINT("App removed correctly\r\n");
  return LDR_OK;
}

ldr_err_t ldrRemoveAll(AppLoader *ldrp) {

  chDbgCheck(ldrp != NULL, "ldrRemoveLast");

  DBGPRINT("Removing all installed applications\r\n");
  ldrp->numapps = 0;
  ldrp->freeadr = ldrp->cfgp->pgmstart;
  return update_flash(ldrp, NULL);
}

ldr_err_t ldrRun(AppLoader *ldrp, const char *namep, Thread **threadsp) {

  uint32_t i;

  chDbgCheck(ldrp != NULL, "ldrRun");
  chDbgCheck(namep != NULL, "ldrRun");
  chDbgCheck(threadsp != NULL, "ldrRun");

  for (i = 0; i < flash_apps.numapps; ++i) {
    const app_info_t *const infop = (const app_info_t *)&flash_apps.infos[i];
    if (0 == strncmp(infop->name, namep, LDR_MAX_NAME_LENGTH)) {
      if (threadsp[i] == NULL) {
        /* Copy the ".data" segment into RAM.*/
        memcpy((void *)infop->dataadr, (void *)infop->datapgmadr,
               (size_t)infop->datalen);

        /* Start the app entry point thread.*/
        threadsp[i] = chThdCreateFromHeap(NULL, infop->stacklen, NORMALPRIO,
                                         (tfunc_t)infop->threadadr,
                                         (void *)threadsp[i]);
        if (threadsp[i] == NULL) {
          DBGPRINT("ERROR: Cannot allocate the \""); DBGPRINT(namep);
          DBGPRINT("\" stack (0x"); DBG(write_dword(DBGCH, infop->stacklen));
          DBGPRINT(" B)\r\n");
          return LDR_ERR_BADVAL;
        }
      } else {
        DBGPRINT("App \""); DBGPRINT(namep); DBGPRINT("\" already run\r\n");
      }
      return LDR_OK;
    }
  }
  DBGPRINT("App \""); DBGPRINT(namep); DBGPRINT("\" not found\r\n");
  return LDR_ERR_BADVAL;
}

ldr_err_t ldrGetInfo(AppLoader *ldrp, const char *namep, app_info_t *infop) {

  uint32_t i;

  chDbgCheck(ldrp != NULL, "ldrGetInfo");
  chDbgCheck(infop != NULL, "ldrGetInfo");

  for (i = 0; i < flash_apps.numapps; ++i) {
    const app_info_t *const p = (const app_info_t *)&flash_apps.infos[i];
    if (0 == strncmp(p->name, namep, LDR_MAX_NAME_LENGTH)) {
      *infop = *p;
      return LDR_OK;
    }
  }
  DBGPRINT("App \""); DBGPRINT(namep); DBGPRINT("\" not found\r\n");
  return LDR_ERR_BADVAL;
}

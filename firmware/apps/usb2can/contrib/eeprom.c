/*
    Abstraction layer for EEPROM ICs.

    Copyright (C) 2012 Uladzimir Pylinski aka barthess

    Licensed under the 3-Clause BSD license, (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://directory.fsf.org/wiki/License:BSD_3Clause
*/

#include "ch.h"
#include "hal.h"

#include "eeprom.h"

#include "24aa.c"

/*
 ******************************************************************************
 * GLOBAL VARIABLES
 ******************************************************************************
 */

/*
 ******************************************************************************
 * LOCAL FUNCTIONS
 ******************************************************************************
 */

static fileoffset_t getsize(void *ip){
  uint32_t h,l;
  chDbgCheck((ip != NULL) && (((EepromFileStream*)ip)->vmt != NULL) && (((EepromFileStream*)ip)->cfg != NULL), "");
  h = ((EepromFileStream*)ip)->cfg->barrier_hi;
  l = ((EepromFileStream*)ip)->cfg->barrier_low;
  return  h - l;
}

static fileoffset_t getposition(void *ip){
  chDbgCheck((ip != NULL) && (((EepromFileStream*)ip)->vmt != NULL), "");
  return ((EepromFileStream*)ip)->position;
}

static fileoffset_t lseek(void *ip, fileoffset_t offset){
  chDbgCheck((ip != NULL) && (((EepromFileStream*)ip)->vmt != NULL), "");
  uint32_t size = getsize(ip);
  if (offset > size)
    offset = size;
  ((EepromFileStream*)ip)->position = offset;
  return offset;
}

static uint32_t close(void *ip) {
  chDbgCheck((ip != NULL) && (((EepromFileStream*)ip)->vmt != NULL), "");
  ((EepromFileStream*)ip)->errors   = FILE_OK;
  ((EepromFileStream*)ip)->position = 0;
  ((EepromFileStream*)ip)->vmt      = NULL;
  ((EepromFileStream*)ip)->cfg      = NULL;
  return FILE_OK;
}

static int geterror(void *ip){
  chDbgCheck((ip != NULL) && (((EepromFileStream*)ip)->vmt != NULL), "");
  return ((EepromFileStream*)ip)->errors;
}

/**
 * Put stub.
 */
static msg_t put(void *ip, uint8_t b){
  (void)ip;
  (void)b;
  return 0;
}

/**
 * Get stub.
 */
static msg_t get(void *ip){
  (void)ip;
  return 0;
}

/**
 * @brief   Determines and returns size of data that can be processed
 */
static size_t __clamp_size(void *ip, size_t n){
  if ((getposition(ip) + n) > getsize(ip))
    return getsize(ip) - getposition(ip);
  else
    return n;
}

/**
 * @brief   Write data that can be fitted in one page boundary
 */
static void __fitted_write(void *ip, const uint8_t *data, size_t len, uint32_t *written){
  msg_t status = RDY_RESET;

  chDbgCheck(len != 0, "something broken in hi level part");

  status  = eeprom_write(((EepromFileStream*)ip)->cfg, getposition(ip), data, len);
  if (status == RDY_OK){
    *written += len;
    lseek(ip, getposition(ip) + len);
  }
}

/**
 * @brief     Write data to EEPROM.
 * @details   Only one EEPROM page can be written at once. So fucntion
 *            splits large data chunks in small EEPROM transactions if needed.
 * @note      To achieve the maximum effectivity use write operations
 *            aligned to EEPROM page boundaries.
 */
static size_t write(void *ip, const uint8_t *bp, size_t n){

  size_t   len = 0;     /* bytes to be written at one trasaction */
  uint32_t written; /* total bytes successfully written */
  uint16_t pagesize;
  uint32_t firstpage;
  uint32_t lastpage;

  chDbgCheck((ip != NULL) && (((EepromFileStream*)ip)->vmt != NULL), "");

  if (n == 0)
    return 0;

  n = __clamp_size(ip, n);
  if (n == 0)
    return 0;

  pagesize  =  ((EepromFileStream*)ip)->cfg->pagesize;
  firstpage = (((EepromFileStream*)ip)->cfg->barrier_low + getposition(ip)) / pagesize;
  lastpage  = (((EepromFileStream*)ip)->cfg->barrier_low + getposition(ip) + n - 1) / pagesize;

  written = 0;
  /* data fitted in single page */
  if (firstpage == lastpage){
    len = n;
    __fitted_write(ip, bp, len, &written);
    bp += len;
    return written;
  }

  else{
    /* write first piece of data to first page boundary */
    len =  ((firstpage + 1) * pagesize) - getposition(ip);
    len -= ((EepromFileStream*)ip)->cfg->barrier_low;
    __fitted_write(ip, bp, len, &written);
    bp += len;

    /* now writes blocks at a size of pages (may be no one) */
    while ((n - written) > pagesize){
      len = pagesize;
      __fitted_write(ip, bp, len, &written);
      bp += len;
    }

    /* wrtie tail */
    len = n - written;
    if (len == 0)
      return written;
    else{
      __fitted_write(ip, bp, len, &written);
    }
  }

  return written;
}

/**
 * Read some bytes from current position in file. After successful
 * read operation the position pointer will be increased by the number
 * of read bytes.
 */
static size_t read(void *ip, uint8_t *bp, size_t n){
  msg_t status = RDY_OK;

  chDbgCheck((ip != NULL) && (((EepromFileStream*)ip)->vmt != NULL), "");

  if (n == 0)
    return 0;

  n = __clamp_size(ip, n);
  if (n == 0)
    return 0;

  /* Stupid I2C cell in STM32F1x does not allow to read single byte.
     So we must read 2 bytes and return needed one. */
#if defined(STM32F1XX_I2C)
  if (n == 1){
    uint8_t __buf[2];
    /* if NOT last byte of file requested */
    if ((getposition(ip) + 1) < getsize(ip)){
      if (read(ip, __buf, 2) == 2){
        lseek(ip, (getposition(ip) + 1));
        bp[0] = __buf[0];
        return 1;
      }
      else
        return 0;
    }
    else{
      lseek(ip, (getposition(ip) - 1));
      if (read(ip, __buf, 2) == 2){
        lseek(ip, (getposition(ip) + 2));
        bp[0] = __buf[1];
        return 1;
      }
      else
        return 0;
    }
  }
#endif /* defined(STM32F1XX_I2C) */

  /* call low level function */
  status  = eeprom_read(((EepromFileStream*)ip)->cfg, getposition(ip), bp, n);
  if (status != RDY_OK)
    return 0;
  else{
    lseek(ip, (getposition(ip) + n));
    return n;
  }
}

static const struct EepromFilelStreamVMT vmt = {
    write,
    read,
    put,
    get,
    close,
    geterror,
    getsize,
    getposition,
    lseek,
};

/*
 ******************************************************************************
 * EXPORTED FUNCTIONS
 ******************************************************************************
 */

/**
 * Open EEPROM IC as file and return pointer to the file object
 * @note      Fucntion allways successfully open file. All checking makes
 *            in read/write functions.
 */
EepromFileStream* EepromFileOpen(EepromFileStream* efs, const I2CEepromFileConfig *eeprom_cfg){
  chDbgCheck((efs != NULL) && (eeprom_cfg != NULL), "");
  chDbgCheck(efs->vmt != &vmt, "file allready opened");
  chDbgCheck(eeprom_cfg->barrier_hi > eeprom_cfg->barrier_low, "wrong barriers")
  chDbgCheck(eeprom_cfg->pagesize < eeprom_cfg->size, "pagesize can not be lager than EEPROM size")
  chDbgCheck(eeprom_cfg->barrier_hi <= eeprom_cfg->size, "barrier out of bounds")

  efs->vmt = &vmt;
  efs->cfg = eeprom_cfg;
  efs->errors = FILE_OK;
  efs->position = 0;
  return efs;
}

uint8_t EepromReadByte(EepromFileStream *EepromFile_p){
  uint8_t buf[1];
  chFileStreamRead(EepromFile_p, buf, sizeof(buf));
  return buf[0];
}

uint16_t EepromReadHalfword(EepromFileStream *EepromFile_p){
  uint8_t buf[2];
  chFileStreamRead(EepromFile_p, buf, sizeof(buf));
  return (buf[0] << 8) | buf[1];
}

uint32_t EepromReadWord(EepromFileStream *EepromFile_p){
  uint8_t buf[4];
  chFileStreamRead(EepromFile_p, buf, sizeof(buf));
  return (buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3];
}

size_t EepromWriteByte(EepromFileStream *EepromFile_p, uint8_t data){
  uint8_t buf[1] = {data};
  return chFileStreamWrite(EepromFile_p, buf, sizeof(buf));
}

size_t EepromWriteHalfword(EepromFileStream *EepromFile_p, uint16_t data){
  uint8_t buf[2] = {
      (data >> 8) & 0xFF,
      data        & 0xFF
  };
  return chFileStreamWrite(EepromFile_p, buf, sizeof(buf));
}

size_t EepromWriteWord(EepromFileStream *EepromFile_p, uint32_t data){
  uint8_t buf[4] = {
      (data >> 24) & 0xFF,
      (data >> 16) & 0xFF,
      (data >> 8)  & 0xFF,
      data         & 0xFF
  };
  return chFileStreamWrite(EepromFile_p, buf, sizeof(buf));
}

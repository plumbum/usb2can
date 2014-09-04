/*
    Abstraction layer for EEPROM ICs.

    Copyright (C) 2012 Uladzimir Pylinski aka barthess

    Licensed under the 3-Clause BSD license, (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://directory.fsf.org/wiki/License:BSD_3Clause
*/

#ifndef EEPROM_FILE_H_
#define EEPROM_FILE_H_

#include "ch.h"
#include "hal.h"

typedef struct I2CEepromFileConfig {
  /**
   * Driver connecte to IC.
   */
  I2CDriver   *i2cp;
  /**
   * Lower barrier of file in EEPROM memory array.
   */
  uint32_t    barrier_low;
  /**
   * Higher barrier of file in EEPROM memory array.
   */
  uint32_t    barrier_hi;
  /**
   * Size of memory array in bytes.
   * Check datasheet!!!
   */
  uint32_t    size;
  /**
   * Size of single page in bytes.
   * Check datasheet!!!
   */
  uint16_t    pagesize;
  /**
   * Address of IC on I2C bus.
   */
  i2caddr_t   addr;
  /**
   * Time needed by IC for single page writing.
   * Check datasheet!!!
   */
  systime_t   write_time;
  /**
   * Pointer to write buffer. The safest size is (pagesize + 2)
   */
  uint8_t     *write_buf;
}I2CEepromFileConfig;

/**
 * @brief   @p EepromFileStream specific data.
 */
#define _eeprom_file_stream_data                                            \
  _base_file_stream_data                                                    \
  const I2CEepromFileConfig   *cfg;                                         \
  uint32_t                errors;                                           \
  uint32_t                position;                                         \

/**
 * @brief   @p EepromFileStream virtual methods table.
 */
struct EepromFilelStreamVMT {
  _base_file_stream_methods
};

/**
 * @brief   EEPROM file stream driver class.
 * @details This class extends @p BaseFileStream by adding some fields.
 */
typedef struct EepromFileStream EepromFileStream;
struct EepromFileStream {
  /** @brief Virtual Methods Table.*/
  const struct EepromFilelStreamVMT *vmt;
  _eeprom_file_stream_data
};

/**
 * @brief   File Stream read.
 * @details The function reads data from a file into a buffer.
 *
 * @param[in] ip        pointer to a @p BaseSequentialStream or derived class
 * @param[out] bp       pointer to the data buffer
 * @param[in] n         the maximum amount of data to be transferred
 * @return              The number of bytes transferred. The return value can
 *                      be less than the specified number of bytes if the
 *                      stream reaches the end of the available data.
 *
 * @api
 */
#define chFileStreamRead(ip, bp, n)  (chSequentialStreamRead(ip, bp, n))

/**
 * @brief   File Stream write.
 * @details The function writes data from a buffer to a file.
 *
 * @param[in] ip        pointer to a @p BaseSequentialStream or derived class
 * @param[in] bp        pointer to the data buffer
 * @param[in] n         the maximum amount of data to be transferred
 * @return              The number of bytes transferred. The return value can
 *                      be less than the specified number of bytes if the
 *                      stream reaches a physical end of file and cannot be
 *                      extended.
 *
 * @api
 */
#define chFileStreamWrite(ip, bp, n) (chSequentialStreamWrite(ip, bp, n))

#ifdef __cplusplus
extern "C" {
#endif
  EepromFileStream* EepromFileOpen(EepromFileStream* efs, const I2CEepromFileConfig *eeprom_cfg);

  uint8_t  EepromReadByte(EepromFileStream *EepromFile_p);
  uint16_t EepromReadHalfword(EepromFileStream *EepromFile_p);
  uint32_t EepromReadWord(EepromFileStream *EepromFile_p);
  size_t EepromWriteByte(EepromFileStream *EepromFile_p, uint8_t data);
  size_t EepromWriteHalfword(EepromFileStream *EepromFile_p, uint16_t data);
  size_t EepromWriteWord(EepromFileStream *EepromFile_p, uint32_t data);
#ifdef __cplusplus
}
#endif

#endif /* EEPROM_FILE_H_ */

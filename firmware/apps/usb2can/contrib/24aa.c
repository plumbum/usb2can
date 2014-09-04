/*
    Abstraction layer for EEPROM ICs.

    Copyright (C) 2012 Uladzimir Pylinski aka barthess

    Licensed under the 3-Clause BSD license, (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://directory.fsf.org/wiki/License:BSD_3Clause
*/

/*****************************************************************************
 * DATASHEET NOTES
 *****************************************************************************
Write cycle time (byte or page) — 5 ms

Note:
Page write operations are limited to writing
bytes within a single physical page,
regardless of the number of bytes
actually being written. Physical page
boundaries start at addresses that are
integer multiples of the page buffer size (or
‘page size’) and end at addresses that are
integer multiples of [page size – 1]. If a
Page Write command attempts to write
across a physical page boundary, the
result is that the data wraps around to the
beginning of the current page (overwriting
data previously stored there), instead of
being written to the next page as might be
expected.
*********************************************************************/

#include <string.h>

#include "ch.h"
#include "hal.h"

/*
 ******************************************************************************
 * DEFINES
 ******************************************************************************
 */
#if defined(SAM7_PLATFORM)
#define EEPROM_I2C_CLOCK (MCK / (((i2cp->config->cwgr & 0xFF) + ((i2cp->config->cwgr >> 8) & 0xFF)) * (1 << ((i2cp->config->cwgr >> 16) & 7)) + 6))
#else
#define EEPROM_I2C_CLOCK (i2cp->config->clock_speed)
#endif

/*
 ******************************************************************************
 * EXTERNS
 ******************************************************************************
 */

/*
 ******************************************************************************
 * GLOBAL VARIABLES
 ******************************************************************************
 */

/*
 *******************************************************************************
 * LOCAL FUNCTIONS
 *******************************************************************************
 */
/**
 * @brief   Split one uint16_t address to two uint8_t.
 *
 * @param[in] txbuf pointer to driver transmit buffer
 * @param[in] addr  uint16_t address
 */
#define eeprom_split_addr(txbuf, addr){                                       \
  (txbuf)[0] = ((uint8_t)((addr >> 8) & 0xFF));                               \
  (txbuf)[1] = ((uint8_t)(addr & 0xFF));                                      \
}

/*
 *******************************************************************************
 * EXPORTED FUNCTIONS
 *******************************************************************************
 */

/**
 * @brief     Calculates requred timeout.
 */
static systime_t calc_timeout(I2CDriver *i2cp, size_t txbytes, size_t rxbytes){
  const uint32_t bitsinbyte = 10;
  uint32_t tmo;
  tmo = ((txbytes + rxbytes + 1) * bitsinbyte * 1000);
  tmo /= EEPROM_I2C_CLOCK;
  tmo += 10; /* some additional milliseconds to be safer */
  return MS2ST(tmo);
}

/**
 * @brief   EEPROM read routine.
 *
 * @param[in] i2efcp    pointer to configuration structure of eeprom file
 * @param[in] offset    addres of 1-st byte to be read
 * @param[in] data      pointer to buffer with data to be written
 * @param[in] len       number of bytes to be red
 */
static msg_t eeprom_read(const I2CEepromFileConfig *i2efcp,
                         uint32_t offset, uint8_t *data, size_t len){

  msg_t status = RDY_RESET;
  systime_t tmo = calc_timeout(i2efcp->i2cp, 2, len);

  chDbgCheck(((len <= i2efcp->size) && ((offset+len) <= i2efcp->size)),
             "out of device bounds");

  eeprom_split_addr(i2efcp->write_buf, (offset + i2efcp->barrier_low));

  #if I2C_USE_MUTUAL_EXCLUSION
  i2cAcquireBus(i2efcp->i2cp);
  #endif

  status = i2cMasterTransmitTimeout(i2efcp->i2cp, i2efcp->addr,
                                    i2efcp->write_buf, 2, data, len, tmo);

  #if I2C_USE_MUTUAL_EXCLUSION
  i2cReleaseBus(i2efcp->i2cp);
  #endif

  return status;
}

/**
 * @brief   EEPROM write routine.
 * @details Function writes data to EEPROM.
 * @pre     Data must be fit to single EEPROM page.
 *
 * @param[in] i2efcp  pointer to configuration structure of eeprom file
 * @param[in] offset  addres of 1-st byte to be write
 * @param[in] data    pointer to buffer with data to be written
 * @param[in] len     number of bytes to be written
 */
static msg_t eeprom_write(const I2CEepromFileConfig *i2efcp, uint32_t offset,
                          const uint8_t *data, size_t len){
  msg_t status = RDY_RESET;
  systime_t tmo = calc_timeout(i2efcp->i2cp, (len + 2), 0);

  chDbgCheck(((len <= i2efcp->size) && ((offset+len) <= i2efcp->size)),
             "out of device bounds");
  chDbgCheck((((offset + i2efcp->barrier_low) / i2efcp->pagesize) ==
             (((offset + i2efcp->barrier_low) + len - 1) / i2efcp->pagesize)),
             "data can not be fitted in single page");

  /* write address bytes */
  eeprom_split_addr(i2efcp->write_buf, (offset + i2efcp->barrier_low));
  /* write data bytes */
  memcpy(&(i2efcp->write_buf[2]), data, len);

  #if I2C_USE_MUTUAL_EXCLUSION
  i2cAcquireBus(i2efcp->i2cp);
  #endif

  status = i2cMasterTransmitTimeout(i2efcp->i2cp, i2efcp->addr,
                                    i2efcp->write_buf, (len + 2), NULL, 0, tmo);

  #if I2C_USE_MUTUAL_EXCLUSION
  i2cReleaseBus(i2efcp->i2cp);
  #endif

  /* wait until EEPROM process data */
  chThdSleep(i2efcp->write_time);

  return status;
}


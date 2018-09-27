/**
*******************************************************************************
* @file    NMEA_utils.c
* @author  AST / Central Lab
* @version V1.0.0
* @date    18-May-2017
* @brief   NMEA utilities
*
*******************************************************************************
* @attention
*
* <h2><center>&copy; COPYRIGHT(c) 2017 STMicroelectronics</center></h2>
*
* Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
* You may not use this file except in compliance with the License.
* You may obtain a copy of the License at:
*
*        http://www.st.com/software_license_agreement_liberty_v2
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
*   1. Redistributions of source code must retain the above copyright notice,
*      this list of conditions and the following disclaimer.
*   2. Redistributions in binary form must reproduce the above copyright notice,
*      this list of conditions and the following disclaimer in the documentation
*      and/or other materials provided with the distribution.
*   3. Neither the name of STMicroelectronics nor the names of its contributors
*      may be used to endorse or promote products derived from this software
*      without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
********************************************************************************
*/

#include <stdlib.h>
#include "string.h"
#include "NMEA_utils.h"

/** @addtogroup MIDDLEWARES
 *  @{
 */

/** @addtogroup ST
 *  @{
 */
 
/** @addtogroup LIB_NMEA
 *  @{
 */

/** @addtogroup NMEA_UTILS 
 * @{
 */

/** @addtogroup EXPORTED_FUNCTIONS
 * @{
 */
/**
 * @brief  Scans a string with UTC Info_t and fills all fields of a
 *         UTC_Info_t struct
 * @param  utc_str       NMEA UTC string
 * @param  utc	         The UTC_Info_t struct to fill 
 * @retval None
 */
void scan_utc (uint8_t* utc_str, UTC_Info_t* utc)
{
  utc->utc = strtol((void *)utc_str,NULL,10);
  
  utc->hh = (utc->utc / 10000);
  utc->mm = (utc->utc - (utc->hh * 10000)) / 100;
  utc->ss = utc->utc - ((utc->hh * 10000) + (utc->mm * 100));
  
  return;
}

/**
 * @brief  Convert a character to unsigned integer
 * @param  c        The character to convert
 * @retval uint32_t The returned unsigned integer
 */
uint32_t char2int(uint8_t c)
{
  uint32_t ret = (unsigned char)0;

  if((c >= '0') && (c <= '9')) {
    ret = (unsigned char)(c - '0');
  }

  if((c >= 'A') && (c <= 'F')) {
    ret = (unsigned char)(c - 'A') + (unsigned)10;
  }

  return ret;
}

/**
 * @brief  Execute the 'OR' operation between first two elements of a buffer
 * @param  buf[]    The input buffer
 * @retval uint32_t The returned checksum
 */
uint32_t nmea_checksum(uint8_t buf[])
{
  return ((char2int(buf[1]) << 4) | (char2int(buf[2])));
}

uint32_t digit2int(uint8_t buf[], int32_t offset, Decimal d)
{
  uint32_t ret = (unsigned char)0;
  uint32_t hu, hd, hc, hm;

  switch (d) {
  case TENS:
    hd = char2int(buf[offset]);
    hu = char2int(buf[offset+1]);
        
    ret = hd*(unsigned)10+hu;
    break;
    
  case HUNDREDS:
    hc = char2int(buf[offset]);
    hd = char2int(buf[offset+1]);
    hu = char2int(buf[offset+2]);
        
    ret = hc*(unsigned)100+hd*(unsigned)10+hu;
    break;
    
  case THOUSANDS:
    hm = char2int(buf[offset]);
    hc = char2int(buf[offset+1]);
    hd = char2int(buf[offset+2]);
    hu = char2int(buf[offset+3]);
        
    ret = hm*(unsigned)1000+hc*(unsigned)100+hd*(unsigned)10+hu;
    break;
    
  default:
    break;
 }

 return ret;
}

void scan_timestamp_time(uint8_t buf[], Timestamp_Info_t *timestamp_Info_t)
{
  /* FORMAT: HHMMSS */
  timestamp_Info_t->hh = digit2int(buf, 0, TENS);
  timestamp_Info_t->mm = digit2int(buf, 2, TENS);
  timestamp_Info_t->ss = digit2int(buf, 4, TENS);
}

void scan_timestamp_date(uint8_t buf[], Timestamp_Info_t *timestamp_Info_t)
{
  /* FORMAT: YYYYMMDD */
  timestamp_Info_t->year = digit2int(buf, 0, THOUSANDS);
  timestamp_Info_t->month = digit2int(buf, 4, TENS);
  timestamp_Info_t->day = digit2int(buf, 6, TENS);
}

/**
* @}
*/
   
/**
* @}
*/

/**
* @}
*/

/**
 * @}
 */

/**
* @}
*/

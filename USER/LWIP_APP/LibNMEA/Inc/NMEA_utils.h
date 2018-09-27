/**
*******************************************************************************
* @file    NMEA_utils.h
* @author  AST / Central Lab
* @version V1.0.0
* @date    19-May-2017
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __NMEA_UTILS_H
#define __NMEA_UTILS_H

#ifdef __cplusplus
extern "C" {
#endif
  
#include <stdio.h>
#include <stdint.h>
  
#include "NMEA_parser.h"
  
/** @addtogroup MIDDLEWARES 
 *  @{
 */

/** @addtogroup ST
 *  @{
 */
 
/** @addtogroup LIB_NMEA
 *  @{
 */

/** @defgroup NMEA_UTILS NMEA_UTILS
 * @{
 */  

/** @defgroup EXPORTED_TYPES EXPORTED TYPES
 * @{
 */
/**
 * @brief Enumeration structure that containes the two success states of a parsing process
 */
typedef enum Decimal_t {
  TENS = 0,
  HUNDREDS,
  THOUSANDS
} Decimal;
/**
 * @}
 */
 
/** @defgroup EXPORTED_FUNCTIONS EXPORTED FUNCTIONS
 * @{
 */
void scan_utc (uint8_t* utc_str, UTC_Info_t* utc);
uint32_t char2int(uint8_t c);
uint32_t nmea_checksum(uint8_t buf[]);
uint32_t digit2int(uint8_t buf[], int32_t offset, Decimal d);
void scan_timestamp_time(uint8_t buf[], Timestamp_Info_t *timestamp_Info_t);
void scan_timestamp_date(uint8_t buf[], Timestamp_Info_t *timestamp_Info_t);
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
 
#ifdef __cplusplus
}
#endif

#endif

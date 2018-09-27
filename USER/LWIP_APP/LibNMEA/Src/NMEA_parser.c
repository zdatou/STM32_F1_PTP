/**
*******************************************************************************
* @file    NMEA_parser.c
* @author  AST / Central Lab
* @version V1.0.0
* @date    18-May-2017
* @brief   NMEA sentence parser
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
#include "NMEA_parser.h"
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

/** @addtogroup NMEA_PARSER 
 * @{
 */

/** @defgroup PRIVATE_FUNCTIONS PRIVATE FUNCTIONS
 * @{
 */
int32_t check_gnsmsg(char header[]);
int32_t check_gsamsg(char header[]);
int32_t check_gsvmsg(char header[]);
int32_t check_geofencemsg(char header[]);
int32_t check_odomsg(char header[]);
int32_t check_datalogmsg(char header[]);
int32_t check_savevarmsg(char header[]);
void reset_gsvmsg(GSV_Info_t *gsv_data);
/**
* @}
*/

/** @addtogroup EXPORTED_FUNCTIONS
 * @{
 */
/**
 * @brief  Function that makes the parsing of the $GPGGA NMEA string with all Global Positioning System Fixed data.
 * @param  gpgga_data     Pointer to GPGGA_Info_t struct
 * @param  NMEA	          NMEA string read by the Gps expansion
 * @retval ParseStatus_t PARSE_SUCC if the parsing process goes ok, PARSE_FAIL if it doesn't
 */
ParseStatus_t parse_gpgga(GPGGA_Info_t *gpgga_data, uint8_t NMEA[])
{
  uint8_t app[MAX_MSG_LEN][MAX_MSG_LEN];
  int32_t new_field;
  BOOL valid_msg = FALSE;

  ParseStatus_t status = PARSE_FAIL;
  
  if(NMEA != NULL) {

    /* clear the app[][] buffer */ 
    for (int32_t i=0; i<MAX_MSG_LEN; i++) {
      memset(app[i], 0, (size_t)MAX_MSG_LEN);
    }

    for(int32_t i = 0, j = 0, k = 0; (NMEA[i] != '\n'); i++)
    {
      new_field = 0;

      if ((NMEA[i] == ',') || (NMEA[i] == '*')) {
        app[j][k] = '\0';
        new_field = 1;
        
        if (strcmp((void *)app[0], "$GPGGA") == 0) {
          j++;
          k = 0;
          valid_msg = TRUE;
        }
        else {
          break;
        }
      }
      if(new_field == 0) {
        app[j][k++] = NMEA[i];
      }
    }

    if (valid_msg == TRUE) {
      int32_t valid = strtol((void *)app[6],NULL,10);
      gpgga_data->valid = (GPS_Valid_t)valid;

      if (gpgga_data->valid == VALID) {
        scan_utc ((void *)app[1], &gpgga_data->utc);
        gpgga_data->xyz.lat = strtod((void *)app[2],NULL);
        gpgga_data->xyz.ns = *((uint8_t*)app[3]);
        gpgga_data->xyz.lon = strtod((void *)app[4],NULL);
        gpgga_data->xyz.ew = *((uint8_t*)app[5]);
        gpgga_data->sats = strtol((void *)app[7],NULL,10);
        gpgga_data->acc = strtof((void *)app[8],NULL);
        gpgga_data->xyz.alt = strtof((void *)app[9],NULL);
        gpgga_data->xyz.mis = *((uint8_t*)app[10]);
        gpgga_data->geoid.height = strtol((void *)app[11],NULL,10);
        gpgga_data->geoid.mis = *((uint8_t*)app[12]);
        gpgga_data->update = strtol((void *)app[13],NULL,10);
        gpgga_data->checksum = nmea_checksum(app[14]);
        
        valid_msg = FALSE;
        status = PARSE_SUCC;  
      }    
    }
  }

  return status;
} 
/**
* @}
*/

/** @addtogroup PRIVATE_FUNCTIONS
 * @{
 */   
int32_t check_gnsmsg(char header[])
{
  int32_t is_gnsmsg = 1;
  
  if (strcmp(header, "$GPGNS") == 0) {
    is_gnsmsg = 0;
  }
  if (strcmp(header, "$GAGNS") == 0) {
    is_gnsmsg = 0;
  }
  if (strcmp(header, "$BDGNS") == 0) {
    is_gnsmsg = 0;
  }
  if (strcmp(header, "$QZGNS") == 0) {
    is_gnsmsg = 0;
  }
  if (strcmp(header, "$GNGNS") == 0) {
    is_gnsmsg = 0;
  }

  return is_gnsmsg;
}
/**
* @}
*/

/** @addtogroup EXPORTED_FUNCTIONS
 * @{
 */
/**
 * @brief  Function that makes the parsing of the string read by the Gps expansion, capturing the right parameters from it.
 * @param  gns_data      Pointer to GNS_Info_t struct
 * @param  NMEA[]        NMEA string read by the Gps expansion
 * @retval ParseStatus_t PARSE_SUCC if the parsing process goes ok, PARSE_FAIL if it doesn't
 */
ParseStatus_t parse_gnsmsg (GNS_Info_t *gns_data, uint8_t NMEA[])
{
  uint8_t app[MAX_MSG_LEN][MAX_MSG_LEN];
  int32_t new_field;
  BOOL valid_msg = FALSE;
  
  ParseStatus_t status = PARSE_FAIL;
  
  if(NMEA != NULL) {

    /* clear the app[][] buffer */ 
    for (int32_t i=0; i<MAX_MSG_LEN; i++) {
      memset(app[i], 0, (size_t)MAX_MSG_LEN);
    }

    for (int32_t i = 0, j = 0, k = 0; (NMEA[i] != '\n'); i++)
    {
      new_field = 0;

      if ((NMEA[i] == ',') || (NMEA[i] == '*')) {
        app[j][k] = '\0';
        new_field = 1;
        
        if (check_gnsmsg((void *)app[0]) == 0) {
          j++;
          k = 0;
          valid_msg = TRUE;
        }
        else {
          break;
        }
      }
      if(new_field == 0) {
        app[j][k++] = NMEA[i];
      }
    }
    
    if (valid_msg == TRUE) {
      strcpy((void *)gns_data->constellation, (void *)app[0]);
      scan_utc ((void *)app[1], &gns_data->utc);
      gns_data->xyz.lat = strtod((void *)app[2],NULL);
      gns_data->xyz.ns = *((uint8_t*)app[3]);
      gns_data->xyz.lon = strtod((void *)app[4],NULL);
      gns_data->xyz.ew = *((uint8_t*)app[5]);
      gns_data->gps_mode = *((uint8_t*)app[6]);
      gns_data->glonass_mode = *((uint8_t*)app[7]);
      gns_data->sats = strtol((void *)app[8],NULL,10);
      gns_data->hdop = strtof((void *)app[9],NULL);
      gns_data->xyz.alt = strtof((void *)app[10],NULL);
      gns_data->geo_sep = strtof((void *)app[11],NULL);
      gns_data->dgnss_age = *((uint8_t*)app[12]);
      gns_data->dgnss_ref = *((uint8_t*)app[13]);
      gns_data->checksum = nmea_checksum(app[14]);
      
      valid_msg = FALSE;
      status = PARSE_SUCC;
    }
  }

  return status;
}
  
/**
 * @brief  Function that makes the parsing of the $GPGST NMEA string with GPS Pseudorange Noise Statistics.
 * @param  GPGST_Info_t    Pointer to a GPGST_Info_t struct
 * @param  NMEA	          NMEA string read by the Gps expansion.
 * @retval ParseStatus_t PARSE_SUCC if the parsing process goes ok, PARSE_FAIL if it doesn't
 */
ParseStatus_t parse_gpgst (GPGST_Info_t *gpgst_data, uint8_t NMEA[])
{
  uint8_t app[MAX_MSG_LEN][MAX_MSG_LEN];
  int32_t new_field;
  BOOL valid_msg = FALSE;
  
  ParseStatus_t status = PARSE_FAIL;
  
  if(NMEA != NULL) {
  
    /* clear the app[][] buffer */ 
    for (int32_t i=0; i<MAX_MSG_LEN; i++) {
      memset(app[i], 0, (size_t)MAX_MSG_LEN);
    }
    
    for (int32_t i = 0, j = 0, k = 0; (NMEA[i] != '\n'); i++)
    {
      new_field = 0;

      if ((NMEA[i] == ',') || (NMEA[i] == '*')) {
        app[j][k] = '\0';
        new_field = 1;
        
        if (strcmp((void *)app[0], "$GPGST") == 0) {
          j++;
          k = 0;
          valid_msg = TRUE;
        }
        else {
          break;
        }
      }
      if(new_field == 0) {
        app[j][k++] = NMEA[i];
      }
    }  
    
    if (valid_msg == TRUE) {         
      scan_utc ((void *)app[1], &gpgst_data->utc);
      gpgst_data->EHPE = strtof((void *)app[2],NULL);
      gpgst_data->semi_major_dev = strtof((void *)app[3],NULL);
      gpgst_data->semi_minor_dev = strtof((void *)app[4],NULL);
      gpgst_data->semi_major_angle = strtof((void *)app[5],NULL);
      gpgst_data->lat_err_dev = strtof((void *)app[6],NULL);
      gpgst_data->lon_err_dev = strtof((void *)app[7],NULL);
      gpgst_data->alt_err_dev = strtof((void *)app[8],NULL);
      gpgst_data->checksum = nmea_checksum(app[9]);
      
      valid_msg = FALSE;
      status = PARSE_SUCC;
    }
  }

  return status;
}

/**
 * @brief  Function that makes the parsing of the $GPRMC NMEA string with Recommended Minimum Specific GPS/Transit data.
 * @param  GPRMC_Info_t    Pointer to a GPRMC_Info_t struct
 * @param  NMEA	          NMEA string read by the Gps expansion.
 * @retval ParseStatus_t PARSE_SUCC if the parsing process goes ok, PARSE_FAIL if it doesn't
 */
ParseStatus_t parse_gprmc (GPRMC_Info_t *gprmc_data, uint8_t NMEA[])
{
  uint8_t app[MAX_MSG_LEN][MAX_MSG_LEN];
  int32_t new_field;
  BOOL valid_msg = FALSE;
  
  ParseStatus_t status = PARSE_FAIL;
  
  if(NMEA != NULL) {
  
    /* clear the app[][] buffer */ 
    for (int32_t i=0; i<MAX_MSG_LEN; i++) {
      memset(app[i], 0, (size_t)MAX_MSG_LEN);
    }
    
    for (int32_t i = 0, j = 0, k = 0; (NMEA[i] != '\n'); i++)
    {
      new_field = 0;

      if ((NMEA[i] == ',') || (NMEA[i] == '*')) {
        app[j][k] = '\0';
        new_field = 1;
        
        if (strcmp((void *)app[0], "$GPRMC") == 0) {
          j++;
          k = 0;
          valid_msg = TRUE;
        }
        else {
          break;
        }
      }
      if(new_field == 0) {
        app[j][k++] = NMEA[i];
      }
    }  
    
    if (valid_msg == TRUE) {      
      scan_utc ((void *)app[1],  &gprmc_data->utc);
      gprmc_data->status = *((uint8_t*)app[2]);
      gprmc_data->xyz.lat = strtod((void *)app[3],NULL);
      gprmc_data->xyz.ns = *((uint8_t*)app[4]);
      gprmc_data->xyz.lon = strtod((void *)app[5],NULL);
      gprmc_data->xyz.ew = *((uint8_t*)app[6]);
      gprmc_data->speed = strtof((void *)app[7],NULL);
      gprmc_data->trackgood = strtof((void *)app[8],NULL);
      gprmc_data->date = strtol((void *)app[9],NULL,10);
      gprmc_data->mag_var = strtof((void *)app[10],NULL);
      gprmc_data->mag_var_dir = *((uint8_t*)app[11]);
      /* WARNING: from received msg, it seems there is another data (app[12]) before the checksum */
      gprmc_data->checksum = nmea_checksum(app[13]);
      
      valid_msg = FALSE;
      status = PARSE_SUCC;
    }
  }

  return status;
}
/**
* @}
*/

/** @addtogroup PRIVATE_FUNCTIONS
 * @{
 */  
int32_t check_gsamsg(char header[])
{
  int32_t is_gsamsg = 1;
  
  if (strcmp(header, "$GPGSA") == 0) {
    is_gsamsg = 0;
  }
  if (strcmp(header, "$GLGSA") == 0) {
    is_gsamsg = 0;
  }
  if (strcmp(header, "$GAGSA") == 0) {
    is_gsamsg = 0;
  }
  if (strcmp(header, "$BDGSA") == 0) {
    is_gsamsg = 0;
  }
  if (strcmp(header, "$GNGSA") == 0) {
    is_gsamsg = 0;
  }

  return is_gsamsg;
}
/**
* @}
*/

/** @addtogroup EXPORTED_FUNCTIONS
 * @{
 */           
/**
 * @brief  Function that makes the parsing of the string read by the Gps expansion, capturing the right parameters from it.
 * @param  GSA_Info_t      Pointer to a GSA_Info_t struct
 * @param  NMEA	          NMEA string read by the Gps expansion.
 * @retval ParseStatus_t PARSE_SUCC if the parsing process goes ok, PARSE_FAIL if it doesn't
 */
ParseStatus_t parse_gsamsg (GSA_Info_t *gsa_data, uint8_t NMEA[])
{
  uint8_t app[MAX_MSG_LEN][MAX_MSG_LEN];
  int32_t new_field;
  BOOL valid_msg = FALSE;
  
  ParseStatus_t status = PARSE_FAIL;
  
  if(NMEA != NULL) {
  
    /* clear the app[][] buffer */ 
    for (int32_t i=0; i<MAX_MSG_LEN; i++) {
      memset(app[i], 0, (size_t)MAX_MSG_LEN);
    }
    
    for (int32_t i = 0, j = 0, k = 0; (NMEA[i] != '\n'); i++)
    {
      new_field = 0;
      
      if ((NMEA[i] == ',') || (NMEA[i] == '*')) {
        app[j][k] = '\0';
        new_field = 1;
        
        if (check_gsamsg((void *)app[0]) == 0) {
          j++;
          k = 0;
          valid_msg = TRUE;
        }
        else {
          break;
        }
      }
      if(new_field == 0) {
        app[j][k++] = NMEA[i];
      }
    }  
    
    if (valid_msg == TRUE) {
      strcpy((void *)gsa_data->constellation, (void *)app[0]);
      gsa_data->operating_mode = *((uint8_t*)app[1]);
      gsa_data->current_mode = strtol((void *)app[2],NULL,10);
      for (int32_t i=0; i<MAX_SAT_NUM; i++) {
        gsa_data->sat_prn[i] = strtol((void *)app[3+i],NULL,10);
      }
      gsa_data->pdop = strtof((void *)app[15],NULL);
      gsa_data->hdop = strtof((void *)app[16],NULL);
      gsa_data->vdop = strtof((void *)app[17],NULL);
      gsa_data->checksum = nmea_checksum(app[18]);
      
      valid_msg = FALSE;
      status = PARSE_SUCC;
    }
  }

  return status;
}
/**
* @}
*/

/** @addtogroup PRIVATE_FUNCTIONS
 * @{
 */ 
int32_t check_gsvmsg(char header[])
{
  int32_t is_gsvmsg = 1;
  
  if (strcmp(header, "$GPGSV") == 0) {
    is_gsvmsg = 0;
  }
  if (strcmp(header, "$GLGSV") == 0) {
    is_gsvmsg = 0;
  }
  if (strcmp(header, "$GAGSV") == 0) {
    is_gsvmsg = 0;
  }
  if (strcmp(header, "$BDGSV") == 0) {
    is_gsvmsg = 0;
  }
  if (strcmp(header, "$QZGSV") == 0) {
    is_gsvmsg = 0;
  }
  if (strcmp(header, "$GNGSV") == 0) {
    is_gsvmsg = 0;
  }

  return is_gsvmsg;
}

void reset_gsvmsg(GSV_Info_t *gsv_data)
{
  /* FIXME: replace w/ function */
  memset(gsv_data->constellation, 0, (size_t)MAX_STR_LEN);
  gsv_data->amount = 0;
  gsv_data->number = 0;
  gsv_data->current_sats = 0;
  gsv_data->tot_sats = 0;
  for (int32_t i=0; i<MAX_SAT_NUM; i++) {
    memset(&gsv_data->gsv_sat_i[i], 0, sizeof(GSV_SAT_Info_t));
  }
}
/**
* @}
*/

/** @addtogroup EXPORTED_FUNCTIONS
 * @{
 */ 
/**
 * @brief  Function that makes the parsing of the string read by the Gps expansion, capturing the right parameters from it.
 * @param  GSV_Info_t      Pointer to a GSV_Info_t struct
 * @param  NMEA	          NMEA string read by the Gps expansion.
 * @retval ParseStatus_t PARSE_SUCC if the parsing process goes ok, PARSE_FAIL if it doesn't
 */
ParseStatus_t parse_gsvmsg(GSV_Info_t *gsv_data, uint8_t NMEA[])
{
  uint8_t app[MAX_MSG_LEN][MAX_MSG_LEN];
  int32_t app_idx;
  int32_t gsv_idx = 0;
  int32_t new_field;
  BOOL valid_gsv_msg = FALSE;
  
  ParseStatus_t status = PARSE_FAIL;

  if(NMEA != NULL) {

    /* clear the app[][] buffer */ 
    for (int32_t i=0; i<MAX_MSG_LEN; i++) {
      memset(app[i], 0, (size_t)MAX_MSG_LEN);
    }
    
    for (int32_t i = 0, j = 0, k = 0; (NMEA[i] != '\n'); i++)
    {
      new_field = 0;

      if ((NMEA[i] == ',') || (NMEA[i] == '*')) {
        app[j][k] = '\0';
        new_field = 1;
        
        if (check_gsvmsg((void *)app[0]) == 0) {
          j++;
          k = 0;
          valid_gsv_msg = TRUE;
        }
        else {
          break;
        }
      }
      if(new_field == 0) {
        app[j][k++] = NMEA[i];
      }
    }

    if (valid_gsv_msg == TRUE) {
      reset_gsvmsg(gsv_data);

      strcpy((void *)gsv_data->constellation, (void *)app[0]);
      gsv_data->amount = strtol((void *)app[1],NULL,10);
      gsv_data->number = strtol((void *)app[2],NULL,10);
      gsv_data->tot_sats = strtol((void *)app[3],NULL,10);
      app_idx = 4;
      for (int32_t i = 1; i <= GSV_MSG_SATS; i++) {
        gsv_data->gsv_sat_i[gsv_idx].prn = strtol((void *)app[app_idx*i],NULL,10);
        gsv_data->gsv_sat_i[gsv_idx].elev = strtol((void *)app[app_idx*i+1],NULL,10);
        gsv_data->gsv_sat_i[gsv_idx].azim = strtol((void *)app[app_idx*i+2],NULL,10);
        gsv_data->gsv_sat_i[gsv_idx].cn0 = strtol((void *)app[app_idx*i+3],NULL,10);

        if(gsv_data->gsv_sat_i[gsv_idx].prn != 0){
          gsv_data->current_sats++;
        }
        gsv_idx++;
      }
      
      valid_gsv_msg = FALSE;
      status = PARSE_SUCC;
    }

  }

  return status;
}

/**
 * @brief  Function that parses of the $PSTMVER NMEA string with version data.
 * @param  pstmver_data   Pointer to PSTMVER_Info_t struct
 * @param  NMEA	          NMEA string read by the Gps expansion
 * @retval ParseStatus_t PARSE_SUCC if the parsing process goes ok, PARSE_FAIL if it doesn't
 */
ParseStatus_t parse_pstmver(PSTMVER_Info_t *pstmver_data, uint8_t NMEA[])
{
  uint8_t app[MAX_MSG_LEN][MAX_MSG_LEN];
  int32_t new_field;
  BOOL valid_msg = FALSE;
  
  ParseStatus_t status = PARSE_FAIL;
  
  if(NMEA != NULL) {
  
    /* clear the app[][] buffer */ 
    for (int32_t i=0; i<MAX_MSG_LEN; i++) {
      memset(app[i], 0, (size_t)MAX_MSG_LEN);
    }
    
    for(int32_t i = 0, j = 0, k = 0; (NMEA[i] != '\n'); i++)
    {
      new_field = 0;

      if ((NMEA[i] == ',') || (NMEA[i] == '*')) {
        app[j][k] = '\0';
        new_field = 1;
        
        if (strcmp((void *)app[0], "$PSTMVER") == 0) {
          j++;
          k = 0;
          valid_msg = TRUE;
        }
        else {
          break;
        }
      }
      if(new_field == 0) {
        app[j][k++] = NMEA[i];
      }
    }
    
    if (valid_msg == TRUE) {
      strcpy((void *)pstmver_data->pstmver_string, (void *)app[1]);
      
      valid_msg = FALSE;
      status = PARSE_SUCC;
    }
  }
  return status;
}
/**
* @}
*/

/** @addtogroup PRIVATE_FUNCTIONS
 * @{
 */ 
int32_t check_geofencemsg(char header[])
{
  int32_t is_geofencemsg = 1;
  
  if (strcmp(header, "$PSTMCFGGEOFENCEOK") == 0) {
    is_geofencemsg = 0;
  }
  if (strcmp(header, "$PSTMCFGGEOFENCEERROR") == 0) {
    is_geofencemsg = 0;
  }
  if (strcmp(header, "$PSTMGEOFENCECFGOK") == 0) {
    is_geofencemsg = 0;
  }
  if (strcmp(header, "$PSTMGEOFENCECFGERROR") == 0) {
    is_geofencemsg = 0;
  }
  if (strcmp(header, "$PSTMGEOFENCESTATUS") == 0) {
    is_geofencemsg = 0;
  }
  if (strcmp(header, "$PSTMGEOFENCE") == 0) {
    is_geofencemsg = 0;
  }
  if (strcmp(header, "$PSTMGEOFENCEREQERROR") == 0) {
    is_geofencemsg = 0;
  }

  return is_geofencemsg;
}
/**
* @}
*/

/** @addtogroup EXPORTED_FUNCTIONS
 * @{
 */ 
/**
 * @brief  This function parses the geofence related messages
 * @param  geofence_data Pointer to Geofence_Info_t
 * @param  NMEA	         NMEA string read by the Gps expansion.
 * @retval ParseStatus_t PARSE_SUCC if the parsing process goes ok, PARSE_FAIL if it doesn't
 */
ParseStatus_t parse_pstmgeofence(Geofence_Info_t *geofence_data, uint8_t NMEA[])
{
  uint8_t app[MAX_MSG_LEN][MAX_MSG_LEN];
  int32_t new_field;
  BOOL valid_msg = FALSE;
  
  ParseStatus_t status = PARSE_FAIL;

  if(NMEA != NULL) {

    /* clear the app[][] buffer */ 
    for (int32_t i=0; i<MAX_MSG_LEN; i++) {
      memset(app[i], 0, (size_t)MAX_MSG_LEN);
    }
    
    for(int32_t i = 0, j = 0, k = 0; (NMEA[i] != '\n'); i++)
    {
      new_field = 0;

      if ((NMEA[i] == ',') || (NMEA[i] == '*')) {
        app[j][k] = '\0';
        new_field = 1;
        
        if (check_geofencemsg((void *)app[0]) == 0) {
          j++;
          k = 0;
          valid_msg = TRUE;
        }
        else {
          break;
        }
      }
      if(new_field == 0) {
        app[j][k++] = NMEA[i];
      }
    }
    
    if (valid_msg == TRUE) {
      /* Enabling */
      if (strcmp((void *)app[0], "$PSTMCFGGEOFENCEOK") == 0) {
        geofence_data->op = GNSS_FEATURE_EN_MSG;
        geofence_data->result = GNSS_OP_OK;
      }
      if (strcmp((void *)app[0], "$PSTMCFGGEOFENCEERROR") == 0) {
        geofence_data->op = GNSS_FEATURE_EN_MSG;
        geofence_data->result = GNSS_OP_ERROR;
      }
      /* Configuring */
      if (strcmp((void *)app[0], "$PSTMGEOFENCECFGOK") == 0) {
        geofence_data->op = GNSS_GEOFENCE_CFG_MSG;
        geofence_data->result = GNSS_OP_OK;
      }
      if (strcmp((void *)app[0], "$PSTMGEOFENCECFGERROR") == 0) {
        geofence_data->op = GNSS_GEOFENCE_STATUS_MSG;
        geofence_data->result = GNSS_OP_ERROR;
      }
      /* Querying Status */
      if (strcmp((void *)app[0], "$PSTMGEOFENCESTATUS") == 0) {
        geofence_data->op = GNSS_GEOFENCE_STATUS_MSG;
        scan_timestamp_time(app[1], &geofence_data->timestamp);
        scan_timestamp_date(app[2], &geofence_data->timestamp);
        for(int32_t i = 0; i<MAX_GEOFENCES_NUM; i++) {
          geofence_data->status[i] = strtol((void *)app[3+i],NULL,10);
        }
      }
      /* Alarm Msg */
      if (strcmp((void *)app[0], "$PSTMGEOFENCE") == 0) {      
        geofence_data->op = GNSS_GEOFENCE_ALARM_MSG;
        scan_timestamp_time(app[1], &geofence_data->timestamp);
        scan_timestamp_date(app[2], &geofence_data->timestamp);
        geofence_data->idAlarm = strtol((void *)app[3],NULL,10);
        geofence_data->coords.lat = strtod((void *)app[4],NULL);
        geofence_data->coords.lon = strtod((void *)app[5],NULL);
        geofence_data->coords.radius = strtod((void *)app[6],NULL);
        geofence_data->coords.distance = strtod((void *)app[7],NULL);
        geofence_data->coords.tolerance = strtod((void *)app[8],NULL);
        geofence_data->status[geofence_data->idAlarm] = strtol((void *)app[9],NULL,10);
      }
      
      valid_msg = FALSE;
      status = PARSE_SUCC;
    }
  }

  return status;
}
/**
* @}
*/

/** @addtogroup PRIVATE_FUNCTIONS
 * @{
 */ 
int32_t check_odomsg(char header[])
{
  int32_t is_odomsg = 1;
  
  if (strcmp(header, "$PSTMCFGODOOK") == 0) {
    is_odomsg = 0;
  }
  if (strcmp(header, "$PSTMCFGODOERROR") == 0) {
    is_odomsg = 0;
  }
  if (strcmp(header, "$PSTMODOSTARTOK") == 0) {
    is_odomsg = 0;
  }
  if (strcmp(header, "$PSTMODOSTARTERROR") == 0) {
    is_odomsg = 0;
  }
  if (strcmp(header, "$PSTMODOSTOPOK") == 0) {
    is_odomsg = 0;
  }
  if (strcmp(header, "$PSTMODOSTOPERROR") == 0) {
    is_odomsg = 0;
  }

  return is_odomsg;
}
/**
* @}
*/

/** @addtogroup EXPORTED_FUNCTIONS
 * @{
 */ 
/**
 * @brief  This function parses the odometer related messages
 * @param  Odometer_Info_t Pointer to a Odometer_Info_t struct
 * @param  NMEA           NMEA string read by the Gps expansion.
 * @retval ParseStatus_t PARSE_SUCC if the parsing process goes ok, PARSE_FAIL if it doesn't
 */
ParseStatus_t parse_pstmodo(Odometer_Info_t *odo_data, uint8_t NMEA[])
{
  uint8_t app[MAX_MSG_LEN][MAX_MSG_LEN];
  int32_t new_field;
  BOOL valid_msg = FALSE;
  
  ParseStatus_t status = PARSE_FAIL;

  if(NMEA != NULL) {

    /* clear the app[][] buffer */ 
    for (int32_t i=0; i<MAX_MSG_LEN; i++) {
      memset(app[i], 0, (size_t)MAX_MSG_LEN);
    }
    
    for(int32_t i = 0, j = 0, k = 0; (NMEA[i] != '\n'); i++)
    {
      new_field = 0;

      if ((NMEA[i] == ',') || (NMEA[i] == '*')) {
        app[j][k] = '\0';
        new_field = 1;
        
        if (check_odomsg((void *)app[0]) == 0) {
          j++;
          k = 0;
          valid_msg = TRUE;
        }
        else {
          break;
        }
      }
      if(new_field == 0) {
        app[j][k++] = NMEA[i];
      }
    }
    
    if (valid_msg == TRUE) {
      /* Enabling */
      if (strcmp((void *)app[0], "$PSTMCFGODOOK") == 0) {
        odo_data->op = GNSS_FEATURE_EN_MSG;
        odo_data->result = GNSS_OP_OK;
      }
      if (strcmp((void *)app[0], "$PSTMCFGODOERROR") == 0) {
        odo_data->op = GNSS_FEATURE_EN_MSG;
        odo_data->result = GNSS_OP_ERROR;
      }
      
      /* Start */
      if (strcmp((void *)app[0], "$PSTMODOSTARTOK") == 0) {
        odo_data->op = GNSS_ODO_START_MSG;
        odo_data->result = GNSS_OP_OK;
      }
      if (strcmp((void *)app[0], "$PSTMODOSTARTERROR") == 0) {
        odo_data->op = GNSS_ODO_START_MSG;
        odo_data->result = GNSS_OP_ERROR;
      }
      
      /* Stop */
      if (strcmp((void *)app[0], "$PSTMODOSTOPOK") == 0) {
        odo_data->op = GNSS_ODO_STOP_MSG;
        odo_data->result = GNSS_OP_OK;
      }
      if (strcmp((void *)app[0], "$PSTMODOSTOPERROR") == 0) {
        odo_data->op = GNSS_ODO_STOP_MSG;
        odo_data->result = GNSS_OP_ERROR;
      }
      
      valid_msg = FALSE;
      status = PARSE_SUCC;
    }
  }
  return status;
}
/**
* @}
*/

/** @addtogroup PRIVATE_FUNCTIONS
 * @{
 */ 
int32_t check_datalogmsg(char header[])
{
  int32_t is_datalogmsg = 1;
  
  if (strcmp(header, "$PSTMCFGLOGOK") == 0) {
    is_datalogmsg = 0;
  }
  if (strcmp(header, "$PSTMCFGLOGERROR") == 0) {
    is_datalogmsg = 0;
  }
  if (strcmp(header, "$PSTMLOGCREATEOK") == 0) {
    is_datalogmsg = 0;
  }
  if (strcmp(header, "$PSTMLOGCREATEERROR") == 0) {
    is_datalogmsg = 0;
  }
  if (strcmp(header, "$PSTMLOGSTARTOK") == 0) {
    is_datalogmsg = 0;
  }
  if (strcmp(header, "$PSTMLOGSTARTERROR") == 0) {
    is_datalogmsg = 0;
  }
  if (strcmp(header, "$PSTMLOGSTOPOK") == 0) {
    is_datalogmsg = 0;
  }
  if (strcmp(header, "$PSTMLOGSTOPERROR") == 0) {
    is_datalogmsg = 0;
  }
  if (strcmp(header, "$PSTMLOGERASEOK") == 0) {
    is_datalogmsg = 0;
  }
  if (strcmp(header, "$PSTMLOGERASEERROR") == 0) {
    is_datalogmsg = 0;
  }

  return is_datalogmsg;
}
/**
* @}
*/

/** @addtogroup EXPORTED_FUNCTIONS
 * @{
 */ 
/**
 * @brief  This function parses the datalog related messages  
 * @param  Datalog_Info_t Pointer to a Datalog_Info_t struct
 * @param  NMEA           NMEA string read by the Gps expansion.
 * @retval ParseStatus_t PARSE_SUCC if the parsing process goes ok, PARSE_FAIL if it doesn't
 */
ParseStatus_t parse_pstmdatalog(Datalog_Info_t *datalog_data, uint8_t NMEA[])
{
  uint8_t app[MAX_MSG_LEN][MAX_MSG_LEN];
  int32_t new_field;
  BOOL valid_msg = FALSE;
  
  ParseStatus_t status = PARSE_FAIL;

  if(NMEA != NULL) {

    /* clear the app[][] buffer */ 
    for (int32_t i=0; i<MAX_MSG_LEN; i++) {
      memset(app[i], 0, (size_t)MAX_MSG_LEN);
    }
    
    for(int32_t i = 0, j = 0, k = 0; (NMEA[i] != '\n'); i++)
    {
      new_field = 0;

      if ((NMEA[i] == ',') || (NMEA[i] == '*')) {
        app[j][k] = '\0';
        new_field = 1;
        
        if (check_datalogmsg((void *)app[0]) == 0) {
          j++;
          k = 0;
          valid_msg = TRUE;
        }
        else {
          break;
        }
      }
      if(new_field == 0) {
        app[j][k++] = NMEA[i];
      }
    }
    
    if (valid_msg == TRUE) {
      /* Enabling */
      if (strcmp((void *)app[0], "$PSTMCFGLOGOK") == 0) {
        datalog_data->op = GNSS_FEATURE_EN_MSG;
        datalog_data->result = GNSS_OP_OK;
      }
      if (strcmp((void *)app[0], "$PSTMCFGLOGERROR") == 0) {
        datalog_data->op = GNSS_FEATURE_EN_MSG;
        datalog_data->result = GNSS_OP_ERROR;
      }
      /* Configuring */
      if (strcmp((void *)app[0], "$PSTMLOGCREATEOK") == 0) {
        datalog_data->op = GNSS_DATALOG_CFG_MSG;
        datalog_data->result = GNSS_OP_OK;
      }
      if (strcmp((void *)app[0], "$PSTMLOGCREATEERROR") == 0) {
        datalog_data->op = GNSS_DATALOG_CFG_MSG;
        datalog_data->result = GNSS_OP_ERROR;
      }
      /* Start */
      if (strcmp((void *)app[0], "$PSTMLOGSTARTOK") == 0) {
        datalog_data->op = GNSS_DATALOG_START_MSG;
        datalog_data->result = GNSS_OP_OK;
      }
      if (strcmp((void *)app[0], "$PSTMLOGSTARTERROR") == 0) {
        datalog_data->op = GNSS_DATALOG_START_MSG;
        datalog_data->result = GNSS_OP_ERROR;
      }
      /* Stop */
      if (strcmp((void *)app[0], "$PSTMLOGSTOPOK") == 0) {
        datalog_data->op = GNSS_DATALOG_STOP_MSG;
        datalog_data->result = GNSS_OP_OK;
      }
      if (strcmp((void *)app[0], "$PSTMLOGSTOPERROR") == 0) {
        datalog_data->op = GNSS_DATALOG_STOP_MSG;
        datalog_data->result = GNSS_OP_ERROR;
      }
      /* Erase */
      if (strcmp((void *)app[0], "$PSTMLOGERASEOK") == 0) {
        datalog_data->op = GNSS_DATALOG_ERASE_MSG;
        datalog_data->result = GNSS_OP_OK;
      }
      if (strcmp((void *)app[0], "$PSTMLOGERASEERROR") == 0) {
        datalog_data->op = GNSS_DATALOG_ERASE_MSG;
        datalog_data->result = GNSS_OP_ERROR;
      }
      
      valid_msg = FALSE;
      status = PARSE_SUCC;
    }
  }
  return status;
}
/**
* @}
*/

/** @addtogroup PRIVATE_FUNCTIONS
 * @{
 */ 
int32_t check_savevarmsg(char header[])
{
  int32_t is_savevarmsg = 1;
  
  if (strcmp(header, "$PSTMSAVEPAROK") == 0) {
    is_savevarmsg = 0;
  }
  if (strcmp(header, "$PSTMSAVEPARERROR") == 0) {
    is_savevarmsg = 0;
  }

  return is_savevarmsg;
}
/**
* @}
*/

/** @addtogroup EXPORTED_FUNCTIONS
 * @{
 */ 
/**
 * @brief  
 * @param  OpResult_t     Ack from Teseo
 * @param  NMEA           NMEA string read by the Gps expansion.
 * @retval ParseStatus_Typedef PARSE_SUCC if the parsing process goes ok, PARSE_FAIL if it doesn't
 */
ParseStatus_t parse_pstmsavepar(OpResult_t *result, uint8_t NMEA[])
{
  uint8_t app[MAX_MSG_LEN][MAX_MSG_LEN];
  int32_t new_field;
  BOOL valid_msg = FALSE;
  
  ParseStatus_t status = PARSE_FAIL;

  if(NMEA != NULL) {

    /* clear the app[][] buffer */ 
    for (int32_t i=0; i<MAX_MSG_LEN; i++) {
      memset(app[i], 0, (size_t)MAX_MSG_LEN);
    }
    
    for(int32_t i = 0, j = 0, k = 0; (NMEA[i] != '\n'); i++)
    {
      new_field = 0;

      if ((NMEA[i] == ',') || (NMEA[i] == '*')) {
        app[j][k] = '\0';
        new_field = 1;
        
        if (check_savevarmsg((void *)app[0]) == 0) {
          j++;
          k = 0;
          valid_msg = TRUE;
        }
        else {break;
        }
      }
      if(new_field == 0) {
        app[j][k++] = NMEA[i];
      }
    }
    
    if (valid_msg == TRUE) {
      /* Enabling */
      if (strcmp((void *)app[0], "$PSTMSAVEPAROK") == 0) {
        *result = GNSS_OP_OK;
      }
      if (strcmp((void *)app[0], "$PSTMSAVEPARERROR") == 0) {
        *result = GNSS_OP_ERROR;
      }
      
      valid_msg = FALSE;
      status = PARSE_SUCC;
    }
  }
  return status;
}

/**
 * @brief  This function makes a copy of the datas stored into gps_t into the 'Info_t' param
 * @param  Info_t	 Instance of a GPGGA_Info_t object where there are the Info_t to be copied
 * @param  gps_t Instance of a GPGGA_Info_t object pointer where the Info_t stored into gps_t have to be copied
 * @retval None
 */
void copy_data(GPGGA_Info_t *info, GPGGA_Info_t gppga_data)
{
  info->acc          = gppga_data.acc;
  info->geoid.height = gppga_data.geoid.height;
  info->geoid.mis    = gppga_data.geoid.mis;
  info->sats         = gppga_data.sats;
  info->update       = gppga_data.update;
  info->utc.hh       = gppga_data.utc.hh;
  info->utc.mm       = gppga_data.utc.mm;
  info->utc.ss       = gppga_data.utc.ss;
  info->utc.utc      = gppga_data.utc.utc;
  info->valid        = gppga_data.valid;
  info->xyz.alt      = gppga_data.xyz.alt;
  info->xyz.lat      = gppga_data.xyz.lat;
  info->xyz.lon      = gppga_data.xyz.lon;
  info->xyz.ew       = gppga_data.xyz.ew;
  info->xyz.ns       = gppga_data.xyz.ns;
  info->xyz.mis      = gppga_data.xyz.mis;
  info->checksum     = gppga_data.checksum;
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

/**
* @}
*/

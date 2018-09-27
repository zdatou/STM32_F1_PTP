#ifndef _init_h_
#define _init_h_

#include "AllHeader.h"



#define SIZE(x) sizeof(x)/4+((sizeof(x)%4)?1:0)

#define FLASH_SAVE_Data  (0x08032000 + 0x4000)



__packed  typedef struct {

 u32 ui_ip;
 u32 ui_mask;
 u32 ui_gatway;
 u16 us_port;

}ts_IP;


__packed typedef struct {

		 ts_IP  ts_local_ipaddr;
	   ts_IP  ts_remote_ipaddr;
		 uint16_t  PTP_MODE;
	   s32			sc_FixOffset;


}ts_FLASH_DATA_SAVE;












void system_init(void);

#endif


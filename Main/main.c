#include "AllHeader.h"

int main(void)
{	
	TimeInternal timeTmp = {1538341505, 0};
    system_init();
	
	setTime(&timeTmp);
	while(1)
	{
	  run_task();
	}  	
}


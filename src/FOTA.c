/*
 ; This file is part of Badadroid project.
 ;
 ; Copyright (C) 2012 Rebellos, mijoma, b_kubica
 ;
 ;
 ; Badadroid is free software: you can redistribute it and/or modify
 ; it under the terms of the GNU General Public License as published by
 ; the Free Software Foundation, either version 3 of the License, or
 ; (at your option) any later version.
 ;
 ; Badadroid is distributed in the hope that it will be useful,
 ; but WITHOUT ANY WARRANTY; without even the implied warranty of
 ; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ; GNU General Public License for more details.
 ;
 ; You should have received a copy of the GNU General Public License
 ; along with Badadroid.  If not, see <http://www.gnu.org/licenses/>.
 ;
 ;
*/
#include "string.h"
#include "BL3.h"
#include "atag.h"


int main(runMode_t mode)
{
   void* kernelImage = L"";
   char* cmdlnRM = "recovery"; // rename it to make "reboot recovery" work
   char* cmdlnBG = "firefox"; 
   char* cmdln = "android";
   
   unsigned char ATAG_buf[512]={0};
   t_stat filestat;
   fun_t kernel;
   int fd;
   unsigned long kernelSize=0;
   
   //here we start the real deal :)
   int mmuctrl = MemMMUCacheEnable(gMMUL1PageTable, 1);
   disp_FOTA_Init();
   disp_FOTA_Printf("*----------------------------*");
   disp_FOTA_Printf("|      FOTA BOOTLOADER       |");
   disp_FOTA_Printf("*----------------------------*");
   disp_FOTA_Printf("| Author:     mijoma         |");
   disp_FOTA_Printf("| Credits to: Rebellos       |");
   disp_FOTA_Printf("*----------------------------*");
   disp_FOTA_Printf("");
      
   //.... Your code here...

   __PfsNandInit();
   __PfsMassInit();
   MemoryCardMount();
   if(mode == rm_FOTA_ANDROID)   
  kernelImage = L"/e/kernel/android"; //great renamed and work
   if(mode == rm_FOTA_RECOVERY)   
 kernelImage = L"/e/kernel/android";  //great //
  if(mode == rm_FOTA_FIRE)    
 kernelImage = L"/e/kernel/firefox";  //great //
  if(mode == rm_FOTA_RECOFIRE)   
  kernelImage = L"/e/kernel/firefox"; //futur test need key combo work
  
   tfs4_stat(kernelImage, &filestat);
   kernelSize = filestat.st_size;
   if ((fd=tfs4_open(kernelImage, 4)) >= 0)
   {
      tfs4_read(fd, &KERNEL_BUF, kernelSize);
      tfs4_close(fd);
   }   
   

   DisableMmuCache(mmuctrl);
   _CoDisableMmu();
   
  
   setup_core_tag(ATAG_buf);
   setup_serial_tag(0x123, 0x456);
   setup_rev_tag('0');
   if(mode == rm_FOTA_FIRE)    
{   
       setup_cmdline_tag(cmdlnBG); 
       disp_FOTA_Printf("Boot Firefox");	
       DRV_Modem_BootingStart();
}
   if(mode == rm_FOTA_RECOFIRE)    
{   
       setup_cmdline_tag(cmdlnRM); 
       disp_FOTA_Printf("Boot Android Recovery Mode");	
}	   
   if(mode == rm_FOTA_ANDROID)
    {   
       setup_cmdline_tag(cmdln);
       disp_FOTA_Printf("Boot Android");
	   DRV_Modem_BootingStart();
	}
   if(mode == rm_FOTA_RECOVERY)    
{   
       setup_cmdline_tag(cmdlnBG); 
       disp_FOTA_Printf("Boot Firefox Recovery Mode");	
       DRV_Modem_BootingStart();
}
   setup_end_tag();
   
   //copy kernel to the right position
   memcpy(&KERNEL_START, &KERNEL_BUF, kernelSize);
   
   //SYSCON operations
   *((unsigned int*)SYSCON_NORMAL_CFG) = 0xFFFFFFFF; 
   _CoDisableDCache();
   _System_DisableVIC();
   _System_DisableIRQ();
   _System_DisableFIQ();
   kernel = (fun_t)&KERNEL_START;
   kernel(0, 8500, ATAG_buf);

   //loop forever
   while(1);
   return 0;
}

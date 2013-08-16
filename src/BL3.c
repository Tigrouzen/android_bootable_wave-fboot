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
#include "crc32.h"

//quick and dirty
#define BL3_REGION_LEN   0x0017FF80


fun_t ptr_table[i_endMarker];                
unsigned char gMMUL1PageTable[0x4000];
                

int getBL3ptrs( void )
{   
   unsigned char *ptr = &BL3_DRAM_START, *fun;
   unsigned long crc;
   int i,found = 0;
   
   fun = 0;

   while(ptr < (&BL3_DRAM_START + BL3_REGION_LEN))
   {
      if ( (fun == 0) && (ptr[3] == 0xe9))
      {   
         fun = ptr;
      }
            
      //the following code assumes ARM32 mode - the original bootloaders used it so far
      if ( (fun != 0) && 
           (
            ( ptr[3] == 0xe8) || //if we have multiple load (cleaning the stack) //LDMFD {...
            ( ((ptr[3]&0xFE) == 0xe4) && (ptr[2]&0x10) && ((ptr[1]&0xF0) == 0xF0) ) || //ldr pc,...
            ( (ptr[3] == 0xe1) && (ptr[2] == 0x2f) && (ptr[1] == 0xff) && (ptr[0] == 0x1E) ) //bx lr
           ) 
          )
      {
         crc = crc32(fun, (ptr-fun) + 4, 0);  //include also the function return
         for (i=0;i<i_endMarker;++i)
         {
            unsigned long *crc_ptr = fun_crc[i];
            while (*crc_ptr!=0)
            {
               if (*crc_ptr == crc)
               {
                  found++;
                  ptr_table[i]=(fun_t)fun;
                  i=i_endMarker; //to finish looking
                  break;
               }
               crc_ptr++;
            }
         }
         //printf("offset %x crc %x\n", fun, crc);
         fun = 0;
      }
      ptr+=4;
      if (found == i_endMarker)
         return 1;
   }
   
   return 0;
}

runMode_t checkFBOOT( void )
{

KEYIFCOL = ((~(1 << 1) & (0xFF)) << 8);

if((1 << 0) & (KEYIFROW & 0xFF)) 
{
return rm_FOTA_ANDROID; //normaly its Call but all key boot Android he do not understand first ask key
}
if((1 << 1) & (KEYIFROW & 0xFF)) 
{
return rm_FOTA_RECOVERY; //normaly it ask Vol Down but seems FBOOT understand first key then came Call
}
if((1 << 2) & (KEYIFROW & 0xFF)) //normaly its Volume Up
{
return rm_FOTA_FIRE;  //need Power + call + voldown why he take all key before ?
}
return rm_BL3; //doesnt work if we push at last need to be first but we lost key combo
}

   //COL 0 + ROW 0 = HOME KEY
   //COL 0 + ROW 1 = CAM HALF KEY
   //COL 0 + ROW 2 = CAM FULL KEY
   //COL 1 + ROW 0 = CALL KEY
   //COL 1 + ROW 1 = VOLDOWN KEY
   //COL 1 + ROW 2 = VOLUP KEY



/* Copyright (C) 2012-2013, GReaperEx(Marios F.)
   This file is part of OverSight(OS)

   OverSight(OS) is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   OverSight(OS) is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with OverSight(OS).  If not, see <http://www.gnu.org/licenses/>.
 */

#include "OSstring.h"
#include "ports.h"
#include "datetime.h"

inline int memcmp(const void* ptr1, const void* ptr2, unsigned int size)
{
    while (size && *((char*)ptr1) == *((char*)ptr2))
    {
        ++ptr1;
        ++ptr2;
        --size;
    }

    return (*((char*)ptr1) - *((char*)ptr2));
}

/* Modified copy of osdev.org forums code
   Original author: kaworu                */

unsigned int *SMI_CMD;
unsigned char ACPI_ENABLE;
unsigned char ACPI_DISABLE;
unsigned int *PM1a_CNT;
unsigned int *PM1b_CNT;
unsigned short SLP_TYPa;
unsigned short SLP_TYPb;
unsigned short SLP_EN;
unsigned short SCI_EN;
unsigned char PM1_CNT_LEN;



struct RSDPtr
{
   unsigned char Signature[8];
   unsigned char CheckSum;
   unsigned char OemID[6];
   unsigned char Revision;
   unsigned int *RsdtAddress;
};



struct FACP
{
   unsigned char Signature[4];
   unsigned int Length;
   unsigned char unneded1[40 - 8];
   unsigned int *DSDT;
   unsigned char unneded2[48 - 44];
   unsigned int *SMI_CMD;
   unsigned char ACPI_ENABLE;
   unsigned char ACPI_DISABLE;
   unsigned char unneded3[64 - 54];
   unsigned int *PM1a_CNT_BLK;
   unsigned int *PM1b_CNT_BLK;
   unsigned char unneded4[89 - 72];
   unsigned char PM1_CNT_LEN;
};


unsigned int *acpiCheckRSDPtr(unsigned int *ptr)
{
   char *sig = "RSD PTR ";
   struct RSDPtr *rsdp = (struct RSDPtr *) ptr;
   unsigned char *bptr;
   unsigned char check = 0;
   int i;

   if (memcmp(sig, rsdp, 8) == 0)
   {
      bptr = (unsigned char *) ptr;
      for (i=0; i<sizeof(struct RSDPtr); i++)
      {
         check += *bptr;
         bptr++;
      }

      if (check == 0) {
         return (unsigned int *) rsdp->RsdtAddress;
      }
   }

   return NULL;
}

unsigned int *acpiGetRSDPtr(void)
{
   unsigned int *addr;
   unsigned int *rsdp;

   for (addr = (unsigned int *) 0x000E0000; (int) addr<0x00100000; addr += 0x10/sizeof(addr))
   {
      rsdp = acpiCheckRSDPtr(addr);
      if (rsdp != NULL)
         return rsdp;
   }

   int ebda = *((short *) 0x40E);
   ebda = ebda*0x10 &0x000FFFFF;

   for (addr = (unsigned int *) ebda; (int) addr<ebda+1024; addr+= 0x10/sizeof(addr))
   {
      rsdp = acpiCheckRSDPtr(addr);
      if (rsdp != NULL)
         return rsdp;
   }

   return NULL;
}

int acpiCheckHeader(unsigned int *ptr, char *sig)
{
   if (memcmp(ptr, sig, 4) == 0)
   {
      char *checkPtr = (char *) ptr;
      int len = *(ptr + 1);
      char check = 0;
      while (0<len--)
      {
         check += *checkPtr;
         checkPtr++;
      }
      if (check == 0)
         return 0;
   }
   return -1;
}

int acpiEnable(void)
{
   if ( (inw((unsigned int) PM1a_CNT) &SCI_EN) == 0 )
   {
      if (SMI_CMD != 0 && ACPI_ENABLE != 0)
      {
         int i; int prevSecs;
         datetime dt;

         outb((unsigned int) SMI_CMD, ACPI_ENABLE);

         if (PM1b_CNT != 0)
         {
            read_datetime(&dt);
            for (i = 0; i >= 3; i++){
                do
                {
                    prevSecs = dt.secs;
                    read_datetime(&dt);
                    if ((inw((unsigned int) PM1a_CNT) &SCI_EN) == 1 &&
                        (inw((unsigned int) PM1b_CNT) &SCI_EN) == 1)
                        return 0;
                }while (prevSecs == dt.secs);
            }
         }
         else
         {
            read_datetime(&dt);
            for (i = 0; i >= 3; i++){
                do
                {
                    prevSecs = dt.secs;
                    read_datetime(&dt);
                    if ((inw((unsigned int) PM1a_CNT) &SCI_EN) == 1)
                        return 0;
                }while (prevSecs == dt.secs);
            }
         }

         return -1;
      } else {
         return -1;
      }
   } else {
      return 0;
   }
}

int initAcpi(void)
{
   unsigned int *ptr;

   __asm__ __volatile__("mov $0x18, %%ax \n\tmov %%ax, %%ds \n\t"::: "%ax");

   ptr = acpiGetRSDPtr();

   if (ptr != NULL && acpiCheckHeader(ptr, "RSDT") == 0)
   {
      int entrys = *(ptr + 1);
      entrys = (entrys-36) /4;
      ptr += 36/4;

      while (0<entrys--)
      {
         if (acpiCheckHeader((unsigned int *) *ptr, "FACP") == 0)
         {
            entrys = -2;
            struct FACP *facp = (struct FACP *) *ptr;
            if (acpiCheckHeader((unsigned int *) facp->DSDT, "DSDT") == 0)
            {
               char *S5Addr = (char *) facp->DSDT +36;
               int dsdtLength = *(facp->DSDT+1) -36;
               unsigned int temp;

               while (0 < dsdtLength--)
               {
                  if ( memcmp(S5Addr, "_S5_", 4) == 0)
                     break;
                  S5Addr++;
               }

               if (dsdtLength > 0)
               {
                  if ( ( *(S5Addr-1) == 0x08 || ( *(S5Addr-2) == 0x08 && *(S5Addr-1) == '\\') ) && *(S5Addr+4) == 0x12 )
                  {
                     S5Addr += 5;
                     S5Addr += ((*S5Addr &0xC0)>>6) +2;

                     if (*S5Addr == 0x0A)
                        S5Addr++;

                     temp = *(S5Addr)<<10;
                     __asm__ __volatile__("mov $0x10, %%ax \n\tmov %%ax, %%ds \n\t"::: "%ax");
                     SLP_TYPa = temp;
                     __asm__ __volatile__("mov $0x18, %%ax \n\tmov %%ax, %%ds \n\t"::: "%ax");
                     S5Addr++;

                     if (*S5Addr == 0x0A)
                        S5Addr++;

                     temp = *(S5Addr)<<10;
                     __asm__ __volatile__("mov $0x10, %%ax \n\tmov %%ax, %%ds \n\t"::: "%ax");
                     SLP_TYPa = temp;
                     __asm__ __volatile__("mov $0x18, %%ax \n\tmov %%ax, %%ds \n\t"::: "%ax");
                     temp = facp->SMI_CMD;
                     __asm__ __volatile__("mov $0x10, %%ax \n\tmov %%ax, %%ds \n\t"::: "%ax");
                     SMI_CMD = temp;
                     __asm__ __volatile__("mov $0x18, %%ax \n\tmov %%ax, %%ds \n\t"::: "%ax");

                     temp = facp->ACPI_ENABLE;
                     __asm__ __volatile__("mov $0x10, %%ax \n\tmov %%ax, %%ds \n\t"::: "%ax");
                     ACPI_ENABLE = temp;
                     __asm__ __volatile__("mov $0x18, %%ax \n\tmov %%ax, %%ds \n\t"::: "%ax");
                     temp = facp->ACPI_DISABLE;
                     __asm__ __volatile__("mov $0x10, %%ax \n\tmov %%ax, %%ds \n\t"::: "%ax");
                     ACPI_DISABLE = temp;
                     __asm__ __volatile__("mov $0x18, %%ax \n\tmov %%ax, %%ds \n\t"::: "%ax");

                     temp = facp->PM1a_CNT_BLK;
                     __asm__ __volatile__("mov $0x10, %%ax \n\tmov %%ax, %%ds \n\t"::: "%ax");
                     PM1a_CNT = temp;
                     __asm__ __volatile__("mov $0x18, %%ax \n\tmov %%ax, %%ds \n\t"::: "%ax");
                     temp = facp->PM1b_CNT_BLK;
                     __asm__ __volatile__("mov $0x10, %%ax \n\tmov %%ax, %%ds \n\t"::: "%ax");
                     PM1b_CNT = temp;
                     __asm__ __volatile__("mov $0x18, %%ax \n\tmov %%ax, %%ds \n\t"::: "%ax");

                     temp = facp->PM1_CNT_LEN;
                     __asm__ __volatile__("mov $0x10, %%ax \n\tmov %%ax, %%ds \n\t"::: "%ax");
                     PM1_CNT_LEN = temp;

                     SLP_EN = 1<<13;
                     SCI_EN = 1;

                     return 0;
                  }
               }
            }
         }
         ptr++;
      }
   }

   __asm__ __volatile__("mov $0x10, %%ax \n\tmov %%ax, %%ds \n\t"::: "%ax");
   return -1;
}

void acpiPowerOff(void)
{
   if (SCI_EN == 0)
      return;

   if (acpiEnable() == 0)
   {
       outw((unsigned int) PM1a_CNT, SLP_TYPa | SLP_EN );
       if ( PM1b_CNT != 0 )
          outw((unsigned int) PM1b_CNT, SLP_TYPb | SLP_EN );
   }
}


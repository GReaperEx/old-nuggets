/* Copyright (C) 2012-2014, GReaperEx(Marios F.)
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

#ifndef __PCI_H_
#define __PCI_H_

#include "OStypes.h"

typedef enum {
    PCI_HT_REGULAR = 0x0, PCI_HT_PCI2PCI = 0x1, PCI_HT_PCI2CARDBUS = 0x2
}pciHeaderTypes;

/* PCI Device Header Type register */
typedef union
{
    uint8_t value;

    struct
    {
        uint8_t type : 7;
        uint8_t MF   : 1; /* Boolean: Multi-Function */
    };
}pciHeaderTypeReg;

/* PCI Device BIST register */
typedef union
{
    uint8_t value;

    struct
    {
        uint8_t completionCode : 4;
        uint8_t _res           : 2;
        uint8_t startBIST      : 1;
        uint8_t BISTcapable    : 1;
    };
}pciBISTreg;

/* PCI Device Command register */
typedef union
{
    uint16_t value;

    struct
    {
        uint16_t ioSpace             : 1;
        uint16_t memSpace            : 1;
        uint16_t busMaster           : 1;
        uint16_t specialCycles       : 1;
        uint16_t memWriteN_InvEnable : 1;
        uint16_t VGApaletteSnoop     : 1;
        uint16_t parityErrorResponse : 1;
        uint16_t _res1               : 1;
        uint16_t SERR_enable         : 1;
        uint16_t fastBtB_enable      : 1;
        uint16_t interDisable        : 1;
        uint16_t _res2               : 5;
    };
}pciCommandReg;

/* PCI Device Status register */
typedef union
{
    uint16_t value;

    struct
    {
        uint16_t _res1                 : 3;
        uint16_t interStatus           : 1;
        uint16_t capabilitiesList      : 1;
        uint16_t MHz66_capable         : 1;
        uint16_t _res2                 : 1;
        uint16_t fastBtB_capable       : 1;
        uint16_t masterDataParityError : 1;
        uint16_t DEVSEL_timing         : 2;
        uint16_t signaledTargetAbort   : 1;
        uint16_t receivedTargetAbort   : 1;
        uint16_t receivedMasterAbort   : 1;
        uint16_t signaledSystemError   : 1;
        uint16_t detectedParityError   : 1;
    };
}pciStatusReg;

/* PCI Memory Space BAR register */
typedef struct
{
    uint32_t alwaysZero     : 1;
    uint32_t type           : 2;
    uint32_t prefetchable   : 1;
    uint32_t baseAddressX16 : 28;
}pciMemSpaceBAR;

/* PCI I/O Space BAR register */
typedef struct
{
    uint32_t alwaysOne     : 1;
    uint32_t _res          : 1;
    uint32_t baseAddressX4 : 30;
}pciIOSpaceBAR;

/* Combining the two for easy access */
typedef union
{
    uint32_t lvalue;

    pciMemSpaceBAR memSpaceBAR;
    pciIOSpaceBAR ioSpaceBAR;
}pciBAR;

/* PCI Regular Device(type 0x0) header */
typedef struct
{
    uint16_t vendorID;
    uint16_t deviceID;

    pciCommandReg command;
    pciStatusReg status;

    uint8_t revisionID;
    uint8_t progIF;
    uint8_t subclass;
    uint8_t classCode;

    uint8_t cacheLineSize;
    uint8_t latencyTimer;
    pciHeaderTypeReg headerType;
    pciBISTreg BIST;

    pciBAR BAR0;
    pciBAR BAR1;
    pciBAR BAR2;
    pciBAR BAR3;
    pciBAR BAR4;
    pciBAR BAR5;

    uint32_t cardbusCISptr;

    uint16_t subsysVendorID;
    uint16_t subsysID;

    uint32_t eROM_baseAddress;

    uint16_t capabilitiesPtr;
    uint16_t _res1;

    uint32_t _res2;

    uint8_t interLine;
    uint8_t interPIN;
    uint8_t minGrant;
    uint8_t maxLatency;
}pciDevRegular;

/* PCI to PCI Device(type 0x1) header */
typedef struct
{
    uint16_t vendorID;
    uint16_t deviceID;

    pciCommandReg command;
    pciStatusReg status;

    uint8_t revisionID;
    uint8_t progIF;
    uint8_t subclass;
    uint8_t classCode;

    uint8_t cacheLineSize;
    uint8_t latencyTimer;
    pciHeaderTypeReg headerType;
    pciBISTreg BIST;

    pciBAR BAR0;
    pciBAR BAR1;

    uint8_t primaryBusNum;
    uint8_t secondaryBusNum;
    uint8_t subordinateBusNum;
    uint8_t secondaryLatencyTimer;

    uint8_t ioBase;
    uint8_t ioLimit;
    pciStatusReg secondaryStatus;

    uint16_t memBase;
    uint16_t memLimit;

    uint16_t prefetchableMemBase;
    uint16_t prefetchableMemLimit;

    uint32_t prefetchableBaseUpper;
    uint32_t prefetchableLimitUpper;

    uint16_t ioBaseUpper;
    uint16_t ioLimitUpper;

    uint16_t capabilitiesPtr;
    uint16_t _res1;

    uint32_t eROM_baseAddress;

    uint8_t interLine;
    uint8_t interPIN;
    uint16_t bridgeControl;
}pciDevPCItoPCI;

/* PCI to CardBus Device(type 0x2) header */
typedef struct
{
    uint16_t vendorID;
    uint16_t deviceID;

    pciCommandReg command;
    pciStatusReg status;

    uint8_t revisionID;
    uint8_t progIF;
    uint8_t subclass;
    uint8_t classCode;

    uint8_t cacheLineSize;
    uint8_t latencyTimer;
    pciHeaderTypeReg headerType;
    pciBISTreg BIST;

    uint32_t cardBusSocket_ExCaBaseAddress;

    uint8_t capabilitiesListOffset;
    uint8_t _res1;
    pciStatusReg secondaryStatus;

    uint8_t PCI_busNum;
    uint8_t cardBus_busNum;
    uint8_t subordinate_busNum;
    uint8_t cardBus_latencyTimer;

    uint32_t memBase0;
    uint32_t memLimit0;

    uint32_t memBase1;
    uint32_t memLimit1;

    uint32_t ioBase0;
    uint32_t ioLimit0;

    uint32_t ioBase1;
    uint32_t ioLimit1;

    uint8_t interLine;
    uint8_t interPIN;
    uint16_t bridgeControl;

    uint16_t subsysDeviceID;
    uint16_t subsysVendorID;

    uint32_t legacyCardModeBase;
}pciDevPCItoCardBus;

/* Defined for easy access, using the headerType to distinguish between them */
typedef union
{
    uint32_t lvalues[18];

    struct
    {
        uint16_t vendorID;
        uint16_t deviceID;

        pciCommandReg command;
        pciStatusReg status;

        uint8_t revisionID;
        uint8_t progIF;
        uint8_t subclass;
        uint8_t classCode;

        uint8_t cacheLineSize;
        uint8_t latencyTimer;
        pciHeaderTypeReg headerType;
        pciBISTreg BIST;
    };

    pciDevRegular regular;
    pciDevPCItoPCI pci2pci;
    pciDevPCItoCardBus pci2cardbus;
}pciDeviceInfo;

/* Holds additional information for easy updating */
typedef struct
{
    pciDeviceInfo devInfo;

    /** These should be set by the appropriate driver */
    void* devMemory; /* The total memory requested to be mapped by BARs */
    size_t devMemSize; /* Size in bytes */

    uint8_t bus;
    uint8_t dev;
    uint8_t func;
}pciDeviceInfoEx;

/* These fetch a byte, word or dword from the PCI Configuration space */
uint8_t  pci_inb(uint8_t bus, uint8_t dev, uint8_t func, uint8_t offset);
uint16_t pci_inw(uint8_t bus, uint8_t dev, uint8_t func, uint8_t offset);
uint32_t pci_ind(uint8_t bus, uint8_t dev, uint8_t func, uint8_t offset);

/* These change a byte, word or dword of the PCI Configuration space */
void pci_outb(uint8_t bus, uint8_t dev, uint8_t func, uint8_t offset, uint8_t  output);
void pci_outw(uint8_t bus, uint8_t dev, uint8_t func, uint8_t offset, uint16_t output);
void pci_outd(uint8_t bus, uint8_t dev, uint8_t func, uint8_t offset, uint32_t output);

/* Fills passed struct with the device's information. Returns 1 on success and 0 on failure */
int pci_getDeviceInfo(pciDeviceInfoEx* devInfo, uint8_t bus, uint8_t dev, uint8_t func);

/* Re-fetches the Status register's value */
void pci_updateDevStatus(pciDeviceInfoEx* devInfo);
/* Writes the status value saved in the struct back to PCI Configuration space */
void pci_commitDevStatus(pciDeviceInfoEx* devInfo);

/* Re-fetches the Command register's value */
void pci_updateDevCommand(pciDeviceInfoEx* devInfo);
/* Writes the command saved in the struct back to PCI Configuration space */
void pci_commitDevCommand(pciDeviceInfoEx* devInfo);

/* Re-fetches the BIST register's value */
void pci_updateDevBISTreg(pciDeviceInfoEx* devInfo);
/* Writes the BIST register value saved in the struct back to PCI Configuration space */
void pci_commitDevBISTreg(pciDeviceInfoEx* devInfo);

/* Returns a pointer to an allocated array of found devices' info, saves their amount in devCount */
pciDeviceInfoEx* pci_enumDevices(int* devCount);

#endif /* __PCI_H_ */

/* Copyright (C) 2014, GReaperEx(Marios F.)
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

#include "PCI.h"
#include "ports.h"
#include "OSalloc.h"

#define CONFIG_ADDRESS  0xCF8
#define CONFIG_DATA     0xCFC

/* Defined for convenient conversion to PCI register address format */
typedef union __attribute__((packed))
{
    uint32_t lvalue;

    struct
    {
        uint32_t _res1   : 2;
        uint32_t regNum  : 6;
        uint32_t funcNum : 3;
        uint32_t devNum  : 5;
        uint32_t busNum  : 8;
        uint32_t _res2   : 7;
        uint32_t enable  : 1;
    };
}pciConfigAddress;


uint8_t pci_inb(uint8_t bus, uint8_t dev, uint8_t func, uint8_t offset)
{
    pciConfigAddress address = { 0 };
    uint8_t input = 0;

    address.busNum = bus;
    address.devNum = dev;
    address.funcNum = func;
    address.regNum = offset >> 2;
    address.enable = 1;

    /* write out the address */
    outd(CONFIG_ADDRESS, address.lvalue);
    /* read in the data */
    input = (uint8_t)((ind(CONFIG_DATA) >> ((offset & 0x3)*8)) & 0xFF);

    return input;
}

uint16_t pci_inw(uint8_t bus, uint8_t dev, uint8_t func, uint8_t offset)
{
    pciConfigAddress address = { 0 };
    uint16_t input = 0;

    address.busNum = bus;
    address.devNum = dev;
    address.funcNum = func;
    address.regNum = offset >> 2;
    address.enable = 1;

    /* write out the address */
    outd(CONFIG_ADDRESS, address.lvalue);
    /* read in the data */
    input = (uint16_t)((ind(CONFIG_DATA) >> ((offset & 0x2)*8)) & 0xFFFF);

    return input;
}

uint32_t pci_ind(uint8_t bus, uint8_t dev, uint8_t func, uint8_t offset)
{
    pciConfigAddress address = { 0 };
    uint32_t input = 0;

    address.busNum = bus;
    address.devNum = dev;
    address.funcNum = func;
    address.regNum = offset >> 2;
    address.enable = 1;

    /* write out the address */
    outd(CONFIG_ADDRESS, address.lvalue);
    /* read in the data */
    input = ind(CONFIG_DATA);

    return input;
}

void pci_outb(uint8_t bus, uint8_t dev, uint8_t func, uint8_t offset, uint8_t output)
{
    pciConfigAddress address = { 0 };
    uint32_t tmp = 0;

    address.busNum = bus;
    address.devNum = dev;
    address.funcNum = func;
    address.regNum = offset >> 2;
    address.enable = 1;

    /* set the address */
    outd(CONFIG_ADDRESS, address.lvalue);
    /* read previous data */
    tmp = ind(CONFIG_DATA);
    /* mask the part to be changed */
    tmp &= 0xFF << ((offset & 0x3)*8);
    /* overlay with new data */
    tmp |= output << ((offset & 0x3)*8);
    /* write data back to device */
    outd(CONFIG_DATA, tmp);
}

void pci_outw(uint8_t bus, uint8_t dev, uint8_t func, uint8_t offset, uint16_t output)
{
    pciConfigAddress address = { 0 };
    uint32_t tmp = 0;

    address.busNum = bus;
    address.devNum = dev;
    address.funcNum = func;
    address.regNum = offset >> 2;
    address.enable = 1;

    /* set the address */
    outd(CONFIG_ADDRESS, address.lvalue);
    /* read previous data */
    tmp = ind(CONFIG_DATA);
    /* mask the part to be changed */
    tmp &= 0xFFFF << ((offset & 0x2)*8);
    /* overlay with new data */
    tmp |= output << ((offset & 0x2)*8);
    /* write data back to device */
    outd(CONFIG_DATA, tmp);
}

void pci_outd(uint8_t bus, uint8_t dev, uint8_t func, uint8_t offset, uint32_t output)
{
    pciConfigAddress address = { 0 };

    address.busNum = bus;
    address.devNum = dev;
    address.funcNum = func;
    address.regNum = offset >> 2;
    address.enable = 1;

    /* set the address */
    outd(CONFIG_ADDRESS, address.lvalue);
    /* write the data */
    outd(CONFIG_DATA, output);
}

int pci_getDeviceInfo(pciDeviceInfoEx* devInfo, uint8_t bus, uint8_t dev, uint8_t func)
{
    uint16_t vendorID;
    int headerSize, i;

    vendorID = pci_inw(bus, dev, func, 0);

    if (vendorID == 0xFFFF || devInfo == NULL)
        return 0;

    devInfo->devInfo.lvalues[0] = pci_ind(bus, dev, func, 0);
    devInfo->devInfo.lvalues[1] = pci_ind(bus, dev, func, 4);
    devInfo->devInfo.lvalues[2] = pci_ind(bus, dev, func, 8);
    devInfo->devInfo.lvalues[3] = pci_ind(bus, dev, func, 12);

    switch (devInfo->devInfo.headerType.type)
    {
        case PCI_HT_REGULAR:
            headerSize = sizeof(pciDevRegular);
        break;
        case PCI_HT_PCI2PCI:
            headerSize = sizeof(pciDevPCItoPCI);
        break;
        case PCI_HT_PCI2CARDBUS:
            headerSize = sizeof(pciDevPCItoCardBus);
        break;
        default:
            headerSize = 16;
    }

    for (i = 16; i < headerSize; i += 4) {
        devInfo->devInfo.lvalues[i/4] = pci_ind(bus, dev, func, i);
    }

    devInfo->devMemory = NULL;
    devInfo->devMemSize = 0;

    devInfo->bus = bus;
    devInfo->dev = dev;
    devInfo->func = func;

    return 1;
}

void pci_updateDevStatus(pciDeviceInfoEx* devInfo)
{
    if (devInfo != NULL)
        devInfo->devInfo.status.value = pci_inw(devInfo->bus, devInfo->dev, devInfo->func, 6);
}

void pci_commitDevStatus(pciDeviceInfoEx* devInfo)
{
    if (devInfo != NULL)
        pci_outw(devInfo->bus, devInfo->dev, devInfo->func, 6, devInfo->devInfo.status.value);
}


void pci_updateDevCommand(pciDeviceInfoEx* devInfo)
{
    if (devInfo != NULL)
        devInfo->devInfo.command.value = pci_inw(devInfo->bus, devInfo->dev, devInfo->func, 4);
}

void pci_commitDevCommand(pciDeviceInfoEx* devInfo)
{
    if (devInfo != NULL)
        pci_outw(devInfo->bus, devInfo->dev, devInfo->func, 4, devInfo->devInfo.command.value);
}

void pci_updateDevBISTreg(pciDeviceInfoEx* devInfo)
{
    if (devInfo != NULL)
        devInfo->devInfo.BIST.value = pci_inb(devInfo->bus, devInfo->dev, devInfo->func, 15);
}

void pci_commitDevBISTreg(pciDeviceInfoEx* devInfo)
{
    if (devInfo != NULL)
        pci_outb(devInfo->bus, devInfo->dev, devInfo->func, 15, devInfo->devInfo.BIST.value);
}

void _addToArray(pciDeviceInfoEx** array, const pciDeviceInfoEx* newValue, int* pageCount, int* curSize);

pciDeviceInfoEx* pci_enumDevices(int* devCount)
{
    pciDeviceInfoEx* devArray;
    int pageCount, curSize;
    int bus, device;
    pciDeviceInfoEx temp;

    devArray = NULL;
    pageCount = curSize = 0;

    for (bus = 0; bus < 256; bus++) {
        for (device = 0; device < 32; device++) {
            if (pci_getDeviceInfo(&temp, bus, device, 0))
            {
                int func;

                _addToArray(&devArray, &temp, &pageCount, &curSize);

                if (temp.devInfo.headerType.MF)
                {
                    for (func = 1; func < 8; func++) {
                        if (pci_getDeviceInfo(&temp, bus, device, func))
                            _addToArray(&devArray, &temp, &pageCount, &curSize);
                    }
                }
            }
        }
    }

    *devCount = curSize/sizeof(pciDeviceInfoEx);

    return devArray;
}

void _addToArray(pciDeviceInfoEx** array, const pciDeviceInfoEx* newValue, int* pageCount, int* curSize)
{
    if (*curSize + sizeof(pciDeviceInfoEx) > (*pageCount)*4096)
    {
        pciDeviceInfoEx* newArray = OSrealloc(*array, *pageCount, *pageCount + 1, TRUE);

        if (newArray == NULL)
            return;

        *array = newArray;
        *pageCount += 1;
    }

    (*array)[*curSize/sizeof(pciDeviceInfoEx)] = *newValue;

    *curSize += sizeof(pciDeviceInfoEx);
}


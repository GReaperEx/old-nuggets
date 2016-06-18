// Copyright (C) 2012, GReaperEx(Marios F.)
// This file is part of the DiLa[Kenbak-1]

#ifndef __PC_UNIT_H_
#define __PC_UNIT_H_

#include <cstdlib>

namespace DiLa
{

enum Buttons
{
    B_Data0 = 0, B_Data1, B_Data2, B_Data3, B_Data4, B_Data5, B_Data6, B_Data7,
    B_Clear, B_Display, B_Set, B_Read, B_Store, B_Start, B_Stop
};

bool IsButtonPressed(Buttons);
void PressButton(Buttons);
void ReleaseButton(Buttons);

bool IsPowerOn();
void TurnPowerOn();
void TurnPowerOff();

enum LEDs
{
    LED7 = 0, LED6, LED5, LED4, 
    LED3, LED2, LED1, LED0, 
    LED8, LED9, LED10, LED11
};

bool IsLEDon(LEDs);

int ResumeOperation();

} // namespace DiLa

#endif // __PC_UNIT_H_


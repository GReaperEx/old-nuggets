#include "PCunit.h"

namespace DiLa
{

unsigned char PCmem[256];

unsigned char Reg_W;

unsigned char& Reg_A      = PCmem[0];
unsigned char& Reg_B      = PCmem[1];
unsigned char& Reg_X      = PCmem[2];
unsigned char& Reg_P      = PCmem[3];
unsigned char& Reg_Output = PCmem[128];
unsigned char& Reg_OCA    = PCmem[129];
unsigned char& Reg_OCB    = PCmem[130];
unsigned char& Reg_OCX    = PCmem[131];
unsigned char& Reg_Input  = PCmem[255];

unsigned int bmpButtons = 0;
unsigned int bmpLEDs = 0;

int FreshPress = -1;

bool Power;
bool Run;

bool IsButtonPressed(Buttons b)
{
    return (bmpButtons >> static_cast<int>(b)) & 0x1;
}

void PressButton(Buttons b)
{
    if (!((bmpButtons >> static_cast<int>(b)) & 0x1))
        FreshPress = static_cast<int>(b);
    
    bmpButtons |= 0x1 << static_cast<int>(b);
}

void ReleaseButton(Buttons b)
{
    bmpButtons &= ~(0x1 << static_cast<int>(b));
}

bool IsPowerOn()
{
    return Power;
}

void TurnPowerOn()
{
    Power = true;
    Run = false;
    
    bmpLEDs = 0;
    FreshPress = -1;
    
    for (int i = 0; i < 256; i++){
        PCmem[i] = rand() & 0xFF;
    }
    
    Reg_P = 4;
}

void TurnPowerOff()
{
    bmpLEDs = 0;
    
    Power = false;
}

bool IsLEDon(LEDs led)
{
    return (bmpLEDs >> static_cast<int>(led)) & 0x1;
}

int ResumeOperation()
{
    bool process = false;
    
    static int mappedValues[4] = { 4, 1, 2, 3 };
    
    if (!Power)
        return -1;
    
    if (FreshPress != -1)
    {
        switch (static_cast<Buttons>(FreshPress))
        {
            case B_Clear:
                Reg_Input = 0;
                bmpLEDs |= 0x1 << static_cast<int>(LED8);
            break;
            case B_Display:
                Reg_Output = Reg_W;
                Reg_Input = Reg_W;
                bmpLEDs |= 0x1 << static_cast<int>(LED9);
            break;
            case B_Set:
                Reg_W = Reg_Input;
                bmpLEDs |= 0x1 << static_cast<int>(LED9);
            break;
            case B_Read:
                if (!Run)
                {
                    Reg_Output = PCmem[Reg_W];
                    Reg_Input = PCmem[Reg_W++];
                    bmpLEDs |= 0x1 << static_cast<int>(LED10);
                }
            break;
            case B_Store:
                if (!Run)
                {
                    PCmem[Reg_W++] = Reg_Input;
                    bmpLEDs |= 0x1 << static_cast<int>(LED10);
                }
            break;
            case B_Start:
                if ((bmpButtons >> static_cast<int>(B_Stop)) & 0x1)
                    process = true;
                else
                {
                    Run = true;
                    
                    bmpLEDs &= ~((0x1 << static_cast<int>(LED8)) |
                                 (0x1 << static_cast<int>(LED9)) |
                                 (0x1 << static_cast<int>(LED10)));
                                 
                    bmpLEDs |= 0x1 << static_cast<int>(LED11);
                }
            break;
            case B_Stop:
                bmpLEDs &= ~(0x1 << static_cast<int>(LED11));
                Run = false;
            break;
            default:
                if (!Run)
                    bmpLEDs |= 0x1 << static_cast<int>(LED8);
        }
        
        FreshPress = -1;
    }
    
    // Even after a button is released, the data must be preserved
    Reg_Input |= bmpButtons & 0xFF;
    
    if (Run || process)
    {
        unsigned char temp = PCmem[Reg_P];
        unsigned char Byte2nd;
        
        switch (temp & 0x7)
        {
            case 0:
                switch (temp >> 6)
                {
                    case 0:
                    case 1:
                        // Halt
                        bmpLEDs &= ~(0x1 << static_cast<int>(LED11));
                        Run = false;
                    break;
                    // Noop
                }
                
                Reg_P += 1;
            break;
            case 1:
                switch (temp >> 6)
                {
                    case 0:
                    {
                        int toShift = mappedValues[(temp >> 3) & 0x3];
                        
                        // Right arithmetic shift
                        __asm__ __volatile__("MOV %1, %%CL \n\t"
                                             "SARb %%CL, %0 \n\t" 
                                             : "=m"(PCmem[(temp >> 5) & 0x1]) 
                                             : "m"(toShift)
                                             : "%cl");
                    }
                    break;
                    case 1:
                    {
                        int toRotate = mappedValues[(temp >> 3) & 0x3];

                        // Right rotate
                        __asm__ __volatile__("MOV %1, %%CL \n\t"
                                             "RORb %%CL, %0 \n\t" 
                                             : "=m"(PCmem[(temp >> 5) & 0x1]) 
                                             : "m"(toRotate)
                                             : "%cl");
                    }
                    break;
                    case 2:
                    {
                        int toShift = mappedValues[(temp >> 3) & 0x3];
                        
                        // Left arithmetic shift
                        __asm__ __volatile__("MOV %1, %%CL \n\t"
                                             "SALb %%CL, %0 \n\t" 
                                             : "=m"(PCmem[(temp >> 5) & 0x1]) 
                                             : "m"(toShift)
                                             : "%cl");
                    }
                    break;
                    case 3:
                    {
                        int toRotate = mappedValues[(temp >> 3) & 0x3];

                        // Left rotate
                        __asm__ __volatile__("MOV %1, %%CL \n\t"
                                             "ROLb %%CL, %0 \n\t" 
                                             : "=m"(PCmem[(temp >> 5) & 0x1]) 
                                             : "m"(toRotate)
                                             : "%cl");
                    }
                    break;
                }
                
                Reg_P += 1;
            break;
            case 2:
                Byte2nd = PCmem[static_cast<unsigned char>(Reg_P+1)];
                
                switch (temp >> 6)
                {
                    case 0: // Set to 0
                        PCmem[Byte2nd] &= ~(0x1 << ((temp >> 3) & 0x7));
                    break;
                    case 1: // Set to 1
                        PCmem[Byte2nd] |= 0x1 << ((temp >> 3) & 0x7);
                    break;
                    case 2: // Skip on 0
                        if (!((PCmem[Byte2nd] >> ((temp >> 3) & 0x7)) & 0x1))
                            Reg_P += 2;
                    break;
                    case 3: // Skip on 1
                        if ((PCmem[Byte2nd] >> ((temp >> 3) & 0x7)) & 0x1)
                            Reg_P += 2;
                    break;
                }
                
                Reg_P += 2;
            break;
            case 3:
                Byte2nd = PCmem[static_cast<unsigned char>(Reg_P+1)];
                
                switch ((temp >> 3) & 0x7)
                {
                    case 0:
                        if ((temp >> 6) < 3) // ADD Reg, Imm
                        {
                            int result = PCmem[temp >> 6] + Byte2nd;
                            PCmem[129 + (temp >> 6)] = 0;
                            
                            if (result > 255)
                                PCmem[129 + (temp >> 6)] |= 0x2;
                            
                            if (result > 127)
                                PCmem[129 + (temp >> 6)] |= 0x1;
                            
                            PCmem[temp >> 6] = static_cast<unsigned char>(result);
                        }
                        else // OR A, Imm
                            Reg_A |= Byte2nd;
                        
                        Reg_P += 2;
                    break;
                    case 1:
                        if ((temp >> 6) < 3) // SUB Reg, Imm
                        {
                            int result = PCmem[temp >> 6] - Byte2nd;
                            PCmem[129 + (temp >> 6)] = 0;
                            
                            if (result > 255)
                                PCmem[129 + (temp >> 6)] |= 0x2;
                            
                            if (result < -128 || result > 127)
                                PCmem[129 + (temp >> 6)] |= 0x1;
                            
                            PCmem[temp >> 6] = static_cast<unsigned char>(result);
                        }
                        // else // 2-Byte Noop
                        
                        Reg_P += 2;
                    break;
                    case 2:
                        if ((temp >> 6) < 3) // LOAD Reg, Imm
                            PCmem[temp >> 6] = Byte2nd;
                        else // AND A, Imm
                            Reg_A &= Byte2nd;
                        
                        Reg_P += 2;
                    break;
                    case 3:
                        if ((temp >> 6) < 3) // STORE Reg, Imm
                            PCmem[Reg_P+1] = PCmem[temp >> 6]; // ToDo: Is this correct ?
                        else // LNEG A, Imm
                            Reg_A = -Byte2nd;
                        
                        Reg_P += 2;
                    break;
                    case 4: // JPD
                        if ((temp >> 6) == 3)
                            Reg_P = Byte2nd;
                        else
                        {
                            if (PCmem[temp >> 6] != 0)
                                Reg_P = Byte2nd;
                            else
                                Reg_P += 2;
                        }
                    break;
                    case 5: // JPI
                        if ((temp >> 6) == 3)
                            Reg_P = PCmem[Byte2nd];
                        else
                        {
                            if (PCmem[temp >> 6] != 0)
                                Reg_P = PCmem[Byte2nd];
                            else
                                Reg_P += 2;
                        }
                    break;
                    case 6: // JMD
                        if ((temp >> 6) == 3)
                            Reg_P = Byte2nd;
                        else
                        {
                            if (PCmem[temp >> 6] != 0)
                                Reg_P = Byte2nd + 1;
                            else
                                Reg_P += 2;
                        }
                    break;
                    case 7: // JMI
                        if ((temp >> 6) == 3)
                            Reg_P = PCmem[Byte2nd];
                        else
                        {
                            if (PCmem[temp >> 6] != 0)
                                Reg_P = PCmem[Byte2nd] + 1;
                            else
                                Reg_P += 2;
                        }
                    break;
                }
            break;
            case 4:
                Byte2nd = PCmem[static_cast<unsigned char>(Reg_P+1)];
                
                switch ((temp >> 3) & 0x7)
                {
                    case 0:
                        if ((temp >> 6) < 3) // ADD Reg, (Mem)
                        {
                            int result = PCmem[temp >> 6] + PCmem[Byte2nd];
                            PCmem[129 + (temp >> 6)] = 0;
                            
                            if (result > 255)
                                PCmem[129 + (temp >> 6)] |= 0x2;
                            
                            if (result > 127)
                                PCmem[129 + (temp >> 6)] |= 0x1;
                            
                            PCmem[temp >> 6] = static_cast<unsigned char>(result);
                        }
                        else // OR A, (Mem)
                            Reg_A |= PCmem[Byte2nd];
                        
                        Reg_P += 2;
                    break;
                    case 1:
                        if ((temp >> 6) < 3) // SUB Reg, (Mem)
                        {
                            int result = PCmem[temp >> 6] - PCmem[Byte2nd];
                            PCmem[129 + (temp >> 6)] = 0;
                            
                            if (result > 255)
                                PCmem[129 + (temp >> 6)] |= 0x2;
                            
                            if (result < -128 || result > 127)
                                PCmem[129 + (temp >> 6)] |= 0x1;
                            
                            PCmem[temp >> 6] = static_cast<unsigned char>(result);
                        }
                        // else // 2-Byte Noop
                        
                        Reg_P += 2;
                    break;
                    case 2:
                        if ((temp >> 6) < 3) // LOAD Reg, (Mem)
                            PCmem[temp >> 6] = PCmem[Byte2nd];
                        else // AND A, (Mem)
                            Reg_A &= PCmem[Byte2nd];
                        
                        Reg_P += 2;
                    break;
                    case 3:
                        if ((temp >> 6) < 3) // STORE Reg, (Mem)
                            PCmem[Byte2nd] = PCmem[temp >> 6];
                        else // LNEG A, (Mem)
                            Reg_A = -PCmem[Byte2nd];
                        
                        Reg_P += 2;
                    break;
                    case 4: // JPD
                        if ((temp >> 6) == 3)
                            Reg_P = Byte2nd;
                        else
                        {
                            if (PCmem[temp >> 6] == 0)
                                Reg_P = Byte2nd;
                            else
                                Reg_P += 2;
                        }
                    break;
                    case 5: // JPI
                        if ((temp >> 6) == 3)
                            Reg_P = PCmem[Byte2nd];
                        else
                        {
                            if (PCmem[temp >> 6] == 0)
                                Reg_P = PCmem[Byte2nd];
                            else
                                Reg_P += 2;
                        }
                    break;
                    case 6: // JMD
                        if ((temp >> 6) == 3)
                        {
                            PCmem[Byte2nd] = Reg_P + 2;
                            Reg_P = Byte2nd + 1;
                        }
                        else
                        {
                            if (PCmem[temp >> 6] == 0)
                            {
                                PCmem[Byte2nd] = Reg_P + 2;
                                Reg_P = Byte2nd + 1;
                            }
                            else
                                Reg_P += 2;
                        }
                    break;
                    case 7: // JMI
                        if ((temp >> 6) == 3)
                        {
                            PCmem[PCmem[Byte2nd]] = Reg_P + 2;
                            Reg_P = PCmem[Byte2nd] + 1;
                        }
                        else
                        {
                            if (PCmem[temp >> 6] == 0)
                            {
                                PCmem[PCmem[Byte2nd]] = Reg_P + 2;
                                Reg_P = PCmem[Byte2nd] + 1;
                            }
                            else
                                Reg_P += 2;
                        }
                    break;
                }
            break;
            case 5:
                Byte2nd = PCmem[static_cast<unsigned char>(Reg_P+1)];
                
                switch ((temp >> 3) & 0x7)
                {
                    case 0:
                        if ((temp >> 6) < 3) // ADD Reg, ((Mem))
                        {
                            int result = PCmem[temp >> 6] + PCmem[PCmem[Byte2nd]];
                            PCmem[129 + (temp >> 6)] = 0;
                            
                            if (result > 255)
                                PCmem[129 + (temp >> 6)] |= 0x2;
                            
                            if (result > 127)
                                PCmem[129 + (temp >> 6)] |= 0x1;
                            
                            PCmem[temp >> 6] = static_cast<unsigned char>(result);
                        }
                        else // OR A, ((Mem))
                            Reg_A |= PCmem[PCmem[Byte2nd]];
                        
                        Reg_P += 2;
                    break;
                    case 1:
                        if ((temp >> 6) < 3) // SUB Reg, ((Mem))
                        {
                            int result = PCmem[temp >> 6] - PCmem[PCmem[Byte2nd]];
                            PCmem[129 + (temp >> 6)] = 0;
                            
                            if (result > 255)
                                PCmem[129 + (temp >> 6)] |= 0x2;
                            
                            if (result < -128 || result > 127)
                                PCmem[129 + (temp >> 6)] |= 0x1;
                            
                            PCmem[temp >> 6] = static_cast<unsigned char>(result);
                        }
                        // else // 2-Byte Noop
                        
                        Reg_P += 2;
                    break;
                    case 2:
                        if ((temp >> 6) < 3) // LOAD Reg, ((Mem))
                            PCmem[temp >> 6] = PCmem[PCmem[Byte2nd]];
                        else // AND A, ((Mem))
                            Reg_A &= PCmem[PCmem[Byte2nd]];
                        
                        Reg_P += 2;
                    break;
                    case 3:
                        if ((temp >> 6) < 3) // STORE Reg, ((Mem))
                            PCmem[PCmem[Byte2nd]] = PCmem[temp >> 6];
                        else // LNEG A, ((Mem))
                            Reg_A = -PCmem[PCmem[Byte2nd]];
                        
                        Reg_P += 2;
                    break;
                    case 4: // JPD
                        if ((temp >> 6) == 3)
                            Reg_P = Byte2nd;
                        else
                        {
                            if (PCmem[temp >> 6] & 0x80)
                                Reg_P = Byte2nd;
                            else
                                Reg_P += 2;
                        }
                    break;
                    case 5: // JPI
                        if ((temp >> 6) == 3)
                            Reg_P = PCmem[Byte2nd];
                        else
                        {
                            if (PCmem[temp >> 6] & 0x80)
                                Reg_P = PCmem[Byte2nd];
                            else
                                Reg_P += 2;
                        }
                    break;
                    case 6: // JMD
                        if ((temp >> 6) == 3)
                        {
                            PCmem[Byte2nd] = Reg_P + 2;
                            Reg_P = Byte2nd + 1;
                        }
                        else
                        {
                            if (PCmem[temp >> 6] & 0x80)
                            {
                                PCmem[Byte2nd] = Reg_P + 2;
                                Reg_P = Byte2nd + 1;
                            }
                            else
                                Reg_P += 2;
                        }
                    break;
                    case 7: // JMI
                        if ((temp >> 6) == 3)
                        {
                            PCmem[PCmem[Byte2nd]] = Reg_P + 2;
                            Reg_P = PCmem[Byte2nd] + 1;
                        }
                        else
                        {
                            if (PCmem[temp >> 6] & 0x80)
                            {
                                PCmem[PCmem[Byte2nd]] = Reg_P + 2;
                                Reg_P = PCmem[Byte2nd] + 1;
                            }
                            else
                                Reg_P += 2;
                        }
                    break;
                }
            break;
            case 6:
                Byte2nd = PCmem[static_cast<unsigned char>(Reg_P+1)];
                
                switch ((temp >> 3) & 0x7)
                {
                    case 0:
                        if ((temp >> 6) < 3) // ADD Reg, (Mem + X)
                        {
                            int result = PCmem[temp >> 6] + PCmem[Byte2nd + Reg_X];
                            PCmem[129 + (temp >> 6)] = 0;
                            
                            if (result > 255)
                                PCmem[129 + (temp >> 6)] |= 0x2;
                            
                            if (result > 127)
                                PCmem[129 + (temp >> 6)] |= 0x1;
                            
                            PCmem[temp >> 6] = static_cast<unsigned char>(result);
                        }
                        else // OR A, (Mem + X)
                            Reg_A |= PCmem[Byte2nd + Reg_X];
                        
                        Reg_P += 2;
                    break;
                    case 1:
                        if ((temp >> 6) < 3) // SUB Reg, (Mem + X)
                        {
                            int result = PCmem[temp >> 6] - PCmem[Byte2nd + Reg_X];
                            PCmem[129 + (temp >> 6)] = 0;
                            
                            if (result > 255)
                                PCmem[129 + (temp >> 6)] |= 0x2;
                            
                            if (result < -128 || result > 127)
                                PCmem[129 + (temp >> 6)] |= 0x1;
                            
                            PCmem[temp >> 6] = static_cast<unsigned char>(result);
                        }
                        // else // 2-Byte Noop
                        
                        Reg_P += 2;
                    break;
                    case 2:
                        if ((temp >> 6) < 3) // LOAD Reg, (Mem + X)
                            PCmem[temp >> 6] = PCmem[Byte2nd + Reg_X];
                        else // AND A, (Mem + X)
                            Reg_A &= PCmem[Byte2nd + Reg_X];
                        
                        Reg_P += 2;
                    break;
                    case 3:
                        if ((temp >> 6) < 3) // STORE Reg, (Mem + X)
                            PCmem[Byte2nd + Reg_X] = PCmem[temp >> 6];
                        else // LNEG A, (Mem + X)
                            Reg_A = -PCmem[Byte2nd + Reg_X];
                        
                        Reg_P += 2;
                    break;
                    case 4: // JPD
                        if ((temp >> 6) == 3)
                            Reg_P = Byte2nd;
                        else
                        {
                            if (!(PCmem[temp >> 6] & 0x80))
                                Reg_P = Byte2nd;
                            else
                                Reg_P += 2;
                        }
                    break;
                    case 5: // JPI
                        if ((temp >> 6) == 3)
                            Reg_P = PCmem[Byte2nd];
                        else
                        {
                            if (!(PCmem[temp >> 6] & 0x80))
                                Reg_P = PCmem[Byte2nd];
                            else
                                Reg_P += 2;
                        }
                    break;
                    case 6: // JMD
                        if ((temp >> 6) == 3)
                        {
                            PCmem[Byte2nd] = Reg_P + 2;
                            Reg_P = Byte2nd + 1;
                        }
                        else
                        {
                            if (!(PCmem[temp >> 6] & 0x80))
                            {
                                PCmem[Byte2nd] = Reg_P + 2;
                                Reg_P = Byte2nd + 1;
                            }
                            else
                                Reg_P += 2;
                        }
                    break;
                    case 7: // JMI
                        if ((temp >> 6) == 3)
                        {
                            PCmem[PCmem[Byte2nd]] = Reg_P + 2;
                            Reg_P = PCmem[Byte2nd] + 1;
                        }
                        else
                        {
                            if (!(PCmem[temp >> 6] & 0x80))
                            {
                                PCmem[PCmem[Byte2nd]] = Reg_P + 2;
                                Reg_P = PCmem[Byte2nd] + 1;
                            }
                            else
                                Reg_P += 2;
                        }
                    break;
                }
            break;
            case 7:
                Byte2nd = PCmem[static_cast<unsigned char>(Reg_P+1)];
                
                switch ((temp >> 3) & 0x7)
                {
                    case 0:
                        if ((temp >> 6) < 3) // ADD Reg, ((Mem) + X)
                        {
                            int result = PCmem[temp >> 6] + PCmem[PCmem[Byte2nd] + Reg_X];
                            PCmem[129 + (temp >> 6)] = 0;
                            
                            if (result > 255)
                                PCmem[129 + (temp >> 6)] |= 0x2;
                            
                            if (result > 127)
                                PCmem[129 + (temp >> 6)] |= 0x1;
                            
                            PCmem[temp >> 6] = static_cast<unsigned char>(result);
                        }
                        else // OR A, ((Mem) + X)
                            Reg_A |= PCmem[PCmem[Byte2nd] + Reg_X];
                        
                        Reg_P += 2;
                    break;
                    case 1:
                        if ((temp >> 6) < 3) // SUB Reg, ((Mem) + X)
                        {
                            int result = PCmem[temp >> 6] - PCmem[PCmem[Byte2nd] + Reg_X];
                            PCmem[129 + (temp >> 6)] = 0;
                            
                            if (result > 255)
                                PCmem[129 + (temp >> 6)] |= 0x2;
                            
                            if (result < -128 || result > 127)
                                PCmem[129 + (temp >> 6)] |= 0x1;
                            
                            PCmem[temp >> 6] = static_cast<unsigned char>(result);
                        }
                        // else // 2-Byte Noop
                        
                        Reg_P += 2;
                    break;
                    case 2:
                        if ((temp >> 6) < 3) // LOAD Reg, ((Mem) + X)
                            PCmem[temp >> 6] = PCmem[PCmem[Byte2nd] + Reg_X];
                        else // AND A, ((Mem) + X)
                            Reg_A &= PCmem[PCmem[Byte2nd] + Reg_X];
                        
                        Reg_P += 2;
                    break;
                    case 3:
                        if ((temp >> 6) < 3) // STORE Reg, ((Mem) + X)
                            PCmem[PCmem[Byte2nd] + Reg_X] = PCmem[temp >> 6];
                        else // LNEG A, ((Mem) + X)
                            Reg_A = -PCmem[PCmem[Byte2nd] + Reg_X];
                        
                        Reg_P += 2;
                    break;
                    case 4: // JPD
                        if ((temp >> 6) == 3)
                            Reg_P = Byte2nd;
                        else
                        {
                            if (!(PCmem[temp >> 6] & 0x80) && PCmem[temp >> 6] != 0)
                                Reg_P = Byte2nd;
                            else
                                Reg_P += 2;
                        }
                    break;
                    case 5: // JPI
                        if ((temp >> 6) == 3)
                            Reg_P = PCmem[Byte2nd];
                        else
                        {
                            if (!(PCmem[temp >> 6] & 0x80) && PCmem[temp >> 6] != 0)
                                Reg_P = PCmem[Byte2nd];
                            else
                                Reg_P += 2;
                        }
                    break;
                    case 6: // JMD
                        if ((temp >> 6) == 3)
                        {
                            PCmem[Byte2nd] = Reg_P + 2;
                            Reg_P = Byte2nd + 1;
                        }
                        else
                        {
                            if (!(PCmem[temp >> 6] & 0x80) && PCmem[temp >> 6] != 0)
                            {
                                PCmem[Byte2nd] = Reg_P + 2;
                                Reg_P = Byte2nd + 1;
                            }
                            else
                                Reg_P += 2;
                        }
                    break;
                    case 7: // JMI
                        if ((temp >> 6) == 3)
                        {
                            PCmem[PCmem[Byte2nd]] = Reg_P + 2;
                            Reg_P = PCmem[Byte2nd] + 1;
                        }
                        else
                        {
                            if (!(PCmem[temp >> 6] & 0x80) && PCmem[temp >> 6] != 0)
                            {
                                PCmem[PCmem[Byte2nd]] = Reg_P + 2;
                                Reg_P = PCmem[Byte2nd] + 1;
                            }
                            else
                                Reg_P += 2;
                        }
                    break;
                }
            break;
        }
    }
    
    if (Run)
        bmpLEDs = (bmpLEDs & 0xFFFFFF00) | Reg_Output;
    else
    {
        if ((bmpLEDs >> static_cast<int>(LED8)) & 0x1)
            bmpLEDs = (bmpLEDs & 0xFFFFFF00) | Reg_Input;
        else
            bmpLEDs = (bmpLEDs & 0xFFFFFF00) | Reg_Output;
    }
    
    return static_cast<int>(Run);
}

} // namespace DiLa



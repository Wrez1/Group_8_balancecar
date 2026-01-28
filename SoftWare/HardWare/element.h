#ifndef _ELEMENT_H__
#define _ELEMENT_H__

#include "zf_common_headfile.h"

void Element_Normal(void);
void Element_Process(void);
void Element_Ring(void);
void Element_Noline(void);
void Element_Stop(void);
void Element_Corner(void);
void Element_Ten(void);
void Element_Straight();
extern float kL,kR;
extern uint8 Element_Flag,Ring_Flag,Noline_Flag,Stop_Flag,Ten_Flag;
#endif

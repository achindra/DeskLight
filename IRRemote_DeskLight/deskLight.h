#ifndef __DESK_LIGHT_H__
#define __DESK_LIGHT_H__

#pragma once

extern unsigned const int CMD_ON        = 0xF7C03F;
extern unsigned const int CMD_OFF       = 0xF740BF;
extern unsigned const int CMD_STROBE    = 0xF7F00F;
extern unsigned const int CMD_DIM       = 0xF7807F;
extern unsigned const int CMD_NIGHT_ON  = 0xFF0011;
extern unsigned const int CMD_NIGHT_OFF = 0xFF0000;

unsigned const int CMD_RED       = 0xF720DF;
unsigned const int CMD_WHITE     = 0xF7E01F;
unsigned const int CMD_DIM       = 0xF7807F;

#endif __DESK_LIGHT_H__

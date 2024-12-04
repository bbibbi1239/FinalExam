#ifndef ELF2D_H
#define ELF2D_H

void Elf2DClearScreen(char* Buffer, int width, int height);

void Elf2DDrawBuffer(char* Buffer);

void Elf2DDrawLine(int x1, int y1, int x2, int y2, char* Buffer, int width, int height, const char* symbol);

void Elf2DInitScreen();

void Elf2DSleep(int ms);

#endif
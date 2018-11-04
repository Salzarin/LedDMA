#include <math.h>
#ifndef COLOR_H
#define COLOR_H
typedef struct HSL{
	float H;
	float S;
	float L;
}HSL;
void RGBtoHSL(unsigned int color, HSL* hsl);
unsigned int HSLtoRGB(HSL hsl);
unsigned int interpolateColor(HSL fromHSL, HSL toHSL, float increment, float step);

void HSLset(HSL* dest, HSL* source);
#endif
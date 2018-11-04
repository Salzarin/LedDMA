#include <math.h>

typedef struct HSL{
	float H;
	float S;
	float L;
}HSL;
void RGBtoHSL(unsigned int color, HSL* hsl);
unsigned int HSLtoRGB(HSL hsl);
unsigned int interpolateColor(HSL fromHSL, HSL toHSL, float increment, float step, HSL * hsl);

void HSLset(HSL* dest, HSL* source){
	dest.H = source.H;
	dest.S = source.S;
	dest.L = source.L;

}
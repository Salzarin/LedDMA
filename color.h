#include <math.h>

typedef struct HSL{
	float H;
	float S;
	float L;
}HSL;
void RGBtoHSL(unsigned int color, float * H, float * S, float * L);
unsigned int HSLtoRGB(float H, float S, float L);
unsigned int interpolateColor(HSL fromHSL, HSL toHSL, float increment, float step);
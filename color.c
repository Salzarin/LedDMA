#include "color.h"





unsigned int interpolateColor(HSL fromHSL, HSL toHSL, float increment, float step){

HSL new;
new.H = fromHSL.H + step*(toHSL.H - fromHSL.H)/increment;
new.S = fromHSL.S + step*(toHSL.S - fromHSL.S)/increment;
new.L = fromHSL.L + step*(toHSL.L - fromHSL.L)/increment;

return HSLtoRGB(new);
}
unsigned int HSLtoRGB(HSL hsl){
	float C = (1 - fabs(2*hsl.L-1))*S;
	float X = C*(1 - fabs(fmod(hsl.H/60, 2)-1));
	float m = hsl.L -C/2;
	
	int type = (int)(H/60.0);
	type = type % 6;
	float rgb[0];
	switch(type){
		case 0:
			rgb[0] = C;
			rgb[1] = X;
			rgb[2] = 0;
			break;
		case 1:
			rgb[0] = X;
			rgb[1] = C;
			rgb[2] = 0;
			break;
			case 2:
			rgb[0] = 0;
			rgb[1] = C;
			rgb[2] = X;
			break;
		case 3:
			rgb[0] = 0;
			rgb[1] = X;
			rgb[2] = C;
			break;
		case 4:
			rgb[0] = X;
			rgb[1] = 0;
			rgb[2] = C;
			break;
		case 5:
			rgb[0] = C;
			rgb[1] = 0;
			rgb[2] = X;
			break;
	}
	
	unsigned char r = (rgb[0]+m)*255;
	unsigned char g = (rgb[1]+m)*255;
	unsigned char b = (rgb[2]+m)*255;
	return (r<<16 | g<<8 | b);
	
}

void RGBtoHSL(unsigned int color, HSL hsl){
	float r = (color >> 16) & 0xFF;
	float g = (color >> 8) & 0xFF;
	float b = color & 0xFF;
	
	r = r/255.0;
	g = g/255.0;
	b = b/255.0;
	
	float rgb[3] = {r,g,b};
	
	float cMax = r;
	float cMin = r;
	int tMax = 0;
	int tMin = 4;
	for(int i = 0; i<3;i++){
		tMax = rgb[i]>cMax ? i : tMax;
		cMax = rgb[i]>cMax ? rgb[i] : cMax;
		tMin = rgb[i]<cMin ? i : tMin;
		cMin = rgb[i]<cMin ? rgb[i] : cMin;		
	}
	
	float delta = cMax-cMin;
	hsl->L = (cMax+cMin)/2;
	
	
	if( delta == 0){
		hsl->H = 0;
		hsl->S = 0;
	}
	else{
		switch(tMax){
			case 0:
				hsl->H = 60 *fmodf(((g-b)/delta),6);
				break;
			case 1:
				hsl->H = 60 *(((b-r)/delta)+2);
				break;
			case 2:
				hsl->H = 60 *(((r-g)/delta)+4);
				break;	
		}
		hsl->S = delta/(1.0 - fabs(2*(*L)-1));
	}
	
	hsl->H = (hsl->H)<0?(hsl->H)+360:(hsl->H);
}


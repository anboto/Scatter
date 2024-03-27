// SPDX-License-Identifier: Apache-2.0
// Copyright 2021 - 2022, the Anboto author and contributors
#include "ScatterDraw.h"

namespace Upp {

ScatterDraw& ScatterDraw::AddSurf(DataSourceSurf &_surf) {
	surf = &_surf;
	if (!IsNum(surfMinZ))
		surfMinZ = _surf.MinZ();
	if (!IsNum(surfMaxZ))
		surfMaxZ = _surf.MaxZ();
	return *this;
}

inline static double Smooth01(double val) {
	double val2 = val*val;
	return 3*val2 - 2*val2*val;
}

inline static double Smooth01Left(double val)  {return 2*Smooth01(val/2);}
inline static double Smooth01Right(double val) {return 2*(Smooth01(0.5 + val/2)-0.5);}

inline static double Filter01(double frac) {
	if (frac >= 1)
		return 1;
	else if (frac < 0)
		return 0;
	return frac;
}

static Color InterpolateColor(double frac, const Color &from, const Color &to) {
	return Color(int(from.GetR() + frac*(to.GetR() - from.GetR())), 
				 int(from.GetG() + frac*(to.GetG() - from.GetG())),
				 int(from.GetB() + frac*(to.GetB() - from.GetB())));
}

Color GetRainbowColor(double frac, RAINBOW rainbow, int numScales) {
	frac = Filter01(frac);
	switch (rainbow) { 
	case BLUE_YELLOW_RED:	frac = 1 - frac;
							return GetRainbowColor(frac, Color(255,0,0), Color(255,255,0), Color(0,0,255), numScales);
	case RED_YELLOW_BLUE:	return GetRainbowColor(frac, Color(255,0,0), Color(255,255,0), Color(0,0,255), numScales);
	case GREEN_YELLOW_RED:	frac = 1 - frac;
							return GetRainbowColor(frac, Color(255,0,0), Color(255,255,0), Color(0,255,0), numScales);
	case RED_YELLOW_GREEN:	return GetRainbowColor(frac, Color(255,0,0), Color(255,255,0), Color(0,255,0), numScales);
	case WHITE_BLACK:		frac = 1 - frac;
							return GetRainbowColor(frac, Color(0,0,0), Color(255,255,255), numScales);
	case BLACK_WHITE:		return GetRainbowColor(frac, Color(0,0,0), Color(255,255,255), numScales);
	case BLUE_WHITE_RED:	frac = 1 - frac;
							return GetRainbowColor(frac, Color(255,0,0), Color(255,255,255), Color(0,0,255), numScales);
	case RED_WHITE_BLUE:	return GetRainbowColor(frac, Color(255,0,0), Color(255,255,255), Color(0,0,255), numScales);
	}
	return Null;
}

Color GetRainbowColor(double frac, const Color &from, const Color &to, int numScales) {
	if (numScales > 0)
		frac = (int(frac*numScales)/double(numScales))/(1 - 1/numScales);
	frac = Filter01(frac);
	return InterpolateColor(frac, from, to);
}

Color GetRainbowColor(double frac, const Color &col0, const Color &col1, const Color &col2, int numScales) {
	ASSERT(frac >= 0 && frac <= 1);
	if (IsNull(col2)) 
		return GetRainbowColor(frac, col0, col1, numScales);
	if (numScales > 0)
		frac = int(frac*numScales)/double(numScales - 1);
	frac = Filter01(frac);
	if (frac < 0.5) 
		return InterpolateColor((numScales > 0) ? 2*frac : Smooth01Right(2*frac), col0, col1);
	else
		return InterpolateColor((numScales > 0) ? 2*(frac - 0.5) : Smooth01Left(2*(frac - 0.5)), col1, col2);
}
	

#define Membercall(fun)	(this->*fun)

int TableData::get_axis_index_area_no_interp(Getdatafun getdataAxis, int lenAxis, double x) {
	int ix;
	double mn = Membercall(getdataAxis)(0);
	double mx = Membercall(getdataAxis)(lenAxis-1);
	if (x < mn || x >= mx) 
		return -1;
	
	if (lenAxis > 30) { // Not sure what is the best threshold here.
		// Binary search with "smart" initial guess.
		double stride_guess = (mx - mn) / (double)(lenAxis-1);
		ix = (int)((x - mn) / stride_guess);
		int dist = std::max(1, (lenAxis-1)/2);
		do {
			ix = std::max<int>(ix, 0);
			ix = std::min<int>(ix, lenAxis-2);
			double x_guess = Membercall(getdataAxis)(ix);
			if(x_guess > x) {
				if((ix == 0) || x >= Membercall(getdataAxis)(ix-1)) {
					ix--;
					break;
				}
				ix-=dist;
			} else if (x_guess <= x) {
				if((ix == lenAxis-2) || x < Membercall(getdataAxis)(ix+1)) break;
				ix+=dist;
			}
			dist = std::max(1, dist/2);
		} while (true); // Note: The above code will exit always.
		ix = std::max<int>(ix, 0);
		ix = std::min<int>(ix, lenAxis-2);
	} else {
		// Linear search
		for (ix = 0; ix < lenAxis-2; ++ix) {
			if (Membercall(getdataAxis)(ix+1) > x) 
				break;
		}
	}
	return ix;
}
	
double TableData::z_area(Getdatafun getdataX, Getdatafun getdataY, Getdatafun getdataZ, 
						double x, double y) {
	int width = rowMajor ? lenxAxis - 1 : lenyAxis - 1;
	if (inter == NO) {
		int ix = get_axis_index_area_no_interp(getdataX, lenxAxis, x);
		if (ix < 0) 
			return Null;
		int iy = get_axis_index_area_no_interp(getdataY, lenyAxis, y);
		if (iy < 0) 
			return Null;
		if (rowMajor)
			return Membercall(getdataZ)(ix 		 + iy*width);
		else 
			return Membercall(getdataZ)(ix*width + iy);
	} else if (inter == BILINEAR) {
		int ix, iy;
		for (ix = 0; ix < lenxAxis-1; ++ix) {
			if (((Membercall(getdataX)(ix) + Membercall(getdataX)(ix+1))/2.) > x) {
				if (ix == 0) 
					return Null;
				else {
					ix--;
					break;
				}
			}
		}
		if (ix == lenxAxis-1)
			return Null;
		for (iy = 0; iy < lenyAxis-1; ++iy) {
			if (((Membercall(getdataY)(iy)+Membercall(getdataY)(iy+1))/2.) > y) {
				if (iy == 0) 
					return Null;
				else {
					iy--;
					break;
				}
			}
		}
		if (iy == lenyAxis-1)
			return Null;
		
		double x1 = (Membercall(getdataX)(ix) + Membercall(getdataX)(ix+1))/2.;
		double x2 = (Membercall(getdataX)(ix+1) + Membercall(getdataX)(ix+2))/2.;
		double y1 = (Membercall(getdataY)(iy) + Membercall(getdataY)(iy+1))/2.;
		double y2 = (Membercall(getdataY)(iy+1) + Membercall(getdataY)(iy+2))/2.;
		double z11, z12, z21, z22;
		if (rowMajor) {
			z11 = Membercall(getdataZ)(ix + iy*width);
			z12 = Membercall(getdataZ)(ix + (iy+1)*width);
			z21 = Membercall(getdataZ)(ix+1+ iy*width);
			z22 = Membercall(getdataZ)(ix+1 + (iy+1)*width);
		} else {
			z11 = Membercall(getdataZ)(ix*width 	+ iy);
			z12 = Membercall(getdataZ)(ix*width     + (iy+1));
			z21 = Membercall(getdataZ)((ix+1)*width + iy);
			z22 = Membercall(getdataZ)((ix+1)*width + (iy+1));
		}
		if (!IsNum(z11) || !IsNum(z12) || !IsNum(z21) || !IsNum(z22))
			return Null;
		return BilinearInterpolate(x, y, x1, x2, y1, y2, z11, z12, z21, z22);
	} else
		return Null;
}
	
double TableData::z_point(Getdatafun getdataX, Getdatafun getdataY, Getdatafun getdataZ, 
						double x, double y) {
	if (x < MinX(getdataX) || x > MaxX(getdataX) ||
		y < MinY(getdataY) || y > MaxY(getdataY))
		return Null;
	
	int width = rowMajor ? lenxAxis : lenyAxis;

	if (inter == NO) {
		int ix, iy;
		if (x < (Membercall(getdataX)(0) + Membercall(getdataX)(1))/2.)
			ix = 0;
		else if (x >= (Membercall(getdataX)(lenxAxis-1) + Membercall(getdataX)(lenxAxis-2))/2.)
			ix = lenxAxis-1;
		else {
			for (ix = 1; ix < lenxAxis-1; ++ix) {
				if ((Membercall(getdataX)(ix) + Membercall(getdataX)(ix+1))/2. > x) 
					break;
			}
		}
		if (y < (Membercall(getdataY)(0) + Membercall(getdataY)(1))/2.)
			iy = 0;
		else if (y >= (Membercall(getdataY)(lenyAxis-1) + Membercall(getdataY)(lenyAxis-2))/2.)
			iy = lenyAxis-1;
		else {
			for (iy = 1; iy < lenyAxis-1; ++iy) {
				if ((Membercall(getdataY)(iy) + Membercall(getdataY)(iy+1))/2. > y) 
					break;
			}
		}
		if (rowMajor)
			return Membercall(getdataZ)(ix 			+ iy*width);
		else
			return Membercall(getdataZ)(ix*width + iy);
	} else if (inter == BILINEAR) {
		int ix, iy;
		for (ix = 0; ix < lenxAxis-1; ++ix) {
			if (Membercall(getdataX)(ix+1) >= x) 
				break;
		}
		if (ix == lenxAxis-1)
			return Null;
		for (iy = 0; iy < lenyAxis-1; ++iy) {
			if (Membercall(getdataY)(iy+1) >= y) 
				break;
		}
		if (iy == lenyAxis-1)
			return Null;
		
		double x1 = Membercall(getdataX)(ix);
		double x2 = Membercall(getdataX)(ix+1);
		double y1 = Membercall(getdataY)(iy);
		double y2 = Membercall(getdataY)(iy+1);
		double z11, z12, z21, z22;
		if (rowMajor) {
			z11 = Membercall(getdataZ)(ix + iy*width);
			z12 = Membercall(getdataZ)(ix + (iy+1)*width);
			z21 = Membercall(getdataZ)(ix+1 + iy*width);
			z22 = Membercall(getdataZ)(ix+1 + (iy+1)*width);
		} else {
			z11 = Membercall(getdataZ)(ix*width 	+ iy);
			z12 = Membercall(getdataZ)(ix*width 	+ (iy+1));
			z21 = Membercall(getdataZ)((ix+1)*width + iy);
			z22 = Membercall(getdataZ)((ix+1)*width + (iy+1));
		}
		if (!IsNum(z11) || !IsNum(z12) || !IsNum(z21) || !IsNum(z22))
			return Null;
		return BilinearInterpolate(x, y, x1, x2, y1, y2, z11, z12, z21, z22);
	} else
		return Null;
}

double TableData::z_data(Getdatafun getdataX, Getdatafun getdataY, Getdatafun getdataZ, 
						int ix, int iy) {
	int width = rowMajor ? lenxAxis : lenyAxis;
	if (areas) 
		width--;				
	if (rowMajor)
		return Membercall(getdataZ)(ix 		 + iy*width);
	else
		return Membercall(getdataZ)(ix*width + iy);													
}
							
double TableData::MinX(Getdatafun getdata) {
	return Membercall(getdata)(0);
}

double TableData::MaxX(Getdatafun getdata) {
	return Membercall(getdata)(lenxAxis-1);
}

double TableData::MinY(Getdatafun getdata) {
	return Membercall(getdata)(0);
}

double TableData::MaxY(Getdatafun getdata) {
	return Membercall(getdata)(lenyAxis-1);
}

double TableData::MinZ(Getdatafun getdata) {
	double ret = -DOUBLE_NULL;
	for (int i = 0; i < lendata; ++i) {
		double d = Membercall(getdata)(i);
		if (!!IsNum(d)) {
			if (ret > d)
				ret = d;
		}
	}
	if (ret == -DOUBLE_NULL)
		return Null;
	return ret;
}

double TableData::MaxZ(Getdatafun getdata) {
	double ret = DOUBLE_NULL;
	for (int i = 0; i < lendata; ++i) {
		double d = Membercall(getdata)(i);
		if (!!IsNum(d)) {
			if (ret < d)
				ret = d;
		}
	}
	if (ret == DOUBLE_NULL)
		return Null;
	return ret;
}

}
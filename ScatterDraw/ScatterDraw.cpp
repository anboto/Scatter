// SPDX-License-Identifier: Apache-2.0
// Copyright 2021 - 2022, the Anboto author and contributors
#include "ScatterDraw.h"

#include <Functions4U/EnableWarnings.h>

namespace Upp {

ScatterDraw::ScatterDraw() {
	lastxRange = xRange;
	lastyRange = yRange;
}
/*
void debug_h() {
	int dummy = 0;		// It does nothing. It just serves as a place to set a breakpoint for templated functions
}
*/
ScatterDraw& ScatterDraw::SetColor(const Color& _color) {
	graphColor = _color;
	return *this;
}

ScatterDraw& ScatterDraw::SetTitle(const String& _title) {
	title = _title;
	return *this;
}

const String& ScatterDraw::GetTitle() {
	return title;	
}

ScatterDraw& ScatterDraw::SetTitleFont(const Font& fontTitle) {
	titleFont = fontTitle;
	return *this;
}

ScatterDraw& ScatterDraw::SetTitleColor(const Color& colorTitle) {
	titleColor = colorTitle;
	return *this;
}

ScatterDraw& ScatterDraw::SetLabels(const String& _xLabel, const String& _yLabel, const String& _yLabel2) {
	xLabel_base = _xLabel;
	yLabel_base = _yLabel;
	yLabel2_base = _yLabel2;
	labelsChanged = true;
	return *this;	
}

ScatterDraw& ScatterDraw::SetLabelX(const String& _xLabel) {
	xLabel_base = _xLabel;
	labelsChanged = true;
	return *this;
}

ScatterDraw& ScatterDraw::SetLabelY(const String& _yLabel) {
	yLabel_base = _yLabel;
	labelsChanged = true;
	return *this;
}

ScatterDraw& ScatterDraw::SetLabelY2(const String& _yLabel) {
	yLabel2_base = _yLabel;
	labelsChanged = true;
	return *this;
}

ScatterDraw& ScatterDraw::SetLabelsFont(const Font& fontLabels) {
	labelsFont = fontLabels;
	return *this;
}

ScatterDraw& ScatterDraw::SetLabelsColor(const Color& colorLabels) {
	labelsColor = colorLabels;
	return *this;
}

ScatterDraw& ScatterDraw::SetGridFont(const Font& fontGrid) {
	reticleFont = fontGrid;
	return *this;
}

ScatterDraw& ScatterDraw::SetPlotAreaMargin(int hLeft, int hRight, int vTop, int vBottom) {
	hPlotLeft   = hLeft;	
	hPlotRight  = hRight;
	vPlotTop    = vTop;
	vPlotBottom = vBottom;
	return *this;
}

ScatterDraw& ScatterDraw::SetPlotAreaLeftMargin(int margin) {	
	hPlotLeft = margin;	
	return *this;
}

ScatterDraw& ScatterDraw::SetPlotAreaRightMargin(int margin) {	
	hPlotRight = margin;	
	return *this;
}

ScatterDraw& ScatterDraw::SetPlotAreaTopMargin(int margin) {	
	vPlotTop = margin;	
	return *this;
}

ScatterDraw& ScatterDraw::SetPlotAreaBottomMargin(int margin) {	
	vPlotBottom = margin;	
	return *this;
}

ScatterDraw& ScatterDraw::SetPlotAreaColor(const Color& p_a_color) {
	plotAreaColor = p_a_color;        
	return *this;
}

ScatterDraw& ScatterDraw::SetAxisColor(const Color& axis_color) {
	axisColor = axis_color;
	return *this;
}

ScatterDraw& ScatterDraw::SetAxisWidth(double axis_width) {
	axisWidth = axis_width;
	return *this;
}

ScatterDraw& ScatterDraw::ShowHGrid(bool show) {
	drawHGrid = show;
	return *this;
}

ScatterDraw& ScatterDraw::ShowVGrid(bool show) {
	drawVGrid = show;
	return *this;
}

bool ScatterDraw::PointInPlot(Point &pt) {
	return 	hPlotLeft*plotScaleX <= pt.x && pt.x <= (GetSize().cx - hPlotRight*plotScaleX) && 
		  	(vPlotTop*plotScaleY + titleHeight) <= pt.y && pt.y <= (GetSize().cy - vPlotBottom*plotScaleY);
}

bool ScatterDraw::PointInBorder(Point &pt)  {
	return !PointInPlot(pt);
}

bool ScatterDraw::PointInLegend(Point &)  {
	return false;
}

void ScatterDraw::AdjustMinUnitX() {
	if (SetGridLinesX)
		return;
	xMinUnit = NumberWithLeastSignificantDigits(xMin, xMin + xRange) - xMin;
	if (xMinUnit < 0)
		xMinUnit += abs(floor(xMinUnit/xMajorUnit))*xMajorUnit;
	else if (xMinUnit - xMajorUnit >= 0)
		xMinUnit -= abs(floor(xMinUnit/xMajorUnit))*xMajorUnit;
}

void ScatterDraw::AdjustMinUnitY() {
	if (SetGridLinesY)
		return;
	yMinUnit = NumberWithLeastSignificantDigits(yMin, yMin + yRange) - yMin;
	if (yMinUnit < 0)
		yMinUnit += abs(floor(yMinUnit/yMajorUnit))*yMajorUnit;
	else if (yMinUnit - yMajorUnit >= 0)
		yMinUnit -= abs(floor(yMinUnit/yMajorUnit))*yMajorUnit;
}

void ScatterDraw::AdjustMinUnitY2() {
	if (SetGridLinesY)
		return;
	yMinUnit2 = NumberWithLeastSignificantDigits(yMin2, yMin2 + yRange2) - yMin2;
	if (yMinUnit2 < 0)
		yMinUnit2 += abs(floor(yMinUnit2/yMajorUnit2))*yMajorUnit2;
	else if (yMinUnit2 - yMajorUnit2 >= 0)
		yMinUnit2 -= abs(floor(yMinUnit2/yMajorUnit2))*yMajorUnit2;
}

void ScatterDraw::AdjustMajorUnitX() {
	if (xRange <= 0 || xMajorUnit == 0 || !IsNum(xMajorUnit))
		return;
	xMajorUnit = GetRangeMajorUnits(xMin, xMin + xRange);
}

void ScatterDraw::AdjustMajorUnitY() {
	if (yRange <= 0 || yMajorUnit == 0 || !IsNum(yMajorUnit))
		return;
	yMajorUnit = GetRangeMajorUnits(yMin, yMin + yRange);
}

void ScatterDraw::AdjustMajorUnitY2() {
	if (yRange2 <= 0 || yMajorUnit2 == 0 || !IsNum(yMajorUnit2))
		return;
	yMajorUnit2 = GetRangeMajorUnits(yMin2, yMin2 + yRange2);
}

ScatterDraw &ScatterDraw::SetRange(double rx, double ry, double ry2) {		// Use of range is unsafe for log10
	//rx = FixLog10(rx, logX);			// No FixLog10 allowed. Its unsafe. For example, if range is from 0.0001 to 1E30, range will be 1E30...
	//ry = FixLog10(ry, logY);
	//ry2= FixLog10(ry2, logY);
	ASSERT(!IsNum(rx)  || rx  >= 0);
	ASSERT(!IsNum(ry)  || ry  >= 0);
	ASSERT(!IsNum(ry2) || ry2 >= 0);
	
	if (IsNum(rx)) {
		xRange = rx;
		AdjustMajorUnitX();
		AdjustMinUnitX();
	}
	if (IsNum(ry)) {
		yRange = ry;
		AdjustMajorUnitY(); 
		AdjustMinUnitY();
	}
	if (IsNum(ry2)) {
		yRange2 = ry2;
		AdjustMajorUnitY2();
		AdjustMinUnitY2();
	}
	WhenSetRange();
	return *this;
}

ScatterDraw &ScatterDraw::SetRange(double rxmax, double rxmin, double rymax, double rymin, double ry2max, double ry2min) {
	rxmax = FixLog10(rxmax, logX);	rxmin = FixLog10(rxmin, logX);
	rymax = FixLog10(rymax, logY);	rymin = FixLog10(rymin, logY);
	ry2max= FixLog10(ry2max, logY);	ry2min= FixLog10(ry2min, logY);
	ASSERT(!IsNum(rxmax)  || !IsNum(rxmin)  || rxmax-rxmin  >= 0);
	ASSERT(!IsNum(rymax)  || !IsNum(rymin)  || rymax-rymin  >= 0);
	ASSERT(!IsNum(ry2max)  || !IsNum(ry2min)  || ry2max-ry2min  >= 0);
	
	if (IsNum(rxmax) && IsNum(rxmax)) {
		xRange = rxmax - rxmin;
		AdjustMajorUnitX();
		AdjustMinUnitX();
	}
	if (IsNum(rymax) && IsNum(rymin)) {
		yRange = rymax - rymin;
		AdjustMajorUnitY(); 
		AdjustMinUnitY();
	}
	if (IsNum(ry2max) && IsNum(ry2min)) {
		yRange2 = ry2max - ry2min;
		AdjustMajorUnitY2();
		AdjustMinUnitY2();
	}
	WhenSetRange();
	return *this;
}

ScatterDraw &ScatterDraw::SetMajorUnitsNum(int nx, int ny) {
	if (IsNum(nx)) 
		xMajorUnit = xRange/nx;
	if (IsNum(ny)) {
		yMajorUnit = yRange/ny;
		yMajorUnit2 = yMajorUnit*yRange2/yRange;
	}
	return *this;
}

ScatterDraw &ScatterDraw::SetMajorUnits(double ux, double uy, double uy2) {
	ASSERT(!IsNum(ux)  || ux  >= 0);
	ASSERT(!IsNum(uy)  || uy  >= 0);
	ASSERT(!IsNum(uy2) || uy2 >= 0);
	
	if (IsNum(ux)) {
		xMajorUnit = ux;
		AdjustMinUnitX();
	}
	if (IsNum(uy)) {
		yMajorUnit = uy;
		yMajorUnit2 = uy*yRange2/yRange;
		AdjustMinUnitY();
		AdjustMinUnitY2();
	} else if (IsNum(uy2)) {
		yMajorUnit2 = uy2;
		yMajorUnit = uy2*yRange/yRange2;
		AdjustMinUnitY();
		AdjustMinUnitY2();
	}
	return *this;
}

ScatterDraw &ScatterDraw::SetMinUnits(double ux, double uy) {
	TimeStop t;		// Added parachute
	double maxs = maxRefresh_ms*1000;
	if (IsNum(ux)) {
		xMinUnit = xMinUnit0 = ux;
		while (xMinUnit < 0 && t.Seconds() < maxs)
			xMinUnit += xMajorUnit;
		while (xMinUnit - xMajorUnit >= 0 && t.Seconds() < maxs)
			xMinUnit -= xMajorUnit;
	}
	if (IsNum(uy)) {	
		yMinUnit = yMinUnit0 = uy;
		yMinUnit2 = yMinUnit20 = yRange2*yMinUnit/yRange;
		while (yMinUnit < 0 && t.Seconds() < maxs)
			yMinUnit += yMajorUnit;
		while (yMinUnit - yMajorUnit >= 0 && t.Seconds() < maxs)
			yMinUnit -= yMajorUnit;
		while (yMinUnit2 < 0 && t.Seconds() < maxs)
			yMinUnit2 += yMajorUnit2;
		while (yMinUnit2 - yMajorUnit2 >= 0 && t.Seconds() < maxs)
			yMinUnit2 -= yMajorUnit2;	
	}
	ASSERT(t.Seconds() < maxs);
	return *this;
}

ScatterDraw &ScatterDraw::SetXYMin(double x, double y, double y2) {
	x = FixLog10(x, logX);
	y = FixLog10(y, logY);
	y2= FixLog10(y2,logY);
	
	if (IsNum(x)) 
		xMin = x;
	if (IsNum(y)) 
		yMin = y;
	if (IsNum(y2)) 
		yMin2 = y2;

	WhenSetXYMin();
	return *this;
}

ScatterDraw &ScatterDraw::SetXYMax(double x, double y, double y2) {
	x = FixLog10(x, logX);
	y = FixLog10(y, logY);
	y2= FixLog10(y2,logY);
	
	if (IsNum(x)) 
		xRange = x - xMin;
	if (IsNum(y)) 
		yRange = y - yMin;
	if (IsNum(y2)) 
		yRange2 = y2 - yMin2;

	WhenSetXYMin();
	return *this;
}

ScatterDraw &ScatterDraw::ZoomToFit(bool horizontal, bool vertical, double factorH, double factorV) {
	if (linkedMaster) {
		linkedMaster->ZoomToFit(horizontal, vertical, factorH, factorV);
		return *this;
	}
	ZoomToFitNonLinked(horizontal, vertical, factorH, factorV);
	if (!linkedCtrls.IsEmpty()) {
		for (int i = 0; i < linkedCtrls.size(); ++i)
	    	linkedCtrls[i]->ZoomToFitNonLinked(horizontal, vertical, factorH, factorV);
	}
	WhenZoomToFit();
	return *this;
}
			
ScatterDraw &ScatterDraw::ZoomToFitNonLinked(bool horizontal, bool vertical, double factorH, double factorV) {
	double minx, maxx, miny, miny2, maxy, maxy2;
	minx = miny = miny2 = std::numeric_limits<double>::max();
	maxx = maxy = maxy2 = std::numeric_limits<double>::lowest();
	
	try {
		if (horizontal || vertical) {	// Both at the same time to avoid a Null point to set the limits
			for (int j = 0; j < series.size(); j++) {
				ScatterSeries &serie = series[j]; 
				if (serie.IsDeleted() || serie.opacity == 0 || serie.Data().IsExplicit())
					continue;
				for (int64 i = 0; i < serie.Data().size(); ++i) {
					double px = FixLog10(serie.Data().x(i), logX);
					double py = FixLog10(serie.Data().y(i), logY);
					if (!IsNum(px) || !IsNum(py)) 
						continue;
					minx = min(minx, px);
					maxx = max(maxx, px);
					if (serie.primaryY) {
						miny = min(miny, py);
						maxy = max(maxy, py);
					} else {
						miny2 = min(miny2, py);
						maxy2 = max(maxy2, py);
					}
				}
			}
			if (minx != std::numeric_limits<double>::max() && maxx != std::numeric_limits<double>::lowest()) {
				double deltaX = (maxx - minx)*factorH;
				minx -= deltaX;
				maxx += deltaX;
			}
			if (miny != std::numeric_limits<double>::max() && maxy != std::numeric_limits<double>::lowest()) {
				double deltaY = (maxy - miny)*factorV;
				miny -= deltaY;
				maxy += deltaY;		
			}
			if (miny2 != std::numeric_limits<double>::max() && maxy2 != std::numeric_limits<double>::lowest()) {
				double deltaY2 = (maxy2 - miny2)*factorV;
				miny2 -= deltaY2;
				maxy2 += deltaY2;		
			}
			if (psurf) {
				minx = min(minx, psurf->MinX());
				maxx = max(maxx, psurf->MaxX());
				miny = min(miny, psurf->MinY());
				maxy = max(maxy, psurf->MaxY());
			}
		}
		if (horizontal) {
			if (minx != std::numeric_limits<double>::max() && maxx != std::numeric_limits<double>::lowest()) {
				if (maxx == minx) {
					if (maxx == 0) {
						xRange = 2;
						xMin = -1;
					} else	
						xRange = 2*maxx;
				} else	
					xRange = maxx - minx;
				double deltaX = xMin - minx;
				xMin -= deltaX;
				
				AdjustMajorUnitX();
				AdjustMinUnitX();
			}
		}
		if (vertical) {
			if (miny != std::numeric_limits<double>::max() && maxy != std::numeric_limits<double>::lowest()) {
				if (maxy == miny) 
					yRange = maxy > 0 ? 2*maxy : 1;
				else	
					yRange = maxy - miny;
				double deltaY = yMin - miny;
				yMin -= deltaY;
				
				AdjustMajorUnitY();
				AdjustMinUnitY();
			}
			if (miny2 != std::numeric_limits<double>::max() && maxy2 != std::numeric_limits<double>::lowest()) {	
				if (maxy2 == miny2) 
					yRange2 = maxy2 > 0 ? 2*maxy2 : 1;
				else	
					yRange2 = maxy2 - miny2;
				double deltaY2 = yMin2 - miny2;
				yMin2 -= deltaY2;
				
				AdjustMajorUnitY2();
				AdjustMinUnitY2();
			}
		}	
	} catch (ValueTypeError err) {
		ASSERT_(true, err);
		return *this;
	}
	WhenSetRange();
	WhenSetXYMin();
	Refresh();
	return *this;
}

ScatterDraw &ScatterDraw::Graduation_FormatX(Formats fi) {
	switch (fi) {
		case EXP: cbModifFormatX = cbModifFormatXGridUnits = ExpFormat;	break;
		case MON: cbModifFormatX = cbModifFormatXGridUnits = MonFormat;	break;
		case DY:  cbModifFormatX = cbModifFormatXGridUnits = DyFormat;	break;
		default:  break;
	}
	return *this;
}

ScatterDraw &ScatterDraw::Graduation_FormatY(Formats fi) {
	switch (fi) {
		case EXP: cbModifFormatY = cbModifFormatYGridUnits = ExpFormat;	break;
		case MON: cbModifFormatY = cbModifFormatYGridUnits = MonFormat;	break;
		case DY:  cbModifFormatY = cbModifFormatYGridUnits = DyFormat;	break;
		default:  break;
	}
	return *this;
}

ScatterDraw &ScatterDraw::Graduation_FormatY2(Formats fi) {
	switch (fi) {
		case EXP: cbModifFormatY2 = cbModifFormatY2GridUnits = ExpFormat;	break;
		case MON: cbModifFormatY2 = cbModifFormatY2GridUnits = MonFormat;	break;
		case DY:  cbModifFormatY2 = cbModifFormatY2GridUnits = DyFormat;	break;
		default:  break;
	}
	return *this;
}

Color ScatterDraw::GetNewColor(int index, int version) {
	static Color oldc[20] = {LtBlue(), LtRed(), LtGreen(), Black(), LtGray(), Brown(), Blue(), Red(), Green(), Gray(), 
			LtBlue(), LtRed(), LtGreen(), Black(), LtGray(), Brown(), Blue(), Red(), Green(), Gray()};
	// Colours from http://tools.medialab.sciences-po.fr/iwanthue/
	static Color newc[20] = {Color(197,127,117), Color(115,214,74), Color(205,80,212), Color(124,193,215), Color(85,82,139),
			Color(109,212,161), Color(207,72,48), Color(209,206,59), Color(194,134,55), Color(63,72,41), 
			Color(201,63,109), Color(193,192,158), Color(91,134,56), Color(105,48,38), Color(201,170,200),
			Color(86,117,119), Color(188,91,165), Color(124,120,216), Color(195,208,119), Color(79,46,75)};
	static Color dark[20] = {Color(93,143,203), Color(98,190,74), Color(182,86,196), Color(168,180,56), 
			Color(101,103,203), Color(214,158,53), Color(187,136,208), Color(75,137,53), Color(214,67,135),
			Color(89,193,135), Color(212,62,65), Color(65,193,195), Color(206,98,47), Color(57,133,95),
			Color(156,76,127), Color(165,177,106), Color(229,135,158), Color(115,111,39), Color(176,81,81), Color(193,132,81)};

	if (index < 20) {
		if (version == 0) 
			return AdjustIfDark(oldc[index]);
		else {
			if (!IsDarkTheme())
				return newc[index];
			else
				return dark[index];
		}
	} else
		return Color((int)Random(), (int)Random(), (int)Random());
}
		
String ScatterDraw::GetNewDash(int index) {
	index %= 7;
	switch(index) {
	case 0:		return LINE_SOLID;
	case 1:		return LINE_DOTTED;
	case 2:		return LINE_DOTTED_FINE;
	case 3:		return LINE_DOTTED_FINER;
	case 4:		return LINE_DASHED;
	case 5:		return LINE_DASHED_LONG;
	case 6:		return LINE_DASH_DOT;
	}
	return LINE_SOLID;
}

MarkPlot *ScatterDraw::GetNewMarkPlot(int index) {
	index %= 9;
	switch(index) {
	case 0:	return new CircleMarkPlot();
	case 1:	return new SquareMarkPlot();
	case 2:	return new TriangleMarkPlot();
	case 3:	return new CrossMarkPlot();
	case 4:	return new XMarkPlot();
	case 5:	return new RhombMarkPlot();
	case 6:	return new InvTriangleMarkPlot();
	case 7:	return new DiamondMarkPlot();
	case 8:	return new AsteriskMarkPlot();
	}
	return new CircleMarkPlot();
}

Color GetOpaqueColor(const Color &color, const Color &background, double opacity) {
	if (opacity == 1)
		return color;
	if (opacity == 0)
		return background;
	return Color(int(opacity*(color.GetR() - background.GetR()) + background.GetR()),
	             int(opacity*(color.GetG() - background.GetG()) + background.GetG()),
	             int(opacity*(color.GetB() - background.GetB()) + background.GetB()));
} 
		
ScatterDraw::ScatterBasicSeries::ScatterBasicSeries() { 
	color = Null;
	thickness = 3;
	legend = "";
	unitsX = "";
	unitsY = "";
	opacity = 1;
	primaryY = true;
	sequential = false;
	dash = LINE_SOLID;	
	seriesPlot = new LineSeriesPlot();
	markPlot = new CircleMarkPlot();
	markWidth = 8;
	markColor = Null;
	markBorderWidth = 1;
	markBorderColor = Null;
	fillColor = Null;
	labels = 0;
	isClosed = false;
	barWidth = 10;
	labelsFont = StdFont();
	labelsColor = Null;
	labelsDx = labelsDy = 0;
	labelsAlign = ALIGN_CENTER;
	showLegend = true;
	legendLine = false;
	angle = 0;
}
		
void ScatterDraw::ScatterBasicSeries::Init(int index) {
	color = GetNewColor(index);
	markColor = Color(max(color.GetR()-30, 0), max(color.GetG()-30, 0), max(color.GetB()-30, 0));
	
	dash = GetNewDash(int(index/6));
	markPlot = GetNewMarkPlot(index);
}

ScatterDraw &ScatterDraw::AddSeries(DataSource &data) {
	return _AddSeries(&data, false);
}

ScatterDraw &ScatterDraw::_AddSeries(DataSource *data, bool owns) {
	ScatterSeries &s = series.Add();
	s.Init(series.GetCount()-1);
	s.SetDataSource(data, owns);
	if (sequentialXAll)
		s.sequential = true;
	OnAddSeries();	
	Refresh();
	return *this;	
}

ScatterDraw &ScatterDraw::InsertSeries(int index, double *yData, int numData, double x0, double deltaX) {
	return InsertSeries<CArray>(index, yData, numData, x0, deltaX);
}
	
ScatterDraw &ScatterDraw::InsertSeries(int index, double *xData, double *yData, int numData) {
	return InsertSeries<CArray>(index, xData, yData, numData);
}

ScatterDraw &ScatterDraw::InsertSeries(int index, Vector<double> &xData, Vector<double> &yData) {
	return InsertSeries<VectorXY>(index, xData, yData);
}

ScatterDraw &ScatterDraw::InsertSeries(int index, Array<double> &xData, Array<double> &yData) {
	return InsertSeries<ArrayXY>(index, xData, yData);
}
		
ScatterDraw &ScatterDraw::InsertSeries(int index, Vector<Pointf> &points) {
	return InsertSeries<VectorPointf>(index, points);
}

ScatterDraw &ScatterDraw::InsertSeries(int index, Array<Pointf> &points) {
	return InsertSeries<ArrayPointf>(index, points);
}

ScatterDraw &ScatterDraw::InsertSeries(int index, double (*function)(double))	 {
	return InsertSeries<FuncSource>(index, function);
}

ScatterDraw &ScatterDraw::InsertSeries(int index, Pointf (*function)(double), int np, double from, double to)	 {
	return InsertSeries<FuncSourcePara>(index, function, np, from, to);
}

ScatterDraw &ScatterDraw::InsertSeries(int index, PlotExplicFunc &function)	 {									
	return InsertSeries<PlotExplicFuncSource>(index, function);
}

ScatterDraw &ScatterDraw::InsertSeries(int index, PlotParamFunc function, int np, double from, double to)	 {
	return InsertSeries<PlotParamFuncSource>(index, function, np, from, to);
}

ScatterDraw &ScatterDraw::_InsertSeries(int index, DataSource *data) {
	ASSERT(IsValid(index));
	
	ScatterSeries &s = series.Insert(index);
	s.Init(index);
	s.SetDataSource(data);
	OnAddSeries();
	Refresh();	
	return *this;
}

int64 ScatterDraw::GetCount(int index) {
	ASSERT(IsValid(index));
	ASSERT(!series[index].IsDeleted());
	
	return series[index].Data().GetCount();
}

void ScatterDraw::GetValues(int index, int64 idata, double &x, double &y) {
	ASSERT(IsValid(index) && !IsNull(GetCount(index)));
	ASSERT(!series[index].IsDeleted());
	ASSERT(idata >= 0 && idata < series[index].Data().GetCount());
	
	try {
		x = series[index].Data().x(idata);
		y = series[index].Data().y(idata);
	} catch(ValueTypeError error) {
		ASSERT_(true, error);
		x = y = Null;
	}
}

double ScatterDraw::GetValueX(int index, int64 idata) {
	ASSERT(IsValid(index) && !IsNull(GetCount(index)));
	ASSERT(!series[index].IsDeleted());
	ASSERT(idata >= 0 && idata < series[index].Data().GetCount());
	
	try {
		return series[index].Data().x(idata);
	} catch(ValueTypeError error) {
		ASSERT_(true, error);
		return Null;
	}
}

Value ScatterDraw::GetStringX(int index, int64 idata, bool removeEnter) {
	double ret = GetValueX(index, idata);
	if (IsNull(ret))
		return Null;
	if (cbModifFormatX) {
		String sret;
		cbModifFormatX(sret, int(idata), ret);
		if (removeEnter) {
			sret.Replace(" \n", " ");
			sret.Replace("\n ", " ");
			sret.Replace("\n", " ");
		}
		return sret;
	} else
		return ret;
}
	

double ScatterDraw::GetValueY(int index, int64 idata) {
	ASSERT(IsValid(index) && !IsNull(GetCount(index)));
	ASSERT(!series[index].IsDeleted());
	ASSERT(idata >= 0 && idata < series[index].Data().GetCount());
	
	try {
		return series[index].Data().y(idata);
	} catch(ValueTypeError error) {
		ASSERT_(true, error);
		return Null;
	}
}

Value ScatterDraw::GetStringY(int index, int64 idata, bool removeEnter) {
	double ret = GetValueY(index, idata);
	if (!IsNum(ret))
		return Null;
	if (cbModifFormatY) {
		String sret;
		cbModifFormatY(sret, int(idata), ret);
		if (removeEnter) {
			sret.Replace(" \n", " ");
			sret.Replace("\n ", " ");
			sret.Replace("\n", " ");
		}
		return sret;
	} else
		return ret;
}
	
ScatterDraw &ScatterDraw::SetNoPlot(int index) {
 	ASSERT(IsValid(index));
 	ASSERT(!series[index].IsDeleted());
 	
 	series[index].seriesPlot = NULL;
 	return *this;
}	

ScatterDraw &ScatterDraw::PlotStyle(int index, SeriesPlot *data) {
	ASSERT(IsValid(index));
	ASSERT(!series[index].IsDeleted());
	
	series[index].seriesPlot = data;
	return *this;	
}

ScatterDraw &ScatterDraw::MarkStyle(int index, MarkPlot *data) {
	ASSERT(IsValid(index));
	ASSERT(!series[index].IsDeleted());
	
	series[index].markPlot = data;
	return *this;
}

ScatterDraw &ScatterDraw::MarkStyle(int index, const String name) {
	ASSERT(IsValid(index));
	ASSERT(!series[index].IsDeleted());
	
	int typeidx = MarkPlot::TypeIndex(name);
	
	if (typeidx >= 0)
		series[index].markPlot = MarkPlot::Create(typeidx);
	else
		series[index].markPlot = 0;
	return *this;
}

const String ScatterDraw::GetMarkStyleName(int index) {
	ASSERT(IsValid(index));
	ASSERT(!series[index].IsDeleted());
	
	if (series[index].markPlot)
		return MarkPlot::TypeName(series[index].markPlot->GetType());
	else
		return t_("No mark");
}

int ScatterDraw::GetMarkStyleType(int index) {
	ASSERT(IsValid(index));
	ASSERT(!series[index].IsDeleted());
	
	if (series[index].markPlot)
		return series[index].markPlot->GetTypeType();
	else
		return -1;
}

ScatterDraw &ScatterDraw::SetMarkStyleType(int index, int type) {
	ASSERT(IsValid(index));
	ASSERT(!series[index].IsDeleted());
	
	if (IsNull(type)) 
		series[index].markPlot = GetNewMarkPlot(index);
	else {
		if (series[index].markPlot)
			series[index].markPlot->SetTypeType(type);
	}
	return *this;	
}

ScatterDraw &ScatterDraw::Stroke(int index, double thickness, Color color) {
	ASSERT(IsValid(index));
	ASSERT(!series[index].IsDeleted());
	
	if (IsNull(color))
		color = GetNewColor(index);
	series[index].color = color;
	series[index].thickness = thickness;
	//if (visibleBN)
	//	series[index].dash = GetNewDash(index);
	
	Refresh();
	return *this;	
}

void ScatterDraw::GetStroke(int index, double &thickness, Color &color) {
	ASSERT(IsValid(index));
	ASSERT(!series[index].IsDeleted());
	
	color = series[index].color;
	thickness  = series[index].thickness;
}

ScatterDraw &ScatterDraw::Fill(Color color) {
	int index = series.GetCount() - 1;

	if (IsNull(color)) {
		color = GetNewColor(index);
		color = Color(min(color.GetR()+30, 255), min(color.GetG()+30, 255), min(color.GetB()+30, 255));
	}
	series[index].fillColor = color;
	
	Refresh();
	return *this;	
}

ScatterDraw &ScatterDraw::MarkColor(Color color) {
	int index = series.GetCount() - 1;

	if (IsNull(color)) {
		color = GetNewColor(index);
		color = Color(max(color.GetR()-30, 0), max(color.GetG()-30, 0), max(color.GetB()-30, 0));
	}
	series[index].markColor = color;
	
	Refresh();
	return *this;	
}

ScatterDraw &ScatterDraw::MarkBorderColor(Color color) {
	int index = series.GetCount() - 1;

	if (IsNull(color)) {
		color = GetNewColor(index + 1);
		color = Color(max(color.GetR()-30, 0), max(color.GetG()-30, 0), max(color.GetB()-30, 0));
	}
	series[index].markBorderColor = color;
	
	Refresh();
	return *this;	
}

ScatterDraw &ScatterDraw::MarkWidth(double markWidth) {
	int index = series.GetCount() - 1;
	
	series[index].markWidth = markWidth;
	
	Refresh();
	return *this;
}

ScatterDraw &ScatterDraw::MarkBorderWidth(double markWidth) {
	int index = series.GetCount() - 1;
	
	series[index].markBorderWidth = markWidth;
	
	Refresh();
	return *this;
}

ScatterDraw &ScatterDraw::ShowSeriesLegend(int index, bool show) {
	ASSERT(IsValid(index));
	ASSERT(!series[index].IsDeleted());
	
	series[index].showLegend = show;
	
	Refresh();
	return *this;
}

ScatterDraw &ScatterDraw::Closed(int index, bool closed) {
	ASSERT(IsValid(index));
	ASSERT(!series[index].IsDeleted());
	
	series[index].isClosed = closed;
	Refresh();
	return *this;
}

bool ScatterDraw::IsClosed(int index) {
	ASSERT(IsValid(index));
	ASSERT(!series[index].IsDeleted());
	
	return series[index].isClosed;
}
	
ScatterDraw &ScatterDraw::BarWidth(int index, double width) {
	ASSERT(IsValid(index));
	ASSERT(!series[index].IsDeleted());
	
	series[index].barWidth = width;
	Refresh();
	return *this;		
}
	
ScatterDraw &ScatterDraw::Dash(const char *dash) {
	ASSERT(CheckDash(dash));
	int index = series.GetCount() - 1;
	
	return Dash(index, dash);		
}

ScatterDraw &ScatterDraw::Dash(int index, const char *dash) {
	ASSERT(CheckDash(dash));
	ASSERT(IsValid(index));
	ASSERT(!series[index].IsDeleted());
	
	series[index].dash = dash;
	Refresh();
	return *this;		
}

ScatterDraw &ScatterDraw::NoDash() {
	int index = series.GetCount() - 1;
	
	return NoDash(index);		
}

ScatterDraw &ScatterDraw::NoDash(int index) {
	ASSERT(IsValid(index));
	ASSERT(!series[index].IsDeleted());
	
	series[index].dash = LINE_SOLID;
	Refresh();
	return *this;		
}

const String ScatterDraw::GetDash(int index) {
	ASSERT(IsValid(index));
	ASSERT(!series[index].IsDeleted());
	
	return series[index].dash;
}

ScatterDraw &ScatterDraw::Legend(const String legend) {
	int index = series.GetCount() - 1;
	
	return Legend(index, legend);
}

ScatterDraw& ScatterDraw::Legend(int index, const String legend) {
	ASSERT(IsValid(index));
	ASSERT(!series[index].IsDeleted());
	
	if (legend.IsEmpty())
		series[index].showLegend = false;
	else {
		series[index].showLegend = true;
		series[index].legend = legend;
	}
	return *this;
}

const String& ScatterDraw::GetLegend(int index) {
	ASSERT(IsValid(index));
	ASSERT(!series[index].IsDeleted());
	
	return series[index].legend;
}

ScatterDraw &ScatterDraw::Units(const String unitsY, const String unitsX) {
	int index = series.GetCount() - 1;
	
	return Units(index, unitsY, unitsX);
}

ScatterDraw& ScatterDraw::Units(int index, const String unitsY, const String unitsX) {
	ASSERT(IsValid(index));
	ASSERT(!series[index].IsDeleted());
	
	series[index].unitsX = unitsX;
	series[index].unitsY = unitsY;
	labelsChanged = true;
	
	return *this;
}

const String ScatterDraw::GetUnitsX(int index) {
	ASSERT(IsValid(index));
	ASSERT(!series[index].IsDeleted());
	
	return series[index].unitsX;
}

const String ScatterDraw::GetUnitsY(int index) {
	ASSERT(IsValid(index));
	ASSERT(!series[index].IsDeleted());
	
	return series[index].unitsY;
}

ScatterDraw& ScatterDraw::SetFillColor(int index, const Color& color) {
	ASSERT(IsValid(index));
	ASSERT(!series[index].IsDeleted());
	
	series[index].fillColor = color;
	Refresh();
	return *this;
}

Color ScatterDraw::GetFillColor(int index) const {
	ASSERT(IsValid(index));
	ASSERT(!series[index].IsDeleted());
	
	return series[index].fillColor;
}

ScatterDraw &ScatterDraw::SetMarkBorderWidth(int index, double width) { 
	ASSERT(IsValid(index));
	ASSERT(!series[index].IsDeleted());
	
	series[index].markBorderWidth = width;
	Refresh();
	return *this;
}
 
double ScatterDraw::GetMarkBorderWidth(int index) { 
 	ASSERT(IsValid(index));
	ASSERT(!series[index].IsDeleted());
	
 	return series[index].markBorderWidth;
}

ScatterDraw &ScatterDraw::SetMarkWidth(int index, double markWidth) {
	ASSERT(IsValid(index));
	ASSERT(!series[index].IsDeleted());
	
	series[index].markWidth = markWidth;
	Refresh();
	return *this;
}

double ScatterDraw::GetMarkWidth(int index) {
	ASSERT(IsValid(index));
	ASSERT(!series[index].IsDeleted());
	
	return series[index].markWidth;
}

ScatterDraw &ScatterDraw::SetMarkColor(int index, const Color& color) {
	ASSERT(IsValid(index));
	ASSERT(!series[index].IsDeleted());
	
	series[index].markColor = color;
	Refresh();
	return *this;
}

Color ScatterDraw::GetMarkColor(int index) const {
	ASSERT(IsValid(index));
	ASSERT(!series[index].IsDeleted());
	
	return series[index].markColor;
}

ScatterDraw &ScatterDraw::SetMarkBorderColor(int index, const Color& color) {
	ASSERT(IsValid(index));
	ASSERT(!series[index].IsDeleted());
	
	series[index].markBorderColor = color;
	Refresh();
	return *this;
}

Color ScatterDraw::GetMarkBorderColor(int index) const {
	ASSERT(IsValid(index));
	ASSERT(!series[index].IsDeleted());
	
	return series[index].markBorderColor;
}

void ScatterDraw::NoMark(int index) {
	ASSERT(IsValid(index));
	ASSERT(!series[index].IsDeleted());
	
	series[index].markWidth = 0;
}

void ScatterDraw::SetDataPrimaryY(int index, bool primary) {
	ASSERT(IsValid(index));
	ASSERT(!series[index].IsDeleted());
	
	series[index].primaryY = primary;
	if (!primary)
		SetDrawY2Reticle(true).SetDrawY2ReticleNumbers();
	Refresh();
}

ScatterDraw &ScatterDraw::SetDataPrimaryY(bool primary) {
	SetDataPrimaryY(series.GetCount()-1, primary);
	return *this;
}

void ScatterDraw::SetDataSecondaryY(int index, bool secondary) {
	ASSERT(IsValid(index));
	ASSERT(!series[index].IsDeleted());
	
	series[index].primaryY = !secondary;
	if (secondary)
		SetDrawY2Reticle().SetDrawY2ReticleNumbers();
	Refresh();
}

ScatterDraw &ScatterDraw::SetDataSecondaryY(bool secondary) {
	SetDataSecondaryY(series.GetCount()-1, secondary);
	return *this;
}

bool ScatterDraw::IsDataPrimaryY(int index) {
	ASSERT(IsValid(index));
	ASSERT(!series[index].IsDeleted());
	
	return series[index].primaryY;	
}

bool ScatterDraw::ThereArePrimaryY() {
	for (int i = 0; i < series.GetCount(); ++i)
		if (series[i].primaryY)
			return true;
	return false;
}

bool ScatterDraw::ThereAreSecondaryY() {
	for (int i = 0; i < series.GetCount(); ++i)
		if (!series[i].primaryY)
			return true;
	return false;
}
	
void ScatterDraw::SetSequentialX(int index, bool sequential) {
	ASSERT(IsValid(index));
	ASSERT(!series[index].IsDeleted());
	
	series[index].sequential = sequential;
	Refresh();
}

ScatterDraw &ScatterDraw::SetSequentialX(bool sequential) {
	SetSequentialX(series.GetCount()-1, sequential);
	return *this;
}

bool ScatterDraw::GetSequentialX(int index) {
	ASSERT(IsValid(index));
	ASSERT(!series[index].IsDeleted());
	
	return series[index].sequential;
}

bool ScatterDraw::GetSequentialX() {
	return GetSequentialX(series.GetCount()-1);
}

ScatterDraw &ScatterDraw::SetSequentialXAll(bool sequential) {
	for (int i = 0; i < series.GetCount(); ++i) {
		const ScatterSeries &serie = series[i]; 
		if (serie.IsDeleted())
			continue;
		SetSequentialX(i, sequential);
	}
	sequentialXAll = sequential;
	return *this;
}

void ScatterDraw::Show(int index, bool show) {
	ASSERT(IsValid(index));
	ASSERT(!series[index].IsDeleted());
	
	series[index].opacity = show ? 1 : 0;
	labelsChanged = true;
	Refresh();
}

bool ScatterDraw::IsVisible(int index) {
	ASSERT(IsValid(index));
	ASSERT(!series[index].IsDeleted());
	
	return series[index].opacity > 0;
}

ScatterDraw &ScatterDraw::ShowAll(bool ) {
	for (int i = 0; i < series.GetCount(); ++i) {
		ScatterSeries &serie = series[i]; 
		if (serie.IsDeleted())
			continue;
		serie.opacity = 1;
	}
	return *this;
}

ScatterDraw& ScatterDraw::Id(int id) {
	return Id(series.GetCount()-1, id);
}

ScatterDraw& ScatterDraw::Id(int index, int id) {
	ASSERT(IsValid(index));
	ASSERT(!series[index].IsDeleted());
	
	series[index].id = id;
	return *this;
}

int ScatterDraw::GetId(int index) {
	ASSERT(IsValid(index));
	ASSERT(!series[index].IsDeleted());
	
	return series[index].id;
}

bool ScatterDraw::RemoveSeries(int index) {
	ASSERT(IsValid(index));
	ASSERT(!series[index].IsDeleted());
	
	if (WhenRemoveSeries)
		if (!WhenRemoveSeries(index))
			return false;
	series.Remove(index);
	Refresh();
	return true;
}

ScatterDraw& ScatterDraw::RemoveAllSeries() {
	series.Clear();
	OnAddSeries();
	Refresh();
	return *this;
}

bool ScatterDraw::SwapSeries(int i1, int i2) {
	ASSERT(IsValid(i1));
	ASSERT(!series[i1].IsDeleted());
	ASSERT(IsValid(i2));
	ASSERT(!series[i2].IsDeleted());
	
	if (WhenSwapSeries)
		if (!WhenSwapSeries(i1, i2))
			return false;
	series.Swap(i1, i2);
	Refresh();	
	return true;
}

Drawing ScatterDraw::GetDrawing() {
	DrawingDraw ddw(size);
	
	SetDrawing<DrawingDraw>(ddw, true);
	PlotTexts(ddw);

	return ddw;
}

Image ScatterDraw::GetImage() {
#ifndef flagGUI
	ASSERT(mode != MD_DRAW);
#endif
	ImageBuffer ib(size);	
	BufferPainter bp(ib, mode);	
	
	bp.LineCap(LINECAP_SQUARE);
	bp.LineJoin(LINEJOIN_MITER);
	SetDrawing(bp, false);

	return ib;
}


double ScatterDraw::GetXByPoint(double x) {
	return FixPow10((x - hPlotLeft)*xRange/(GetSize().cx - (hPlotLeft + hPlotRight) - 1) + GetXMin(), logX);
}

double ScatterDraw::GetYByPoint(double y) {
	return FixPow10((GetSize().cy - vPlotTop - y - 1)*yRange/(GetSize().cy - (vPlotTop + vPlotBottom) - GetTitleFont().GetHeight() - GetTitleFont().GetDescent() - 1) + GetYMin(), logY);
}

double ScatterDraw::GetY2ByPoint(double y) {
	return FixPow10((GetSize().cy - vPlotTop - y - 1)*yRange2/(GetSize().cy - (vPlotTop + vPlotBottom) - GetTitleFont().GetHeight() - GetTitleFont().GetDescent() - 1) + GetYMin2(), logY);
}

double ScatterDraw::GetXPointByValue(double x) {
	return FixPow10((x - GetXMin())/xRange*(GetSize().cx - (hPlotLeft + hPlotRight) - 1) + hPlotLeft, logX);
}

double ScatterDraw::GetYPointByValue(double y) {
	return FixPow10((GetSize().cy - vPlotTop - 1) - (y - GetYMin())/yRange*(GetSize().cy - (vPlotTop + vPlotBottom) - GetTitleFont().GetHeight() - GetTitleFont().GetDescent() - 1), logY);
}

ScatterDraw &ScatterDraw::SetRangeLinked(double rx, double ry, double ry2) {
	if (linkedMaster) {
		linkedMaster->SetRangeLinked(rx, ry, ry2);
		linkedMaster->Refresh();
		return *this;
	}
	SetRange(rx, ry, ry2);
	if (!linkedCtrls.IsEmpty()) {
		for (int i = 0; i < linkedCtrls.GetCount(); ++i) {
	    	linkedCtrls[i]->SetRange(rx, ry, ry2);
	    	linkedCtrls[i]->Refresh();
		}
	}
	return *this;
}

ScatterDraw &ScatterDraw::SetXYMinLinked(double x, double y, double y2) {
	if (linkedMaster) {
		linkedMaster->SetXYMinLinked(x, y, y2);
		linkedMaster->Refresh();
		return *this;
	}
	SetXYMin(x, y, y2);
	if (!linkedCtrls.IsEmpty()) {
		for (int i = 0; i < linkedCtrls.GetCount(); ++i) {
	    	linkedCtrls[i]->SetXYMin(x, y, y2);
	    	linkedCtrls[i]->Refresh();
		}
	}	
	return *this;
}

ScatterDraw &ScatterDraw::SetXYMaxLinked(double x, double y, double y2) {
	if (linkedMaster) {
		linkedMaster->SetXYMaxLinked(x, y, y2);
		linkedMaster->Refresh();
		return *this;
	}
	SetXYMax(x, y, y2);
	if (!linkedCtrls.IsEmpty()) {
		for (int i = 0; i < linkedCtrls.GetCount(); ++i) {
	    	linkedCtrls[i]->SetXYMax(x, y, y2);
	    	linkedCtrls[i]->Refresh();
		}
	}	
	return *this;
}

void ScatterDraw::Zoom(double scale, bool mouseX, bool mouseY) {
	if (linkedMaster) {
		linkedMaster->Zoom(scale, mouseX, mouseY);
		return;
	}
	ZoomNonLinked(scale, mouseX, mouseY);
	if (!linkedCtrls.IsEmpty()) {
		for (int i = 0; i < linkedCtrls.GetCount(); ++i) 
	    	linkedCtrls[i]->ZoomNonLinked(scale, mouseX, mouseY);
	}
}

void ScatterDraw::ZoomNonLinked(double scale, bool mouseX, bool mouseY) {
	if (scale == 1)
		return;
	lastRefresh_sign = (scale >= 0) ? 1 : -1;
	
	if (scale > 1) {
		if (maxXRange > 0) {
			if (xRange*scale > maxXRange) {
				highlight_0 = (int)GetTickCount();
				if (xRange == maxXRange) {
					Refresh();
					return;
				} else 
					scale = maxXRange/xRange;
			} 
		}
		if (maxYRange > 0) {
			if (yRange*scale > maxYRange) {
				highlight_0 = (int)GetTickCount();
				if (yRange == maxYRange) {
					Refresh();
					return;
				} else 
					scale = maxYRange/xRange;
			} 
		}
	} else {
		if (maxXRange > 0) {
			if (xRange*scale < minXRange) {
				highlight_0 = (int)GetTickCount();
				if (xRange == minXRange) {
					Refresh();
					return;
				} else 
					scale = minXRange/xRange;
			} 
		}
		if (maxYRange > 0) {
			if (yRange*scale < minYRange) {
				highlight_0 = (int)GetTickCount();
				if (yRange == minYRange) {
					Refresh();
					return;
				} else 
					scale = minYRange/xRange;
			} 
		}
	}
	
	if (mouseX) {
		if (zoomStyleX == TO_CENTER) {
			if (IsNum(minXmin) && xMin + xRange*(1-scale)/2. <= minXmin) {
				highlight_0 = (int)GetTickCount();
				Refresh();
				return;
			}
			if (IsNum(maxXmax) && xMin + xRange*scale + xRange*(1-scale)/2. >= maxXmax) {
				highlight_0 = (int)GetTickCount();
				Refresh();
				return;
			}
			double oldXMin = xMin;
			xMin += xRange*(1-scale)/2.;
			xMinUnit = oldXMin + xMinUnit - xMin;
			AdjustMinUnitX();
		}
		xRange *= scale;
		
		AdjustMajorUnitX();
		AdjustMinUnitX();
		lastxRange = xRange;
	}
	if (mouseY) {
		if (zoomStyleY == TO_CENTER) {
			if (IsNum(minYmin) && yMin + yRange*(1-scale)/2. <= minYmin) {
				highlight_0 = (int)GetTickCount();
				Refresh();
				return;
			}
			if (IsNum(maxYmax) && yMin + yRange*scale + yRange*(1-scale)/2. >= maxYmax) {
				highlight_0 = (int)GetTickCount();
				Refresh();
				return;
			}
			double oldYMin = yMin;
			yMin += yRange*(1 - scale)/2.;
			yMinUnit = oldYMin + yMinUnit - yMin;
			AdjustMajorUnitY();
			AdjustMinUnitY();
			double oldYMin2 = yMin2;
			yMin2 += yRange2*(1-scale)/2.;
			yMinUnit2 = oldYMin2 + yMinUnit2 - yMin2;
			AdjustMajorUnitY2();
			AdjustMinUnitY2();
		}
		yRange *= scale;
		yRange2 *= scale;
		
		AdjustMajorUnitY();
		AdjustMinUnitY();
		lastyRange = yRange;	
	}
	if (mouseX || mouseY) {
		WhenSetRange();
		if (zoomStyleX == TO_CENTER || zoomStyleY == TO_CENTER)
			WhenSetXYMin();
		Refresh();
		WhenZoomScroll();
	}
}

void ScatterDraw::Scroll(double factorX, double factorY) {
	if (linkedMaster) {
		linkedMaster->Scroll(factorX, factorY);
		return;
	}
	ScrollNonLinked(factorX, factorY);
	if (!linkedCtrls.IsEmpty()) {
		for (int i = 0; i < linkedCtrls.GetCount(); ++i)
	    	linkedCtrls[i]->ScrollNonLinked(factorX, factorY);
	}
}

void ScatterDraw::ScrollNonLinked(double factorX, double factorY) {
	if (factorX != 0) {
		double deltaX = factorX*xRange;
		if (IsNum(minXmin) && factorX > 0) {
			if (xMin > minXmin) {
				if (xMin - deltaX < minXmin) {
					highlight_0 = (int)GetTickCount();
					deltaX = xMin - minXmin;
				}
			} else {
				factorX = Null;
				highlight_0 = (int)GetTickCount();
			}
		}
		if (IsNum(maxXmax) && factorX < 0) {
			if (xMin + xRange < maxXmax) {
				if (xMin + xRange - deltaX > maxXmax) {
					highlight_0 = (int)GetTickCount();
					deltaX = xMin + xRange - maxXmax;
				}
			} else {
				factorX = Null;
				highlight_0 = (int)GetTickCount();
			}
		}
		if (factorX != 0 && !IsNull(factorX)) {	
			xMin -= deltaX;
			xMinUnit += deltaX;
			AdjustMinUnitX();
		}
	}
	if (factorY != 0) {
		double deltaY = factorY*yRange;
		if (IsNum(minYmin) && factorY > 0) {
			if (yMin > minYmin) {
				if (yMin - deltaY < minYmin) {
					highlight_0 = (int)GetTickCount();
					deltaY = yMin - minYmin;
				}
			} else {
				factorY = Null;
				highlight_0 = (int)GetTickCount();
			}
		}
		if (IsNum(maxYmax) && factorY < 0) {
			if (yMin + yRange < maxYmax) {
				if (yMin + yRange - deltaY > maxYmax) {
					highlight_0 = (int)GetTickCount();
					deltaY = yMin + yRange - maxYmax;
				}
			} else {
				factorY = Null;
				highlight_0 = (int)GetTickCount();
			}
		}	
		if (factorY != 0 && IsNum(factorY)) {	
			yMin -= deltaY;
			yMinUnit += deltaY;
			AdjustMinUnitY();
			if (drawY2Reticle) {
				double deltaY2 = factorY*yRange2;
				yMin2 -= deltaY2;
				yMinUnit2 += deltaY2;
				AdjustMinUnitY2();
			}
		}
	}
	if (!IsNum(factorX) || !IsNum(factorY)) 
		Refresh();
	else if (factorX != 0 || factorY != 0) {	
		WhenSetXYMin();	
		Refresh();
		WhenZoomScroll();
	}
}

ScatterDraw &ScatterDraw::LinkedWith(ScatterDraw &ctrl) {
	Unlinked();

	if (ctrl.linkedMaster) 
		linkedMaster = ctrl.linkedMaster;
	else 
		linkedMaster = &ctrl;

	linkedMaster->linkedCtrls.FindAdd(this);

	return *this;
}

void ScatterDraw::Unlinked() {
	if (!linkedMaster) {
		for (int i = 0; i < linkedCtrls.GetCount(); ++i) 
			linkedCtrls[i]->linkedMaster = 0;
		linkedCtrls.Clear();
	} else {
		linkedMaster->linkedCtrls.RemoveKey(this);
		linkedMaster = 0;
	}
}

ScatterDraw& ScatterDraw::SetMouseHandling(bool valx, bool valy) {
	mouseHandlingX = valx;
	mouseHandlingY = valy;
	return *this;
}

ScatterDraw& ScatterDraw::SetMouseHandlingLinked(bool valx, bool valy) {
	if (linkedMaster) 
		return linkedMaster->SetMouseHandlingLinked(valx, valy);
	
	SetMouseHandling(valx, valy);
	if (!linkedCtrls.IsEmpty()) {
		for (int i = 0; i < linkedCtrls.GetCount(); ++i)
	    	linkedCtrls[i]->SetMouseHandling(valx, valy);
	}
	return *this;
}

void ScatterDraw::AddId(Vector<Vector<int>> &idGroups, int id) {
	if (idGroups.IsEmpty()) {
		idGroups.Add() << id;
		return;
	}
	for (int i = 0; i < idGroups.GetCount(); ++i) {
		if (series[id].Data().SameX(series[idGroups[i][0]].Data())) {
			idGroups[i] << id;
			return;
		}
	}
	idGroups.Add() << id;
}

String ScatterDraw::GetCSV() {
	String ret;
	String sep = GetDefaultCSVSeparator();
	
	if (!GetTitle().IsEmpty())	
		ret << GetTitle() + "\n";
	
	if (IsSurf()) {
		if (GetSurf().IsExplicit())
			ret << t_("Impossible to save explicit data (data from a equation, not from a data table)");
		else {
			DataSourceSurf& surf = GetSurf();
			if (surf.IsAreas()) {
				for (int iy = 0; iy < surf.rows()-1; ++iy) {
					ret << (FDS(surf.y(iy), 8) + " - " + FDS(surf.y(iy+1), 8)) << sep;
					for (int ix = 0; ix < surf.cols()-1; ++ix) {
						if (iy == 0)
							ret << (FDS(surf.x(ix), 8) + " - " + FDS(surf.x(ix+1), 8)) << sep;
						ret << surf.z_data(ix, iy) << sep;
					}
					ret << "\n";
				}
			} else {
				for (int iy = 0; iy < surf.rows(); ++iy) {
					ret << FDS(surf.y(iy), 8) << sep;
					for (int ix = 0; ix < surf.cols(); ++ix) {
						if (iy == 0)
							ret << FDS(surf.x(ix), 8) << sep;
						ret << surf.z_data(ix, iy) << sep;
					}
					ret << "\n";
				}
			}
		}
	} else {
		Vector<Vector<int>> idGroups;
		for (int i = 0; i < series.GetCount(); ++i) {
			const ScatterSeries &serie = series[i]; 
			const DataSource &data = serie.Data();
			if (!serie.IsDeleted() && serie.opacity > 0  && !data.IsExplicit()) {
				int64 sz = data.GetCount();
				if (!IsNull(sz) && sz > 0) 
					AddId(idGroups, i);
			}
		}
		//sep.Replace("\t", "	");
		for (int i = 0; i < idGroups.size(); i++) {
			for (int ii = 0; ii < idGroups[i].size(); ii++) {
				const ScatterSeries &serie = series[idGroups[i][ii]];
				if (ii == 0) {
					String str = GetLabelX();
					if (!serie.unitsX.IsEmpty()) {
						if (!GetLabelX().IsEmpty())
							str << " ";
						str << "[" << serie.unitsX << "]";
					}
					if (i > 0)
						ret << sep;
					ret << str;
				}
				String str = serie.legend;
				if (!serie.unitsY.IsEmpty()) {
					if (!serie.legend.IsEmpty())
						str << " ";
					str << "[" << serie.unitsY << "]";
				}
				ret << sep << str;
			}
		}
		bool thereIsData = true;
		for (int64 row = 0; thereIsData; row++) {
			String line = "\n";
			thereIsData = false;
			for (int i = 0; i < idGroups.size(); i++) {
				bool plot = series[idGroups[i][0]].Data().size() > row;
				if (plot)
					thereIsData = true;
				if (i > 0)
					line << sep;
				for (int ii = 0; ii < idGroups[i].size(); ii++) {
					//ScatterSeries &serie = series[idGroups[i][ii]];
					//DataSource &data = serie.Data();
					if (ii == 0) {
						if (plot) 
							line << GetStringX(idGroups[i][ii], row, true); //data.x(row);
					}
					line << sep;
					if (plot)
						line << GetStringY(idGroups[i][ii], row, true); //data.y(row);
				}
			}
			if (thereIsData)
				ret << line;	
		}
	}
	return ret;
}

bool ScatterDraw::CheckDash(const char *dash) {
	for (const char *c = dash; *c != '\0'; ++c) {
		if ((*c < '0' || *c > '9') && *c != ' ')
			return false;
	}
	return true;
}

Vector<Pointf> ScatterDraw::DataAddPoints(DataSource& data, bool primaryY, bool sequential) {
	Vector<Pointf> points;
		
	if (data.IsReverse()) {
		points = DataAddPoints(dynamic_cast<DataWrapper&>(data).Data(), primaryY, sequential);
		ReverseX(points);
	} else if (data.IsAppend()) {
		for (int i = 0; i < 2; i++)
			points.Append(DataAddPoints(dynamic_cast<DataAppend&>(data).DataAt(i), primaryY, sequential));
		if (data.IsRange()) {
			Vector<Pointf> filtered_points;
			filtered_points.AppendRange(FilterRange(points, [](Pointf p) { return IsNum(p); }));
			points = pick(filtered_points);
		}
	} else if (data.IsParam()) {
		double xmin = 0;
		double xmax = double(data.GetCount());
		for (double x = xmin; x <= xmax; x++) {
			double xx = data.x(x);
			double yy = data.y(x);
			if (!IsNum(xx) || !IsNum(yy))
				points << Null;
			else
				points << Pointf(GetPosX(xx), GetPosY(yy, primaryY));
		}
	} else if (data.IsExplicit()) {
		double xmin = xMin - 1;
		double xmax = xMin + xRange + 1;
		double dx = (xmax - xmin)/plotW;
		for (double xx = xmin; xx < xmax; xx += dx) {
			double yy = data.f(xx);
			if (!IsNum(yy))
				points << Null;
			else
				points << Pointf(GetPosX(xx), GetPosY(yy, primaryY));
		}
	} else {
		int64 imin, imax;
		if (sequential) {
			imin = imax = Null;
			for (int64 i = 0; i < data.GetCount(); ++i) {
				double xx = data.x(i);
				if (IsNum(xx)) {
					if (IsNull(imin)) {
						if (xx >= xMin) 
							imin = i;
					}
					imax = i;
					if (xx >= xMin + xRange) {
						imax = i;
						break;
					}
				}
			}
			if (IsNull(imin) || IsNull(imax))
				return points;
			/*
			if (IsNull(imin))
			    imin = 0;
			if (IsNull(imax))
			    imax = data.GetCount() - 1;*/
		} else {
			imin = 0;
			imax = data.GetCount() - 1;
		}
		if (fastViewX) {
			double dxpix = (data.x(imax) - data.x(imin))/plotW;
			int npix = 1;
			for (int64 i = imin; i <= imax; ) {
				double xx = data.x(i);
				if (!IsNum(xx)) {
					points << Null;						
					++i;
				} else {
					double maxv = data.x(imin) + dxpix*npix; 
					if (xx >= maxv) {					// No saving, data is not grouped in X
						double yy = data.y(i);
						if (!IsNum(yy)) 
							points << Null;
						else
							points << Pointf(GetPosX(xx), GetPosY(yy, primaryY));
						++i;
					} else {
						double maxY = Null, minY = Null;
						int64 ii;
						for (ii = 0; (i + ii < imax) && IsNum(data.x(i + ii)) && data.x(i + ii) < maxv; ++ii) {
							double dd = data.y(i + ii);
							if (IsNum(dd)) {
								if (IsNum(maxY))
									maxY = max(maxY, dd);
								else
									maxY = dd;
								if (IsNum(minY))
									minY = min(minY, dd);
								else
									minY = dd;
							}
						}
						if (ii == 0 && i == imax)
							break;
						
						if (!IsNum(minY) || !IsNum(maxY)) 
							points << Null;	
						else {
							double ix = GetPosX(xx);
							double iMax = GetPosY(maxY, primaryY);
							double iMin = GetPosY(minY, primaryY);
							points << Pointf(ix, iMax);
							if (iMax != iMin)
								points << Pointf(ix, iMin);	
						}
						i += ii;
					}
					npix++;
				}
			} 
		} else {
			for (int64 i = imin; i <= imax; ++i) {	
				double xx = data.x(i);
				double yy = data.y(i);
				if (!IsNum(xx) || !IsNum(yy)) 
					points << Null;
				else
					points << Pointf(GetPosX(xx), GetPosY(yy, primaryY));
			}
		}
	}
	return points;
}

double ScatterDraw::GetSeriesMaxX() {
	if (series.IsEmpty())
		return Null;
	double mx = series[0].Data().MaxX();
	for (int i = 1; i < series.size(); ++i) {
		double d = series[i].Data().MaxX();
		if (d > mx)
			mx = d;
	}
	return mx;
}

double ScatterDraw::GetSeriesMinX() {
	if (series.IsEmpty())
		return Null;
	double mn = series[0].Data().MinX();
	for (int i = 1; i < series.size(); ++i) {
		double d = series[i].Data().MinX();
		if (d < mn)
			mn = d;
	}
	return mn;
}

double ScatterDraw::GetSeriesMaxY() {
	if (series.IsEmpty())
		return Null;
	double mx = series[0].Data().MaxY();
	for (int i = 1; i < series.size(); ++i) {
		double d = series[i].Data().MaxY();
		if (d > mx)
			mx = d;
	}
	return mx;
}

double ScatterDraw::GetSeriesMinY() {
	if (series.IsEmpty())
		return Null;
	double mn = series[0].Data().MinY();
	for (int i = 1; i < series.size(); ++i) {
		double d = series[i].Data().MinY();
		if (d < mn)
			mn = d;
	}
	return mn;
}

void ScatterDraw::InitPlot() {
	plotW = size.cx - fround((hPlotLeft + hPlotRight)*plotScaleX);
	plotH = size.cy - fround((vPlotTop + vPlotBottom)*plotScaleY) - titleHeight;
	
	gridX.Clear();
	gridXLog.Clear();
	
	if (SetGridLinesX)
		SetGridLinesX(gridX);
	else if (logX) {
		int count = 0;
		for(int i = 0; xMinUnit + i*xMajorUnit <= xRange && count < 100; i++) 
			gridX << xMinUnit + i*xMajorUnit;
		count = 0;
		for(int i = -1; xMinUnit + i*xMajorUnit <= xRange && count < 100; i++, count++) {		// Log subgrid
			for (int ii = 1; ii < 10; ++ii) {
				double d = xMinUnit + (i + log10(ii))*xMajorUnit;
				if (d > 0)
					gridXLog << d;
			}
		}
	} else {
		if (xMajorUnit > 0) {
			int count = 0;
			for(int i = 0; xMinUnit + i*xMajorUnit <= xRange && count < 100; i++, count++) // 100 max just in case
				gridX << xMinUnit + i*xMajorUnit;
			ASSERT(count <= 100);
		}
	}

	gridY.Clear();
	gridYLog.Clear();
	
	if (SetGridLinesY)
		SetGridLinesY(gridY);
	else if (logY) {
		int count = 0;
		for(int i = 0; yMinUnit + i*yMajorUnit <= yRange && count < 100; i++) 
			gridY << yMinUnit + i*yMajorUnit;
		count = 0;
		for(int i = -1; yMinUnit + i*yMajorUnit <= yRange && count < 100; i++, count++) {		// Log subgrid
			for (int ii = 1; ii < 10; ++ii) {
				double d = yMinUnit + (i + log10(ii))*yMajorUnit;
				if (d > 0)
					gridYLog << d;
			}
		}
	} else {
		if (yMajorUnit > 0) {
			int count = 0;
			for(int i = 0; yMinUnit + i*yMajorUnit <= yRange && count < 100; i++, count++) // 100 max just in case
				gridY << yMinUnit + i*yMajorUnit;
			ASSERT(count < 100);
		}
	}
}

bool IsQTF(const String &str) {
	return str[0] == '\1' || str[0] == '[';
}

const String ScatterDraw::LINE_SOLID 	     = "";
const String ScatterDraw::LINE_NONE 		 = "0";
const String ScatterDraw::LINE_DOTTED_FINER = "2 10";
const String ScatterDraw::LINE_DOTTED_FINE  = "2 6";
const String ScatterDraw::LINE_DOTTED 	     = "4 10";
const String ScatterDraw::LINE_DOTTED_SEP   = "4 20";
const String ScatterDraw::LINE_DASHED 	     = "12 12";
const String ScatterDraw::LINE_DASHED_LONG  = "12 4";
const String ScatterDraw::LINE_DASH_DOT 	 = "12 8 3 8";	// Reduced. Previous was too long
const String ScatterDraw::LINE_BEGIN_END	 = "2";
	
struct ScatterDraw::StaticConstructor {
	StaticConstructor() {
		ONCELOCK {
			SeriesPlot::Register<LineSeriesPlot>("Line");
			SeriesPlot::Register<StaggeredSeriesPlot>("Staggered");
			SeriesPlot::Register<BarSeriesPlot>("Bar");
			
			MarkPlot::Register<CircleMarkPlot>("Circle");
			MarkPlot::Register<SquareMarkPlot>("Square");
			MarkPlot::Register<TriangleMarkPlot>("Triangle");
			MarkPlot::Register<CrossMarkPlot>("Cross");
			MarkPlot::Register<XMarkPlot>("X mark");
			MarkPlot::Register<RhombMarkPlot>("Rhomb");
			MarkPlot::Register<RangePlot>("Range");
			MarkPlot::Register<BubblePlot>("Bubble");
			MarkPlot::Register<InvTriangleMarkPlot>("Inv. Triangle");
			MarkPlot::Register<AsteriskMarkPlot>("Asterisk");
			MarkPlot::Register<DiamondMarkPlot>("Diamond");
	
			DashStyle::Register("LINE_SOLID", ScatterDraw::LINE_SOLID);
			DashStyle::Register("LINE_DOTTED_FINER", ScatterDraw::LINE_DOTTED_FINER);
			DashStyle::Register("LINE_DOTTED_FINE", ScatterDraw::LINE_DOTTED_FINE);
			DashStyle::Register("LINE_DOTTED", ScatterDraw::LINE_DOTTED);
			DashStyle::Register("LINE_DOTTED_SEP", ScatterDraw::LINE_DOTTED_SEP);
			DashStyle::Register("LINE_DASHED", ScatterDraw::LINE_DASHED);
			DashStyle::Register("LINE_DASH_DOT", ScatterDraw::LINE_DASH_DOT);
			DashStyle::Register("LINE_BEGIN_END", ScatterDraw::LINE_BEGIN_END);
			DashStyle::Register("LINE_NONE", ScatterDraw::LINE_NONE);
		}
	}
};
ScatterDraw::StaticConstructor ScatterDraw::staticConstructorInstance;

}
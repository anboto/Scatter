// SPDX-License-Identifier: Apache-2.0
// Copyright 2021 - 2022, the Anboto author and contributors
#ifndef _ScatterDraw_ScatterDraw_h
#define _ScatterDraw_ScatterDraw_h

#include <Draw/Draw.h>
#include <Painter/Painter.h>
#include "DataSource.h"
#include "Equation.h"
#include "DrawingFunctions.h"
#include "SeriesPlot.h"
#include "MarkPlot.h"
#include <Functions4U/Functions4U.h>
#include <immintrin.h>		// For _mm_cvttsd_si32() and _mm_set_sd()

namespace Upp {

Color GetOpaqueColor(const Color &color, const Color &background, double opacity);

void debug_h();			// Dummy function used to debug .h files

class DashStyle {
public:
	static int Register(const String& name, String style) {
		return map().FindAdd(name, style);
	}
	static void Unregister(const String& name) {
		int id = TypeIndex(name);
		ASSERT(id >= 0);
		map().Remove(id);
	}
	static void UnregisterFrom(int id) {
		for (int i = GetCount() - 1; i >= id; --i)
			map().Remove(i);
	}
	static String         TypeName(int i)         		{return map().GetKey(i);}
	static String         Style(int i)         			{return map()[i];}
	static int            TypeIndex(const String& name) {return map().Find(name);}
	static int            StyleIndex(const String& style) {
		for (int i = 0; i < GetCount(); ++i) {
			if (map()[i] == style)
				return i;
		}
		return -1;
	}
	static int            GetCount()   		{return map().GetCount();}
	     
protected:
	static VectorMap<String, String>& map()	{static VectorMap<String, String> map; 	 return map;}
};

enum RAINBOW {BLUE_YELLOW_RED, RED_YELLOW_BLUE, GREEN_YELLOW_RED, RED_YELLOW_GREEN, 
			  BLUE_WHITE_RED, RED_WHITE_BLUE, WHITE_BLACK, BLACK_WHITE};

Color GetRainbowColor(double frac, RAINBOW rainbow, int numScales);
Color GetRainbowColor(double frac, const Color &from, const Color &to, int numScales);
Color GetRainbowColor(double frac, const Color &col0, const Color &col1, const Color &col2, int numScales);

class ScatterDraw {
public:
	ScatterDraw();
	virtual ~ScatterDraw() noexcept {}
	
	enum Formats {
		EXP,
		MON,
		DY,		
		CUSTOM
	};
	enum {
		MD_DRAW		   = -1,
		MD_ANTIALIASED = MODE_ANTIALIASED,
		MD_NOAA        = MODE_NOAA,
		MD_SUBPIXEL    = MODE_SUBPIXEL
	};
	
	static const String LINE_SOLID;
	static const String LINE_NONE;
	static const String LINE_DOTTED_FINER;
	static const String LINE_DOTTED_FINE;
	static const String LINE_DOTTED;
	static const String LINE_DOTTED_SEP;
	static const String LINE_DASHED;
	static const String LINE_DASHED_LONG;
	static const String LINE_DASH_DOT;
	static const String LINE_BEGIN_END;
	
protected:
	class ScatterBasicSeries {
	public:
		ScatterBasicSeries();
		void Init(int index);
		
		bool primaryY;
		bool sequential;
		
		One<SeriesPlot> seriesPlot;
		double thickness;
		Color color;
		String dash;
		
		One<MarkPlot> markPlot;
		double markWidth;
		Color markColor;
		double markBorderWidth;
		Color markBorderColor;
		
		Color fillColor;
		
		String legend;
		String unitsX, unitsY;
		
		double opacity;
		
		double barWidth;
		bool isClosed;

		Vector<String> *labels;
		Font labelsFont;
		Color labelsColor;
		int labelsDx, labelsDy;
		Alignment labelsAlign;
		
		bool showLegend;
		bool legendLine; // show line in legend even if series is NoPlot
		
		int angle;
		
		int id;
	
	private:
		template <class T>
		void Ize(T& io) {
			int seriesP = Null;
			int markP = Null;
			if (io.IsStoring()) {
				if (markPlot)
					markP = markPlot->GetType();
				if (seriesPlot)
					seriesP = seriesPlot->GetType();
			}
			io
				("primaryY", primaryY)
				("sequential", sequential)
				("thickness", thickness)
				("color", color)
				("dash", dash)
				("markWidth", markWidth)
				("markColor", markColor)
				("markBorderWidth", markBorderWidth)
				("markBorderColor", markBorderColor)
				("fillColor", fillColor)
				("markBorderColor", markBorderColor)
				("legend", legend)
				("unitsX", unitsX)
				("unitsY", unitsY)
				("opacity", opacity)
				("id", id)
				("seriesP", seriesP)
				("markPlot", markP)
				("seriesPlot", seriesP)
				("barWidth", barWidth)
				("isClosed", isClosed)
				("showLegend", showLegend)
				("legendLine", legendLine)
			;
			if (io.IsLoading()) {
				if (!IsNull(markP))
					markPlot = MarkPlot::Create(markP);
				else
					markPlot = 0;
				if (!IsNull(seriesP))
					seriesPlot = SeriesPlot::Create(seriesP);
				else
					seriesPlot = 0;
			}
		}
	
	public:
		void Xmlize(XmlIO& xml) 	{Ize(xml);}
		void Jsonize(JsonIO& json) 	{Ize(json);}

		void Serialize(Stream& s) {
			int seriesP = Null;
			int markP = Null;
			if (s.IsStoring()) {
				if (markPlot)
					markP = markPlot->GetType();
				if (seriesPlot)
					seriesP = seriesPlot->GetType();
			}
			s	% primaryY
				% sequential
				% thickness
				% color
				% dash
				% markWidth
				% markColor
				% markBorderWidth
				% markBorderColor
				% fillColor
				% markBorderColor
				% legend
				% unitsX
				% unitsY
				% opacity
				% id
				% seriesP
				% markP
				% seriesP
				% barWidth
				% isClosed
				% showLegend
				% legendLine
			;
			if (s.IsLoading()) {
				if (!IsNull(markP))
					markPlot = MarkPlot::Create(markP);
				else
					markPlot = 0;
				if (!IsNull(seriesP))
					seriesPlot = SeriesPlot::Create(seriesP);
				else
					seriesPlot = 0;
			}
		} 
	};
		
	class ScatterSeries : public Moveable<ScatterSeries>, public ScatterBasicSeries {
	public:
		ScatterSeries()	: owns(false), serializeData(false) {dataS.Init(&data);}
		void SetDataSource(DataSource *pointsData, bool ownsData = true) {
			DeletePD();
			pD = pointsData; 
			owns = ownsData;
		}
		void SetDataSourceInternal() {
			CopyInternal();
			DeletePD();
			pD = &dataS;
			serializeData = true;
			owns = false;
		}
		DataSource &Data()		 				{return *(~pD);}
		const DataSource &Data() const	 		{return *(~pD);}
		bool IsDeleted() const {
			if (~pD == nullptr)
				return true;
			bool ismagic = (~pD)->IsMagic();
			ASSERT(ismagic);
			return !ismagic;
		}
		virtual ~ScatterSeries() noexcept {
			DeletePD();
		}
		void SerializeData(bool ser = true) 	{serializeData = ser;}
		void SerializeFormat(bool ser = false) 	{serializeFormat = ser;}
		void Xmlize(XmlIO& xio) 				{XmlizeByJsonize(xio, *this);}
		void Jsonize(JsonIO& json) {
			ScatterBasicSeries::Jsonize(json);
			if (json.IsStoring() && pD) 
				CopyInternal();
			json("data", data);
			if (json.IsLoading()) {
				if (!data.IsEmpty()) {
					pD = &dataS;
					serializeData = true;
				}
			}
		}
		void Serialize(Stream& s) { 
			ScatterBasicSeries::Serialize(s);
			if (s.IsStoring() && pD) 
				CopyInternal();
			s % data;
			if (s.IsLoading()) {
				if (!data.IsEmpty()) {
					pD = &dataS;
					serializeData = true;
				}
			}
		}
		
	private:
		Ptr<DataSource> pD;
		bool owns;
		Vector<Pointf> data;
		VectorPointf dataS;
		bool serializeData, serializeFormat;
		
		void CopyInternal() {
			ASSERT_(!pD->IsExplicit(), "Explicit series cannot be internal");
			int64 sz = pD->GetCount();
			if (IsNull(sz) || sz == 0)
				return;
			data.SetCount(int(sz));
			for (int64 i = 0; i < sz; ++i) {
				data[int(i)].x = pD->x(i);
				data[int(i)].y = pD->y(i);
			}
		}
		bool IsUsed()	{return pD && owns;}
		void DeletePD() {
			if(IsUsed()) {
				delete pD;
				pD = 0;
			}
		}
	};
	
	void WhenPaint(Painter &w)	{WhenPainter(w);}
	void WhenPaint(Draw &w) 	{WhenDraw(w);}
	
	int maxRefresh_ms = 500;
	
public:	
	template<class T>
	void SetDrawing(T& w, bool ctrl = false);
	
	void InitPlot();
	
	ScatterDraw& SetMaxRefreshTime(int _maxRefresh_ms) 	{maxRefresh_ms = _maxRefresh_ms; return *this;}
	int GetMaxRefreshTime() 							{return maxRefresh_ms;}
	
	static Color GetNewColor(int index, int version = 1);
	static String GetNewDash(int index);
	static MarkPlot *GetNewMarkPlot(int index);
	
	Function<void(String&, int, double)> cbModifFormatX;
	Function<void(String&, int, double)> cbModifFormatXGridUnits;
	Function<void(String&, int, double)> cbModifFormatDeltaX;
	Function<void(String&, int, double)> cbModifFormatY;
	Function<void(String&, int, double)> cbModifFormatYGridUnits;
	Function<void(String&, int, double)> cbModifFormatDeltaY;
	Function<void(String&, int, double)> cbModifFormatY2;
	Function<void(String&, int, double)> cbModifFormatY2GridUnits;
	Function<void(String&, int, double)> cbModifFormatDeltaY2;
	
	Function<void(Vector<double>&)> SetGridLinesX;
	Function<void(Vector<double>&)> SetGridLinesY;
	
	Function<void()> WhenZoomScroll;
	Function<void()> WhenSetRange;
	Function<void()> WhenSetXYMin;
	Function<void(Painter&)> WhenPainter;
	Function<void(Draw&)> WhenDraw;
	Function<void()> WhenZoomToFit;
	
	Function <bool(int)> WhenRemoveSeries;
	Function <bool(int, int)> WhenSwapSeries;
	
	ScatterDraw& SetSize(const Size &sz) {
		size = sz; 
		plotScaleX   = responsive ? responsivenessFactor*size.cx/600. : 1;
		plotScaleY   = responsive ? responsivenessFactor*size.cy/400. : 1;
		plotScaleAvg = responsive ? (plotScaleX + plotScaleY)/2. : 1;
		return *this;
	};
	Size GetSize() const				{return size;};
	ScatterDraw& Responsive(bool _responsive = true, double factor = 1) {
		this->responsive = _responsive;
		responsivenessFactor = factor;
		plotScaleX   = _responsive ? responsivenessFactor*size.cx/600. : 1;
		plotScaleY   = _responsive ? responsivenessFactor*size.cy/400. : 1;
		plotScaleAvg = _responsive ? (plotScaleX + plotScaleY)/2. : 1;
		return *this;
	}
	bool IsResponsive()		{return responsive;}
	double GetResponsivenessFactor() {return responsivenessFactor;}
	double GetPlotScaleX()	{return plotScaleX;}
	double GetPlotScaleY()	{return plotScaleY;}
	double GetPlotScaleAvg(){return plotScaleAvg;}
	
	ScatterDraw& SetColor(const Color& _color);
	ScatterDraw& SetTitle(const String& _title);
	const String& GetTitle();
	ScatterDraw& SetTitleFont(const Upp::Font& fontTitle);
	Upp::Font& GetTitleFont() {return titleFont;};
	ScatterDraw& SetTitleColor(const Color& colorTitle);
	Upp::Color& GetTitleColor() {return titleColor;};
	
	ScatterDraw& SetLabels(const String& _xLabel, const String& _yLabel, const String& _yLabel2 = "");
	ScatterDraw& SetLabelX(const String& _xLabel);
	const String &GetLabelX()	{return xLabel_base;} 
	ScatterDraw& SetLabelY(const String& _yLabel);
	const String &GetLabelY()	{return yLabel_base;} 
	ScatterDraw& SetLabelYOrientation(bool angle)		{yLabel_orientation = angle; return *this;}
	bool GetLabelYOrientation()	{return yLabel_orientation;} 
	ScatterDraw& SetLabelY2(const String& _yLabel);
	const String &GetLabelY2()	{return yLabel2_base;}
	ScatterDraw& SetLabelY2Orientation(bool angle)		{yLabel2_orientation = angle; return *this;}
	bool GetLabelY2Orientation()	{return yLabel2_orientation;} 
	ScatterDraw& SetLabelsFont(const Upp::Font& fontLabels);
	Upp::Font GetLabelsFont() 	{return labelsFont;};
	ScatterDraw& SetLabelsColor(const Color& colorLabels);
	Upp::Color GetLabelsColor() {return labelsColor;};
	ScatterDraw& SetGridFont(const Upp::Font& fontGrid);
	Upp::Font GetGridFont() 	{return reticleFont;};
	
	ScatterDraw& SetPlotAreaMargin(int hLeft, int hRight, int vTop, int vBottom);
	ScatterDraw& SetPlotAreaLeftMargin(int margin);	
	int GetPlotAreaLeftMargin()						{return hPlotLeft;}
	ScatterDraw& SetPlotAreaRightMargin(int margin);	
	int GetPlotAreaRightMargin()					{return hPlotRight;}
	ScatterDraw& SetPlotAreaTopMargin(int margin);	
	int GetPlotAreaTopMargin()						{return vPlotTop;}
	ScatterDraw& SetPlotAreaBottomMargin(int margin);
	int GetPlotAreaBottomMargin()					{return vPlotBottom;}
	
	ScatterDraw& SetMargin(int hLeft, int hRight, int vTop, int vBottom) {return SetPlotAreaMargin(hLeft, hRight, vTop, vBottom);}
	ScatterDraw& SetLeftMargin(int margin)  {return SetPlotAreaLeftMargin(margin);}
	int GetLeftMargin()						{return hPlotLeft;}
	ScatterDraw& SetRightMargin(int margin)	{return SetPlotAreaRightMargin(margin);}	
	int GetRightMargin()					{return hPlotRight;}
	ScatterDraw& SetTopMargin(int margin)	{return SetPlotAreaTopMargin(margin);}
	int GetTopMargin()						{return vPlotTop;}
	ScatterDraw& SetBottomMargin(int margin){return SetPlotAreaBottomMargin(margin);}
	int GetBottomMargin()					{return vPlotBottom;}
	
	ScatterDraw& SetPlotAreaColor(const Color& p_a_color);
	Color& GetPlotAreaColor()						{return plotAreaColor;}
	
	ScatterDraw& SetAxisColor(const Color& axis_color);
	ScatterDraw& SetAxisWidth(double axis_width);
	double GetAxisWidth()							{return axisWidth;}
	
	ScatterDraw& SetGridColor(const Color& grid_color){gridColor = grid_color;	return *this;}
	Color &GetGridColor() 							{return gridColor;}
	ScatterDraw& SetGridWidth(double grid_width)	{gridWidth = grid_width;	return *this;}
	double GetGridWidth() 							{return gridWidth;}
	ScatterDraw& SetGridDash(const char *dash)		{ASSERT(CheckDash(dash));	gridDash = dash;	return *this;}
	const char *GetGridDash()						{return gridDash;}
	
	ScatterDraw& ShowVGrid(bool show);
	ScatterDraw& ShowHGrid(bool show);
	
	ScatterDraw& ShowLegend(bool show = true) 		{showLegend = show;		return *this;}
	bool GetShowLegend()							{return showLegend;}
	ScatterDraw& NoShowLegend()						{return ShowLegend(false);}
	ScatterDraw& SetLegendPos(const Point &pos) 	{legendPos = pos;		return *this;}
	ScatterDraw& SetLegendPosX(int x) 				{legendPos.x = x;		return *this;}
	ScatterDraw& SetLegendPosY(int y) 				{legendPos.y = y;		return *this;}
	Point& GetLegendPos() 							{return legendPos;}
	ScatterDraw& SetLegendFont(const Font &fnt) 	{legendFont = fnt;		return *this;}
	Font &GetLegendFont()							{return legendFont;}
	ScatterDraw& SetLegendNumCols(int num) 			{legendNumCols = num;	return *this;}
	int GetLegendNumCols() 							{return legendNumCols;}
	ScatterDraw& SetLegendRowSpacing(int num) 		{legendRowSpacing = num;return *this;}
	int GetLegendRowSpacing() 						{return legendRowSpacing;}
	enum LEGEND_POS {
		TOP,
		LEFT_TOP, 
		RIGHT_TOP, 
		LEFT_BOTTOM, 
		RIGHT_BOTTOM
	};
	ScatterDraw& SetLegendAnchor(LEGEND_POS anchor) 		{legendAnchor = anchor;	return *this;}
	LEGEND_POS GetLegendAnchor() 							{return legendAnchor;}
	ScatterDraw& SetLegendFillColor(const Color &color) 	{legendFillColor = color;	return *this;}
	ScatterDraw& SetLegendBorderColor(const Color &color) 	{legendBorderColor = color;	return *this;}	
	Color& GetLegendFillColor() 							{return legendFillColor;}
	Color& GetLegendBorderColor() 							{return legendBorderColor;}
	
	ScatterDraw& SetLegendWithUnits(bool show = true) 		{legend_w_units = show;		return *this;}
	bool GetLegendWithUnits()								{return legend_w_units;}
	
	ScatterDraw& SetSciExpTop(bool top = true) 				{sciExpTop = top;		return *this;}
	bool GetSciExpTop()										{return sciExpTop;}
	
	ScatterDraw& SetMode(int _mode = MD_ANTIALIASED)		{mode = _mode; Refresh(); return *this;};
	int GetMode()											{return mode;};
	
	ScatterDraw &ZoomToFit(bool horizontal, bool vertical, double factorH, double factorV);
	ScatterDraw &ZoomToFit(bool horizontal = true, bool vertical = false, double factor = 0) {
		return ZoomToFit(horizontal, vertical, factor, factor);
	}
	//ScatterDraw &ZoomToFit(bool horizontal, double minx, double maxx, bool vertical, double minxy, double maxy, 
	//				bool vertical2, double miny2, double maxy2, double factor);
	//ScatterDraw &ZoomToFitSmart(bool horizontal, double minx, double maxx, bool vertical, double minxy, double maxy, 
	//				bool vertical2, double miny2, double maxy2, double factor);
	ScatterDraw &ZoomToFitNonLinked(bool horizontal, bool vertical, double factorH, double factorV);
	//ScatterDraw &DoFitToData(bool horizontal, double minx, double maxx, bool vertical, double minxy, double maxy, 
	//				bool vertical2, double miny2, double maxy2, double factor = 0);
	//ScatterDraw &DoFitToDataSmart(bool horizontal, double minx, double maxx, bool vertical, double minxy, double maxy, 
	//				bool vertical2, double miny2, double maxy2, double factor = 0);
	
	void Zoom(double scale, bool hor = true, bool ver = true); 
	void ZoomNonLinked(double scale, bool hor, bool ver); 
	
	void Scroll(double factorX, double factorY);
	void ScrollNonLinked(double factorX, double factorY);
	
	enum ZoomStyle {TO_CENTER, FROM_BASE};
	ScatterDraw &SetZoomStyleX(ZoomStyle style = TO_CENTER) {zoomStyleX = style; return *this;}
	ScatterDraw &SetZoomStyleY(ZoomStyle style = TO_CENTER) {zoomStyleY = style; return *this;}

	ScatterDraw& SetRange(double rx, double ry = Null, double ry2 = Null);
	ScatterDraw &SetRange(double rxmax, double rxmin, double rymax, double rymin, double ry2max, double ry2min);
	ScatterDraw& SetRangeLinked(double rx, double ry, double ry2 = 100);
	ScatterDraw &SetRangeLinked(double rxmax, double rxmin, double rymax, double rymin, double ry2max, double ry2min);
	double GetXRange()const 	{return xRange;}
	double GetYRange()const 	{return yRange;}
	double GetY2Range()const 	{return yRange2;}
	ScatterDraw &SetMajorUnits(double ux, double uy = Null, double uy2 = Null);
	ScatterDraw &SetMajorUnitsNum(int nx, int ny = Null);
	double GetMajorUnitsX() 	{return xMajorUnit;}
	double GetMajorUnitsY() 	{return yMajorUnit;}
	double GetMajorUnitsY2() 	{return yMajorUnit2;}
	ScatterDraw& SetMinUnits(double ux, double uy = Null);
	double GetXMinUnit() const  {return xMinUnit;}
	double GetYMinUnit() const  {return yMinUnit;}	
	double GetYMinUnit2() const {return yMinUnit2;}	
	
	ScatterDraw& SetXYMin(double xmin, double ymin = Null, double ymin2 = Null);
	ScatterDraw& SetXYMinLinked(double xmin, double ymin = Null, double ymin2 = Null);
	double GetXMin() const 		{return FixPow10(xMin, logX);}
	double GetYMin() const 		{return FixPow10(yMin, logY);}	
	double GetYMin2() const 	{return FixPow10(yMin2, logY);}
	double GetY2Min() const 	{return FixPow10(yMin2, logY);}
	ScatterDraw& SetXYMax(double xmax, double ymax = Null, double ymax2 = Null);
	ScatterDraw& SetXYMaxLinked(double xmax, double ymax = Null, double ymax2 = Null);
	double GetXMax() const 		{return FixPow10(xMin + xRange, logX);}
	double GetYMax() const 		{return FixPow10(yMin + yRange, logY);}	
	double GetYMax2() const 	{return FixPow10(yMin2 + yRange2, logY);}
	double GetY2Max() const 	{return FixPow10(yMin2 + yRange2, logY);}
	
	ScatterDraw &Graduation_FormatX(Formats fi);	
	ScatterDraw &Graduation_FormatY(Formats fi);
	ScatterDraw &Graduation_FormatY2(Formats fi);
	
	//ScatterDraw &SetPolar(bool polar = true)			{isPolar = polar; 	return *this;};
	
	ScatterDraw &AddSeries(double *yData, int numData, double x0, double deltaX)
														{return AddSeries<CArray>(yData, numData, x0, deltaX);}
	ScatterDraw &AddSeries(double *xData, double *yData, int numData)
														{return AddSeries<CArray>(xData, yData, numData);}
	ScatterDraw &AddSeries(Eigen::VectorXd &yData, double x0, double deltaX)
														{return AddSeries<EigenVector>(yData, x0, deltaX);}
	ScatterDraw &AddSeries(Eigen::VectorXd &xData, Eigen::VectorXd &yData)
														{return AddSeries<EigenVector>(xData, yData);}
	ScatterDraw &AddSeries(Vector<double> &xData, Vector<double> &yData)
														{return AddSeries<VectorXY>(xData, yData);}
	ScatterDraw &AddSeries(Array<double> &xData, Array<double> &yData)
														{return AddSeries<ArrayXY>(xData, yData);}		
	ScatterDraw &AddSeries(Vector<Pointf> &points)		{return AddSeries<VectorPointf>(points);}
	ScatterDraw &AddSeries(Array<Pointf> &points)		{return AddSeries<ArrayPointf>(points);}
	template <class Y>
	ScatterDraw &AddSeries(Vector<Vector <Y> > &data, int idx, int idy, 
		Vector<int> &idsx, Vector<int> &idsy, Vector<int> &idsFixed, bool useCols = true, int beginData = 0, int numData = Null, double gainX = 1) {
		return AddSeries<VectorVectorY<Y> >(data, idx, idy, idsx, idsy, idsFixed, useCols, beginData, numData, gainX);
	}
	ScatterDraw &AddSeries(Function <double(double)> function)	{return AddSeries<FuncSource>(function);}
	ScatterDraw &AddSeries(double (*function)(double))			{return AddSeries<FuncSource>(function);}
	ScatterDraw &AddSeries(void (*function)(double&, double))
														{return AddSeries<FuncSourceV>(function);}
	ScatterDraw &AddSeries(Pointf (*function)(double), int np, double from = 0, double to = 1)	
														{return AddSeries<FuncSourcePara>(function, np, from, to);}
	ScatterDraw &AddSeries(PlotExplicFunc function)		{return AddSeries<PlotExplicFuncSource>(function);}
	ScatterDraw &AddSeries(PlotParamFunc function, int np, double from = 0, double to = 1)	
														{return AddSeries<PlotParamFuncSource>(function, np, from, to);}
	
	ScatterDraw &AddSeries(DataSource &data);

	template <class C>
	ScatterDraw &AddSeries() 	{return _AddSeries(new C());}	
	template <class C, class T1>
	ScatterDraw &AddSeries(T1 &arg1) 			
								{return _AddSeries(new C(arg1));}
	template <class C, class T1, class T2>
	ScatterDraw &AddSeries(T1 &arg1, T2 &arg2) 	
								{return _AddSeries(new C(arg1, arg2));}
	template <class C, class T1, class T2, class T3>
	ScatterDraw &AddSeries(T1 &arg1, T2 &arg2, T3 &arg3) 								
								{return _AddSeries(new C(arg1, arg2, arg3));}
	template <class C, class T1, class T2, class T3, class T4>
	ScatterDraw &AddSeries(T1 &arg1, T2 &arg2, T3 &arg3, T4 &arg4) 						
								{return _AddSeries(new C(arg1, arg2, arg3, arg4));}
	template <class C, class T1, class T2, class T3, class T4, class T5>
	ScatterDraw &AddSeries(T1 &arg1, T2 &arg2, T3 &arg3, T4 &arg4, T5 &arg5)			
								{return _AddSeries(new C(arg1, arg2, arg3, arg4, arg5));}
	template <class C, class T1, class T2, class T3, class T4, class T5, class T6>
	ScatterDraw &AddSeries(T1 &arg1, T2 &arg2, T3 &arg3, T4 &arg4, T5 &arg5, T6 &arg6)	
								{return _AddSeries(new C(arg1, arg2, arg3, arg4, arg5, arg6));}
	template <class C, class T1, class T2, class T3, class T4, class T5, class T6, class T7>
	ScatterDraw &AddSeries(T1 &arg1, T2 &arg2, T3 &arg3, T4 &arg4, T5 &arg5, T6 &arg6, T7 &arg7)	
								{return _AddSeries(new C(arg1, arg2, arg3, arg4, arg5, arg6, arg7));}
	template <class C, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
	ScatterDraw &AddSeries(T1 &arg1, T2 &arg2, T3 &arg3, T4 &arg4, T5 &arg5, T6 &arg6, T7 &arg7, T8 &arg8)	
								{return _AddSeries(new C(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8));}									
	template <class C, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
	ScatterDraw &AddSeries(T1 &arg1, T2 &arg2, T3 &arg3, T4 &arg4, T5 &arg5, T6 &arg6, T7 &arg7, T8 &arg8, T9 &arg9)	
								{return _AddSeries(new C(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9));}	
	template <class C, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10>
	ScatterDraw &AddSeries(T1 &arg1, T2 &arg2, T3 &arg3, T4 &arg4, T5 &arg5, T6 &arg6, T7 &arg7, T8 &arg8, T9 &arg9, T10 &arg10)	
								{return _AddSeries(new C(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10));}
																							
	template <class Y>
	ScatterDraw &AddSeries(Vector<Y> &yData, double x0, double deltaX)		{return _AddSeries(new VectorY<Y>(yData, x0, deltaX));}
	template <class Y>
	ScatterDraw &AddSeries(Array<Y> &yData, double x0, double deltaX)		{return _AddSeries(new ArrayY<Y>(yData, x0, deltaX));}
	template <class X, class Y>
	ScatterDraw &AddSeries(VectorMap<X, Y> &data)	{return _AddSeries(new VectorMapXY<X, Y>(data));}
	template <class X, class Y>
	ScatterDraw &AddSeries(ArrayMap<X, Y> &data)	{return _AddSeries(new ArrayMapXY<X, Y>(data));}
	
	DataSource &GetDataSource(int index) 	{ASSERT(IsValid(index));ASSERT(!series[index].IsDeleted());	return series[index].Data();}
	bool IsDeletedDataSource(int index) 	{return series[index].IsDeleted();}	
	
	ScatterDraw &InsertSeries(int index, double *yData, int numData, double x0 = 0, double deltaX = 1);
	ScatterDraw &InsertSeries(int index, double *xData, double *yData, int numData);
	ScatterDraw &InsertSeries(int index, Vector<double> &xData, Vector<double> &yData);
	ScatterDraw &InsertSeries(int index, Array<double> &xData, Array<double> &yData);
	ScatterDraw &InsertSeries(int index, Vector<Pointf> &points);
	ScatterDraw &InsertSeries(int index, Array<Pointf> &points);
	ScatterDraw &InsertSeries(int index, double (*function)(double));
	ScatterDraw &InsertSeries(int index, Pointf (*function)(double), int np, double from = 0, double to = 1);
	ScatterDraw &InsertSeries(int index, PlotExplicFunc &function);
	ScatterDraw &InsertSeries(int index, PlotParamFunc function, int np, double from = 0, double to = 1);
	ScatterDraw &_InsertSeries(int index, DataSource *data);
	
	template <class C>
	ScatterDraw &InsertSeries(int index) 	{return _InsertSeries(index, new C());}	
	template <class C, class T1>
	ScatterDraw &InsertSeries(int index, T1 &arg1) 				
									{return _InsertSeries(index, new C(arg1));}
	template <class C, class T1, class T2>
	ScatterDraw &InsertSeries(int index, T1 &arg1, T2 &arg2) 		
									{return _InsertSeries(index, new C(arg1, arg2));}
	template <class C, class T1, class T2, class T3>
	ScatterDraw &InsertSeries(int index, T1 &arg1, T2 &arg2, T3 &arg3) 								
									{return _InsertSeries(index, new C(arg1, arg2, arg3));}
	template <class C, class T1, class T2, class T3, class T4>
	ScatterDraw &InsertSeries(int index, T1 &arg1, T2 &arg2, T3 &arg3, T4 &arg4)						
									{return _InsertSeries(index, new C(arg1, arg2, arg3, arg4));}
	template <class C, class T1, class T2, class T3, class T4, class T5>
	ScatterDraw &InsertSeries(int index, T1 &arg1, T2 &arg2, T3 &arg3, T4 &arg4, T5 &arg5)				
									{return _InsertSeries(index, new C(arg1, arg2, arg3, arg4, arg5));}
	template <class C, class T1, class T2, class T3, class T4, class T5, class T6>
	ScatterDraw &InsertSeries(int index, T1 &arg1, T2 &arg2, T3 &arg3, T4 &arg4, T5 &arg5, T6 &arg6)	
									{return _InsertSeries(index, new C(arg1, arg2, arg3, arg4, arg5, arg6));}

	template <class Y>
	ScatterDraw &InsertSeries(int index, Vector<Y> &yData, double x0, double deltaX)		{return _InsertSeries(index, new VectorY<Y>(yData, x0, deltaX));}
	template <class Y>
	ScatterDraw &InsertSeries(int index, Array<Y> &yData, double x0, double deltaX)	{return _InsertSeries(index, new ArrayY<Y>(yData, x0, deltaX));}
	template <class X, class Y>
	ScatterDraw &InsertSeries(int index, VectorMap<X, Y> &data)	{return _InsertSeries(index, new VectorMapXY<X, Y>(data));}
	template <class X, class Y>
	ScatterDraw &InsertSeries(int index, ArrayMap<X, Y> &data)	{return _InsertSeries(index, new ArrayMapXY<X, Y>(data));}
	
	const ScatterSeries &GetSeries(int i)	{return series[i];}
	
	int64 GetCount(int index);
	void GetValues(int index, int64 idata, double &x, double &y);
	double GetValueX(int index, int64 idata);
	Value GetStringX(int index, int64 idata, bool removeEnter = false);
	double GetValueY(int index, int64 idata);
	Value GetStringY(int index, int64 idata, bool removeEnter = false);
	
	ScatterDraw &SetNoPlot(int index);
	
	ScatterDraw &PlotStyle()								{return PlotStyle(0);};
	template <class C>
	ScatterDraw &PlotStyle()								{return PlotStyle(new C());}
	template <class C, class T1>
	ScatterDraw &PlotStyle(T1 arg1)							{return PlotStyle(new C(arg1));}
	template <class C, class T1, class T2>
	ScatterDraw &PlotStyle(T1 arg1, T2 arg2)				{return PlotStyle(new C(arg1, arg2));}
	template <class C, class T1, class T2, class T3>
	ScatterDraw &PlotStyle(T1 arg1, T2 arg2, T3 arg3)		{return PlotStyle(new C(arg1, arg2, arg3));}		
	
	ScatterDraw &PlotStyle(int index, SeriesPlot *data);
	ScatterDraw &PlotStyle(SeriesPlot *data)				{return PlotStyle(series.GetCount() - 1, data);}
		
	ScatterDraw &NoPlot()	{return PlotStyle();};

	ScatterDraw &Stacked(bool _stacked = true)				{stacked = _stacked; return *this;}

	ScatterDraw &MarkStyle()								{return MarkStyle(0);}
	template <class C>
	ScatterDraw &MarkStyle()								{return MarkStyle(new C());}
	template <class C, class T1>
	ScatterDraw &MarkStyle(T1 arg1)							{return MarkStyle(new C(arg1));}
	template <class C, class T1, class T2>
	ScatterDraw &MarkStyle(T1 arg1, T2 arg2)				{return MarkStyle(new C(arg1, arg2));}
	template <class C, class T1, class T2, class T3>
	ScatterDraw &MarkStyle(T1 arg1, T2 arg2, T3 arg3)		{return MarkStyle(new C(arg1, arg2, arg3));}		
	
	ScatterDraw &MarkStyle(int index, MarkPlot *data);
	ScatterDraw &MarkStyle(MarkPlot *data)					{return MarkStyle(series.GetCount() - 1, data);}
	ScatterDraw &MarkStyle(int index, const String name);
	ScatterDraw &MarkStyle(int index, int typeidx);
	ScatterDraw &MarkStyle(const String name)				{return MarkStyle(series.GetCount() - 1, name);}
	const String GetMarkStyleName(int index);
	ScatterDraw &SetMarkStyleType(int index, int type = Null);
	ScatterDraw &SetMarkStyleType(int type = Null)			{return SetMarkStyleType(series.GetCount() - 1, type);}
	int GetMarkStyleType(int index);
	
	ScatterDraw &NoMark()	{return MarkStyle();};
		
	ScatterDraw &Stroke(int index, double thickness, Color color);
	ScatterDraw &Stroke(double thickness, Color color = Null)   {return Stroke(series.GetCount() - 1, thickness, color);}
	
	void GetStroke(int index, double &thickness, Color &color);
	ScatterDraw &Closed(int index, bool closed);
	ScatterDraw &Closed(bool closed)							{return Closed(series.GetCount() - 1, closed);}
	bool IsClosed(int index);
	ScatterDraw &BarWidth(int index, double width);
	ScatterDraw &BarWidth(double width)							{return BarWidth(series.GetCount() - 1, width);}
	double GetBarWidth(int index);
	ScatterDraw &Dash(const char *dash);
	ScatterDraw &Dash(int index, const char *dash);
	ScatterDraw &NoDash();
	ScatterDraw &NoDash(int index);
	const String GetDash(int index);
	ScatterDraw &Fill(Color color = Null);
	ScatterDraw &MarkColor(Color color = Null);
	ScatterDraw &MarkBorderColor(Color color = Null);
	ScatterDraw &MarkWidth(double markWidth = 8);
	ScatterDraw &MarkBorderWidth(double markWidth = 1);
	ScatterDraw &Hide() {series[series.GetCount() - 1].opacity = 0;	return *this;}
	
	ScatterDraw &ShowSeriesLegend(int index, bool show = false);
	ScatterDraw &ShowSeriesLegend(bool show = false)	{return ShowSeriesLegend(series.GetCount() - 1, show);}
	bool GetShowSeriesLegend(int index)					{ASSERT(IsValid(index));ASSERT(!series[index].IsDeleted());return series[index].showLegend;}
	ScatterDraw &NoSeriesLegend(int index)				{return ShowSeriesLegend(index, false);}
	ScatterDraw &NoSeriesLegend()						{return ShowSeriesLegend(false);}
	
	ScatterDraw &Opacity(double opacity = 1) {series[series.GetCount() - 1].opacity = opacity;	return *this;}
	ScatterDraw &Legend(const String legend);
	ScatterDraw &Legend(int index, const String legend);
	const String &GetLegend(int index);
	ScatterDraw &Units(const String unitsY, const String unitsX = "");
	ScatterDraw &Units(int index, const String unitsY, const String unitsX = "");
	const String GetUnitsX(int index);
	const String GetUnitsY(int index);
	
	ScatterDraw &LegendLine(bool b = true)              { series.Top().legendLine = b; return *this; }
	
	inline bool IsValid(int index) const {return (index >= 0 && index < series.GetCount());}
	
	ScatterDraw &SetDrawXReticle(bool set = true)  	{drawXReticle = set;	return *this;}
	ScatterDraw &SetDrawYReticle(bool set = true)  	{drawYReticle = set;	return *this;}
	ScatterDraw &SetDrawY2Reticle(bool set = true) 	{drawY2Reticle = set;	return *this;}
	ScatterDraw &NoDrawXReticle()  					{return SetDrawXReticle(false);}
	ScatterDraw &NoDrawYReticle()  					{return SetDrawYReticle(false);}
	ScatterDraw &NoDrawY2Reticle() 					{return SetDrawY2Reticle(false);}
	bool GetDrawXReticle()							{return drawXReticle;}
	bool GetDrawYReticle()							{return drawYReticle;}
	bool GetDrawY2Reticle()							{return drawY2Reticle;}
	ScatterDraw &SetDrawXReticleNumbers(bool set = true) {drawXReticleNumbers = set;	return *this;}
	ScatterDraw &SetDrawYReticleNumbers(bool set = true) {drawYReticleNumbers = set;	return *this;}
	ScatterDraw &SetDrawY2ReticleNumbers(bool set = true){drawY2ReticleNumbers = set;	return *this;}
	ScatterDraw &NoDrawXReticleNumbers() 			{return SetDrawXReticleNumbers(false);}
	ScatterDraw &NoDrawYReticleNumbers() 			{return SetDrawYReticleNumbers(false);}
	ScatterDraw &NoDrawY2ReticleNumbers() 			{return SetDrawY2ReticleNumbers(false);}
	bool GetDrawXReticleNumbers()					{return drawXReticleNumbers;}
	bool GetDrawYReticleNumbers()					{return drawYReticleNumbers;}
	bool GetDrawY2ReticleNumbers()					{return drawY2ReticleNumbers;}
	ScatterDraw &SetReticleFont(const Font &fnt)	{reticleFont = fnt;			return *this;}
	Font &GetReticleFont()							{return reticleFont;}
	ScatterDraw &SetReticleColor(const Color &col)	{reticleColor = col;		return *this;}
	Color &GetReticleColor()						{return reticleColor;}
	
	ScatterDraw &SetFillColor(int index, const Color& color);
	ScatterDraw &SetFillColor(const Color& color) {return SetFillColor(series.GetCount() - 1, color);}
	Color GetFillColor(int index) const;

	ScatterDraw &SetMarkWidth(int index, double width);
	ScatterDraw &SetMarkWidth(double width) {return SetMarkWidth(series.GetCount() - 1, width);}
	double GetMarkWidth(int index);
	ScatterDraw &SetMarkColor(int index, const Color& pcolor);
	ScatterDraw &SetMarkColor(const Color& pcolor) {return SetMarkColor(series.GetCount() - 1, pcolor);}
	Color GetMarkColor(int index) const;
	ScatterDraw &SetMarkBorderWidth(int index, double width);
	double GetMarkBorderWidth(int index);
	ScatterDraw &SetMarkBorderColor(int index, const Color& pcolor);
	Color GetMarkBorderColor(int index) const;
	void NoMark(int index);
	bool IsShowMark(int index);
	
	void SetDataPrimaryY(int index, bool primary = true);
	ScatterDraw &SetDataPrimaryY(bool primary = true); 	
	void SetDataSecondaryY(int index, bool secondary = true);
	ScatterDraw &SetDataSecondaryY(bool secondary = true);
	void SetRightY(int index)		{SetDataSecondaryY(index);} 	
	ScatterDraw &SetRightY()		{return SetDataSecondaryY();}
	bool IsDataPrimaryY(int index);
	bool ThereArePrimaryY();	
	bool ThereAreSecondaryY();
	
	ScatterDraw &SetLogX(bool s = true)	{logX = s;	return *this;}
	bool GetLogX()						{return logX;}
	ScatterDraw &SetLogY(bool s = true)	{logY = s;	return *this;}
	bool GetLogY()						{return logY;}
	
	void SetSequentialX(int index, bool sequential);
	ScatterDraw &SetSequentialX(bool sequential = true);
	ScatterDraw &SetSequentialXAll(bool sequential = true);
	bool GetSequentialX(int index);
	bool GetSequentialX();
		
	void Show(int index, bool show = true);
	bool IsVisible(int index);
	ScatterDraw &ShowAll(bool show = true);

	bool RemoveSeries(int index);
	ScatterDraw &RemoveAllSeries();
	
	bool SwapSeries(int i1, int i2);
	
	ScatterDraw& Id(int id);
	ScatterDraw& Id(int index, int id);
	int GetId(int index);
	
	Drawing GetDrawing();
	Image GetImage();
	
	#ifdef PLATFORM_WIN32
	void SaveAsMetafile(const char* file) const;
	#endif
	
	ScatterDraw& SetMinZoom(double x, double y = -1) 	{return SetMinRange(x, y);} 
	ScatterDraw& SetMaxZoom(double x, double y = -1) 	{return SetMaxRange(x, y);}
	ScatterDraw& SetMinRange(double x, double y = -1) 	{minXRange = x; minYRange = y; return *this;} 
	ScatterDraw& SetMaxRange(double x, double y = -1) 	{maxXRange = x; maxYRange = y; return *this;}
	ScatterDraw& SetMinXmin(double val)					{minXmin = val; return *this;}
	ScatterDraw& SetMinYmin(double val)					{minYmin = val; return *this;}
	ScatterDraw& SetMaxXmax(double val)					{maxXmax = val; return *this;}
	ScatterDraw& SetMaxYmax(double val)					{maxYmax = val; return *this;}

	ScatterDraw& SetFastViewX(bool set = true) 			{fastViewX = set;	return *this;}
	bool GetFastViewX() 								{return fastViewX;}
	
	double GetXByPoint(double x);
	double GetYByPoint(double y);
	double GetY2ByPoint(double y);
	double GetXPointByValue(double x);
	double GetYPointByValue(double y);

	double GetSeriesMaxX();
	double GetSeriesMinX();
	double GetSeriesMaxY();
	double GetSeriesMinY();

	int GetCount() 	{return series.GetCount();}
	bool IsEmpty()	{return series.IsEmpty();}
	
	ScatterDraw& LinkedWith(ScatterDraw& ctrl);
	void Unlinked();
	
	double GetMinX()			{return xMin;}
	double GetMaxX()			{return xMin+xRange;}
	double GetMinY()			{return yMin;}
	double GetMaxY()			{return yMin+yRange;}
	int GetPlotWidth()			{return plotW;}
	int GetPlotHeight()			{return plotH;}

	double GetPosX(double x) {
		return SafeDoubleToInt(plotW*(FixLog10(x, logX) - xMin)/xRange);
	}
	double GetPosY(double y, bool primaryY = true) {
		if (primaryY)
			return SafeDoubleToInt(plotH*(1 - (FixLog10(y, logY) - yMin)/yRange));
		else
			return SafeDoubleToInt(plotH*(1 - (FixLog10(y, logY) - yMin2)/yRange2));
	}
	double GetSizeX(double cx) 	{return plotW*cx/xRange;}
	double GetSizeY(double cy) 	{return plotH*cy/yRange;}		
	double GetSizeY2(double cy) {return plotH*cy/yRange2;}
	
	double GetRealPosX(double x) {
		return FixPow10(xMin + (x - hPlotLeft*plotScaleX)*xRange/plotW, logX);
	}
	double GetRealPosY(double y) {
		return FixPow10(yMin + yRange - (y - plotScaleY*vPlotTop - titleHeight)*yRange/plotH, logY);
	}		
	double GetRealPosY2(double y) {
		return FixPow10(yMin2 + yRange2 - (y - plotScaleY*vPlotTop - titleHeight)*yRange2/plotH, logY);
	}		
	
	double GetScatterPosX(double x) {
		return (FixLog10(x, logX) - xMin)*plotW/xRange + hPlotLeft*plotScaleX;
	}
	double GetScatterPosY(double y) {
		return (yMin + yRange - FixLog10(y, logY))*plotH/yRange + plotScaleY*vPlotTop + titleHeight;
	}
	double GetScatterPosY2(double y) {
		return (yMin2 + yRange2 - FixLog10(y, logY))*plotH/yRange2 + plotScaleY*vPlotTop + titleHeight;
	}
	double GetScatterDistance(double dx, double dy)  {return sqrt(sqr(GetScatterPosX(dx)) + sqr(GetScatterPosY(dy)));}
	double GetScatterDistance2(double dx, double dy) {return sqrt(sqr(GetScatterPosX(dx)) + sqr(GetScatterPosY2(dy)));}
		
	double GetPixelThickX()		{return xRange/plotW;}
	double GetPixelThickY()		{return yRange/plotH;}
	
	ScatterDraw& SetMouseHandling(bool valx = true, bool valy = false);
	ScatterDraw& SetMouseHandlingLinked(bool valx = true, bool valy = false);
	bool GetMouseHandlingX()	{return mouseHandlingX;}
	bool GetMouseHandlingY()	{return mouseHandlingY;}
	
	ScatterDraw& AddSurf(DataSourceSurf &surf);
	DataSourceSurf& GetSurf()					{return *psurf;}
	ScatterDraw& RemoveSurf()					{psurf = nullptr;	return *this;}
	bool IsSurf()								{return psurf;}
	ScatterDraw& SetSurfMinZ(double val)		{surfMinZ = val;	return *this;}
	double GetSurfMinZ() const					{return surfMinZ;}
	ScatterDraw& SetSurfMaxZ(double val)		{surfMaxZ = val;	return *this;}
	double GetSurfMaxZ() const					{return surfMaxZ;}
	double GetSurfMinX()						{return psurf->MinX();}
	double GetSurfMinY()						{return psurf->MinY();}
	double GetSurfMaxX()						{return psurf->MaxX();}
	double GetSurfMaxY()						{return psurf->MaxY();}
	ScatterDraw& ZoomToFitZ()					{surfMinZ = psurf->MinZ();	surfMaxZ = psurf->MaxZ(); return *this;}
	
	ScatterDraw& SurfNumColor(int num, bool cont){surfNumColor = num; continuousColor = cont; return *this;}
	ScatterDraw& SurfRainbow(RAINBOW val)		{surfRainbow = val;  return *this;}
	RAINBOW SurfRainbow()						{return surfRainbow;}
	
	ScatterDraw& SurfUnits(const String &units)	{surfUnits = units;	 return *this;}
	enum SURF_UNITS_POS {
		UNITS_TOP,
		UNITS_BOTTOM, 
		UNITS_LEFT, 
		UNITS_RIGHT
	};
	ScatterDraw& SurfUnitsPos(SURF_UNITS_POS pos)			{surfUnitsPos = pos;	return *this;}
	enum SURF_LEGEND_POS {
		LEGEND_LEFT, 
		LEGEND_RIGHT
	};
	ScatterDraw& SurfLegendPos(SURF_LEGEND_POS pos)			{surfLegendPos = pos;	return *this;}
	
	ScatterDraw& ShowRainbowPalette(bool show = true)		{showRainbow = show;	return *this;}
	bool GetShowRainbowPalette()							{return showRainbow;}
	ScatterDraw& SetRainbowPalettePos(const Point &p) 		{rainbowPos = p;		return *this;}	
	Point& GetRainbowPalettePos() 							{return rainbowPos;}
	ScatterDraw& SetRainbowPaletteSize(const Size &sz) 		{rainbowSize = sz;		return *this;}	
	Size& GetRainbowPaletteSize() 							{return rainbowSize;}
	ScatterDraw& SetRainbowPaletteAnchor(LEGEND_POS anchor) {rainbowAnchor = anchor;return *this;}
	LEGEND_POS GetRainbowPaletteAnchor() 					{return rainbowAnchor;}
	ScatterDraw& SetRainbowPaletteBorderColor(const Color &color) 	{rainbowBorderColor = color;return *this;}	
	Color& GetRainbowPaletteBorderColor() 					{return rainbowBorderColor;}
	ScatterDraw& SetRainbowPaletteFont(const Font &fnt) 	{rainbowPaletteFont = fnt;return *this;}	
	Font& GetRainbowPaletteFont() 							{return rainbowPaletteFont;}
	ScatterDraw& SetRainbowPaletteTextColor(const Color &color) 	{rainbowPaletteTextColor = color;return *this;}	
	Color& GetRainbowPaletteTextColor() 					{return rainbowPaletteTextColor;}
	
	ScatterDraw &AddLabelSeries(Vector<String> &labels, int dx = 0, int dy = 0, Font font = StdFont(), 
					Alignment align = ALIGN_CENTER, Color color = SColorText(), int angle = 0) {
		int index = series.GetCount() - 1;
		
		return AddLabelSeries(index, labels, dx, dy, font, align, color, angle);
	}
	ScatterDraw& AddLabelSeries(int index, Vector<String> &labels, int dx = 0, int dy = 0, Font font = StdFont(), 
					Alignment align = ALIGN_CENTER, Color color = SColorText(), int angle = 0) {	
		ASSERT(IsValid(index));
		ASSERT(!series[index].IsDeleted());
		
		series[index].labels = &labels;
		series[index].labelsDx = dx;
		series[index].labelsDy = dy;
		series[index].labelsFont = font;
		series[index].labelsAlign = align;
		series[index].labelsColor = color;
		series[index].angle = angle;
		return *this;
	}
	
	ScatterDraw& SetDataSourceInternal() {
		for (int i = 0; i < series.GetCount(); ++i) {
			ScatterSeries &serie = series[i]; 
			if (serie.IsDeleted())
				continue;
			serie.SetDataSourceInternal();
		}
		return *this;
	}
	ScatterDraw& SerializeData(bool ser = true) {
		for (int i = 0; i < series.GetCount(); ++i) {
			ScatterSeries &serie = series[i]; 
			if (serie.IsDeleted())
				continue;
			serie.SerializeData(ser);
		}
		return *this;
	}
	ScatterDraw& SerializeFormat(bool ser = true) {
		for (int i = 0; i < series.GetCount(); ++i) {
			ScatterSeries &serie = series[i]; 
			if (serie.IsDeleted())
				continue;
			serie.SerializeFormat(ser);
		}
		serializeFormat = ser;
		return *this;
	}
	
	template <class T>
	void Ize(T& io) {
		if (serializeFormat) {
			int intlegendAnchor = 0, intrainbowAnchor = 0, intsurfRainbow = 0, intsurfUnitsPos = 0, intsurfLegendPos = 0;
			if (io.IsStoring()) {
				intlegendAnchor = legendAnchor;
				intrainbowAnchor = rainbowAnchor;
				intsurfRainbow = surfRainbow;
				intsurfUnitsPos = surfUnitsPos;
				intsurfLegendPos = surfLegendPos;
			}
			io
				("title", title)
				("titleFont", titleFont)
				("titleColor", titleColor)
				("titleHeight", titleHeight)
				("xLabel_base", xLabel_base)
				("yLabel_base", yLabel_base)
				("yLabel2_base", yLabel2_base)
				("labelsFont", labelsFont)
				("labelsColor", labelsColor)
				("xRange", xRange)
				("yRange", yRange)
				("yRange2", yRange2)
				("xMin", xMin)
				("yMin", yMin)
				("yMin2", yMin2)
				("xMajorUnit", xMajorUnit)
				("yMajorUnit", yMajorUnit)
				("yMajorUnit2", yMajorUnit2)
				("xMinUnit", xMinUnit)
				("yMinUnit", yMinUnit)
				("yMinUnit2", yMinUnit2)
				("minXRange", minXRange)
				("maxXRange", maxXRange)
				("minYRange", minYRange)
				("maxYRange", maxYRange)
				("minXmin", minXmin)
				("minYmin", minYmin)
				("maxXmax", maxXmax)
				("maxYmax", maxYmax)
				("hPlotLeft", hPlotLeft)
				("hPlotRight", hPlotRight)
				("vPlotTop", vPlotTop)  
				("vPlotBottom", vPlotBottom)
				("size", size)
				("responsive", responsive)
				("legendAnchor", intlegendAnchor)
				("legendPos", legendPos)
				("legendFillColor", legendFillColor)
				("legendBorderColor", legendBorderColor)
				("legendFont", legendFont)
				("legendNumCols", legendNumCols)
				("legendRowSpacing", legendRowSpacing)
				("series", series)
				("mouseHandlingX", mouseHandlingX)
				("mouseHandlingY", mouseHandlingY)
				("plotAreaColor", plotAreaColor)
				("drawXReticle", drawXReticle)
				("drawYReticle", drawYReticle)
				("drawY2Reticle", drawY2Reticle)
				("drawXReticleNumbers", drawXReticleNumbers)
				("drawYReticleNumbers", drawYReticleNumbers)
				("drawY2ReticleNumbers", drawY2ReticleNumbers)
				("axisWidth", axisWidth)
				("reticleFont", reticleFont)
				("reticleColor", reticleColor)
				("gridColor", gridColor)
				("gridWidth", gridWidth)
				("gridDash",  gridDash)
				("drawVGrid", drawVGrid)
				//("drawVGrid2", drawVGrid2)
				("drawHGrid", drawHGrid)
				//("drawHGrid2", drawHGrid2)
				//("freqGrid2", freqGrid2)
				("showRainbow", showRainbow)
				("rainbowPos", rainbowPos)
				("rainbowSize", rainbowSize)
				("rainbowAnchor", intrainbowAnchor)
				("rainbowBorderColor", rainbowBorderColor)
				("rainbowPaletteFont", rainbowPaletteFont)
				("rainbowPaletteTextColor", rainbowPaletteTextColor)
				("surfRainbow", intsurfRainbow)
				("surfNumColor", surfNumColor)
				("continuousColor", continuousColor)
				("surfMinZ", surfMinZ)
				("surfMaxZ", surfMaxZ)
				("surfUnits", surfUnits)
				("surfUnitsPos", intsurfUnitsPos)
				("surfLegendPos", intsurfLegendPos)
				("showLegend", showLegend)
				("legend_w_units", legend_w_units)
				("sciExpTop", sciExpTop)
				("logX", logX)
				("logY", logY)
				("yLabel_orientation", yLabel_orientation)
				("yLabel2_orientation", yLabel2_orientation)
			;
			if (io.IsLoading()) {
				labelsChanged = true;
				legendAnchor = static_cast<LEGEND_POS>(intlegendAnchor);
				rainbowAnchor = static_cast<LEGEND_POS>(intrainbowAnchor);
				surfRainbow = static_cast<RAINBOW>(intsurfRainbow);
				surfUnitsPos = static_cast<SURF_UNITS_POS>(intsurfUnitsPos);
				surfLegendPos = static_cast<SURF_LEGEND_POS>(intsurfLegendPos);
			}
		} else
			io("series", series);
	}
		
	void Xmlize(XmlIO& xml) 	{Ize(xml);}
	void Jsonize(JsonIO& json) 	{Ize(json);}
		
	void Serialize(Stream& s) { 
		if (serializeFormat) {
			int intlegendAnchor = 0, intrainbowAnchor = 0, intsurfRainbow = 0, intsurfUnitsPos = 0, intsurfLegendPos = 0;
			if (s.IsStoring()) {
				intlegendAnchor = legendAnchor;
				intrainbowAnchor = rainbowAnchor;
				intsurfRainbow = surfRainbow;
				intsurfUnitsPos = surfUnitsPos;
				intsurfLegendPos = surfLegendPos;
			}
			s 	% title
				% titleFont
				% titleColor
				% titleHeight
				% xLabel_base
				% yLabel_base
				% yLabel2_base
				% labelsFont
				% labelsColor
				% xRange
				% yRange
				% yRange2
				% xMin
				% yMin
				% yMin2
				% xMajorUnit
				% yMajorUnit
				% yMajorUnit2
				% xMinUnit
				% yMinUnit
				% yMinUnit2
				% minXRange
				% maxXRange
				% minYRange
				% maxYRange
				% minXmin
				% minYmin
				% maxXmax
				% maxYmax
				% hPlotLeft
				% hPlotRight
				% vPlotTop
				% vPlotBottom
				% size
				% intlegendAnchor
				% legendPos
				% legendFillColor
				% legendBorderColor
				% legendFont
				% series
				% mouseHandlingX
				% mouseHandlingY
				% plotAreaColor
				% drawXReticle
				% drawYReticle
				% drawY2Reticle
				% reticleFont
				% reticleColor
				% responsive
				% showRainbow
				% rainbowPos
				% rainbowSize
				% intrainbowAnchor
				% rainbowBorderColor
				% rainbowPaletteFont
				% rainbowPaletteTextColor
				% intsurfRainbow
				% surfNumColor
				% continuousColor
				% surfMinZ
				% surfMaxZ
				% surfUnits
				% intsurfUnitsPos
				% intsurfLegendPos
				% drawXReticleNumbers
				% drawYReticleNumbers
				% drawY2ReticleNumbers
				% axisWidth
				% showLegend
				% legend_w_units
				% sciExpTop
				% logX
				% logY
				% yLabel_orientation
				% yLabel2_orientation
			;
			if (s.IsLoading()) {
				labelsChanged = true;
				legendAnchor = static_cast<LEGEND_POS>(intlegendAnchor);
				rainbowAnchor = static_cast<LEGEND_POS>(intrainbowAnchor);
				surfRainbow = static_cast<RAINBOW>(intsurfRainbow);
				surfUnitsPos = static_cast<SURF_UNITS_POS>(intsurfUnitsPos);
				surfLegendPos = static_cast<SURF_LEGEND_POS>(intsurfLegendPos);
			}
		} else
			s % series;
	}
	
	String GetCSV();
	static void SetDefaultCSVSeparator(String sep) {
		if (sep == "\\t")
			defaultCSVseparator = "\t";
		else
			defaultCSVseparator = sep;
	}
	static String GetDefaultCSVSeparator() {
		if (defaultCSVseparator == "\t")
			return "\\t";
		return defaultCSVseparator;
	}

	String VariableFormatX(double d) const  {return FormatDoubleAutosize(d, xRange);}
	String VariableFormatY(double d) const  {return FormatDoubleAutosize(d, yRange);} 
	String VariableFormatY2(double d) const {return FormatDoubleAutosize(d, yRange2);}
	String VariableFormatZ(double d) const  {return FormatDoubleAutosize(d, GetSurfMaxZ()-GetSurfMinZ());}
		
protected:
	ScatterDraw &_AddSeries(DataSource *data, bool owns = true);
	virtual void Refresh() {};
	Function<void()> OnAddSeries;

	void PaintBackground(Draw& w)	{w.DrawRect(size, graphColor);}
	
	int mode = MD_ANTIALIASED;
	Color graphColor = SColorPaper();	
	String title;
	Upp::Font titleFont = Arial(20);
	Color titleColor = SColorText();
	int titleHeight;
	
	String xLabel, yLabel, yLabel2;
	String xLabel_base, yLabel_base, yLabel2_base;
	Upp::Font labelsFont = GetStdFont();
	Color labelsColor = SColorText();
	
	int yLabel_orientation = true, yLabel2_orientation = true;
	
	int   hPlotLeft = 30, hPlotRight = 30, 
		  vPlotTop = 30, vPlotBottom = 30;
	Color plotAreaColor = SColorPaper();
	
	bool fastViewX = false, sequentialXAll = false;
	
	Color axisColor = SColorText();
	double axisWidth = 0.5;
	
	double xRange = 100., yRange = 100., yRange2 = 100.;
	double xMin = 0, yMin = 0, yMin2 = 0;
	double xMajorUnit = 10, yMajorUnit = 10, yMajorUnit2 = 10;
	
	double xMinUnit = 0, yMinUnit = 0, yMinUnit2 = 0;
	double xMinUnit0 = 0, yMinUnit0 = 0, yMinUnit20 = 0;
	double minXRange = -1, maxXRange = -1, minYRange = -1, maxYRange = -1;
	double minXmin = Null, minYmin = Null, maxXmax = Null, maxYmax = Null;
	double lastxRange, lastyRange;
	bool drawXReticle = true, drawYReticle = true, drawY2Reticle = false;
	bool drawXReticleNumbers = true, drawYReticleNumbers = true, drawY2ReticleNumbers = false;
	Font reticleFont = GetStdFont();
	Color reticleColor = SColorText();
	
	Color gridColor = SColorDkShadow();
	double gridWidth = 0.5;
	String gridDash = LINE_DOTTED_FINE;
	bool drawVGrid = true, drawHGrid = true;	
	
	bool drawVGrid2, drawHGrid2;
	
	int butDownX, butDownY;
	bool isScrolling, isLabelPopUp;
	ZoomStyle zoomStyleX = TO_CENTER, 
			  zoomStyleY = TO_CENTER;	
	
	Array<ScatterSeries> series;
	
	bool showLegend = true;
	bool legend_w_units = true;
	bool sciExpTop = false;
	
	bool isPolar = false;
	
	int lastRefresh_sign;
	int highlight_0 = Null;
	
	Point legendPos = Point(5, 5);
	int legendNumCols = 1;
	LEGEND_POS legendAnchor = RIGHT_TOP;
	int legendRowSpacing = 5;
	Color legendFillColor = SColorPaper();
	Color legendBorderColor = SColorText();
	Font legendFont = GetStdFont();
	
	void DrawLegend(Draw& w) const;

	void Scrolling(bool down, Point &pt, bool isOut = false);
	
	static void ExpFormat(String& s, int , double d)	{s = FormatE(d, 1);}
	static void MonFormat(String& s, int , double d)	{s = Format("%Mon", int(d));}
	static void DyFormat(String& s, int , double d)		{s = Format("%Dy", int(d));}

	Vector<Pointf> DataAddPoints(DataSource& data, bool primaryY, bool sequential);
	template<class T>
	void Plot(T& w);	
	template<class T>
	bool PlotTexts(T& w, bool boldX = false, bool boldY = false);
		
	void AdjustMinUnitX();
	void AdjustMinUnitY();
	void AdjustMinUnitY2();
	
	void AdjustMajorUnitX();
	void AdjustMajorUnitY();
	void AdjustMajorUnitY2();
	
	bool PointInPlot(Point &pt);
	bool PointInBorder(Point &pt);
	bool PointInLegend(Point &pt);
	
	Upp::Index<ScatterDraw *> linkedCtrls;
	ScatterDraw *linkedMaster = nullptr;
	
	void ChangeMouseHandlingX()					{mouseHandlingX = !mouseHandlingX;}
	void ChangeMouseHandlingY()					{mouseHandlingY = !mouseHandlingY;}
	
	bool mouseHandlingX = true, mouseHandlingY = true;
	
	DataSourceSurf *psurf = nullptr;
	RAINBOW surfRainbow = BLUE_YELLOW_RED;
	int surfNumColor = 4;
	bool continuousColor = true;
	double surfMinZ = Null, surfMaxZ = Null;
	String surfUnits;
	SURF_UNITS_POS surfUnitsPos = UNITS_TOP;
	SURF_LEGEND_POS surfLegendPos = LEGEND_RIGHT;
	
	bool showRainbow = true;
	Point rainbowPos = Point(5, 5);
	Size rainbowSize = Size(10, 50);
	LEGEND_POS rainbowAnchor = RIGHT_BOTTOM;
	Color rainbowBorderColor = SColorText();
	Font rainbowPaletteFont = StdFont();
	Color rainbowPaletteTextColor = SColorText();
	
	void DrawRainbowPalette(Draw& w) const;
	
private:
	struct StaticConstructor;
	static StaticConstructor staticConstructorInstance;
            
	static inline double SafeDoubleToInt(double value) {	// Return a double between the limits of signed int
		//constexpr double minInt = std::numeric_limits<int>::min();
    	//constexpr double maxInt = std::numeric_limits<int>::max();
    	//return max(minInt, min(value, maxInt));
    	if (IsNull(value))
    		return Null;
    	return (double)_mm_cvttsd_si32(_mm_set_sd(value)); 	// Fast SSE4.1 truncation
	}
	
	Size size{Size(800, 400)};		// Size to be used for all but screen painting
	double plotScaleX = 1, plotScaleY = 1, plotScaleAvg = 1;
	bool responsive = false;
	double responsivenessFactor = 1;
	bool logX = false, logY = false;
	
	static double FixLog10(double d, bool log) {
		if (log) {
			if (d <= 0)
				return Null;
			d = log10(d);
		}
		return d;
	}
	static double FixPow10(double d, bool log) {
		if (log) 
			d = pow(10, d);
		return d;
	}
		
	static void ParseTextMultiline(const String &text, Upp::Font &fnt, 
								   UVector <String> &texts, UVector <Size> &sizes);
	
	int NumSeriesLegend() const;
	
	void AddId(Vector<Vector<int>> &ids, int id);
		
	static bool CheckDash(const char *dash);
	
	int plotW = Null, plotH = Null;
	bool labelsChanged = false;
	bool stacked = false;
	bool serializeFormat = true;
	static inline String defaultCSVseparator = ";";
	
	Vector<double> gridX, gridXLog, gridY, gridYLog;
};

bool IsQTF(const String &str);

template <class T>
void ScatterDraw::SetDrawing(T& w, bool ctrl) {
	w.DrawRect(size, graphColor);
	
	if (title.IsEmpty())
		titleHeight = 0;
	else if (!IsQTF(title))
		titleHeight = fround(min(plotScaleX, plotScaleY)*(titleFont.GetHeight()+titleFont.GetDescent()));
		
	InitPlot();
			
	Plot(w);
		
	if (!ctrl) {
		if (!PlotTexts(w))
			return;
	}
}

template <class T>
bool ScatterDraw::PlotTexts(T& w, bool boldX, bool boldY) {
	if(titleHeight > 0) {
		Upp::Font fontTitle6;
		fontTitle6 = titleFont;
		fontTitle6.Height(titleHeight);
		Size sz = GetTextSizeSpace(title, fontTitle6);
		if (sz.cx > size.cx*0.95) {
			fontTitle6.Height(fround((fontTitle6.GetHeight()+fontTitle6.GetDescent())*size.cx*(0.95/sz.cx)));
			sz = GetTextSizeSpace(title, fontTitle6);
		}
		DrawText(w, fround((size.cx - sz.cx)/2.), plotScaleY*2, 0, title, fontTitle6, titleColor);   
	}	
	if(showLegend) 
		DrawLegend(w);
	
	if (psurf && showRainbow)
		DrawRainbowPalette(w);
		
	if (plotW < 0 || plotH < 0 || xRange <= 0 || yRange <= 0)
		return false;
	
	w.Offset(Point(fround(plotScaleX*hPlotLeft), fround(plotScaleY*vPlotTop + titleHeight)));
	
	Upp::Font fontLabel = labelsFont;
	fontLabel.Height(fround(min(plotScaleX, plotScaleY)*(labelsFont.GetHeight()+labelsFont.GetDescent())));
	Upp::Font fontX = fontLabel;
	if (boldX)
		fontX.Bold();
	Upp::Font fontY = fontLabel;
	if (boldY)
		fontY.Bold();
	Upp::Font fontY2 = fontY;
	fontY2.Italic();
	
	if (labelsChanged) {
		xLabel = xLabel_base;
		yLabel = yLabel_base;
		yLabel2 = yLabel2_base;
		String yLabelLegends, yLabelLegends2;
		Upp::Index<String> xUnits, yUnits, yUnits2;
		for (int i = 0; i < series.size(); ++i) {
			const ScatterSeries &serie = series[i]; 
			if (serie.IsDeleted() || serie.opacity == 0)
				continue;
			if (serie.primaryY) {
				if (yLabel.IsEmpty()) {
					if (!yLabelLegends.IsEmpty())
						yLabelLegends << ";";
					yLabelLegends << serie.legend;
				}
			} else {
				if (yLabel2.IsEmpty()) {
					if (!yLabelLegends2.IsEmpty())
						yLabelLegends2 << ";";
					yLabelLegends2 << serie.legend;
				}
			}
			if (!serie.unitsX.IsEmpty())
				xUnits.FindAdd(serie.unitsX);
			if (!serie.unitsY.IsEmpty()) {
				if (serie.primaryY) 
					yUnits.FindAdd(serie.unitsY);
				else
					yUnits2.FindAdd(serie.unitsY);
			}
		}
		yLabel << yLabelLegends;
		yLabel2 << yLabelLegends2;
		if (xLabel.Find('[') < 0 && !xUnits.IsEmpty()) {
			xLabel += " ";
			for (int i = 0; i < xUnits.GetCount(); ++i)
				xLabel += "[" + xUnits[i] + "]";
		}
		if (yLabel.Find('[') < 0 && !yUnits.IsEmpty()) {
			yLabel += " ";
			for (int i = 0; i < yUnits.GetCount(); ++i)
				yLabel += "[" + yUnits[i] + "]";
		}				
		if (yLabel2.Find('[') < 0 && !yUnits2.IsEmpty()) {
			yLabel2 += " ";
			for (int i = 0; i < yUnits2.GetCount(); ++i)
				yLabel2 += "[" + yUnits2[i] + "]";
		}						
		labelsChanged = false;	
	}
	Size lx  = GetTextSizeSpace(xLabel, 	fontX);
	Size ly  = GetTextSizeSpace(yLabel, 	fontY);
	Size ly2 = GetTextSizeSpace(yLabel2, fontY2);
	DrawText(w, (plotW - lx.cx)/2., plotH + plotScaleY*(vPlotBottom - 2) - lx.cy, 0, xLabel, fontX, labelsColor);
	if (yLabel_orientation)
		DrawText(w, plotScaleX*(2 - hPlotLeft), (plotH + ly.cx)/2., 900, yLabel,  fontY, labelsColor);
	else
		DrawText(w, plotScaleX*(2 - hPlotLeft) + ly.cy, (plotH - ly.cx)/2., -900, yLabel,  fontY, labelsColor);
	if (yLabel2_orientation)
		DrawText(w, size.cx - plotScaleX*(2 + hPlotLeft) - ly2.cy, (plotH + ly2.cx)/2., 900, yLabel2, fontY2, labelsColor);
	else
		DrawText(w, size.cx - plotScaleX*(2 + hPlotLeft), (plotH - ly2.cx)/2., -900, yLabel2, fontY2, labelsColor);

	drawXReticle  &= (xMajorUnit != 0);
	drawYReticle  &= (yMajorUnit != 0);
	drawY2Reticle &= (yMajorUnit != 0);
	drawXReticleNumbers  &= (xMajorUnit != 0);
	drawYReticleNumbers  &= (yMajorUnit != 0);
	drawY2ReticleNumbers &= (yMajorUnit != 0);
	
	Upp::Font standard6 = reticleFont;
	standard6.Height(fround(min(plotScaleX, plotScaleY)*(standard6.GetHeight()+standard6.GetDescent())));
	Upp::Font fontXNum = standard6;
	if (boldX)
		fontXNum.Bold();
	Upp::Font fontYNum = standard6;
	if (boldY)
		fontYNum.Bold();
	Upp::Font fontY2Num = fontYNum;
	fontY2Num.Italic();
	
	double factorX = plotW/xRange;

	double widthX = 0.9*xMajorUnit*factorX;
	int numNumX   = int(widthX/(fontXNum.GetLeftSpace('0') + fontXNum.GetWidth('0')));	
	double widthY = plotScaleX*hPlotLeft - plotScaleX*2 - 1.5*fontY.GetHeight();
	int numNumY   = int(widthY/(fontYNum.GetLeftSpace('0') + fontYNum.GetWidth('0')));
	double widthY2= plotScaleX*hPlotRight - plotScaleX*2 - 1.5*fontY2.GetHeight();
	int numNumY2  = int(widthY2/(fontY2Num.GetLeftSpace('0') + fontY2Num.GetWidth('0')));
	
	if (drawXReticle || drawXReticleNumbers) {
		for(int i = 0; i < gridX.size(); ++i) {
			double reticleX = factorX*gridX[i];
			if (reticleX >=0 && reticleX <= plotW+0.001) {
				double gX = xMin + gridX[i];
				String gridLabelX;
				if (cbModifFormatXGridUnits)
					cbModifFormatXGridUnits(gridLabelX, i, gX);
				else if (cbModifFormatX)
					cbModifFormatX(gridLabelX, i, gX);
				else if (logX) 
					gridLabelX = FDS(pow(10, gX), 2, false);
				else
					gridLabelX = FDS(gX, min(numNumX, NumAdvicedDigits(xRange)), false);
				
				if (!gridLabelX.IsEmpty()) {
					if (drawXReticleNumbers) {
						UVector <String> texts;
						UVector <Size> sizes;
						ParseTextMultiline(gridLabelX, fontXNum, texts, sizes);
						for (int irowtext = 0; irowtext < texts.size(); ++irowtext) {
							int cy = irowtext == 0 ? 0 : sizes[irowtext - 1].cy;
							DrawText(w, reticleX - sizes[irowtext].cx/2., 
										plotH + plotScaleY*6 + irowtext*cy, 0, texts[irowtext], fontXNum, reticleColor);
						}
					}
					if (drawXReticle) 
						DrawLineOpa(w, reticleX, plotH, reticleX, plotH + plotScaleY*4, plotScaleAvg, 1, axisWidth, axisColor, LINE_SOLID);
				}
			}
		}
	}
	if (drawYReticle || drawYReticleNumbers || drawY2ReticleNumbers) {
		double factorY = plotH/yRange;
		Vector<double> gY, gY2;
		//if (drawYReticleNumbers) {
			gY.SetCount(gridY.size());
			for(int i = 0; i < gridY.size(); ++i) 
				gY[i] = yMin + gridY[i];
		//}
		if (drawY2ReticleNumbers) {
			gY2.SetCount(gridY.size());
			for(int i = 0; i < gridY.size(); ++i) 
				gY2[i] = (gY[i] - yMin)/yRange*yRange2 + yMin2;
		}
		int minExp = std::numeric_limits<int>::max(), minExp2 = std::numeric_limits<int>::max();
		if (sciExpTop) {
			if (drawYReticleNumbers) {
				for(int i = 0; i < gY.size(); ++i) {
					if (gY[i] != 0) {
						int exp = GetExponent10(gY[i]);
						minExp = min(minExp, exp);
					}
				}
				double div = minExp < std::numeric_limits<int>::max() ? Pow10Int<double>(minExp) : 1;
				for(int i = 0; i < gY.size(); ++i) 
					gY[i] /= div;
			}
			if (drawY2ReticleNumbers) {
				for(int i = 0; i < gY2.size(); ++i) {
					if (gY2[i] != 0) {
						int exp = GetExponent10(gY2[i]);
						minExp2 = min(minExp2, exp);
					}
				}	
				double div2 = minExp2 < std::numeric_limits<int>::max() ? Pow10Int<double>(minExp2) : 1;
				for(int i = 0; i < gY2.size(); ++i) 
					gY2[i] /= div2;
			}
		
		}
		for(int i = 0; i < gridY.GetCount(); ++i) {
			double reticleY = plotH - factorY*gridY[i];
			if (drawYReticle)
				DrawLineOpa(w, -plotScaleX*4, reticleY, 0, reticleY, plotScaleAvg, 1, axisWidth, axisColor, LINE_SOLID);
			if (drawY2Reticle)
				DrawLineOpa(w, plotW+plotScaleX*4, reticleY, plotW, reticleY, plotScaleAvg, 1, axisWidth, axisColor, LINE_SOLID);
			if (drawYReticleNumbers) {
				String gridLabelY;
				if (cbModifFormatYGridUnits)
					cbModifFormatYGridUnits(gridLabelY, i, gY[i]);
				else if (cbModifFormatY)
					cbModifFormatY(gridLabelY, i, gY[i]);
				else if (logY) 
					gridLabelY = FDS(pow(10, gY[i]), 2, false);
				else
					gridLabelY = FDS(gY[i], min(numNumY, NumAdvicedDigits(yRange)), false);
				Size sz = GetTextSizeSpace(gridLabelY, fontYNum);
				DrawText(w, -sz.cx - plotScaleX*6, reticleY - sz.cy/2, 0, gridLabelY, fontYNum, axisColor);
			}
			if (drawY2ReticleNumbers) {
				String gridLabelY2;
				if (cbModifFormatY2GridUnits)
					cbModifFormatY2GridUnits(gridLabelY2, i, gY2[i]);
				else if (cbModifFormatY2)
					cbModifFormatY2(gridLabelY2, i, gY2[i]);
				else if (logY) 
					gridLabelY2 = FDS(pow(10, gY2[i]), 2, false);
				else
					gridLabelY2 = FDS(gY2[i], min(numNumY2, NumAdvicedDigits(yRange2)), false);
				Size sz = GetTextSizeSpace(gridLabelY2, fontY2Num);
				DrawText(w, plotW + plotScaleX*10, reticleY - sz.cy/2, 0, gridLabelY2, fontY2Num, axisColor);
			}
		}
		if (sciExpTop) {
			if (drawYReticle && minExp != 0)
				DrawText(w, 0, -1.5*fontYNum.GetHeight(), 0, "x10" + NumToSubSupScript(minExp, false), fontYNum, axisColor);
			if (drawY2Reticle && minExp != 0) {
				String str = "x10" + NumToSubSupScript(minExp2, false);
				Size sz = GetTextSizeSpace(str, fontY2Num);
				DrawText(w, plotW - sz.cx, -1.5*fontY2Num.GetHeight(), 0, str, fontY2Num, axisColor);
			}
		}
	}
	double borderWidth = 1.5*gridWidth;//fround(gridWidth*plotScaleAvg);
#ifdef flagGUI		// Control highlight
	if (!IsNull(highlight_0)) {
		double delayFactor = 4*(1000. - (GetTickCount() - highlight_0))/1000.;
		if (delayFactor < 1) {
			delayFactor = 1;
			highlight_0 = Null;
		} 
		borderWidth = delayFactor*borderWidth;
	}
#endif
	DrawRectangle(w, 0, 0, plotW, plotH, 1, borderWidth, SColorText());
	
	w.End();	
	
	return true;
}
	
template <class T>
void ScatterDraw::Plot(T& w) {
	if (plotW <= 0 || plotH <= 0 || xRange <= 0 || yRange <= 0)
		return;
		
	double factorX = plotW/xRange;
	double factorY = plotH/yRange;
	
	w.Offset(Point(fround(plotScaleX*hPlotLeft), fround(plotScaleY*vPlotTop + titleHeight)));
	Clip(w, 0, 0, plotW, plotH);
	
	double left, top, d = min(plotW, plotH);//, r = d/2.;
	if (!isPolar) {
		if (!psurf)
			w.DrawRect(0, 0, plotW, plotH, plotAreaColor);
		else {
			ImageBuffer out_image(plotW, plotH);
			Upp::Fill(~out_image, plotAreaColor, int(out_image.GetLength()));

			if (!IsNum(surfMaxZ) || !IsNum(surfMinZ))
				return;
			
			double deltaz = surfMaxZ - surfMinZ;
			if (deltaz == 0) 
				Upp::Fill(~out_image, GetRainbowColor(0, surfRainbow, 0), int(out_image.GetLength()));	
			else {
				CoWork co;
				for (int ix = 0; ix < plotW; ++ix) {
					co & [=, &out_image] {
						for (int iy = 0; iy < plotH; ++iy) {
							double x = xMin + (ix + 0.5)/factorX;
							double y = yMin + (iy + 0.5)/factorY;
							double z = psurf->z(x, y);
							if (IsNum(z))
								out_image[plotH - iy - 1][ix] = GetRainbowColor((z - surfMinZ)/deltaz, 
										surfRainbow, continuousColor ? 0 : surfNumColor);	
						}
					};
				}
			}
			w.DrawImage(0, 0, out_image);
		}
	} else {
		if (plotW > plotH) {
			left = (plotW - d)/2.;
			top = 0;
		} else {
			left = 0;
			top = (plotH - d)/2.;
		}
		w.DrawEllipse(fround(left), fround(top), fround(d), fround(d), plotAreaColor);
	}

	if (drawVGrid) {
		if (!isPolar) {
			for (int i = 0; i < gridX.size(); i++) {
				double reticleX = factorX*gridX[i];
				if (reticleX > 2*gridWidth*plotScaleAvg && reticleX < plotW - 2*gridWidth*plotScaleAvg) {
					if (gridDash.GetCount() == 1 && gridDash[0] == '-') {
						DrawLineOpa(w, reticleX, 0, reticleX, 8*plotScaleAvg, plotScaleAvg, 1, gridWidth, gridColor, LINE_SOLID);
						DrawLineOpa(w, reticleX, plotH-8*plotScaleAvg, reticleX, plotH, plotScaleAvg, 1, gridWidth, gridColor, LINE_SOLID);
					} else
						DrawLineOpa(w, reticleX, 0, reticleX, plotH, plotScaleAvg, 1, gridWidth, gridColor, gridDash);
				}
			}
			for(int i = 0; i < gridXLog.size(); ++i) {
				double reticleX = factorX*gridXLog[i];
				DrawLineOpa(w, reticleX, 0, reticleX, plotH, plotScaleAvg, 1, gridWidth, gridColor, gridDash);
			}
		} /*else {
			double ang0 = 2*M_PI*xMinUnit/xRange;
			for(double i = 0; xMinUnit + i*xMajorUnit < xRange; i++) {
				double ang = ang0 + i*2*M_PI*xMajorUnit/xRange;
				DrawLineOpa(w, x_c, y_c, x_c + r*cos(ang), y_c + r*sin(ang), plotScaleAvg, 1, gridWidth*plotScaleAvg, gridColor, gridDash);
			}				
		}*/
	}
	if (drawHGrid) {
		if (!isPolar) {
			for (int i = 0; i < gridY.size(); i++) {
				double reticleY = plotH - factorY*gridY[i];
				if (reticleY > 2*gridWidth*plotScaleAvg && reticleY < plotH - 2*gridWidth*plotScaleAvg) {
					if (gridDash.GetCount() == 1 && gridDash[0] == '-') {
						DrawLineOpa(w, 0, reticleY, 8*plotScaleAvg, reticleY, plotScaleAvg, 1, gridWidth, gridColor, LINE_SOLID);
						DrawLineOpa(w, plotW-8*plotScaleAvg, reticleY, plotW, reticleY, plotScaleAvg, 1, gridWidth, gridColor, LINE_SOLID);
					} else 
						DrawLineOpa(w, 0, reticleY, plotW, reticleY, plotScaleAvg, 1, gridWidth, gridColor, gridDash);
				}
			}
			for(int i = 0; i < gridYLog.size(); ++i) {
				double reticleY = plotH - factorY*gridYLog[i];
				DrawLineOpa(w, 0, reticleY, plotW, reticleY, plotScaleAvg, 1, gridWidth, gridColor, gridDash);
			}
		} /*else {
			double y0 = -plotH*yMinUnit/r + plotH;
			for(double i = 0; yMinUnit + i*yMajorUnit < yRange; i++) {
				double reticleY = y0 + i*r*yRange/yMajorUnit;
				DrawCircleOpa(w, plotW/2, plotH/2, reticleY, 1, 1, gridWidth, gridColor, gridDash);
			}
		}*/
	}
	try {
		for (int j = 0; j < series.size(); j++) {
			ScatterSeries &serie = series[j];
			if (serie.IsDeleted())
				continue;
			DataSource &data = serie.Data();
			if (serie.opacity == 0 || (!serie.seriesPlot && !serie.markPlot))
				continue;
			if (!data.IsExplicit() && data.GetCount() == 0)
				continue;
			if (data.IsExplicit() && IsNull(data.GetCount()))
				continue;
			
			Vector<Pointf> points = DataAddPoints(serie.Data(), serie.primaryY, serie.sequential);
			if (!points.IsEmpty() && serie.seriesPlot && (serie.thickness > 0 || !IsNull(serie.fillColor)))
				serie.seriesPlot->Paint(w, points, plotScaleAvg, serie.opacity, 
											serie.thickness, serie.color, 
											serie.dash, plotAreaColor, serie.fillColor, plotW/xRange, plotH/yRange, 
											plotH*(1 + yMin/yRange), serie.barWidth, 
											serie.isClosed);
		
			if (serie.markWidth >= 1 && serie.markPlot) {
				if (!serie.markPlot->IsMultiPlot()) {
					for (int i = 0; i < points.GetCount(); i++) { 
						if (IsNum(points[i])) 
							serie.markPlot->Paint(w, plotScaleAvg, points[i], 
								serie.markWidth, serie.markColor, 
								serie.markBorderWidth, serie.markBorderColor);              
					}
				} else {
					for (int64 i = 0; i < data.GetCount(); ++i) {
						double ix = GetPosX(data.x(i));
						double iy = GetPosY(data.y(i), serie.primaryY);
						Vector<double> dataX, dataY;
						Vector<double> dataFixed;
						for (int ii = 0; ii < data.GetznxCount(i); ++ii) 
							dataX << GetPosX(data.znx(ii, i));
						for (int ii = 0; ii < data.GetznyCount(i); ++ii) 
							dataY << GetPosY(data.zny(ii, i), serie.primaryY);
						for (int ii = 0; ii < data.GetznFixedCount(); ++ii) 
							dataFixed << data.znFixed(ii, i);
						serie.markPlot->Paint(w, plotScaleAvg, (int)ix, (int)iy, dataX, dataY, dataFixed, 
							serie.markWidth, serie.markColor, 
							serie.markBorderWidth, serie.markBorderColor);   
					}
				}
			}
			if (serie.labels) {
				int dx = int(serie.labelsDx*plotScaleX);
				int dy = int(serie.labelsDy*plotScaleY);
				Font fnt = serie.labelsFont;
				fnt.Height(int((fnt.GetHeight() + fnt.GetDescent())*min(plotScaleX, plotScaleY)));
				for (int i = 0; i < points.GetCount() && i < serie.labels->GetCount(); i++) {
					if (IsNum(points[i])) {
						String txt = (*(serie.labels))[i];
						Size sz = GetTextSizeSpace(txt, fnt);
						double ddy = static_cast<int>(-sz.cy/2.);
						double ddx;
						switch (serie.labelsAlign) {
						case ALIGN_LEFT:	ddx = 0;		break;
						case ALIGN_CENTER:	ddx = -sz.cx/2.;break;
						case ALIGN_RIGHT:	ddx = -sz.cx;	break;
						default: 			NEVER();
						}
						if (serie.angle != 0) {
							double l = sqrt(ddx*ddx + ddy*ddy);
							double ang = serie.angle + atan2(ddy, ddx)*180/M_PI; 
							ang *= M_PI/180;
							ddx = l*cos(ang);
							ddy = l*sin(ang);
						}
						double x = points[i].x + dx + ddx;
						double y = points[i].y + dy + ddy;
						DrawText(w, x, y, serie.angle*10, txt, fnt, serie.labelsColor);
					}
				}
			}
		}
	} catch(ValueTypeError error) {
		ASSERT_(true, error);
	}
	
	WhenPaint(w);
	ClipEnd(w);
	w.End();
}
			
}
		
#endif


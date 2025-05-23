// SPDX-License-Identifier: Apache-2.0
// Copyright 2021 - 2022, the Anboto author and contributors
#include <ScatterDraw/ScatterDraw.h>

using namespace Upp;

#include <plugin/png/png.h>
#include <PdfDraw/PdfDraw.h>


void NullDataDemo(String fileName);

CONSOLE_APP_MAIN
{
	StdLogSetup(LOG_COUT|LOG_FILE);
	
	UppLog() << "\nScatterDraw_Demo";
	
	String fileName = GetExeDirFile("Scatter");
	
	ScatterDraw scatter;
	scatter.cbModifFormatY = [](String& ret, int, double num) {ret = Format("%.1f", num);};
	
	Vector<Pointf> s1 = {{10,14}, {20,65}, {30,29}, {40,32}, {50,12}};
	Vector<double> x = {20, 30, 40, 50, 60, 70};
	Vector<double> y = {34, 85, 49, 72, 56, 34};
	
	UppLog() << "\nPreparing scatter";
	
	scatter.AddSeries(s1).Legend("Series 1").NoPlot().Units("ºC", "seg");
	
	SplineEquation spline;
	VectorPointf s1data(s1);
	spline.Fit(s1data);
	scatter.AddSeries(spline).Legend("Series 1 Softened").NoMark().Dash(ScatterDraw::LINE_SOLID).Stroke(1.5);
	
	scatter.AddSeries(x, y).Units("ºC", "seg").Legend("Series 2").Stroke(5, Null)
		   .Dash(ScatterDraw::LINE_DASHED).MarkStyle<SquareMarkPlot>().MarkWidth(12);
		   
	scatter.SetXYMin(10, 0).SetRange(50, 100).SetMajorUnits(10, 20).SetMinUnits(20, 0);		// Done by hand
	
	scatter.SetTitle("ScatterDraw_Demo").SetTitleFont(SansSerif(14).Bold());
	scatter.SetLabelY("Temperature").SetLabelX("Time").SetLabelsFont(SansSerif(12));
	scatter.SetMargin(80, 30, 40, 50);
	scatter.SetSize(Size(1000, 500));
	scatter.SetMode(ScatterDraw::MD_ANTIALIASED);
	scatter.SetLegendAnchor(ScatterDraw::RIGHT_TOP);
	
	PNGEncoder().SaveFile(fileName + ".png", scatter.GetImage());
	
	PdfDraw pdf(Size(1000, 500));
	scatter.SetSize(Size(1000, 500));
	scatter.SetDrawing(pdf, false);
	SaveFile(fileName + ".pdf", pdf.Finish());
	
	scatter.SetDefaultCSVSeparator(",");
	SaveFileBOMUtf8(fileName + ".csv", scatter.GetCSV());
								
	UppLog() << "\nSaved '" << fileName << "'";
	
	NullDataDemo(fileName);
	
	#ifdef flagDEBUG
	Cout() << "\nPress enter key to end";
	ReadStdIn();
	#endif
}

void NullDataDemo(String fileName) {
	ScatterDraw scatter;

	UppLog() << "\nPreparing Null data scatter";

	Vector<Pointf> s1 = {{1,14}, {2,65}, {3,29}, {Null,Null}, {5,40}, {6,50}, {7,Null}, {8,25}, {9,10}, {10,Null}, {Null,Null}};
	
	UppLog() << "\nScatter with Null data";
	
	scatter.AddSeries(s1).Legend("Series null").Units("kW", "seg");
	
	scatter.SetTitle("ScatterDraw Null data demo").SetTitleFont(SansSerif(14).Bold());
	scatter.SetLabelY("Power").SetLabelX("Time").SetLabelsFont(SansSerif(12));
	scatter.SetLeftMargin(70).SetRightMargin(30).SetTopMargin(40).SetBottomMargin(50);
	scatter.SetMode(ScatterDraw::MD_ANTIALIASED).SetLegendAnchor(ScatterDraw::RIGHT_TOP);
	scatter.SetSize(Size(1000, 500)).ZoomToFit(true, true);
	scatter.SetSequentialXAll().SetFastViewX();
	scatter.WhenPainter = [&](Painter &p) {
		double sizex = scatter.GetSizeX(0.5);
		DrawText(p, scatter.GetPosX(4) - sizex, scatter.GetPosY(35), 0, "This is a Null", Arial(14), Black());
		DrawText(p, scatter.GetPosX(7) - sizex, scatter.GetPosY(40), 0, "This is another Null", Arial(14), Black());
	};
	
	fileName = fileName + " null data.png";
	PNGEncoder().SaveFile(fileName, scatter.GetImage());	
	
	UppLog() << "\nSaved '" << fileName << "'";
}

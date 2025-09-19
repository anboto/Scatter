// SPDX-License-Identifier: Apache-2.0
// Copyright 2021 - 2022, the Anboto author and contributors
#include "ScatterCtrl.h"
#include <Functions4U/Functions4U_Gui.h>
#include <STEM4U/Integral.h>

#include <Functions4U/EnableWarnings.h>

namespace Upp {
	
void ScatterCtrl::DoProcessing() 
{
	ProcessingDlg dlg;
	dlg.Init(*this);
	dlg.Run();
}

void PropertiesDlg::Init(ScatterCtrl& scatter) 
{
	CtrlLayout(*this, t_("Scatter properties"));
	Sizeable().Zoomable();

	this->pscatter = &scatter;
	
	tab.Add(measures, t_("Measures"));
	tab.Add(texts, 	  t_("Texts"));
	tab.Add(legend,   t_("Legend"));
	tab.Add(series,   t_("Series"));
	tab.Add(general,  t_("General"));
	//OnTab();
	
	tab.WhenAction = [=]{OnTab();};
	butOK.WhenAction = [=] {Close();};
}

void PropertiesDlg::Set(int itab)
{
	tab.Set(itab);
	OnTab();
}

void PropertiesDlg::OnTab()
{
	if (tab.IsAt(measures))
		measures.Init(*pscatter);
	else if (tab.IsAt(texts))
		texts.Init(*pscatter);
	else if (tab.IsAt(legend))
		legend.Init(*pscatter);
	else if (tab.IsAt(series))
		series.Init(*pscatter);
	else if (tab.IsAt(general))
		general.Init(*pscatter);
}

void ProcessingDlg::Init(ScatterCtrl& scatter) 
{
	Title(Nvl(scatter.GetTitle(), "Data") + " processing");
	
	Add(splitter.SizePos());
	CtrlLayout(list);
	CtrlLayout(right);
	splitter.Horz(list.SizePos(), right.SizePos());
	splitter.SetPos(1500, 0);
	Sizeable().Zoomable();
	
	this->pscatter = &scatter;
	
	list.list.Reset();
	list.list.SetLineCy(EditField::GetStdHeight());
	list.list.AddColumn(t_("Name"));
	list.list.AddColumn(t_("Id"));
	list.list.ColumnWidths("1 0");
	for(int i = 0; i < scatter.GetCount(); i++) {
		if (scatter.ScatterDraw::IsVisible(i)) {
			list.list.Add(scatter.GetLegend(i), i);
			ProcessingTab& tab = tabs.Add();
			tab.Init(scatter, tabs);
			CtrlLayout(tab);
			right.rect.Add(tab.SizePos());
		}
	}
	if (list.list.GetCount() > 0)
		list.list.SetCursor(0);
	list.list.WhenSel = [=] {UpdateFields();};
	UpdateFields();
	
	right.butOK.WhenAction = [=] {Close();};
}

void ProcessingDlg::UpdateFields() 
{
	int index = list.list.GetCursor();
	if (index < 0)
		return;
	
	for (int i = 0; i < list.list.GetCount(); ++i)
		tabs[i].Hide();
	tabs[index].UpdateField(~list.list.Get(0), int(list.list.Get(1)));
}

int r2Compare(const Vector<Value>& v1, const Vector<Value>& v2) {return double(v1[2]) > double(v2[2]);}

ProcessingTab::ProcessingTab() 
{
	CtrlLayout(*this);
	
	CtrlLayout(tabFitLeft);
	CtrlLayout(tabFitRight);
	
	splitterTabFit.Horz(tabFitLeft.SizePos(), tabFitRightScroll.AddPaneV(tabFitRight).SizePos());
	splitterTabFit.SetPos(6500, 0);
	CtrlLayout(tabFreqLeft);
	CtrlLayout(tabFreqRight);
	splitterTabFreq.Horz(tabFreqLeft.SizePos(), tabFreqRightScroll.AddPaneV(tabFreqRight).SizePos());
	splitterTabFreq.SetPos(8000, 0);
	CtrlLayout(tabOpLeft);
	CtrlLayout(tabOpRight);
	splitterTabOp.Horz(tabOpLeft.SizePos(), tabOpRight.SizePos());
	splitterTabOp.SetPos(8500, 0);
	CtrlLayout(tabBestFitLeft);
	CtrlLayout(tabBestFitRight);
	splitterTabBestFit.Horz(tabBestFitLeft.SizePos(), tabBestFitRight.SizePos());
	splitterTabBestFit.SetPos(6000, 0);
	CtrlLayout(tabHistLeft);
	CtrlLayout(tabHistRight);
	splitterTabHist.Horz(tabHistLeft.SizePos(), tabHistRight.SizePos());
	splitterTabHist.SetPos(8000, 0);	
	
	tab.Add(splitterTabFit.SizePos(),  		t_("Processing"));
	tab.Add(splitterTabFreq.SizePos(), 		t_("Frequency"));
	tab.Add(splitterTabOp.SizePos(),   		t_("Operations"));
	tab.Add(splitterTabBestFit.SizePos(), 	t_("Best fit"));
	tab.Add(splitterTabHist.SizePos(), 		t_("Histogram"));
	tab.WhenSet = [=] {OnSet();};
	
	tabFreqRight.butFFT.WhenAction = [=] {OnFFT();};
	tabFreqRight.opXAxis = 0;
	tabFreqRight.opXAxis.WhenAction = [=] {OnFFT();};
	tabFreqRight.type.WhenAction = [=] {OnFFT();};
	tabFreqRight.type = 0;
	
	for (int i = 0; i < DataSource::GetFFTWindowCount(); ++i)
		tabFreqRight.window.Add(InitCaps(DataSource::GetFFTWindowStr(i)));
	tabFreqRight.window.SetIndex(0);
	tabFreqRight.window.WhenAction = [=] {OnFFT();};
	tabFreqRight.num <<= 1;
	tabFreqRight.overlapping <<= 0.1;
	
	tabFitRight.opSeries = true;
	tabFitRight.opSeries.WhenAction = [=] {OnOp();};
	tabFitRight.opAverage.WhenAction = [=] {OnOp();};
	tabFitRight.opSlope.WhenAction = [=] {OnOp();};
	tabFitRight.opLinear.WhenAction = [=] {OnOp();};
	tabFitRight.opQuadratic.WhenAction = [=] {OnOp();};
	tabFitRight.opCubic.WhenAction = [=] {OnOp();};
	tabFitRight.opSinus.WhenAction = [=] {OnOp();};
	tabFitRight.opSinusTend.WhenAction = [=] {OnOp();};
	tabFitRight.opSpline.WhenAction = [=] {OnOp();};
	tabFitRight.opDerivative.WhenAction = [=] {OnOp();};
	tabFitRight.derOrder.WhenAction = [=] {OnOp();};
	tabFitRight.derAccuracy.WhenAction = [=] {OnOp();};
	tabFitRight.opSG.WhenAction = [=] {OnOp();};
	tabFitRight.sgOrder.WhenAction = [=] {OnOp();};
	tabFitRight.sgSize.WhenAction = [=] {OnOp();};
	tabFitRight.sgDeg.WhenAction = [=] {OnOp();};
	tabFitRight.opFFT.WhenAction = [=] {OnOp();};
	tabFitRight.fromT.WhenAction = [=] {OnOp();};
	tabFitRight.toT.WhenAction = [=] {OnOp();};
	tabFitRight.opMax.WhenAction = [=] {OnOp();};
	tabFitRight.opMin.WhenAction = [=] {OnOp();};
	tabFitRight.opMovAvg.WhenAction = [=] {OnOp();};
	tabFitRight.opSecAvg.WhenAction = [=] {OnOp();};
	tabFitRight.opCumAvg.WhenAction = [=] {OnOp();};
	tabFitRight.butAutoSensSector.WhenAction = [=] {OnAutoSensSector();}; 
	tabFitRight.width.WhenLostFocus = [=] {OnUpdateSensitivity();};
	tabFitRight.width.WhenAction = [=] {OnUpdateSensitivity();};
	tabFitRight.fromX.WhenAction = [=] {OnIntegral();};
	tabFitRight.toX.WhenAction = [=] {OnIntegral();};
	
	tabFitRight.butSetToAll.WhenAction = [=] {OnSetToAll();};
	
	tabFitRight.opDerivative.Tip(t_("Numerical derivative including derivative order and accuracy (related to window size)"));
	tabFitRight.derOrder <<= 1;
	tabFitRight.derOrder.Tip(t_("Implemented orders are 1 (first) and 2 (second derivative)"));
	tabFitRight.derAccuracy <<= 6;
	tabFitRight.derAccuracy.SetInc(2);
	tabFitRight.derAccuracy.Tip(t_("Implemented accuracies are 2, 4, 6 and 8"));
	tabFitRight.opSG.Tip(t_("Savitzky–Golay filter including derivative order, window size and polynomial degree"));
	tabFitRight.sgOrder <<= 0;
	tabFitRight.sgOrder.Tip(t_("Implemented orders are 0 (just filter), 1 (first) and 2 (second derivative)"));
	tabFitRight.sgSize <<= 5;
	//tabFitRight.sgSize.SetInc(2);
	tabFitRight.sgSize.Tip(t_("Window size"));
	tabFitRight.sgDeg <<= 3;
	tabFitRight.sgDeg.Tip(t_("Polynomial degree"));
	tabFitRight.opFFT.Tip(t_("FFT filter indicating period from which and to which to filter"));
	tabFitRight.fromT.Tip(t_("Period from which to filter"));
	tabFitRight.toT.Tip(t_("Period to which to filter"));
	tabFitRight.numDecimals <<= 3;
	tabFitRight.numDecimals.WhenAction = [=] {UpdateEquations();};
	tabFitRight.showEquation.WhenAction = [=] {OnShowEquation();};
	
	tabFitRight.dropIntegral.Add(TRAPEZOIDAL, t_("Trapezoidal")).
							 Add(SIMPSON_1_3, t_("Simpson 1/3")).
							 Add(SIMPSON_3_8, t_("Simpson 3/8")).
							 Add(SPLINE, t_("Spline"));
	tabFitRight.dropIntegral <<= SIMPSON_3_8;
	tabFitRight.dropIntegral.WhenAction = [=]{OnIntegral();};
	
	tabFitRight.array.MultiSelect().SetLineCy(EditField::GetStdHeight());
	tabFitRight.array.WhenBar = [&](Bar &menu) {ArrayCtrlWhenBar(menu, tabFitRight.array);};
	tabFitRight.array.AddColumn(t_("Parameter"));
	tabFitRight.array.AddColumn(t_("Full series"));
	tabFitRight.array.AddColumn(t_("Visible x"));
	int row = 0;
	tabFitRight.array.Set(row++, 0, t_("Max x,y"));
	tabFitRight.array.Set(row++, 0, t_("Min x,y"));
	tabFitRight.array.Set(row++, 0, t_("Mean"));
	tabFitRight.array.Set(row++, 0, t_("RMS"));
	tabFitRight.array.Set(row++, 0, t_("StdDev"));
	
	tabOpRight.xLow.WhenLostFocus  = tabOpRight.xLow.WhenEnter  = 
	tabOpRight.xHigh.WhenLostFocus = tabOpRight.xHigh.WhenEnter = [=] {OnOperation();};
	
	tabBestFitRight.coefficients = 0;
	tabBestFitRight.coefficients.Tip(t_("To show real equation coefficients with different precisions or just in text"));
	tabBestFitRight.minR2 = 0.8;
	tabBestFitRight.minR2.Tip(t_("Min. R2 to plot the equation"));
	tabBestFitRight.userFormula <<= "c0 + c1*x^2; c0=0; c1=1";
	tabBestFitRight.userFormula.Tip(t_("User suppled equation. Initial guess values separated with ';'"));
	tabBestFitRight.gridTrend.AddColumn(t_("Type"), 10);
	tabBestFitRight.gridTrend.AddColumn(t_("Equation"), 30);
	tabBestFitRight.gridTrend.AddColumn(t_("R2"), 5);
	tabBestFitRight.gridTrend.SetLineCy(EditField::GetStdHeight()).MultiSelect();
	tabBestFitRight.gridTrend.WhenBar = [&](Bar &menu) {ArrayCtrlWhenBar(menu, tabBestFitRight.gridTrend);};
	tabBestFitRight.gridTrend.Sort(r2Compare);
	for (int i = 0; i < ExplicitEquation::GetEquationCount(); ++i) 
		equationTypes.Add(ExplicitEquation::Create(i));
	userEquation = new UserEquation;
	equationTypes.Add(userEquation);
	
	tabBestFitRight.butFit.Tip(t_("It tries to fit the series with the supported equations"));
	tabBestFitRight.butFit.WhenPush = [=] {OnFit();};
	
	tabHistRight.axis.Add(t_("X"));
	tabHistRight.axis.Add(t_("Y"));
	tabHistRight.axis.SetIndex(1);
	tabHistRight.axis.WhenAction = [=] {OnSet();};
	tabHistRight.butHist.WhenAction = [=] {OnHist();};
	tabHistRight.numVals <<= 30;
	tabHistRight.valNormalize <<= 100;
	tabHistRight.opStaggered <<= true;
	tabHistRight.percentile <<= 0.99;
	tabHistRight.butPercentile.WhenAction = [=] {OnPercentile();};
	
	tabHistRight.opNormalize.WhenAction   = [&] {
		tabHistRight.valNormalize.Enable(~tabHistRight.opNormalize);
		tabHistRight.labNormalize.Enable(~tabHistRight.opNormalize);
	};
	tabHistRight.opNormalize.WhenAction();
	
	tabFreqFirst = tabOpFirst = tabBestFitFirst = tabHistFirst = true;
	avgFirst = slopeFirst = linearFirst = quadraticFirst = cubicFirst = sinusFirst = sinusTendFirst = splineFirst = true;

	exclamationOpened = false;
	
	newWidthMovAvg = tabFitRight.width;
	newWidthMax = newWidthMin = newWidthMovAvg-1;
	mpm = Null;
}

void ProcessingTab::ArrayCopy() {
	tabBestFitRight.gridTrend.SetClipboard(true, true);
}

void ProcessingTab::ArraySelect() {
	tabBestFitRight.gridTrend.Select(0, tabBestFitRight.gridTrend.GetCount(), true);
}

void ProcessingTab::OnFit() {
	WaitCursor waitcursor;
	
	if (pscatter->IsDeletedDataSource(id))
		return;
	DataSource &ds = pscatter->GetDataSource(id);
	
	userEquation->Init("User", ~tabBestFitRight.userFormula, "x");
	
	Array<double> r2;
	r2.SetCount(equationTypes.GetCount());
	
	Progress progress(t_("Fitting..."), equationTypes.GetCount());
	progress.Title(t_("Searching for the function that best fits")); 	
	for (int i = 0; i < equationTypes.size(); ++i) {
		//equationTypes[i].GuessCoeff(ds);
		equationTypes[i].Fit(ds, r2[i]);
		progress.SetPos(i+1);
	}
	tabBestFitLeft.scatter.RemoveAllSeries();
	tabBestFitLeft.scatter.AddSeries(ds).Legend("Series").NoMark();
	for (int i = 0; i < equationTypes.GetCount(); ++i) {
		if (!IsNull(r2[i]) && r2[i] >= tabBestFitRight.minR2)
			tabBestFitLeft.scatter.AddSeries(equationTypes[i]).Legend(equationTypes[i].GetFullName()).NoMark().Stroke(2);
	}
	tabBestFitLeft.scatter.ZoomToFit(true, true);
	
	int numDecimals = 3;
	switch (tabBestFitRight.coefficients) {
	case 1:	numDecimals = 40;	break;
	case 2:	numDecimals = Null;	break;
	}
	tabBestFitRight.gridTrend.Clear();
	for (int i = 0; i < equationTypes.GetCount(); ++i) 
		tabBestFitRight.gridTrend.Add(equationTypes[i].GetFullName(), equationTypes[i].GetEquation(numDecimals), r2[i]);
	tabBestFitRight.gridTrend.SetSortColumn(2, true);
}

void ProcessingTab::OnOp() 
{
	if (tabFitLeft.scatter.IsDeletedDataSource(0))
		return;
	DataSource &data = tabFitLeft.scatter.GetDataSource(0);
	
	if (data.IsParam() || data.IsExplicit()) 
		return;
	
	if (~tabFitRight.opAverage && avgFirst) {	
		double r2;
		average.Fit(data, r2);
		avgFirst = false;
	}
	if (~tabFitRight.opSlope && slopeFirst) {	
		if (slope.Fit(data, r2Slope) < 0) {
			tabFitRight.opSlope <<= false;
			tabFitRight.opSlope.Enable(false);
		} else
			slopeFirst = false;
	}
	if (~tabFitRight.opLinear && linearFirst) {	
		if (linear.Fit(data, r2Linear) < 0) {
			tabFitRight.opLinear <<= false;
			tabFitRight.opLinear.Enable(false);
		} else
			linearFirst = false;
	}
	if (~tabFitRight.opQuadratic && quadraticFirst) {		
		quadratic.GuessCoeff(data);
		if (quadratic.Fit(data, r2Quadratic) < 0) {
			tabFitRight.opQuadratic <<= false;
			tabFitRight.opQuadratic.Enable(false);
		} else
			quadraticFirst = false;
	}
	if (~tabFitRight.opCubic && cubicFirst) {		
		cubic.GuessCoeff(data);
		if (cubic.Fit(data, r2Cubic) < 0) {
			tabFitRight.opCubic <<= false;
			tabFitRight.opCubic.Enable(false);
		} else
			cubicFirst = false;
	}
	if (~tabFitRight.opSinus && sinusFirst) {		
		sinus.GuessCoeff(data);
		if (sinus.Fit(data, r2Sinus) < 0) {
			tabFitRight.opSinus <<= false;
			tabFitRight.opSinus.Enable(false);
		} else
			sinusFirst = false;
	}
	if (~tabFitRight.opSinusTend && sinusTendFirst) {
		DataXRange dataXRange2;
		dataXRange2.Init(data, Null, Null);		
		double r2SinusTendBest = Null;
		SinEquation sinusTendBest;
		for (int iLow = 9; iLow >= 0; iLow--) {
			double xLow = data.x(data.GetCount()*iLow/10);
			dataXRange2.SetXLow(xLow);
			sinusTend.GuessCoeff(dataXRange2);
			if (sinusTend.Fit(dataXRange2, r2SinusTend) < 0)
				break;
			if (!IsNull(r2SinusTendBest) && r2SinusTendBest > r2SinusTend)
				break;
			r2SinusTendBest = r2SinusTend;
			sinusTendBest = sinusTend;
		}
		if (IsNull(r2SinusTendBest)) {
			tabFitRight.opSinusTend <<= false;
			tabFitRight.opSinusTend.Enable(false);
		} else {
			sinusTendFirst = false;
			r2SinusTend = r2SinusTendBest;
			sinusTend = sinusTendBest;
		}
	} 
	if (~tabFitRight.opSpline && splineFirst) {		
		if (spline.Fit(data) < 0) {
			tabFitRight.opSpline <<= false;
			tabFitRight.opSpline.Enable(false);
		} else
			splineFirst = false;
	}
	OnUpdateSensitivity();
	
	int iid = 0;
	tabFitLeft.scatter.ScatterDraw::Show(iid++, tabFitRight.opSeries);
	tabFitLeft.scatter.ScatterDraw::Show(iid++, tabFitRight.opAverage);
	tabFitLeft.scatter.ScatterDraw::Show(iid++, tabFitRight.opSlope);
	tabFitLeft.scatter.ScatterDraw::Show(iid++, tabFitRight.opLinear);
	tabFitLeft.scatter.ScatterDraw::Show(iid++, tabFitRight.opQuadratic);
	tabFitLeft.scatter.ScatterDraw::Show(iid++, tabFitRight.opCubic);
	tabFitLeft.scatter.ScatterDraw::Show(iid++, tabFitRight.opSinus);
	tabFitLeft.scatter.ScatterDraw::Show(iid++, tabFitRight.opSinusTend);
	tabFitLeft.scatter.ScatterDraw::Show(iid++, tabFitRight.opSpline);
	tabFitLeft.scatter.ScatterDraw::Show(iid++, tabFitRight.opDerivative);
	tabFitLeft.scatter.ScatterDraw::Show(iid++, tabFitRight.opSG);
	tabFitLeft.scatter.ScatterDraw::Show(iid++, tabFitRight.opFFT);
	tabFitLeft.scatter.ScatterDraw::Show(iid++, tabFitRight.opMax);
	tabFitLeft.scatter.ScatterDraw::Show(iid++, tabFitRight.opMin);
	tabFitLeft.scatter.ScatterDraw::Show(iid++, tabFitRight.opMovAvg);
	tabFitLeft.scatter.ScatterDraw::Show(iid++, tabFitRight.opSecAvg);
	tabFitLeft.scatter.ScatterDraw::Show(iid++, tabFitRight.opCumAvg);
	
	UpdateEquations();
	OnShowEquation();
}

void ProcessingTab::OnAutoSensSector() 
{
	if (tabFitLeft.scatter.IsDeletedDataSource(0))
		return;
	DataSource &data = tabFitLeft.scatter.GetDataSource(0);
	//Vector<Pointf> secAvg;
	double baseWidth;
	
	baseWidth = 0;
	for (int64 i = 1; i < data.GetCount(); ++i)
		baseWidth += (data.x(i) - data.x(i-1));
	baseWidth /= double(data.GetCount() - 1);
	
	double rangeX = data.x(data.GetCount() - 1) - data.x(int64(0));
	
	for(double width = baseWidth; width < rangeX/10.; width += baseWidth) {
		secAvg = data.SectorAverageY(width);
		VectorPointf sector(secAvg);
		Vector<int64> ids;
		sector.MaxListY(ids, 10*baseWidth);
		if (ids.GetCount() < 5) {
			tabFitRight.width <<= width;
			return;
		}
	}
	tabFitLeft.scatter.Refresh();
}

void ProcessingTab::OnOperation()
{
	if (exclamationOpened)	// To avoid WhenLostFocus to be called when Exclamation() is opened
		return;
	exclamationOpened = true;
	if (!IsNull(tabOpRight.xLow) && !IsNull(tabOpRight.xHigh)) {
		if (tabOpRight.xLow >= tabOpRight.xHigh) {
			Exclamation(t_("'x >' has to be lower than 'x <'"));
			exclamationOpened = false;
			return;
		}
	}
	exclamationOpened = false;
	if (pscatter->IsDeletedDataSource(id))
		return;
	dataXRange.Init(pscatter->GetDataSource(id), tabOpRight.xLow, tabOpRight.xHigh);
	tabOpLeft.scatter.Refresh();
}

void ProcessingTab::GetVisibleData() {
	DataSource &data0 = tabFitLeft.scatter.GetDataSource(0);

	if (!data0.IsParam()/* && !data.IsExplicit()*/) {	
		double xmin = tabFitLeft.scatter.GetXMin();
		double xmax = tabFitLeft.scatter.GetXMax();

		DataXRange data(data0, xmin, xmax);
		double avg = data.AvgY();
		if (!IsNull(avg)) {
			tabFitRight.array.Set(2, 2, avg);
			tabFitRight.array.Set(3, 2, data.RMSY());
			tabFitRight.array.Set(4, 2, data.StdDevY(avg));
		}
		int64 idmx;
		double val;
		val = data.MaxY(idmx);
		if (!IsNull(val)) {
			tabFitRight.array.Set(0, 2, Format("%f,%f", data.x(idmx), val));
			val = data.MinY(idmx);
			if (!IsNull(val))
				tabFitRight.array.Set(1, 2, Format("%f,%f", data.x(idmx), val));
		}
	}
}

void ProcessingTab::UpdateField(const String _name, int _id) 
{
	id = _id;
	name.SetText(_name);
	
	tabFitLeft.scatter.RemoveAllSeries();
	if (pscatter->IsDeletedDataSource(id))
		return;
	tabFitLeft.scatter.AddSeries(pscatter->GetDataSource(id)).SetSequentialX(pscatter->GetSequentialX())
				   .Legend(pscatter->GetLegend(id));
	tabFitLeft.scatter.SetFastViewX(pscatter->GetFastViewX());
	
	tabFitLeft.scatter.SetFillColor(0, pscatter->GetFillColor(id));
	tabFitLeft.scatter.Dash(0, pscatter->GetDash(id));
	
	Upp::Color color;
	double thickness;
	pscatter->GetStroke(0, thickness, color);
	tabFitLeft.scatter.Stroke(0, thickness, color);
	tabFitLeft.scatter.MarkStyle(0, pscatter->GetMarkStyleName(id));
	tabFitLeft.scatter.SetMarkColor(0, pscatter->GetMarkColor(id));
	tabFitLeft.scatter.SetMarkWidth(0, pscatter->GetMarkWidth(id));
	tabFitLeft.scatter.MarkStyle(0, pscatter->GetMarkStyleName(id));
	tabFitLeft.scatter.SetLegendAnchor(ScatterDraw::RIGHT_TOP).SetLegendFillColor(Null);
	
	tabFitLeft.scatter.Units(0, pscatter->GetUnitsY(id), pscatter->GetUnitsX(id));
	tabFitLeft.scatter.SetLabelX(pscatter->GetLabelX());
	
	bool primary = pscatter->IsDataPrimaryY(id);
    tabFitLeft.scatter.SetRange(pscatter->GetXRange(), primary ? pscatter->GetYRange() : pscatter->GetY2Range());
	tabFitLeft.scatter.SetMajorUnits(pscatter->GetMajorUnitsX(), primary ? pscatter->GetMajorUnitsY() : pscatter->GetMajorUnitsY2());
	tabFitLeft.scatter.SetXYMin(pscatter->GetXMin(), primary ? pscatter->GetYMin() : pscatter->GetY2Min());
	
	tabFitLeft.scatter.ShowInfo().ShowContextMenu().ShowProcessDlg().ShowPropertiesDlg().SetMouseHandlingLinked(true, true);
	
	if (tabFitLeft.scatter.IsDeletedDataSource(0))
		return;
	DataSource &data = tabFitLeft.scatter.GetDataSource(0);
	
	tabFitLeft.scatter.WhenZoomScroll = [&]() {
		ScatterCtrl::MouseAction action = tabFitLeft.scatter.GetMouseAction();
		if (action == ScatterCtrl::NONE)
			GetVisibleData();
	};
	
	if (!data.IsParam()/* && !data.IsExplicit()*/) {	
		double avg = data.AvgY();
		tabFitRight.array.Set(2, 1, avg);
		tabFitRight.array.Set(3, 1, data.RMSY());
		tabFitRight.array.Set(4, 1, data.StdDevY(avg));
		int64 idmx;
		double val;
		val = data.MaxY(idmx);
		if (!IsNull(val)) {
			tabFitRight.array.Set(0, 1, Format("%f,%f", data.x(idmx), val));
			val = data.MinY(idmx);
			if (!IsNull(val))
				tabFitRight.array.Set(1, 1, Format("%f,%f", data.x(idmx), val));
		}
	}
	if (!data.IsParam() && !data.IsExplicit()) {	
		tabFitRight.width <<= pscatter->GetXRange()/15.;
		tabFitRight.width.SetInc(pscatter->GetXRange()/15./2.);
		
		if (IsNull(tabFitRight.fromX))
			tabFitRight.fromX = data.x(int64(0));
		if (IsNull(tabFitRight.toX))
			tabFitRight.toX = data.x(data.GetCount()-1);
		OnIntegral();
		
		tabFitLeft.scatter.AddSeries(average).NoMark().Stroke(1.5);
		tabFitLeft.scatter.AddSeries(slope).NoMark().Stroke(1.5);
		tabFitLeft.scatter.AddSeries(linear).NoMark().Stroke(1.5);
		tabFitLeft.scatter.AddSeries(quadratic).NoMark().Stroke(1.5);
		tabFitLeft.scatter.AddSeries(cubic).NoMark().Stroke(1.5);		
		tabFitLeft.scatter.AddSeries(sinus).NoMark().Stroke(1.5);
		tabFitLeft.scatter.AddSeries(sinusTend).NoMark().Stroke(1.5);
		tabFitLeft.scatter.AddSeries(spline).NoMark().Dash(ScatterDraw::LINE_SOLID).Stroke(1.5);
		tabFitLeft.scatter.AddSeries(derivative).NoMark().Dash(ScatterDraw::LINE_SOLID).Stroke(1.5);
		tabFitLeft.scatter.AddSeries(sg).NoMark().Dash(ScatterDraw::LINE_SOLID).Stroke(1.5);
		tabFitLeft.scatter.AddSeries(fftFilter).NoMark().Dash(ScatterDraw::LINE_SOLID).Stroke(1.5);
		tabFitLeft.scatter.AddSeries(upperEnvelope).Legend(pscatter->GetLegend(id) + String("-") + t_("Max"))
						.NoMark().Dash(ScatterDraw::LINE_DASHED).Stroke(1.5).SetSequentialX(true);
		tabFitLeft.scatter.AddSeries(lowerEnvelope).Legend(pscatter->GetLegend(id) + String("-") + t_("Min"))
						.NoMark().Dash(ScatterDraw::LINE_DASHED).SetSequentialX(true);
		tabFitLeft.scatter.AddSeries(movAvg).Stroke(1.5).Legend(pscatter->GetLegend(id) + String("-") + t_("MovAvg")).NoMark();		
		tabFitLeft.scatter.AddSeries(secAvg).Stroke(1.5).Legend(pscatter->GetLegend(id) + String("-") + t_("SecAvg")).NoMark();		
		tabFitLeft.scatter.AddSeries(cumAvg).Stroke(1.5).Legend(pscatter->GetLegend(id) + String("-") + t_("CumAvg")).NoMark();		
					
		OnOp();
	} else {
		tabFitRight.opSeries.Enable(false);
		tabFitRight.opAverage.Enable(false);
		tabFitRight.opSlope.Enable(false);
		tabFitRight.opLinear.Enable(false);
		tabFitRight.opQuadratic.Enable(false);
		tabFitRight.opCubic.Enable(false);
		tabFitRight.opSinus.Enable(false);
		tabFitRight.opSinusTend.Enable(false);
		tabFitRight.opSpline.Enable(false);
		tabFitRight.opDerivative.Enable(false);
		tabFitRight.derOrder.Enable(false);
		tabFitRight.derAccuracy.Enable(false);
		tabFitRight.opSG.Enable(false);
		tabFitRight.opFFT.Enable(false);
		tabFitRight.sgOrder.Enable(false);
		tabFitRight.opMax.Enable(false);
		tabFitRight.opMin.Enable(false);
		tabFitRight.opMovAvg.Enable(false);
		tabFitRight.opSecAvg.Enable(false);
		tabFitRight.opCumAvg.Enable(false);
	}
	
	GetVisibleData();
	
	Show();	
}

void ProcessingTab::OnSetToAll() {
	Array<ProcessingTab> &tabs = *ptabs;
	for (int i = 0; i < tabs.size(); ++i) {
		if (&(tabs[i]) != this) {
			tabs[i].tabFitRight.dropIntegral = ~tabFitRight.dropIntegral;
			tabs[i].tabFitRight.fromX = ~tabFitRight.fromX;
			tabs[i].tabFitRight.toX = ~tabFitRight.toX;
			tabs[i].tabFitRight.opSeries = ~tabFitRight.opSeries;
			tabs[i].tabFitRight.opAverage = ~tabFitRight.opAverage;
			tabs[i].tabFitRight.opSlope = ~tabFitRight.opSlope;
			tabs[i].tabFitRight.opLinear = ~tabFitRight.opLinear;
			tabs[i].tabFitRight.opQuadratic = ~tabFitRight;
			tabs[i].tabFitRight.opCubic = ~tabFitRight.opCubic;
			tabs[i].tabFitRight.opSinus = ~tabFitRight.opSinus;
			tabs[i].tabFitRight.opSinusTend = ~tabFitRight.opSinusTend;
			tabs[i].tabFitRight.opFFT = ~tabFitRight.opFFT;
			tabs[i].tabFitRight.fromT = ~tabFitRight.fromT;
			tabs[i].tabFitRight.toT = ~tabFitRight.toT;
			tabs[i].tabFitRight.opSG = ~tabFitRight.opSG;
			tabs[i].tabFitRight.sgOrder <<= ~tabFitRight.sgOrder;
			tabs[i].tabFitRight.sgDeg <<= ~tabFitRight.sgDeg;
			tabs[i].tabFitRight.sgSize <<= ~tabFitRight.sgSize;
			tabs[i].tabFitRight.opSpline = ~tabFitRight.opSpline;
			tabs[i].tabFitRight.numDecimals <<= ~tabFitRight.numDecimals;
			tabs[i].tabFitRight.showEquation = ~tabFitRight.showEquation;
			tabs[i].tabFitRight.opMax = ~tabFitRight.opMax;
			tabs[i].tabFitRight.opMin = ~tabFitRight.opMin;
			tabs[i].tabFitRight.opMovAvg = ~tabFitRight.opMovAvg;
			tabs[i].tabFitRight.opCumAvg = ~tabFitRight.opCumAvg;
			tabs[i].tabFitRight.opSecAvg = ~tabFitRight.opSecAvg;
			tabs[i].tabFitRight.opDerivative = ~tabFitRight.opDerivative;
			tabs[i].tabFitRight.derAccuracy <<= ~tabFitRight.derAccuracy;
			tabs[i].tabFitRight.derOrder <<= ~tabFitRight.derOrder;
		}
	}
}

void ProcessingTab::OnIntegral() {
	double fromX = tabFitRight.fromX;
	double toX = tabFitRight.toX;
	
	tabFitLeft.comments.SetText("");
	
	double integral, avg;
	if (IsNull(fromX) || IsNull(toX)) 
		integral = avg = Null;
	else if (fromX == toX)
		integral = avg = 0;
	else if (fromX > toX) {
		integral = avg = Null;
		tabFitLeft.comments.SetText(t_("'From x' has to be lower than 'To x'"));
	} else {
		DataSource &data = tabFitLeft.scatter.GetDataSource(0);
		Vector<double> x, y;
		data.CopyXY(x, y);
		int sz = x.size();
		int idFrom = 0, idTo = sz-1;
		for (int i = 0; i < sz; ++i) {
			if (x[i] <= fromX)
				idFrom = i;
			if (x[i] >= toX && idTo == sz-1)	
				idTo = i;
		}
		if (idTo != sz-1) {
			y[idTo] = y[idTo-1] + (toX - x[idTo-1])*(y[idTo]-y[idTo-1])/(x[idTo]-x[idTo-1]);			// Linear interpolation
			x[idTo] = toX;
			x.Remove(idTo+1, sz-idTo-1);
			y.Remove(idTo+1, sz-idTo-1);
		} else if (toX > x[idTo])
			tabFitLeft.comments.SetText(t_("'To x' is higher than the maximum x"));
		if (idFrom != 0) {
			y[idFrom] = y[idFrom] + (fromX - x[idFrom])*(y[idFrom+1]-y[idFrom])/(x[idFrom+1]-x[idFrom]);// Linear interpolation
			x[idFrom] = fromX;
			x.Remove(0, idFrom);
			y.Remove(0, idFrom);
		} else if (fromX < x[0])
			tabFitLeft.comments.SetText(t_("'From x' is lower than the minimum x"));
		integral = Integral(x, y, (IntegralType)(int)(~tabFitRight.dropIntegral));
		avg = integral /(toX - fromX);
	}	
	tabFitRight.integral <<= FormatF(integral, tabFitRight.numDecimals);
	tabFitRight.integralAvg <<= FormatF(avg, tabFitRight.numDecimals);
}

void ProcessingTab::OnUpdateSensitivity() 
{
	if (tabFitLeft.scatter.IsDeletedDataSource(0))
		return;
	DataSource &data = tabFitLeft.scatter.GetDataSource(0);
	
	bool refresh = false;
	tabFitLeft.comments.SetText("");
	if (tabFitRight.opDerivative) {
		bool isOdd = int(~(tabFitRight.derAccuracy))%2;
		if (IsNull(tabFitRight.derAccuracy) || isOdd)
			derivative.Clear();
		else
			derivative = data.DerivativeY(~tabFitRight.derOrder, ~tabFitRight.derAccuracy);

		refresh = true;
	}
	if (tabFitRight.opSG) {
		int side = int(~tabFitRight.sgSize)/2;
		if (!SavitzkyGolay_CheckParams(side, side, ~tabFitRight.sgDeg, ~tabFitRight.sgOrder))
			sg.Clear();
		else
			sg = data.SavitzkyGolayY(~tabFitRight.sgDeg, ~tabFitRight.sgSize, ~tabFitRight.sgOrder);
		
		if (sg.IsEmpty())
			tabFitLeft.comments.SetText(t_("Impossible to filter series"));
		refresh = true;
	}
	if (tabFitRight.opFFT) {
		double fromT = ~tabFitRight.fromT;
		double toT = ~tabFitRight.toT;
		
		if ((!IsNull(fromT) || !IsNull(toT)) && (fromT < toT))  {
			fftFilter = data.FilterFFTY(fromT, toT);
			if (fftFilter.IsEmpty())
				tabFitLeft.comments.SetText(t_("Impossible to filter series"));
			refresh = true;	
		} else
			fftFilter.Clear();
	}
	if (tabFitRight.opMax && newWidthMax != tabFitRight.width) {
		newWidthMax = tabFitRight.width;
		
		upperEnvelope.Clear();
		Vector<int64> idsUpper = data.UpperEnvelopeY(tabFitRight.width);
		mpm = data.StdDevY()*sqrt(2*log(idsUpper.GetCount()));
		for (int i = 0; i < idsUpper.GetCount(); ++i) 
			upperEnvelope << Pointf(data.x(idsUpper[i]), data.y(idsUpper[i]));
		refresh = true;
	}
	tabFitRight.labNumMax.Enable(tabFitRight.opMax);
	tabFitRight.numMax.Enable(tabFitRight.opMax);
	tabFitRight.numMax <<= (tabFitRight.opMax ? upperEnvelope.GetCount() : Null);
	if (tabFitRight.opMax) {
		tabFitRight.array.Set(5, 0, t_("MPM"));
		tabFitRight.array.Set(5, 1, mpm);
	}
	if (tabFitRight.opMin && newWidthMin != tabFitRight.width) {
		newWidthMin = tabFitRight.width;
		
		lowerEnvelope.Clear();
		Vector<int64> idsLower = data.LowerEnvelopeY(tabFitRight.width);
		for (int i = 0; i < idsLower.GetCount(); ++i) 
			lowerEnvelope << Pointf(data.x(idsLower[i]), data.y(idsLower[i]));
		refresh = true;
	} 
	tabFitRight.labNumMin.Enable(tabFitRight.opMin);
	tabFitRight.numMin.Enable(tabFitRight.opMin);
	tabFitRight.numMin <<= (tabFitRight.opMin ? lowerEnvelope.GetCount() : Null);
	if (tabFitRight.opMovAvg && newWidthMovAvg != tabFitRight.width) {
		newWidthMovAvg = tabFitRight.width;
		
		movAvg = data.MovingAverageY(tabFitRight.width);
		refresh = true;
	}
	if (tabFitRight.opSecAvg && newWidthMovAvg != tabFitRight.width) {
		newWidthMovAvg = tabFitRight.width;
		
		secAvg = data.SectorAverageY(tabFitRight.width);
		refresh = true;
	}
	if (tabFitRight.opCumAvg) {
		cumAvg = data.CumulativeAverageY();
		refresh = true;
	}
			
	if (refresh) {
		tabFitLeft.scatter.Refresh();
		tabFitLeft.comments.Refresh();
	}
}

void ProcessingTab::OnSet() 
{
	if (tabFitLeft.scatter.IsDeletedDataSource(0))
		return;
	DataSource &data = tabFitLeft.scatter.GetDataSource(0);
	
	if (tabFreqFirst && tab.IsAt(splitterTabFreq)) {
		tabFreqFirst = false;
		if (data.IsParam() || data.IsExplicit()) {
			tabFreqLeft.comments.SetText(t_("Impossible to calculate FFT from a not sampled series"));
			tabFreqRight.butFFT.Enable(false);
		} else if (data.GetCount() < 5) {
			tabFreqLeft.comments.SetText(t_("Insufficient data to calculate FFT"));
			tabFreqRight.butFFT.Enable(false);
		} else {
			double mindT, maxdT = Null;
			mindT = -maxdT;
			for (int64 i = 1; i < data.GetCount(); ++i) {
				double d2 = data.x(i), d1 = data.x(i - 1);
				
				if (!IsNull(d1) && !IsNull(d2)) {
					double delta = (d2 - d1);
					mindT = min(delta, mindT);
					maxdT = max(delta, maxdT);
				}
			}
			if ((maxdT - mindT)/maxdT > 0.00001) 
				tabFreqLeft.comments.SetText(Format(t_("Sampling time changes from %f to %f"), mindT, maxdT));
			else 
				tabFreqLeft.comments.SetText("");
			tabFreqRight.samplingTime = (maxdT + mindT)/2.;
		}
	} else if (tabOpFirst && tab.IsAt(splitterTabOp)) {
		if (pscatter->IsDeletedDataSource(id))
			return;
		
		tabOpFirst = false; 
		
		tabOpLeft.scatter.RemoveAllSeries();
		String legend = pscatter->GetLegend(id);
		double xLow = pscatter->GetDataSource(id).MinX();
		if (IsNull(xLow))
			xLow = pscatter->GetXMin();
		tabOpRight.xLow <<= xLow;
		double xHigh = pscatter->GetDataSource(id).MaxX();
		if (IsNull(xHigh))
			xHigh = pscatter->GetXMin() + pscatter->GetXRange();
		tabOpRight.xHigh <<= xHigh;
		dataXRange.Init(pscatter->GetDataSource(id), xLow, xHigh);
		tabOpLeft.scatter.AddSeries(dataXRange).SetSequentialX(pscatter->GetSequentialX())
					   .Legend(legend + String("-") + t_("Processed")).NoMark()
					   .Stroke(8, Upp::Color(115, 214, 74));				   
		tabOpLeft.scatter.AddSeries(pscatter->GetDataSource(id)).SetSequentialX(pscatter->GetSequentialX())
					   .Legend(legend).NoMark().Stroke(2, Blue());				   
		tabOpLeft.scatter.SetFastViewX(pscatter->GetFastViewX());
	
		tabOpLeft.scatter.SetLegendAnchor(ScatterDraw::RIGHT_TOP).SetLegendFillColor(Null);
		
		tabOpLeft.scatter.Units(0, pscatter->GetUnitsY(id), pscatter->GetUnitsX(id));
		tabOpLeft.scatter.SetLabelX(pscatter->GetLabelX());
		
		bool primary = pscatter->IsDataPrimaryY(id);
		tabOpLeft.scatter.SetRange(pscatter->GetXRange(), primary ? pscatter->GetYRange() : pscatter->GetY2Range());
		tabOpLeft.scatter.SetMajorUnits(pscatter->GetMajorUnitsX(), primary ? pscatter->GetMajorUnitsY() : pscatter->GetMajorUnitsY2());
		tabOpLeft.scatter.SetXYMin(pscatter->GetXMin(), primary ? pscatter->GetYMin() : pscatter->GetY2Min());
		
		tabOpLeft.scatter.ShowInfo().ShowContextMenu().ShowProcessDlg().ShowPropertiesDlg().SetMouseHandlingLinked(true, true);	
	} else if (tabBestFitFirst && tab.IsAt(splitterTabBestFit)) {
		if (pscatter->IsDeletedDataSource(id))
			return;
				
		tabBestFitFirst = false; 
		
		tabBestFitLeft.scatter.RemoveAllSeries();
		String legend = pscatter->GetLegend(id);
		
		tabBestFitLeft.scatter.AddSeries(pscatter->GetDataSource(id)).SetSequentialX(pscatter->GetSequentialX())
					   .Legend(legend).NoMark().Stroke(2);				   
		tabBestFitLeft.scatter.SetFastViewX(pscatter->GetFastViewX());
	
		tabBestFitLeft.scatter.SetLegendAnchor(ScatterDraw::RIGHT_TOP).SetLegendFillColor(Null);
		
		tabBestFitLeft.scatter.Units(0, pscatter->GetUnitsY(id), pscatter->GetUnitsX(id));
		tabBestFitLeft.scatter.SetLabelX(pscatter->GetLabelX());
		
		bool primary = pscatter->IsDataPrimaryY(id);
		tabBestFitLeft.scatter.SetRange(pscatter->GetXRange(), primary ? pscatter->GetYRange() : pscatter->GetY2Range());
		tabBestFitLeft.scatter.SetMajorUnits(pscatter->GetMajorUnitsX(), primary ? pscatter->GetMajorUnitsY() : pscatter->GetMajorUnitsY2());
		tabBestFitLeft.scatter.SetXYMin(pscatter->GetXMin(), primary ? pscatter->GetYMin() : pscatter->GetY2Min());
		
		tabBestFitLeft.scatter.ShowInfo().ShowContextMenu().ShowProcessDlg().ShowPropertiesDlg().SetMouseHandlingLinked(true, true);	
	} else if (tab.IsAt(splitterTabHist)) {
		//tabHistFirst = false;
		if (data.IsParam() || data.IsExplicit()) {
			tabHistLeft.comments.SetText(t_("Impossible to calculate histogram from a not sampled series"));
			tabHistRight.butHist.Enable(false);
		} else if (data.GetCount() < 5) {
			tabHistLeft.comments.SetText(t_("Insufficient data to calculate histogram"));
			tabHistRight.butHist.Enable(false);
		} else {
			if (~tabHistRight.axis == t_("Y")) {
				tabHistRight.minVal <<= data.MinY();	
				tabHistRight.maxVal <<= data.MaxY();
			} else {
				tabHistRight.minVal <<= data.MinX();	
				tabHistRight.maxVal <<= data.MaxX();
			}
		}
	}
}

void ProcessingTab::UpdateEquations()
{
	tabFitRight.eqAverage = tabFitRight.opAverage ? average.GetEquation(tabFitRight.numDecimals) : "";
	tabFitRight.eqSlope = tabFitRight.opSlope ? slope.GetEquation(tabFitRight.numDecimals) : "";
	tabFitRight.r2Slope = tabFitRight.opSlope ? r2Slope : Null;
	tabFitRight.eqLinear = tabFitRight.opLinear ? linear.GetEquation(tabFitRight.numDecimals) : "";
	tabFitRight.r2Linear = tabFitRight.opLinear ? r2Linear : Null;
	tabFitRight.eqQuadratic = tabFitRight.opQuadratic ? quadratic.GetEquation(tabFitRight.numDecimals) : "";
	tabFitRight.r2Quadratic = tabFitRight.opQuadratic ? r2Quadratic : Null;
	tabFitRight.eqCubic = tabFitRight.opCubic ? cubic.GetEquation(tabFitRight.numDecimals) : "";
	tabFitRight.r2Cubic = tabFitRight.opCubic ? r2Cubic : Null;
	tabFitRight.eqSinus = tabFitRight.opSinus ? sinus.GetEquation(tabFitRight.numDecimals) : "";
	tabFitRight.r2Sinus = tabFitRight.opSinus ? r2Sinus : Null;
	tabFitRight.eqSinusTend = tabFitRight.opSinusTend ? sinusTend.GetEquation(tabFitRight.numDecimals) : "";
	tabFitRight.r2SinusTend = tabFitRight.opSinusTend ? r2SinusTend : Null;
}

void ProcessingTab::OnShowEquation()
{
	bool show = tabFitRight.showEquation;
	int i = 1;
	tabFitLeft.scatter.Legend(i++, pscatter->GetLegend(id) + String("-") + 
						(show && tabFitRight.opAverage ? average.GetEquation(tabFitRight.numDecimals) : String(t_("Average"))));
	tabFitLeft.scatter.Legend(i++, pscatter->GetLegend(id) + String("-") + 
						(show && tabFitRight.opSlope ? slope.GetEquation(tabFitRight.numDecimals) : String(t_("Slope"))));
	tabFitLeft.scatter.Legend(i++, pscatter->GetLegend(id) + String("-") + 
						(show && tabFitRight.opLinear ? linear.GetEquation(tabFitRight.numDecimals) : String(t_("Linear"))));
	tabFitLeft.scatter.Legend(i++, pscatter->GetLegend(id) + String("-") + 
						(show && tabFitRight.opQuadratic ? quadratic.GetEquation(tabFitRight.numDecimals) : String(t_("Quadratic"))));
	tabFitLeft.scatter.Legend(i++, pscatter->GetLegend(id) + String("-") + 
						(show && tabFitRight.opCubic ? cubic.GetEquation(tabFitRight.numDecimals) : String(t_("Cubic"))));
	tabFitLeft.scatter.Legend(i++, pscatter->GetLegend(id) + String("-") + 
						(show && tabFitRight.opSinus ? sinus.GetEquation(tabFitRight.numDecimals) : String(t_("Sinusoidal"))));
	tabFitLeft.scatter.Legend(i++, pscatter->GetLegend(id) + String("-") + 
						(show && tabFitRight.opSinusTend ? sinusTend.GetEquation(tabFitRight.numDecimals) : String(t_("Sinusoidal tend"))));
	tabFitLeft.scatter.Legend(i++, pscatter->GetLegend(id) + String("-") + String(t_("Spline")));
	tabFitLeft.scatter.Legend(i++, pscatter->GetLegend(id) + String("-") + String(Format(t_("Der_%d"), ~tabFitRight.derOrder)));
	tabFitLeft.scatter.Legend(i++, pscatter->GetLegend(id) + String("-") + String(Format(t_("S_G_%d"), ~tabFitRight.sgOrder)));
	tabFitLeft.scatter.Legend(i++, pscatter->GetLegend(id) + String("-") + String(t_("FFT_filter")));
	tabFitLeft.scatter.Refresh();
}

void ProcessingTab::OnFFT() 
{
	String errText;
	tabFreqLeft.scatter.RemoveAllSeries();
	double samplingTime = tabFreqRight.samplingTime;
	if (samplingTime < 0) {
		Exclamation(t_("Incorrect sampling time"));
		return;
	}
	int64 idMaxFFT;
	{
		WaitCursor waitcursor;
		
		if (tabFitLeft.scatter.IsDeletedDataSource(0))
			return;
		DataSource &data = tabFitLeft.scatter.GetDataSource(0);

		Eigen::VectorXd sourcex, sourcey;
		data.CopyXY(sourcex, sourcey);
			
		Eigen::VectorXd resx, resy;
		Resample(sourcex, sourcey, resx, resy, samplingTime);
		
		EigenVector series(resy, resx[0], samplingTime);
		fft = series.FFTY(samplingTime, tabFreqRight.opXAxis == 1, tabFreqRight.type, 
							tabFreqRight.window.GetIndex(), tabFreqRight.num, tabFreqRight.overlapping);
		VectorPointf fftData(fft);
		double mxy = fftData.MaxY(idMaxFFT);
		if (!IsNull(mxy))
			tabFreqRight.eMax <<= Format("(%f,%f)", fftData.x(idMaxFFT), mxy);
		
		if (tabFreqRight.type == FFT_TYPE::T_PSD) {
			double m_1, m0, m1, m2;
			fftData.GetSpectralMomentsY(tabFreqRight.opXAxis == 1, m_1, m0, m1, m2);
			tabFreqRight.m_1 <<= FormatDouble(m_1);
			tabFreqRight.m0  <<= FormatDouble(m0);
			tabFreqRight.m1  <<= FormatDouble(m1);
			tabFreqRight.m2  <<= FormatDouble(m2);
		} else {
			tabFreqRight.m_1 <<= "";
			tabFreqRight.m0  <<= "";
			tabFreqRight.m1  <<= "";
			tabFreqRight.m2  <<= "";
		}
		tabFreqRight.m_1.Enable(tabFreqRight.type == FFT_TYPE::T_PSD);
		tabFreqRight.m0.Enable(tabFreqRight.type == FFT_TYPE::T_PSD);
		tabFreqRight.m1.Enable(tabFreqRight.type == FFT_TYPE::T_PSD);
		tabFreqRight.m2.Enable(tabFreqRight.type == FFT_TYPE::T_PSD);
		tabFreqRight.labSpectral.Enable(tabFreqRight.type == FFT_TYPE::T_PSD);
	}
	if (fft.IsEmpty()) {
		tabFreqLeft.comments.SetText(errText);
		Exclamation(t_("Error obtaining FFT"));
		return;
	}
	
	String strtype;
	switch(tabFreqRight.type) {
	case FFT_TYPE::T_FFT:	strtype = t_("FFT");					break;
	case FFT_TYPE::T_PHASE:	strtype = t_("FFT-phase [rad]");		break;
	case FFT_TYPE::T_PSD:	strtype = t_("Power Spectral Density");	break;
	}
	String legend = tabFitLeft.scatter.GetLegend(0) + String("-") + strtype;
	tabFreqLeft.scatter.AddSeries(fft).Legend(legend);
	tabFreqLeft.scatter.ShowInfo().ShowContextMenu().ShowProcessDlg().ShowPropertiesDlg().SetMouseHandlingLinked(true, true);
	tabFreqLeft.scatter.SetLabelX(tabFreqRight.opXAxis == 1 ? t_("Frequency [Hz]") : t_("Period [sec]"));
	tabFreqLeft.scatter.SetLabelY(legend);
	tabFreqLeft.scatter.ZoomToFit(true, true);
	if (idMaxFFT > 0 && fft[int(idMaxFFT)].x < (fft[fft.GetCount() - 1].x)/2)
		tabFreqLeft.scatter.SetRange(fft[int(idMaxFFT)].x*2, Null);
	
	tabFreqLeft.comments.SetText(errText);
}

void ProcessingTab::OnHist() {
	tabHistLeft.scatter.RemoveAllSeries();

	if (tabFitLeft.scatter.IsDeletedDataSource(0))
		return;
	DataSource &data = tabFitLeft.scatter.GetDataSource(0);

	double minVal = ~tabHistRight.minVal;
	double maxVal = ~tabHistRight.maxVal;
	if (minVal >= maxVal) {
		Exclamation(Format(t_("Min val %d has to be lower than Max val %f"), minVal, maxVal));
		return;
	}
	int numVals = ~tabHistRight.numVals;
	bool normalize = ~tabHistRight.opNormalize;
	double valNormalize = ~tabHistRight.valNormalize;
	bool isY = ~tabHistRight.axis == t_("Y");
	
	histogram.Create(data, minVal, maxVal, numVals, isY).Cumulative(~tabHistRight.opCumulative);
	
	if (normalize)
		histogram.Normalize(valNormalize);
	
	String legend = tabFitLeft.scatter.GetLegend(0) + String("-") + t_("Histogram");
	tabHistLeft.scatter.AddSeries(histogram).Legend(legend).NoMark().
		Units(normalize ? "x" + FormatDouble(valNormalize) : "#", isY ? 
											tabFitLeft.scatter.GetUnitsY(0) : 
											tabFitLeft.scatter.GetUnitsX(0));
	if (~tabHistRight.opStaggered)
		tabHistLeft.scatter.PlotStyle<StaggeredSeriesPlot>().Dash("").NoMark().Fill(Blue()).Opacity(0.3).Stroke(2, LtBlue());
	tabHistLeft.scatter.ShowInfo().ShowContextMenu().ShowProcessDlg().ShowPropertiesDlg().SetMouseHandlingLinked(true, true);
	tabHistLeft.scatter.SetLabelX(isY ? tabFitLeft.scatter.GetLegend(0) : tabFitLeft.scatter.GetLabelX());
	tabHistLeft.scatter.SetLabelY(t_("Number"));
	tabHistLeft.scatter.ZoomToFit(true, true);
	double ymax = tabHistLeft.scatter.GetYMax();
	tabHistLeft.scatter.SetXYMin(Null, 0);
	tabHistLeft.scatter.SetRange(Null, ymax);
}

void ProcessingTab::OnPercentile() {
	if (tabFitLeft.scatter.IsDeletedDataSource(0))
		return;
	DataSource &data = tabFitLeft.scatter.GetDataSource(0);
	
	double minVal = ~tabHistRight.minVal;
	double maxVal = ~tabHistRight.maxVal;
	if (minVal >= maxVal) {
		Exclamation(Format(t_("Min val %d has to be lower than Max val %f"), minVal, maxVal));
		return;
	}
	bool isY = ~tabHistRight.axis == t_("Y");
	
	double per = ~tabHistRight.percentile;
	
	if (isY)
		tabHistRight.resPercentile <<= data.PercentileValY(per, minVal, maxVal);
	else
		tabHistRight.resPercentile <<= data.PercentileValX(per, minVal, maxVal);

	tabHistRight.resPercentileW <<= data.PercentileWeibullVal(isY, per, minVal, maxVal);	
}

}
// SPDX-License-Identifier: Apache-2.0
// Copyright 2021 - 2022, the Anboto author and contributors
#include "ScatterCtrl_Demo.h"


void Tab6_Logarithmic::Init() {
	CtrlLayout(*this);	
	SizePos();
	
	scatter.SetRange(6,100);
	scatter.SetMajorUnits(1,20);
	scatter.ShowInfo();
	scatter.SetMouseHandling(true, true);
	
	s1 << Pointf(log10(5),14)<<Pointf(log10(1e2),25)<<Pointf(log10(1e3),39)<<Pointf(log10(1e4),44)<<Pointf(log10(1e5),76);
	scatter.AddSeries(s1).Legend("series1");
	
	scatter.cbModifFormatX = scatter.cbModifFormatDeltaX = THISBACK(FormatX);
	scatter.cbModifFormatXGridUnits = THISBACK(FormatXGridUnits);
	scatter.SetGridLinesX = THISBACK(SetGridLinesX);
}

void Tab6_Logarithmic::FormatX(String& s, int , double d) {
	if (d < 1)
		return;
	s = FormatE(pow(10, d), 0);
}

void Tab6_Logarithmic::FormatXGridUnits(String& s, int i, double d) {
	if (d < 1)
		return;
	if (abs(d - int(d)) < 0.0001)
		s = FormatE(pow(10, d), 0);
}

void Tab6_Logarithmic::SetGridLinesX(Vector<double>& unitsX) {
	for(int i = 0; scatter.GetXMinUnit() + i*scatter.GetMajorUnitsX() <= scatter.GetXRange(); i++) 
		for (int ii = 1; ii < 10; ++ii)
			unitsX << scatter.GetXMinUnit() + (i + log10(ii))*scatter.GetMajorUnitsX();
}

ScatterDemo *Construct6() {
	static Tab6_Logarithmic tab;
	return &tab;
}

INITBLOCK {
	RegisterExample("Log", Construct6, __FILE__);
}
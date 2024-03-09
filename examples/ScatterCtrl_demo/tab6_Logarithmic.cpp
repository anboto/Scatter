// SPDX-License-Identifier: Apache-2.0
// Copyright 2021 - 2022, the Anboto author and contributors
#include "ScatterCtrl_Demo.h"


void Tab6_Logarithmic::Init() {
	CtrlLayout(*this);	
	SizePos();
	
	scatter.ShowInfo();
	scatter.SetMouseHandling(true, true);
	
	opLog = 0;
	opLog.WhenAction = [&]() {WhenOp();};

	scatter.AddSeries(s1).Legend("series1");
	
	WhenOp();	
}

void Tab6_Logarithmic::WhenOp() {
	s1.Clear();
	if (opLog == 0)
		s1 << Pointf(-0.1,4) << Pointf(0.000001,2) << Pointf(0.1,4) << Pointf(0.5,8) << Pointf(1,10) << Pointf(10,14) << Pointf(100,25) << Pointf(1000,39) << Pointf(10000,44) << Pointf(10000000,76) << Pointf(10000000000,101) << Pointf(1000000000000,123) << Pointf(1E+21,156) << Pointf(1E+30,197) << Pointf(1E+35,235);
	else
		s1 << Pointf(-2, 0) << Pointf(0, 0.1) << Pointf(0, pow(10, 1)) << Pointf(1, pow(10, 2)) << Pointf(2, pow(10, 3)) << Pointf(3, pow(10, 4)) << Pointf(4, pow(10, 5)) << Pointf(5, pow(10, 6));
	
	scatter.SetLogX(opLog == 0).SetLogY(opLog == 1);
	scatter.ZoomToFit(true, true);
}

ScatterDemo *Construct6() {
	static Tab6_Logarithmic tab;
	return &tab;
}

INITBLOCK {
	RegisterExample("Log", Construct6, __FILE__);
}
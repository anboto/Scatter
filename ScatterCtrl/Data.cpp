// SPDX-License-Identifier: Apache-2.0
// Copyright 2021 - 2022, the Anboto author and contributors
#include "ScatterCtrl.h"

namespace Upp {

void ScatterCtrl::DoShowData() {
	DataDlg dlg;
	dlg.Init(*this);
	dlg.Run();
}

void DataDlg::Init(ScatterCtrl& scatter) {
	CtrlLayout(*this, t_("Scatter data"));
	Sizeable().Zoomable();
	
	this->pscatter = &scatter;

	tab.WhenAction = [=]{OnTab();};
	butOK.WhenAction = [=] {Close();};
	butSave.WhenAction = [&] {scatter.SaveToFileData();};
	
	tab.Reset();
	series.Clear();
	if (scatter.IsSurf() && !scatter.GetSurf().IsExplicit()) {
		WithDataSeries <StaticRect> &dataseries = series.Add();
		CtrlLayout(dataseries);
		dataseries.scatterIndex.Hide();
		tab.Add(dataseries.SizePos(), scatter.GetTitle());
	} else {
		for(int c = 0; c < scatter.GetCount(); c++)
			if (!IsNull(scatter.GetCount(c))) {
				WithDataSeries <StaticRect> &dataseries = series.Add();
				CtrlLayout(dataseries);
				dataseries.scatterIndex.Hide();
				dataseries.scatterIndex = c;
				tab.Add(dataseries.SizePos(), scatter.GetLegend(c));
			}
	}
	if (tab.GetCount() < 1)
		return;
	OnTab();
	
	if (!scatter.IsSurf()) {
		if (scatter.IsDeletedDataSource(0))
			return;
		bool addedAll = true;
		for (int c = 1; c < scatter.GetCount(); c++) {
			if (!IsNull(scatter.GetCount(c) && !scatter.IsDeletedDataSource(c))) {
				DataSource &serie0 = scatter.GetDataSource(c-1);
				DataSource &serie  = scatter.GetDataSource(c);
				if (!serie0.SameX(serie, false)) {
					addedAll = false;
					break;
				}
			}
		}
		if (addedAll) {
			WithDataSeries <StaticRect> &dataseries = series.Insert(0);
			CtrlLayout(dataseries);
			dataseries.scatterIndex.Hide();
			dataseries.scatterIndex = -1;
			tab.Insert(0, dataseries.SizePos(), t_("All"));
			tab.Set(0);
			OnTab();
		}
	}
}

Value DataDlg::DataSourceX::Format(const Value& q) const {
	if (pscatter->IsDeletedDataSource(index))
		return Null;
	if (int(q) >= pscatter->GetDataSource(index).GetCount())
		return Null;
	if (pscatter->GetDataSource(index).IsExplicit())
		return Null;
	Value ret = pscatter->GetStringX(index, q, true);
	return ret;
}

Value DataDlg::DataSourceY::Format(const Value& q) const {
	if (pscatter->IsDeletedDataSource(index))
		return Null;
	if (int(q) >= pscatter->GetDataSource(index).GetCount())
		return Null;
	if (pscatter->GetDataSource(index).IsExplicit())
		return Null;
	Value ret = pscatter->GetStringY(index, q, true);
	return ret;
}

void DataDlg::OnTab() {
	int index = tab.Get();
	if (index < 0)
		return;
	
	ScatterCtrl &scatter = *pscatter;
	ArrayCtrl &array = series[index].array;
	array.Reset();
	array.MultiSelect().SetLineCy(EditField::GetStdHeight());
	if (scatter.IsSurf() && !scatter.GetSurf().IsExplicit()) {
		array.HeaderObject().Absolute();
		DataSourceSurf& surf = scatter.GetSurf();
		array.AddColumn("", 3*EditField::GetStdHeight());
		if (surf.IsAreas()) {
			for (int iy = 0; iy < surf.rows()-1; ++iy) {
				array.Set(iy, 0, FDS(surf.y(iy), 8) + " - " + FDS(surf.y(iy+1), 8));
				for (int ix = 0; ix < surf.cols()-1; ++ix) {
					if (iy == 0)
						array.AddColumn(FDS(surf.x(ix), 8) + " - " + FDS(surf.x(ix+1), 8), 3*EditField::GetStdHeight());
					array.Set(iy, ix+1, surf.z_data(ix, iy));
				}
			}
		} else {
			for (int iy = 0; iy < surf.rows(); ++iy) {
				array.Set(iy, 0, FDS(surf.y(iy), 8));
				for (int ix = 0; ix < surf.cols(); ++ix) {
					if (iy == 0)
						array.AddColumn(FDS(surf.x(ix), 8), 3*EditField::GetStdHeight());
					array.Set(iy, ix+1, surf.z_data(ix, iy));
				}
			}
		}
	} else {
		int scatterIndex = series[index].scatterIndex;
		dataSourceYArr.Clear();
		
		if (scatterIndex >= 0) {
			array.SetVirtualCount(int(scatter.GetCount(scatterIndex)));
			DataSourceY &dataSourceY = dataSourceYArr.Add();
			dataSourceX.pscatter = dataSourceY.pscatter = pscatter;
			dataSourceX.index = dataSourceY.index = scatterIndex;
			
			String textX = pscatter->GetLabelX();
			if (!pscatter->GetUnitsX(scatterIndex).IsEmpty()) {
				String unitsX = "[" + pscatter->GetUnitsX(scatterIndex) + "]";
				if (textX.Find(unitsX) < 0)
					textX << " " << unitsX;
			}
			array.AddRowNumColumn(textX).SetConvert(dataSourceX);
			
			String textY = pscatter->GetLegend(scatterIndex);
			if (!pscatter->GetUnitsY(scatterIndex).IsEmpty()) {
				String unitsY = "[" + pscatter->GetUnitsY(scatterIndex) + "]";
				if (textY.Find(unitsY) < 0)
					textY << " " << unitsY;
			}
			array.AddRowNumColumn(textY).SetConvert(dataSourceY);
		} else {
			array.SetVirtualCount(int(scatter.GetCount(0)));
			dataSourceX.pscatter = pscatter;
			dataSourceX.index = 0;
			
			String textX = pscatter->GetLabelX();
			if (!pscatter->GetUnitsX(0).IsEmpty()) {
				String unitsX = "[" + pscatter->GetUnitsX(0) + "]";
				if (textX.Find(unitsX) < 0)
					textX << " " << unitsX;
			}
			array.AddRowNumColumn(textX).SetConvert(dataSourceX);
			
			for (int c = 0; c < scatter.GetCount(); ++c) {
				if (!IsNull(scatter.GetCount(c))) {
					DataSourceY &dataY = dataSourceYArr.Add();
					dataY.pscatter = pscatter;
					dataY.index = c;
					
					String textY = pscatter->GetLegend(c);
					if (!pscatter->GetUnitsY(c).IsEmpty()) {
						String unitsY = "[" + pscatter->GetUnitsY(c) + "]";
						if (textY.Find(unitsY) < 0)
							textY << " " << unitsY;
					}
					array.AddRowNumColumn(textY).SetConvert(dataY);	
				}
			}
		}
	}
	array.WhenBar = [=](Bar &bar) {OnArrayBar(bar);};
}

void DataDlg::ArrayCopy() {
	int index = tab.Get();
	if (index < 0)
		return;
	
	ArrayCtrl &array = series[index].array;
	array.SetClipboard(true, true);
}

void DataDlg::ArraySaveToFile(String fileName) {
	int index = tab.Get();
	if (index < 0)
		return;
	
	FileSel fileToSave;
	
	fileToSave.NoExeIcons();
	
	GuiLock __;
	if (IsNull(fileName)) {
		String name = pscatter->GetTitle() + " " + pscatter->GetLegend(series[index].scatterIndex);
		if (name.IsEmpty())
			name = t_("Scatter plot data");
		fileToSave.PreSelect(ForceExt(name, ".csv"));
		fileToSave.ClearTypes();
		fileToSave.Type(Format(t_("%s file"), t_("Comma separated values (.csv)")), "*.csv");
	    if(!fileToSave.ExecuteSaveAs(t_("Saving plot data"))) {
	        Exclamation(t_("Plot data has not been saved"));
	        return;
	    }
        fileName = fileToSave;
	} 
	ArrayCtrl &array = series[index].array;
	SaveFileBOMUtf8(fileName, array.AsText(AsString, true, pscatter->GetDefaultCSVSeparator(), 
								"\r\n", pscatter->GetDefaultCSVSeparator(), "\r\n"));
}

void DataDlg::ArraySelect() {
	int index = tab.Get();
	if (index < 0)
		return;
	
	ArrayCtrl &array = series[index].array;
	array.Select(0, array.GetCount(), true);
}

void DataDlg::OnArrayBar(Bar &menu) {
	int index = tab.Get();
	if (index < 0)
		return;
	
	menu.Add(t_("Select all"), Null, [=] {ArraySelect();}).Key(K_CTRL_A)
								.Help(t_("Select all rows"));
								
	ArrayCtrl &array = series[index].array;							
	int count = array.GetSelectCount();
	if (count == 0)
		menu.Add(t_("No row selected"), Null, Null).Enable(false).Bold(true);
	else {
		menu.Add(Format(t_("Selected %d rows"), count), Null, Null).Enable(false).Bold(true);
		menu.Add(t_("Copy"), ScatterImgP::Copy(), [=] {ArrayCopy();}).Key(K_CTRL_C)
									.Help(t_("Copy selected rows to clipboard"));
		menu.Add(t_("Save to file"), ScatterImgP::Save(), [=] {ArraySaveToFile(Null);}).Key(K_CTRL_S)
									.Help(t_("Save to .csv file"));
	}
}

}
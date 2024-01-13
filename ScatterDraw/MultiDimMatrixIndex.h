// SPDX-License-Identifier: Apache-2.0
// Copyright 2021 - 2022, the Anboto author and contributors
#ifndef _ScatterDraw_MultiDimMatrixIndex_h_
#define _ScatterDraw_MultiDimMatrixIndex_h_

namespace Upp {

class MultiDimMatrixIndex {
public:
	MultiDimMatrixIndex()			   {};
	template<typename... Args>
	MultiDimMatrixIndex(Args... args)  {SetAxis(args...);}
	
	void SetNumAxis(int numAxis)	   {axisDim.SetCount(numAxis);};
	void SetAxisDim(int axis, int dim) {
		ASSERT(axis >= 0 && axis < axisDim.size() && dim > 0);
		axisDim[axis] = dim;
	}
	void SetAxis(int dimX) {
		ASSERT(dimX > 0);
		axisDim << dimX;
	}
	template<typename... Args>
	void SetAxis(int t, Args... args) {
		ASSERT(t > 0);
		axisDim << t;
		SetAxis(args...);
	}
		
	Vector<int> &GetAxisDim()	{return axisDim;};
	int GetIndex(const Vector<int> &idx) const {
		if (colMajor) {
			int index = 0;
			int multiplier = 1;
			for (int ix = 0; ix < axisDim.size(); ++ix) {
				ASSERT(idx[ix] >= 0 && idx[ix] < axisDim[ix]);
				index += multiplier*idx[ix];
				multiplier *= axisDim[ix];
			}
			return index;
		} else {
			int index = 0;
			int multiplier = 1;
			for (int ix = axisDim.size()-1; ix >= 0; --ix) {
				ASSERT(idx[ix] >= 0 && idx[ix] < axisDim[ix]);
				index += multiplier*idx[ix];
				multiplier *= axisDim[ix];
			}
			return index;
		}
	}
	template<typename T, typename... Args>
	int GetIndex(T t, Args... args) const {
		Vector<int> index;
		
		index << t;
	    AddIndex(index, args...);
	
	    return GetIndex(index);
	}
	template<typename... Args>
	inline int operator()(Args... args) const {return GetIndex(args...);}
	
	inline int GetIndex(int x, int y) const {
		ASSERT(IsValid(x, y));
		if (colMajor) 
			return x + axisDim[0]*y;
		else
			return y + axisDim[1]*x;
	}
	inline int operator()(int x, int y) const  {return GetIndex(x, y);}
		
	inline bool IsValid(int x, int y) const  {
		return x >= 0 && x < axisDim[0] && y >= 0 && y < axisDim[1];
	}
	inline int GetNumAxis() const  {return axisDim.size();}
	int GetNumData() const {
		int ret = 1;
		for (auto dim : axisDim)
			ret *= dim;
		return ret;
	}
	int size() const			{return GetNumData();}
	int size(int dim) const		{return axisDim[dim];}
	
	void ColMajor(bool c = true){colMajor = c;}
	void RowMajor(bool c = true){colMajor = !c;}
	
	void Xmlize(XmlIO xml) {
		xml
			("axisDim", axisDim)	
		;	
	}
	void Jsonize(JsonIO& json) {
		json
			("axisDim", axisDim)	
		;	
	}
private:
	Vector<int> axisDim;
	
	template<typename T>
	static void AddIndex(Vector<int> &index, T t) {
		index << t;
	}	
	template<typename T, typename... Args>
	static void AddIndex(Vector<int> &index, T t, Args... args) {
		index << t;
		AddIndex(index, args...);
	}

protected:
	bool colMajor = true;
};

class MultiDimMatrixIndexRowMajor : public MultiDimMatrixIndex {
public:
	MultiDimMatrixIndexRowMajor() 				{colMajor = false;};
	template<typename... Args>
	MultiDimMatrixIndexRowMajor(Args... args) 	{colMajor = false; SetAxis(args...);}
};
	
}

#endif

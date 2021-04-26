// Matrix.  Steve Dunn 2003.  Apache License Version 2.0, January 2004.
// Please see LICENSE file.

#ifndef MATRIX_H_INCLUDED
#define MATRIX_H_INCLUDED

#pragma once

#pragma  warning( disable : 4786 )
#include <stdio.h>
#include <list>
#include <vector>
#include <map>
#include <comdef.h>
#include <assert.h>
#include <tchar.h>

namespace Matrix
{
	/* CHANGE THIS IF THE YOU THIS FILE.  THIS CONSTANT CAN BE USED BY CLIENTS TO CHECK IF THEY'RE COMPILING WITH
	   THE CORRECT VERSION OF THIS FILE
	*/

	const UINT __MATRIX_VERSION = 18;

	enum direction {
		north,
		northeast,
		east,
		southeast,
		south,
		southwest,
		west,
		northwest,
		nowhere
	};

	static struct _dir
	{
		direction dirStart;
		direction dirEnd;
		int xStep;
		int yStep;
	} dirs[] =
	{
		{ north, south, 0, 1 },
		{ south, north,	0, -1 } ,

		{ west,	east, 1, 0 } ,
		{ east,	west, -1, 0 },

		{ northeast, southwest,	-1,	1 } ,
		{ southwest, northeast,	1,	-1 } ,

		{ northwest, southeast,	1,	1 } ,
		{ southeast, northwest,	-1, -1 } ,

		{ nowhere, nowhere,	0, 0 }
	};

	template <typename T> class CRow;
	template <typename T> class CMatrix;
	template <typename T> class CMarker;
	template <typename T> class CCell;

	class CMatrixBase;

#define CELL( m, x , y ) m.cell(x,y)

#define XYSTATUS(x,y) assert(x>=0); \
    assert(y>=0);

	class  CStreamOperator
	{
	protected:
		_bstr_t	m_strName;

	public:
		CStreamOperator() {	};
		CStreamOperator(const TCHAR* pszName) : m_strName(pszName)
		{}
		const _bstr_t& name() const throw() { return m_strName; }
		virtual bool go(CMatrixBase& s) = 0;
	};

	class CExpandable
	{
	public:
		virtual bool expand(const long n) = 0;
	};

	template <typename T>
	class CMarker
	{
		typedef class CMarker<T>* LPMARKER;
	protected:
		long m_x, m_y;
		CMatrix<T>& m_matrix;
	public:
		CMarker(CMatrix<T>& m, long x = 0, long y = 0);
		friend CMatrix<T>;
	};

	template<typename T>
	class  CCell
	{
		typedef T cellType;
		typedef CCell<cellType> myT;
		typedef myT* LPCELL;
	protected:
		T		m_type;

	public:
		CCell() { 	}
		operator T& () { return m_type; }
		operator const T& () const { return m_type; }
		operator T* () { return &m_type; }
		/*
			if you get a compilation error here and are constructing a matrix from a
			variant (CMatrix m( theVariant )), then the matrix you are creating is
			not variant compatible, for instance, instead of using CMatrix<CString> matrix( theVariant),
			use CMatrix<_bstr_t> matrix( theVariant );

		*/
		CCell(T t) { m_type = t; }
		CCell<T>& operator = (T t) { m_type = t; return *this; }
		CCell<T>& operator = (T* t) { m_type = *t; return *this; }
		virtual ~CCell() throw()
		{	};

	};

	template<typename T>
	class CRow :
		public std::vector<CCell<T>*>,
		public CExpandable
	{
	public:
		typedef std::vector<CCell<T>*> COLUMN;
		CCell<T>& operator[](const ULONG n);
		const CCell<T>& operator[](const ULONG n) const;
		virtual ~CRow() throw()
		{
			for (iterator i = begin(); i != end(); i++)
				delete (*i);

			clear();
		}

		virtual bool expand(const long n);
	};

	/*	THE MATRIX BASE CLASS.  EVERYTHING IN HERE IS NOT RELIANT UPON THE TEMPLATE
			AND HENCE CAN BE USED FROM EXTERNAL CLIENTS THAT ALSO DO NOT NEED THE
			INFORMATION CONTAINED IN THE TEMPLATE
	*********************************************************************************/

	class  CMatrixBase
	{
	protected:
		_bstr_t	m_strMarker;
		long m_x,
			m_y,
			m_nGrow;

		CMatrixBase()
		{
			m_x = m_y = m_nGrow = 0;
			m_strMarker = "";
		}

		virtual ~CMatrixBase() throw() {};


		bool getDirectionSteps(long& x, long& y, direction dStart, direction dEnd)
		{
			x = y = 0;		long i = -1;
			while (dirs[++i].dirStart != nowhere)
			{
				_dir& d = dirs[i];

				if (d.dirStart == dStart && d.dirEnd == dEnd)
				{
					x = d.xStep;
					y = d.yStep;
					return true;
				}
			}
			return false;
		}

	public:
		virtual void setBookmark(const _bstr_t& pszBookmark)
		{
			m_strMarker = pszBookmark;
		}

		const _bstr_t& getBookmark() const
		{
			return m_strMarker;
		}

		void setCords(long x, long y)
		{
			XYSTATUS(m_x, m_y); m_y = y; m_x = x;
		}

		long y() const { return m_y; }
		long x() const { return m_x; }
	};

	template<typename T>
	class  CMatrix
		: public std::vector<CRow<T>*>,
		public std::map<_bstr_t, CMarker<T>*>,
		public CExpandable,
		public CMatrixBase
	{
		typedef std::vector<CRow<T>*> ROW;
		typedef std::vector<CCell<T>*> COLUMN;
		typedef std::map<_bstr_t, CMarker<T>*> MARKER;

	protected:

	public:
		CMatrix(long x = 0, long y = 0, long grow = 0);
		CMatrix(const CMatrix<T>& m);

		virtual ~CMatrix() throw();
		CMatrix<T>& operator =(const CMatrix<T>& m);

#ifndef MATRIX_NO_VARIANT
		CMatrix<T>& operator =(VARIANT& v);
		CMatrix(VARIANT& v);
		virtual operator VARIANT();
#endif //MATRIX_NO_VARIANT

		virtual void clear();

		CRow<T>& operator[] (const ULONG n);
		const CRow<T>& operator[] (const ULONG n) const;

		CMarker<T>& operator[] (const TCHAR* p)
		{
			MARKER::iterator it = MARKER::find(p);
			assert(it != MARKER::end());

			CMarker<T>* pMarker = it->second;
			assert(pMarker);

			return *pMarker;
		}

		const CMarker<T>& operator[] (const TCHAR* p) const
		{
			MARKER::const_iterator it = MARKER::find(p);
			assert(it != MARKER::end());

			CMarker<T>* pMarker = it->second;
			assert(pMarker);

			return *pMarker;
		}

		CMarker<T>& addMarker(const _TCHAR*, long x, long y);
		CMatrix<T>& attach(CMatrix<T>& m);
		bool expand(const long n);

		ULONG width() const
		{
			ULONG n = 0;
			for (ROW::const_iterator i = ROW::begin(); i != ROW::end(); i++)
			{
				CRow<T>& r = (**i);
				ULONG nRowSize = r.size();

				if (n < nRowSize)
					n = nRowSize;
			}

			return n;
		}

		ULONG height() const {
			return ROW::size();
		}

		const T& cell(const long x, const long y)	const
		{
			if (!m_strMarker.length())
				return ((const CMatrix&)*this)[y][x];
		
			const CMarker<T>& p = ((const CMatrix&)*this)[(const TCHAR*)m_strMarker];
			
			return ((const CMatrix&)*this)[y + p.m_y][x + p.m_x];
		}

		T& cell(long x, long y)
		{
			if (!m_strMarker.length())  
				return (*this)[y][x];
			
			CMarker<T>& p = (*this)[(const TCHAR*)m_strMarker];
			
			return (*this)[y + p.m_y][x + p.m_x];
		}

		bool insertRow(ULONG lPrevRow, ULONG lRows);
		bool insertColumn(ULONG lPrevColumn, ULONG lColumns);
		bool deleteRow(ULONG lRow);
		bool deleteColumn(ULONG lColumn);

		void set(const T& t)
		{
			cell(m_x, m_y) = t;
		}

		friend CMatrix<T>& operator << (CMatrix<T>& m, const T& t)
		{
			m.set(t);		return m;
		}
		friend CMatrix<T>& operator << (CMatrix<T>& m1, CMatrix<T>& m2)
		{
			return ((CMatrix<T>&) m1).attach(m2);
		}
		friend CMatrix<T>& operator << (CMatrix<T>& mb, CStreamOperator& so)
		{
			so.go(mb);	return mb;
		}

		/*
			INSERT THE CONTENTS OF A SERIES OF ITERATORS INTO THE MATRIX
		********************************************/
		template< typename IT>	CMatrix& fill(IT itStart, IT itEnd, direction compassStart = west, direction compassEnd = east)
		{
			long n = 0, x = m_x, y = m_y, xStep, yStep;
			bool b = getDirectionSteps(xStep, yStep, compassStart, compassEnd);
			assert(b);
			for (IT t = itStart; t != itEnd; t++)
			{
				cell(x, y) = (*t);
				x += xStep;	y += yStep;
				assert(x >= 0);	assert(y >= 0);
			}
			return *this;
		};

		/*
			THE METHOD BELOW WAS IMPLEMENTED AS AN ALTERNATIVE TO
			IMPLEMENTING THE DESIRED FUNCTIONALITY IN THE TEMPLATED
			FUNCTION "FILL()" WHICH ASSUMES AN ITERATOR OF SOME KIND.
		**************************************************************/
		template<typename T>
		CMatrix& fillFromMap(T itStart, T itEnd, direction compassStart = west, direction compassEnd = east, int nFirstOrSecond = 1)
		{
			long n = 0, x = m_x, y = m_y, xStep, yStep;
			bool b = getDirectionSteps(xStep, yStep, compassStart, compassEnd);
			assert(b);
			for (T t = itStart; t != itEnd; t++)
			{
				if (nFirstOrSecond == 2)
					cell(x, y) = (T&)(*t).second;
				else
					cell(x, y) = (T&)(*t).first;
				x += xStep;	y += yStep;
				XYSTATUS(x, y);
			}
			return *this;
		};
	};

	class move : public CStreamOperator
	{
	protected:
		long	m_x, m_y;
	public:
		move(long x, long y) : m_y(y), m_x(x) {};
		virtual bool go(CMatrixBase& s)
		{
			s.setCords(m_x, m_y);	return true;
		}
	};

	class offset : public move
	{
	public:
		offset(long x, long y) :move(x, y) {};
		virtual bool go(CMatrixBase& mb)
		{
			m_x += mb.x(); m_y += mb.y(); return move::go(mb);
		}
	};


	class bookmark : public CStreamOperator
	{
	public:
		bookmark(const TCHAR* pszName = NULL) : CStreamOperator(pszName) {}
		virtual bool go(CMatrixBase& so) { so.setBookmark(name());	return true; }
	};

	template<typename T>
	CMatrix<T>::CMatrix(long x/*=0*/, long y/*=0*/, long grow /* =0 */) : CMatrixBase()
	{
		m_x = m_y = 0;
		m_nGrow = grow;
		m_strMarker = "";
		for (long i = 0; i < y; i++)
		{
			CRow<T>* p;	push_back(p = new CRow<T>());
			for (long a = 0; a < x; a++)
				p->push_back(new CCell<T>());
		}
	}

	template<typename T>
	CMatrix<T>::~CMatrix() throw()
	{
		clear();
	}

	template<typename T>
	void CMatrix<T>::clear()
	{
		for (ROW::iterator i = ROW::begin(); i != ROW::end(); i++)
			delete (*i);

		ROW::clear();

		for (MARKER::iterator a = MARKER::begin(); a != MARKER::end(); a++)
			delete (*a).second;

		MARKER::clear();
	}

	template <typename T>
	CRow<T>& CMatrix<T>::operator[](const ULONG n)
	{
		if (n >= ROW::size())
			expand(n);

		return *ROW::operator[](n);
	}

	template <typename T>
	const CRow<T>& CMatrix<T>::operator[](const ULONG n) const
	{
		assert(n < ROW::size());
		return *ROW::operator[](n);
	}

	template <typename T>
	CMarker<T>&	CMatrix<T>::addMarker(const _TCHAR* strName, long x, long y)
	{
		XYSTATUS(x, y);
		CMarker<T>* pMarker = new CMarker<T>(*this, x, y);
		MARKER::insert(MARKER::value_type(strName, pMarker));

		return *pMarker;
	}


	/*		
	IF YOU GET A COMPILIATION ERROR LIKE THE ONE BELOW, IT MEANS THAT YOUR TYPE CANNOT BE PUT
	INTO A VARIANT.  PROVIDE YOUR CLASS WITH AN OPERATOR THAT WILL INFORM THE VARIANT AS TO WHAT
	TYPE IT ACTUALLY IS

	error C2679: binary '=' : no operator defined which takes a right-hand operand of type 'class CCell<class CMyObject>'

	AN EXAMPLE WOULD BE 'CMyObject', ADD....

		operator LPCTSTR( ) { return "Hello World!" ; }
	*********************************************************************************************************/

	template <typename T>
	CMatrix<T>&	CMatrix<T>::attach(CMatrix<T>& m)
	{
		_bstr_t strOldMarker = m.getBookmark();
		m << bookmark();

		/*	COPY THE CONTENTS OF THE SOURCE MATRIX TO THIS MATRIX*/
		long height, width;
		for (height = 0; height < m.height(); height++)
			for (width = 0; width < m.width(); width++)
				cell(x() + width, y() + height) = m.cell(width, height);

		m << bookmark(strOldMarker);
		
		return *this;
	}

	template <typename T>
	CMatrix<T>::CMatrix(const CMatrix<T>& m)
	{
		*this = m;
	}

	template <typename T>
	CMatrix<T>&	CMatrix<T>::operator=(const CMatrix<T>& m)
	{
		m_x = m.m_x;
		m_y = m.m_y;
		m_nGrow = m.m_nGrow;
		m_strMarker = m.m_strMarker;
		long height, width;
		clear();
		long x = m.width(), y = m.height();
		
		for (height = 0; height < y; height++)
			for (width = 0; width < x; width++)
				cell(width, height) = m.cell(width, height);
		
		return *this;
	}

#ifndef MATRIX_NO_VARIANT
	template <typename T>
	CMatrix<T>&	CMatrix<T>::operator=(VARIANT& v)
	{
		clear();
		assert(v.vt & VT_ARRAY);

		if (v.vt == VT_EMPTY)
			return *this;
		
		long xLow, xHigh, yLow, yHigh, ix[2];
		
		xLow = xHigh = yLow = yHigh = ix[0] = ix[1] = m_nGrow = 0;
		
		SafeArrayGetUBound(v.parray, 1, &xHigh); SafeArrayGetLBound(v.parray, 1, &xLow);
		SafeArrayGetUBound(v.parray, 2, &yHigh); SafeArrayGetLBound(v.parray, 2, &yLow);
		
		long columns = xHigh - xLow, rows = yHigh - yLow;

		for (long c = 0; c <= columns; c++)
		{
			for (long r = 0; r <= rows; r++)
			{
				ix[0] = c;		ix[1] = r;
				_variant_t vt;
				HRESULT hr = SafeArrayGetElement(v.parray, ix, &vt);
				cell(c, r) = vt;

				if (V_VT(&vt) == VT_UNKNOWN)
					V_UNKNOWN(&vt)->Release();
				if (V_VT(&vt) == VT_DISPATCH)
					V_DISPATCH(&vt)->Release();
			}
		}
		
		return *this;
	}

	template <typename T>
	CMatrix<T>::operator VARIANT()
	{
		VARIANT v;	VariantInit(&v);
		long rows = height(), cols = width();
		SAFEARRAYBOUND saBound[2];
		saBound[0].cElements = cols;	saBound[0].lLbound = 0;
		saBound[1].cElements = rows;	saBound[1].lLbound = 0;
		SAFEARRAY FAR* psa;
		psa = SafeArrayCreate(VT_VARIANT, 2, saBound);

		long idx[2];
		for (long col = 0; col < cols; col++)
			for (long row = 0; row < rows; row++)
			{
#pragma warning( push ) //warning C4800: 'class <T> *' : forcing value to bool 'true' or 'false' (performance warning)
#pragma warning( disable: 4800 )
				_variant_t vt = cell(col, row);
#pragma warning( pop ) //warning C4800: 'class <T>*' : forcing value to bool 'true' or 'false' (performance warning)

				idx[0] = col;		idx[1] = row;
				HRESULT hr = SafeArrayPutElement(psa, idx, &vt);
				assert(SUCCEEDED(hr));
			}
		v.parray = psa; v.vt = VT_ARRAY | VT_VARIANT;
		return v;
	}

	template <typename T>
	CMatrix<T>::CMatrix(VARIANT& v) : CMatrixBase()
	{
		*this = v;
	}
#endif //MATRIX_NO_VARIANT

	/*
				MARKER
	************************************/

	template<typename T>
	CMarker<T>::CMarker(CMatrix<T>& m, long x /*= 0*/, long y /* = 0*/)
		: m_matrix(m)
	{
		m_x = x; m_y = y;
		XYSTATUS(m_x, m_y);
	}

	/*		ROW
	*/

	template <typename T>
	CCell<T>& CRow<T>::operator[](const ULONG n)
	{
		if (n >= size())
			expand(n);
		return *(COLUMN::operator[ ] (n));
	}

	template <typename T>
	const CCell<T>&	CRow<T>::operator[](const ULONG n) const
	{
		assert(n < size());
		return *(COLUMN::operator[ ] (n));
	}

	template<typename T>
	bool CRow<T>::expand(const long n)
	{
		for (long i = size(); i <= (n /* + CMatrix::nGrow */); i++)
			push_back(new CCell<T>());
		
		return true;
	}

	template<typename T>
	bool CMatrix<T>::deleteRow(ULONG lRow)
	{
		assert(lRow < height());
		ROW::iterator it = ROW::begin() + lRow;
		delete (*it);
		ROW::erase(it);
		
		return true;
	}

	template<typename T>
	bool CMatrix<T>::deleteColumn(ULONG lColumn)
	{
		for (ROW::iterator rowIt = ROW::begin(); rowIt != ROW::end(); rowIt++)
		{
			if (lColumn < (*rowIt)->size())
			{
				COLUMN::iterator colIt = (*rowIt)->begin() + lColumn;
				delete (*colIt);
				(*rowIt)->erase(colIt);
			}
		}
		return true;
	}

	template <typename T>
	bool CMatrix<T>::expand(const long n)
	{
		long lSize(ROW::size());
		long lRows = n + m_nGrow;
		for (long i = lSize; i <= lRows; i++)
			push_back(new CRow<T>());
		
		return true;
	}

	template<typename T>
	bool CMatrix<T>::insertRow(ULONG lPrevRow, ULONG lRows)
	{
		if (lPrevRow < height())
		{
			ROW::iterator it = ROW::begin() + lPrevRow;
			while (lRows--)
				it = ROW::insert(it, new CRow<T>());
		}
		else
			expand(lPrevRow + lRows);
	
		return true;
	}

	template<typename T>
	bool CMatrix<T>::insertColumn(ULONG lPrevColumn, ULONG lColumns)
	{
		for (ROW::iterator rowIt = ROW::begin(); rowIt != ROW::end(); rowIt++)
		{
			if ((*rowIt)->size() <= lPrevColumn)
				(*rowIt)->expand(lPrevColumn + lColumns - 1); //try to expand regardless of whether it's already wide enough
			else
			{
				long lc(lColumns);
				while (lc--)
				{
					if ((*rowIt)->size())
						(*rowIt)->insert((*rowIt)->begin() + lPrevColumn, new CCell<T>());
				}
			}
		}
		
		return true;
	}
}//namespace Matrix

#endif //MATRIX_H_INCLUDED

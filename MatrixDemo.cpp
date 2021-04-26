#include <iostream>
#include "Matrix.h"
#include <iostream>
#include <fstream>

using namespace std;
using namespace Matrix;

/*
 * What follows are 3 methods.  The first is verbose and uses the COM API directly with no use of Matrix.
 * The second uses the Matrix. The third is a more concise version of the second.
 */

// This is here just to demonstrate using the COM API and how using the Matrix can simplify and shorten things.
void version1()
{
	SAFEARRAYBOUND saBound[2];
	saBound[0].cElements = 2;	saBound[0].lLbound = 0;
	saBound[1].cElements = 2;	saBound[1].lLbound = 0;
	SAFEARRAY FAR* psa = SafeArrayCreate(VT_VARIANT, 2, saBound);

	_variant_t v1 = 1L, v2 = 2L, v3 = 3L, v4 = 4L;

	long idx[2];
	idx[0] = 0;
	idx[1] = 0;
	HRESULT hr = SafeArrayPutElement(psa, idx, &v1);

	idx[0] = 0;
	idx[1] = 1;
	hr = SafeArrayPutElement(psa, idx, &v2);

	idx[0] = 1;
	idx[1] = 0;
	hr = SafeArrayPutElement(psa, idx, &v3);

	idx[0] = 1;
	idx[1] = 1;
	hr = SafeArrayPutElement(psa, idx, &v4);

	VARIANT v;
	VariantInit(&v);

	v.parray = psa; v.vt = VT_ARRAY | VT_VARIANT;

	// now get the stuff back out
	long xLow, xHigh, yLow, yHigh, ix[2];
	xLow = xHigh = yLow = yHigh = ix[0] = ix[1] = 0;

	SafeArrayGetUBound(v.parray, 1, &xHigh);
	SafeArrayGetLBound(v.parray, 1, &xLow);

	SafeArrayGetUBound(v.parray, 2, &yHigh);
	SafeArrayGetLBound(v.parray, 2, &yLow);

	_variant_t vcell1, vcell2, vcell3, vcell4;

	ix[0] = 0;
	ix[1] = 0;
	hr = SafeArrayGetElement(v.parray, ix, &vcell1);

	ix[0] = 0;
	ix[1] = 1;
	hr = SafeArrayGetElement(v.parray, ix, &vcell2);

	ix[0] = 1;
	ix[1] = 0;
	hr = SafeArrayGetElement(v.parray, ix, &vcell3);

	ix[0] = 1;
	ix[1] = 1;
	hr = SafeArrayGetElement(v.parray, ix, &vcell4);

	cout << "this is version 1 using the COM API\n" <<
		"vcell1=" << vcell1.iVal << "\n" <<
		"vcell2=" << vcell2.iVal << "\n" <<
		"vcell3=" << vcell3.iVal << "\n" <<
		"vcell4=" << vcell4.iVal << "\n" <<

		"press enter to continue\n\n";

	cin.get();
}

//THIS IS THE MATRIX VERSION OF THE CODE ABOVE
void version2()
{
	_variant_t v1 = 1L, v2 = 2L, v3 = 3L, v4 = 4L;

	CMatrix<_variant_t> matrix;
	matrix.cell(0, 0) = v1;
	matrix.cell(0, 1) = v2;
	matrix.cell(1, 0) = v3;
	matrix.cell(1, 1) = v4;

	VARIANT vIn = matrix;

	// now get the stuff back out

	_variant_t vcell1, vcell2, vcell3, vcell4;

	CMatrix<_variant_t> m;

	m = vIn;
	vcell1 = m.cell(0, 0);
	vcell2 = m.cell(0, 1);
	vcell3 = m.cell(1, 0);
	vcell4 = m.cell(1, 1);

	cout << "this is version 2 using the Matrix\n" <<
		"vcell1=" << vcell1.iVal << "\n" <<
		"vcell2=" << vcell2.iVal << "\n" <<
		"vcell3=" << vcell3.iVal << "\n" <<
		"vcell4=" << vcell4.iVal << "\n" <<

		"press enter to continue\n\n";

	cin.get();
}

//THIS IS A LESS VERBOSE VERSION OF THE CODE ABOVE
void version3()
{
	CMatrix<_variant_t> matrix;
	matrix.cell(0, 0) = 1L;
	matrix.cell(0, 1) = 2L;
	matrix.cell(1, 0) = 3L;
	matrix.cell(1, 1) = 4L;

	VARIANT vIn = matrix;

	// now get the stuff back out
	CMatrix<_variant_t> m(vIn);
	cout << "this is version 3 (less verbose than version 2)\n" <<
		"vcell1=" << m.cell(0, 0).iVal << "\n" <<
		"vcell2=" << m.cell(0, 1).iVal << "\n" <<
		"vcell3=" << m.cell(1, 0).iVal << "\n" <<
		"vcell4=" << m.cell(1, 1).iVal << "\n" <<

		"press enter to continue\n\n";

	cin.get();
}

int main()
{
	version1();
	version2();
	version3();
}

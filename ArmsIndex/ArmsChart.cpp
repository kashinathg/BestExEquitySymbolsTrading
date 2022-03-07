#include "stdafx.h"

#ifdef RGB
#undef RGB
// windows COLORREF is backwards from what ChartDir is expecting
#endif
#define RGB(r,g,b)          ((COLORREF)(((BYTE)(b)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(r))<<16)))
#include <OUCommon/Colour.h>

#include "ArmsChart.h"

ArmsChart::ArmsChart( void )
{
}

ArmsChart::~ArmsChart(void) {
}

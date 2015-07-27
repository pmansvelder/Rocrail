/*
 Rocrail - Model Railroad Software

Copyright (c) 2002-2015 Robert Jan Versluis, Rocrail.net

 


 All rights reserved.
*/
#ifndef SVG_H_
#define SVG_H_

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "rocs/public/list.h"
#include "rocs/public/map.h"

/*!
 * Includes
 */
#include "rocs/public/node.h"

class svgPoly {
  public:
  wxPoint* poly;
  int cnt;
  char* stroke;
  char* stroke_width;
  char* fill;
  bool arc;
};

class svgCircle {
  public:
  int cx;
  int cy;
  int r;
  char* stroke;
  char* stroke_width;
  char* fill;
};

class svgSymbol {
  public:
  iOList polyList;
  iOList circleList;
  iOList polyListAlt;
  iOList circleListAlt;
  char* name;
  int cx;
  int cy;
  int width;
  int height;
};

class svgReader {
  private:
  void addPoly2List( iOList polyList, int cnt, int xpoints[], int ypoints[], const char* stroke, const char* fill, bool arc, const char* stroke_width );
  void addCircle2List( iOList circleList, int cx, int cy, int r, const char* stroke, const char* fill, const char* stroke_width );
  bool parsePoly( const char* d, int xpoints[], int ypoints[], int* cnt );
  int evalCoord( const char* d, int* x, int* y );

  public:
  svgReader();
  iOMap readSvgSymbols( const char* path, iOMap map, const char* themespath );
  svgSymbol* parseSvgSymbol( const char* svgStr );
};



#endif /*SVG_H_*/

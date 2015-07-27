/*
 Rocrail - Model Railroad Software

Copyright (c) 2002-2015 Robert Jan Versluis, Rocrail.net




 All rights reserved.
*/
#ifndef __ROCRAIL_rocview_CELLRENDERER_H
#define __ROCRAIL_rocview_CELLRENDERER_H

#include <wx/grid.h>
#include <wx/image.h>
#include <wx/bitmap.h>
#include "rocview/symbols/renderer.h"


class CellRenderer: public wxGridCellStringRenderer
{
  char* imageName;
  wxBitmap* imageBitmap;
  SymbolRenderer* m_Renderer;
  bool m_bDidResize;
  double m_Scale;
  int m_RowSize;
  char* m_Tip;

public:
  CellRenderer(const char* imageName);
  CellRenderer(const char* imageName, SymbolRenderer* l_Renderer, double scale=1.0, const char* tip=NULL);
  CellRenderer(wxBitmap* bitmap);

  ~CellRenderer();
  virtual void Draw(wxGrid& grid, wxGridCellAttr& attr, wxDC& dc, const wxRect& rect, int row, int col, bool isSelected);
  void updateImage(const wxRect& rect);
  wxSize GetBestSize(wxGrid& grid, wxGridCellAttr& attr, wxDC& dc, int row, int col);
  int GetRowSize() { return m_RowSize;}
  const char* GetTip() { return m_Tip;}
};

#endif

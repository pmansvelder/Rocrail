/*
 Rocrail - Model Railroad Software

Copyright (c) 2002-2015 Robert Jan Versluis, Rocrail.net

 


 All rights reserved.
*/
#ifndef MODPANEL_H_
#define MODPANEL_H_


#include "rocview/public/basepanel.h"
#include "rocview/dialogs/modplandlg.h"

#include "rocs/public/list.h"


class ModPanel : public BasePanel
{
  double m_Scale;
  int    m_ItemSize;
  iOList m_ModList;
  int m_mouseX;
  int m_mouseY;
  int m_X;
  int m_Y;
  wxWindow* m_Parent;
  ModPlanDlg* m_ModPlanDlg;

public:
  ModPanel(wxWindow *parent, int itemsize, double scale );
  void OnPaint(wxPaintEvent& event);
  void addModule(BasePanel* modpanel);
  void removeModule(iONode zlevel);
  void OnPopup(wxMouseEvent& event);
  void OnAddModule(wxCommandEvent& event);
  void OnProperties(wxCommandEvent& event);

  void showTooltip(bool show);
  void modelEvent( iONode node );
  void reScale( double scale );
  void removeItemFromList( iONode item );
  void init(bool modview=false);
  void clean();
  const char* getZLevelTitle();
  int getZ();
  const char* getZID(){ return NULL; }
  iONode getZLevel();
  void refresh(bool eraseBackground = true);
  bool SetBackgroundColor(int red, int green, int blue, bool savecolor);
  void OnPanelProps(wxCommandEvent& event);
  void OnBackColor(wxCommandEvent& event);
  void OnModuleProps(wxCommandEvent& event);
  bool hasZLevel( iONode zlevel );
  BasePanel* updateZLevel(iONode zlevel);
  iOList getModList() { return m_ModList; }
  void* GetItem( const char* key );

private:
  DECLARE_EVENT_TABLE()
};

enum {
  ME_AddModule = 801,
  ME_ModPlanProperties,
  ME_ModuleProperties,
  ME_ModPlanBackColor,
};


#endif /*MODPANEL_H_*/

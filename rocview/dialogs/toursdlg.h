/*
 Rocrail - Model Railroad Software

 Copyright (C) Rob Versluis <r.j.versluis@rocrail.net>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 3
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/
#ifndef __toursdlg__
#define __toursdlg__

/**
@file
Subclass of toursdlggen, which is generated by wxFormBuilder.
*/

#include "toursdlggen.h"
#include "rocs/public/node.h"

/** Implementing toursdlggen */
class ToursDlg : public toursdlggen
{
  iONode m_Props;
  bool   m_bSave;
  const char* m_StartBlock;

  void initLabels();
  void initIndex();
  void initValues();
  void initScheduleCombo();
  bool evaluate();
  bool isFirst(iONode tour);

public:
	/** Constructor */
	ToursDlg( wxWindow* parent );
  ToursDlg( wxWindow* parent, iONode tour, bool save, const char* startblock=NULL );
  iONode getProperties() { return m_Props; };

  void onAddTour( wxCommandEvent& event );
  void onModifyTour( wxCommandEvent& event );
  void onDeleteTour( wxCommandEvent& event );
  void onApply( wxCommandEvent& event );
  void onTourSelect( wxCommandEvent& event );
  void onEntryAdd( wxCommandEvent& event );
  void onEntryDelete( wxCommandEvent& event );
  void onCancel( wxCommandEvent& event );
  void onOK( wxCommandEvent& event );
  void onShowAll( wxCommandEvent& event );


};

#endif // __toursdlg__

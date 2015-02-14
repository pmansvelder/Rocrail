/*
 Rocrail - Model Railroad Software

 Copyright (C) 2002-2014 Rob Versluis, Rocrail.net

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
#include "weatherdlg.h"

#include "rocview/public/guiapp.h"
#include "rocrail/wrapper/public/Weather.h"
#include "rocrail/wrapper/public/WeatherList.h"
#include "rocrail/wrapper/public/Sunrise.h"
#include "rocrail/wrapper/public/Noon.h"
#include "rocrail/wrapper/public/Sunset.h"
#include "rocrail/wrapper/public/Night.h"
#include "rocrail/wrapper/public/WeatherColor.h"
#include "rocrail/wrapper/public/Output.h"
#include "rocrail/wrapper/public/OutputList.h"
#include "rocrail/wrapper/public/ModelCmd.h"
#include "rocrail/wrapper/public/WeatherTheme.h"
#include "rocrail/wrapper/public/ModelCmd.h"
#include "rocrail/wrapper/public/Plan.h"
#include "rocrail/wrapper/public/Item.h"
#include "rocrail/wrapper/public/SysCmd.h"

#include "rocs/public/trace.h"
#include "rocs/public/list.h"
#include "rocs/public/strtok.h"

#include "actionsctrldlg.h"

#include <wx/filedlg.h>
#include <wx/colordlg.h>


WeatherDlg::WeatherDlg( wxWindow* parent, iONode props ):WeatherDlgGen( parent )
{
  m_PropsList = props;
  m_Props = NULL;
  m_SelectedRow = -1;
  initLabels();


  m_IndexPanel->GetSizer()->Layout();
  m_DayPanel->GetSizer()->Layout();
  m_NightPanel->GetSizer()->Layout();
  m_ThemePanel->GetSizer()->Layout();
  m_ColorPanel->GetSizer()->Layout();

  m_WeatherBook->Fit();

  GetSizer()->Fit(this);
  GetSizer()->SetSizeHints(this);

  initColorGrid();

  initIndex();
  m_WeatherBook->SetSelection(0);

  if( m_IndexList->GetCount() > 0 ) {
    m_IndexList->SetSelection(0);
    m_Props = (iONode)m_IndexList->GetClientData(m_IndexList->GetSelection());
    if( m_Props != NULL ) {
      initValues();
      initThemeIndex();
      m_SelectedRow = wxGetApp().getFrame()->GetHour();
      m_RGBWPanel->setWeather(m_Props, m_SelectedRow, m_ColorWhite->IsChecked(), m_ColorBrightness->IsChecked(), m_ColorSaturation->IsChecked(), m_ColorWhite2->IsChecked());
      m_RGBWPanel->setDayTime(m_SelectedRow);
      m_ColorGrid->SelectRow(m_SelectedRow);
      m_ColorGrid->MakeCellVisible(m_SelectedRow, 0);
    }
  }


}


/* comparator for sorting by id: */
static int __sortID(obj* _a, obj* _b)
{
    iONode a = (iONode)*_a;
    iONode b = (iONode)*_b;
    const char* idA = wItem.getid( a );
    const char* idB = wItem.getid( b );
    return strcmp( idA, idB );
}


void WeatherDlg::initIndex() {
  m_IndexList->Clear();

  iONode model = wxGetApp().getModel();
  if( model != NULL ) {
    iONode weatherlist = wPlan.getweatherlist( model );
    if( weatherlist != NULL ) {
      iOList list = ListOp.inst();
      int cnt = NodeOp.getChildCnt( weatherlist );
      for( int i = 0; i < cnt; i++ ) {
        iONode weather = NodeOp.getChild( weatherlist, i );
        const char* id = wWeather.getid( weather );
        TraceOp.trc( "weatherdlg", TRCLEVEL_INFO, __LINE__, 9999, "initIndex add [%s] (%d of %d)", id, i, cnt );
        if( id != NULL ) {
          ListOp.add(list, (obj)weather);
        }
      }

      ListOp.sort(list, &__sortID);
      cnt = ListOp.size( list );
      for( int i = 0; i < cnt; i++ ) {
        iONode weather = (iONode)ListOp.get( list, i );
        const char* id = wWeather.getid( weather );
        m_IndexList->Append( wxString(id,wxConvUTF8), weather );
      }
      /* clean up the temp. list */
      ListOp.base.del(list);

      if( m_Props != NULL ) {
        m_IndexList->SetStringSelection( wxString(wWeather.getid( m_Props ),wxConvUTF8) );
        m_IndexList->SetFirstItem( wxString(wWeather.getid( m_Props ),wxConvUTF8) );
        char* title = StrOp.fmt( "%s %s", (const char*)wxGetApp().getMsg("weather").mb_str(wxConvUTF8), wWeather.getid( m_Props ) );
        SetTitle( wxString(title,wxConvUTF8) );
        StrOp.free( title );
      }
      else
        TraceOp.trc( "weatherdlg", TRCLEVEL_INFO, __LINE__, 9999, "no selection" );
    }

  }

}


void WeatherDlg::onIndexList( wxCommandEvent& event ) {
  if( m_IndexList->GetSelection() != wxNOT_FOUND ) {
    m_Props = (iONode)m_IndexList->GetClientData(m_IndexList->GetSelection());
    if( m_Props != NULL ) {
      initValues();
      initThemeIndex();
      m_RGBWPanel->setWeather(m_Props, m_SelectedRow, m_ColorWhite->IsChecked(), m_ColorBrightness->IsChecked(), m_ColorSaturation->IsChecked(), m_ColorWhite2->IsChecked() );
    }
    else
      TraceOp.trc( "weatherdlg", TRCLEVEL_INFO, __LINE__, 9999, "no selection..." );
  }
}


void WeatherDlg::onAddWeather( wxCommandEvent& event ) {
  int i = m_IndexList->FindString( _T("NEW") );
  if( i == wxNOT_FOUND ) {
    m_IndexList->Append( _T("NEW") );
    iONode model = wxGetApp().getModel();
    if( model != NULL ) {
      iONode weatherlist = wPlan.getweatherlist( model );
      if( weatherlist == NULL ) {
        weatherlist = NodeOp.inst( wWeatherList.name(), model, ELEMENT_NODE );
        NodeOp.addChild( model, weatherlist );
      }

      if( weatherlist != NULL ) {
        iONode weather = NodeOp.inst( wWeather.name(), weatherlist, ELEMENT_NODE );
        NodeOp.addChild( weatherlist, weather );
        wWeather.setid( weather, "NEW" );
        m_Props = weather;
        m_RGBWPanel->setWeather(m_Props, m_SelectedRow, m_ColorWhite->IsChecked(), m_ColorBrightness->IsChecked(), m_ColorSaturation->IsChecked(), m_ColorWhite2->IsChecked());

      }
    }
  }
  m_IndexList->SetStringSelection( _T("NEW") );
  m_IndexList->SetFirstItem( _T("NEW") );
  initValues();
  initThemeIndex();

}


void WeatherDlg::onDeleteWeather( wxCommandEvent& event ) {
  if( m_Props == NULL )
    return;

  int action = wxMessageDialog( this, wxGetApp().getMsg("removewarning"), _T("Rocrail"), wxYES_NO | wxICON_EXCLAMATION ).ShowModal();
  if( action == wxID_NO )
    return;

  wxGetApp().pushUndoItem( (iONode)NodeOp.base.clone( m_Props ) );
  /* Notify RocRail. */
  iONode cmd = NodeOp.inst( wModelCmd.name(), NULL, ELEMENT_NODE );
  wModelCmd.setcmd( cmd, wModelCmd.remove );
  NodeOp.addChild( cmd, (iONode)m_Props->base.clone( m_Props ) );
  wxGetApp().sendToRocrail( cmd );
  cmd->base.del(cmd);

  iONode model = wxGetApp().getModel();
  if( model != NULL ) {
    iONode weatherlist = wPlan.getweatherlist( model );
    if( weatherlist != NULL ) {
      NodeOp.removeChild( weatherlist, m_Props );
      m_Props = NULL;
      m_RGBWPanel->setWeather(m_Props, m_SelectedRow, m_ColorWhite->IsChecked(), m_ColorBrightness->IsChecked(), m_ColorSaturation->IsChecked(), m_ColorWhite2->IsChecked());

    }
  }
  initIndex();
}


void WeatherDlg::onApply( wxCommandEvent& event ) {
  if( m_Props == NULL )
    return;

  if( !evaluate() )
    return;
  evaluateTheme();

  if( !wxGetApp().isStayOffline() ) {
    /* Notify RocRail. */
    iONode cmd = NodeOp.inst( wModelCmd.name(), NULL, ELEMENT_NODE );
    wModelCmd.setcmd( cmd, wModelCmd.modify );
    NodeOp.addChild( cmd, (iONode)m_Props->base.clone( m_Props ) );
    wxGetApp().sendToRocrail( cmd );
    cmd->base.del(cmd);
  }
  else {
    wxGetApp().setLocalModelModified(true);
  }

  initIndex();

}


void WeatherDlg::onActivateWeather( wxCommandEvent& event ) {
  if( m_Props == NULL )
    return;

  iONode cmd = NodeOp.inst( wSysCmd.name(), NULL, ELEMENT_NODE );
  wSysCmd.setcmd( cmd, wSysCmd.weather );
  wSysCmd.setid( cmd, wWeather.getid(m_Props) );
  wxGetApp().sendToRocrail( cmd );
  cmd->base.del(cmd);
}


void WeatherDlg::onDeactivateWeather( wxCommandEvent& event ) {
  if( m_Props == NULL )
    return;

  iONode cmd = NodeOp.inst( wSysCmd.name(), NULL, ELEMENT_NODE );
  wSysCmd.setcmd( cmd, wSysCmd.weather );
  wxGetApp().sendToRocrail( cmd );
  cmd->base.del(cmd);
}


void WeatherDlg::onCancel( wxCommandEvent& event )
{
  EndModal(0);
}


void WeatherDlg::initColorGrid() {
  m_ColorGrid->AppendRows(24);
  for( int i = 0; i < 24; i++ ) {
    m_ColorGrid->SetRowLabelValue(i, wxString::Format(wxT("%02d:%02d"), i, 0) );
    for( int n = 0; n < 7; n++) {
      m_ColorGrid->SetCellAlignment(wxALIGN_CENTRE, i, n);
      m_ColorGrid->SetCellValue(i, n, wxT("0"));
    }
  }
  for( int n = 0; n < 7; n++)
    m_ColorGrid->SetColFormatNumber(n);
}


void WeatherDlg::initLabels() {
  SetTitle(wxGetApp().getMsg( "weather" ));
  m_WeatherBook->SetPageText( 0, wxGetApp().getMsg( "index" ) );
  m_WeatherBook->SetPageText( 1, wxGetApp().getMsg( "day" ) );
  m_WeatherBook->SetPageText( 2, wxGetApp().getMsg( "night" ) );
  m_WeatherBook->SetPageText( 3, wxGetApp().getMsg( "color" ) );
  m_WeatherBook->SetPageText( 4, wxGetApp().getMsg( "theme" ) );

  // Index
  m_labID->SetLabel( wxGetApp().getMsg( "id" ) );
  m_AddWeather->SetLabel( wxGetApp().getMsg( "add" ) );
  m_DeleteWeather->SetLabel( wxGetApp().getMsg( "delete" ) );
  m_ActivateWeather->SetLabel( wxGetApp().getMsg( "activate" ) );
  m_Actions->SetLabel( wxGetApp().getMsg( "actions" )+_T("...") );

  // Day
  m_labOutputs->SetLabel( wxGetApp().getMsg( "outputs" ) );
  m_labMaxBri->SetLabel( wxGetApp().getMsg( "maxbri" ) );
  m_labMinBri->SetLabel( wxGetApp().getMsg( "minbri" ) );
  m_labColorSliding->SetLabel( wxGetApp().getMsg( "colorsliding" ) );
  m_labNightSliding->SetLabel( wxGetApp().getMsg( "nightsliding" ) );
  m_SlidingDaylight->SetLabel( wxGetApp().getMsg( "slidingdaylight" ) );
  m_RelativeTime->SetLabel( wxGetApp().getMsg( "relative" ) );
  m_ColorTable->SetLabel( wxGetApp().getMsg( "colortable" ) );
  m_OffAtShutdown->SetLabel( wxGetApp().getMsg( "offatshutdown" ) );

  m_SunriseBox->GetStaticBox()->SetLabel( wxGetApp().getMsg( "sunrise" ) );
  m_NoonBox->GetStaticBox()->SetLabel( wxGetApp().getMsg( "noon" ) );
  m_SunsetBox->GetStaticBox()->SetLabel( wxGetApp().getMsg( "sunset" ) );

  m_labSunriseTime->SetLabel( wxGetApp().getMsg( "time" ) );
  m_labNoonTime->SetLabel( wxGetApp().getMsg( "time" ) );
  m_labSunsetTime->SetLabel( wxGetApp().getMsg( "time" ) );

  m_labRGBSunrise->SetLabel( wxGetApp().getMsg( "rgb" ) );
  m_labRGBNoon->SetLabel( wxGetApp().getMsg( "rgb" ) );
  m_labRGBSunset->SetLabel( wxGetApp().getMsg( "rgb" ) );

  // Night
  m_labOutputsNight->SetLabel( wxGetApp().getMsg( "outputs" ) );
  m_labBrightnessNight->SetLabel( wxGetApp().getMsg( "brightness" ) );
  m_labRGBNight->SetLabel( wxGetApp().getMsg( "rgb" ) );

  // Theme
  m_ThemeAdd->SetLabel( wxGetApp().getMsg( "add" ) );
  m_ThemeModify->SetLabel( wxGetApp().getMsg( "modify" ) );
  m_ThemeDelete->SetLabel( wxGetApp().getMsg( "delete" ) );
  m_labThemeID->SetLabel( wxGetApp().getMsg( "id" ) );
  m_labThemeOutputs->SetLabel( wxGetApp().getMsg( "outputs" ) );
  m_labThemeSound->SetLabel( wxGetApp().getMsg( "sound" ) );
  m_labThemeDim->SetLabel( wxGetApp().getMsg( "dim" ) );
  m_labThemeTime->SetLabel( wxGetApp().getMsg( "time" ) );
  m_labThemeDuration->SetLabel( wxGetApp().getMsg( "duration" ) );
  m_labThemeDurationMinutes->SetLabel( wxGetApp().getMsg( "minutes" ) );
  m_labThemeRandom->SetLabel( wxGetApp().getMsg( "random" ) );

  iONode model = wxGetApp().getModel();
  if( model != NULL ) {
    iONode colist = wPlan.getcolist( model );
    if( colist != NULL ) {
      iONode co = wOutputList.getco(colist);
      while( co != NULL ) {
        m_SoundOutput->Append(wxString(wOutput.getid(co),wxConvUTF8));
        co = wOutputList.nextco(colist, co);
      }
    }
  }

  // Color
  m_ColorGrid->SetColLabelValue(0, wxGetApp().getMsg("red") );
  m_ColorGrid->SetColLabelValue(1, wxGetApp().getMsg("green") );
  m_ColorGrid->SetColLabelValue(2, wxGetApp().getMsg("blue") );
  m_ColorGrid->SetColLabelValue(3, wxGetApp().getMsg("white") + wxT(" 1") );
  m_ColorGrid->SetColLabelValue(4, wxGetApp().getMsg("brightness") );
  m_ColorGrid->SetColLabelValue(5, wxGetApp().getMsg("saturation") );
  m_ColorGrid->SetColLabelValue(6, wxGetApp().getMsg("white") + wxT(" 2") );
  m_ColorImport->SetLabel( wxGetApp().getMsg( "import" ) + wxT("...") );
  m_ColorExport->SetLabel( wxGetApp().getMsg( "export" ) + wxT("...") );

  m_ColorWhite->SetLabel(wxGetApp().getMsg("white") + wxT(" 1"));
  m_ColorWhite->SetBackgroundColour(wxColour(255,255,255));
  m_ColorWhite2->SetLabel(wxGetApp().getMsg("white") + wxT(" 2"));
  m_ColorWhite2->SetBackgroundColour(wxColour(255,0,255));
  m_ColorBrightness->SetLabel(wxGetApp().getMsg("brightness"));
  m_ColorBrightness->SetBackgroundColour(wxColour(255,255,0));
  m_ColorSaturation->SetLabel(wxGetApp().getMsg("saturation"));
  m_ColorSaturation->SetBackgroundColour(wxColour(0,255,255));

  // Buttons
  m_StdButtonsOK->SetLabel( wxGetApp().getMsg( "ok" ) );
  m_StdButtonsApply->SetLabel( wxGetApp().getMsg( "apply" ) );
  m_StdButtonsCancel->SetLabel( wxGetApp().getMsg( "cancel" ) );
  m_StdButtonsHelp->SetLabel( wxGetApp().getMsg( "help" ) );
}

void WeatherDlg::initValues() {
  char* title = StrOp.fmt( "%s %s", (const char*)wxGetApp().getMsg("weather").mb_str(wxConvUTF8), wWeather.getid( m_Props ) );
  SetTitle( wxString(title,wxConvUTF8) );
  StrOp.free( title );

  m_ID->SetValue( wxString(wWeather.getid(m_Props),wxConvUTF8) );
  m_Outputs->SetValue( wxString(wWeather.getoutputs(m_Props),wxConvUTF8) );
  m_MaxBri->SetValue( wWeather.getmaxbri(m_Props) );
  m_MinBri->SetValue( wWeather.getminbri(m_Props) );
  m_ColorSliding->SetValue( wWeather.getcolorsliding(m_Props) );
  m_NightSliding->SetValue( wWeather.getnightsliding(m_Props) );
  m_SlidingDaylight->SetValue(wWeather.isslidingdaylight(m_Props)?true:false);
  m_RelativeTime->SetValue(wWeather.isrelativetime(m_Props)?true:false);
  m_ColorTable->SetValue(wWeather.iscolortable(m_Props)?true:false);
  m_OffAtShutdown->SetValue(wWeather.isoffatshutdown(m_Props)?true:false);

  iONode sunrise = wWeather.getsunrise(m_Props);
  if( sunrise == NULL ) {
    sunrise = NodeOp.inst(wSunrise.name(), m_Props, ELEMENT_NODE );
    NodeOp.addChild(m_Props, sunrise);
  }
  m_SunriseHour->SetValue( wSunrise.gethour( sunrise ) );
  m_SunriseMin->SetValue( wSunrise.getminute( sunrise ) );
  m_SunriseRed->SetValue( wSunrise.getred( sunrise ) );
  m_SunriseGreen->SetValue( wSunrise.getgreen( sunrise ) );
  m_SunriseBlue->SetValue( wSunrise.getblue( sunrise ) );

  iONode noon = wWeather.getnoon(m_Props);
  if( noon == NULL ) {
    noon = NodeOp.inst(wNoon.name(), m_Props, ELEMENT_NODE );
    NodeOp.addChild(m_Props, noon);
  }
  m_NoonHour->SetValue( wNoon.gethour( noon ) );
  m_NoonMin->SetValue( wNoon.getminute( noon ) );
  m_NoonRed->SetValue( wNoon.getred( noon ) );
  m_NoonGreen->SetValue( wNoon.getgreen( noon ) );
  m_NoonBlue->SetValue( wNoon.getblue( noon ) );

  iONode sunset = wWeather.getsunset(m_Props);
  if( sunset == NULL ) {
    sunset = NodeOp.inst(wSunset.name(), m_Props, ELEMENT_NODE );
    NodeOp.addChild(m_Props, sunset);
  }
  m_SunsetHour->SetValue( wSunset.gethour( sunset ) );
  m_SunsetMin->SetValue( wSunset.getminute( sunset ) );
  m_SunsetRed->SetValue( wSunset.getred( sunset ) );
  m_SunsetGreen->SetValue( wSunset.getgreen( sunset ) );
  m_SunsetBlue->SetValue( wSunset.getblue( sunset ) );

  iONode night = wWeather.getnight(m_Props);
  if( night == NULL ) {
    night = NodeOp.inst(wNight.name(), m_Props, ELEMENT_NODE );
    NodeOp.addChild(m_Props, night);
  }
  m_OutputsNight->SetValue( wxString(wNight.getoutputs(night),wxConvUTF8) );
  m_BrightnessNight->SetValue( wNight.getbri(night) );
  m_RedNight->SetValue( wNight.getred(night) );
  m_GreenNight->SetValue( wNight.getgreen(night) );
  m_BlueNight->SetValue( wNight.getblue(night) );

  iONode color = wWeather.getweathercolor(m_Props);
  while(color != NULL) {
    int hour = wWeatherColor.gethour(color);
    m_ColorGrid->SetCellValue(hour, 0, wxString::Format(wxT("%d"), wWeatherColor.getred(color)));
    m_ColorGrid->SetCellValue(hour, 1, wxString::Format(wxT("%d"), wWeatherColor.getgreen(color)));
    m_ColorGrid->SetCellValue(hour, 2, wxString::Format(wxT("%d"), wWeatherColor.getblue(color)));
    m_ColorGrid->SetCellValue(hour, 3, wxString::Format(wxT("%d"), wWeatherColor.getwhite(color)));
    m_ColorGrid->SetCellValue(hour, 4, wxString::Format(wxT("%d"), wWeatherColor.getbri(color)));
    m_ColorGrid->SetCellValue(hour, 5, wxString::Format(wxT("%d"), wWeatherColor.getsat(color)));
    m_ColorGrid->SetCellValue(hour, 6, wxString::Format(wxT("%d"), wWeatherColor.getwhite2(color)));
    color = wWeather.nextweathercolor(m_Props, color);
  }

  m_ColorWhite->SetValue( wWeather.isusewhite(m_Props)?true:false);
  m_ColorWhite2->SetValue( wWeather.isusewhite2(m_Props)?true:false);
  m_ColorBrightness->SetValue( wWeather.isusebri(m_Props)?true:false);
  m_ColorSaturation->SetValue( wWeather.isusesat(m_Props)?true:false);

  if( m_ColorWhite->IsChecked() ) ShowCol(3);
  else HideCol(3);
  if( m_ColorBrightness->IsChecked() ) ShowCol(4);
  else HideCol(4);
  if( m_ColorSaturation->IsChecked() ) ShowCol(5);
  else HideCol(5);
  if( m_ColorWhite2->IsChecked() ) ShowCol(6);
  else HideCol(6);

}

bool WeatherDlg::evaluate() {
  if( m_Props == NULL ) {
    return false;
  }

  if( m_ID->GetValue().Len() == 0 ) {
    wxMessageDialog( this, wxGetApp().getMsg("invalidid"), _T("Rocrail"), wxOK | wxICON_ERROR ).ShowModal();
    m_ID->SetValue( wxString(wItem.getid( m_Props ),wxConvUTF8) );
    return false;
  }
  wItem.setprev_id( m_Props, wItem.getid(m_Props) );

  wWeather.setid( m_Props, m_ID->GetValue().mb_str(wxConvUTF8) );
  wWeather.setoutputs( m_Props, m_Outputs->GetValue().mb_str(wxConvUTF8) );
  wWeather.setmaxbri(m_Props, m_MaxBri->GetValue() );
  wWeather.setminbri(m_Props, m_MinBri->GetValue() );
  wWeather.setcolorsliding(m_Props, m_ColorSliding->GetValue() );
  wWeather.setnightsliding(m_Props, m_NightSliding->GetValue() );
  wWeather.setslidingdaylight(m_Props, m_SlidingDaylight->IsChecked()?True:False);
  wWeather.setrelativetime(m_Props, m_RelativeTime->IsChecked()?True:False);
  wWeather.setcolortable(m_Props, m_ColorTable->IsChecked()?True:False);
  wWeather.setoffatshutdown(m_Props, m_OffAtShutdown->IsChecked()?True:False);
  iONode sunrise = wWeather.getsunrise(m_Props);
  wSunrise.sethour( sunrise, m_SunriseHour->GetValue() );
  wSunrise.setminute( sunrise, m_SunriseMin->GetValue() );
  wSunrise.setred( sunrise, m_SunriseRed->GetValue() );
  wSunrise.setgreen( sunrise, m_SunriseGreen->GetValue() );
  wSunrise.setblue( sunrise, m_SunriseBlue->GetValue() );

  iONode noon = wWeather.getnoon(m_Props);
  wNoon.sethour( noon, m_NoonHour->GetValue() );
  wNoon.setminute( noon, m_NoonMin->GetValue() );
  wNoon.setred( noon, m_NoonRed->GetValue() );
  wNoon.setgreen( noon, m_NoonGreen->GetValue() );
  wNoon.setblue( noon, m_NoonBlue->GetValue() );

  iONode sunset = wWeather.getsunset(m_Props);
  wSunset.sethour( sunset, m_SunsetHour->GetValue() );
  wSunset.setminute( sunset, m_SunsetMin->GetValue() );
  wSunset.setred( sunset, m_SunsetRed->GetValue() );
  wSunset.setgreen( sunset, m_SunsetGreen->GetValue() );
  wSunset.setblue( sunset, m_SunsetBlue->GetValue() );

  iONode night = wWeather.getnight(m_Props);
  wNight.setoutputs( night, m_OutputsNight->GetValue().mb_str(wxConvUTF8) );
  wNight.setbri(night, m_BrightnessNight->GetValue() );
  wNight.setred(night, m_RedNight->GetValue() );
  wNight.setgreen(night, m_GreenNight->GetValue() );
  wNight.setblue(night, m_BlueNight->GetValue() );

  iONode color = wWeather.getweathercolor(m_Props);
  while(color != NULL) {
    NodeOp.removeChild(m_Props, color);
    NodeOp.base.del(color);
    color = wWeather.getweathercolor(m_Props);
  }
  for( int i = 0; i < 24; i++ ) {
    color = NodeOp.inst(wWeatherColor.name(), m_Props, ELEMENT_NODE);
    NodeOp.addChild(m_Props, color);
    wWeatherColor.sethour(color, i);
    wWeatherColor.setred  (color, atoi(m_ColorGrid->GetCellValue(i, 0).mb_str(wxConvUTF8)));
    wWeatherColor.setgreen(color, atoi(m_ColorGrid->GetCellValue(i, 1).mb_str(wxConvUTF8)));
    wWeatherColor.setblue (color, atoi(m_ColorGrid->GetCellValue(i, 2).mb_str(wxConvUTF8)));
    wWeatherColor.setwhite(color, atoi(m_ColorGrid->GetCellValue(i, 3).mb_str(wxConvUTF8)));
    wWeatherColor.setbri  (color, atoi(m_ColorGrid->GetCellValue(i, 4).mb_str(wxConvUTF8)));
    wWeatherColor.setsat  (color, atoi(m_ColorGrid->GetCellValue(i, 5).mb_str(wxConvUTF8)));
    wWeatherColor.setwhite2(color, atoi(m_ColorGrid->GetCellValue(i, 6).mb_str(wxConvUTF8)));
  }

  return true;
}

void WeatherDlg::onOK( wxCommandEvent& event )
{
  onApply(event);
  EndModal( wxID_OK );
}

void WeatherDlg::onHelp( wxCommandEvent& event ) {
  switch( m_WeatherBook->GetSelection() ) {
  case 0: wxGetApp().openLink( "weather", "index" ); break;
  case 1: wxGetApp().openLink( "weather", "day" ); break;
  case 2: wxGetApp().openLink( "weather", "night" ); break;
  case 3: wxGetApp().openLink( "weather", "color" ); break;
  case 4: wxGetApp().openLink( "weather", "theme" ); break;
  default: wxGetApp().openLink( "weather" ); break;
  }

}



void WeatherDlg::initThemeValues() {
  if( m_ThemeList->GetSelection() != wxNOT_FOUND ) {
    iONode weathertheme = (iONode)m_ThemeList->GetClientData(m_ThemeList->GetSelection());
    m_ThemeID->SetValue( wxString(wWeatherTheme.getid(weathertheme),wxConvUTF8) );
    m_ThemeOutputs->SetValue( wxString(wWeatherTheme.getoutputs(weathertheme),wxConvUTF8) );
    m_ThemeSound->SetValue( wxString(wWeatherTheme.getsound(weathertheme),wxConvUTF8) );
    m_ThemeDim->SetValue( wWeatherTheme.getdim(weathertheme) );
    m_ThemeHour->SetValue( wWeatherTheme.gethour(weathertheme) );
    m_ThemeMinute->SetValue( wWeatherTheme.getminute(weathertheme) );
    m_ThemeDuration->SetValue( wWeatherTheme.getduration(weathertheme) );
    m_ThemeRandom->SetValue( wWeatherTheme.israndom(weathertheme) ? true:false);
    m_ThemeSoundRandomRate->SetValue( wWeatherTheme.getsoundrandomrate(weathertheme) );
    m_SoundOutput->SetStringSelection(wxString(wWeatherTheme.getsoundoutput(weathertheme),wxConvUTF8));
  }
  else {
    m_ThemeID->SetValue( wxString("",wxConvUTF8) );
    m_ThemeOutputs->SetValue( wxString("",wxConvUTF8) );
    m_ThemeSound->SetValue( wxString("",wxConvUTF8) );
    m_ThemeDim->SetValue( 0 );
    m_ThemeHour->SetValue( 0 );
    m_ThemeMinute->SetValue( 0 );
    m_ThemeDuration->SetValue( 0 );
    m_ThemeRandom->SetValue( false);
    m_ThemeSoundRandomRate->SetValue( 5 );
  }
}


void WeatherDlg::initThemeIndex() {
  if( m_Props == NULL ) {
    return;
  }

  int prevSel = m_ThemeList->GetSelection();
  m_ThemeList->Clear();
  iONode weathertheme = wWeather.getweathertheme(m_Props);
  while( weathertheme != NULL ) {
    m_ThemeList->Append( wxString(wWeatherTheme.getid(weathertheme),wxConvUTF8), weathertheme );
    weathertheme = wWeather.nextweathertheme(m_Props, weathertheme);
  }

  if( prevSel != wxNOT_FOUND ) {
    m_ThemeList->SetSelection( prevSel );
  }

}


bool WeatherDlg::evaluateTheme() {
  if( m_ThemeList->GetSelection() != wxNOT_FOUND ) {
    iONode weathertheme = (iONode)m_ThemeList->GetClientData(m_ThemeList->GetSelection());
    wWeatherTheme.setid( weathertheme, m_ThemeID->GetValue().mb_str(wxConvUTF8) );
    wWeatherTheme.setoutputs( weathertheme, m_ThemeOutputs->GetValue().mb_str(wxConvUTF8) );
    wWeatherTheme.setsound( weathertheme, m_ThemeSound->GetValue().mb_str(wxConvUTF8) );
    wWeatherTheme.setdim( weathertheme, m_ThemeDim->GetValue() );
    wWeatherTheme.sethour( weathertheme, m_ThemeHour->GetValue() );
    wWeatherTheme.setminute( weathertheme, m_ThemeMinute->GetValue() );
    wWeatherTheme.setduration( weathertheme, m_ThemeDuration->GetValue() );
    wWeatherTheme.setrandom( weathertheme, m_ThemeRandom->IsChecked() ? True:False);
    wWeatherTheme.setsoundrandomrate( weathertheme, m_ThemeSoundRandomRate->GetValue() );
    wWeatherTheme.setsoundoutput( weathertheme, m_SoundOutput->GetStringSelection().mb_str(wxConvUTF8) );
    initThemeIndex();
  }
  return true;
}


void WeatherDlg::onThemeSelected( wxCommandEvent& event ) {
  initThemeValues();
}


void WeatherDlg::onThemeAdd( wxCommandEvent& event ) {
  if( m_Props == NULL ) {
    return;
  }

  int i = m_ThemeList->FindString( _T("NEW") );
  if( i == wxNOT_FOUND ) {
    iONode weathertheme = NodeOp.inst( wWeatherTheme.name(), m_Props, ELEMENT_NODE );
    NodeOp.addChild( m_Props, weathertheme );
    wWeatherTheme.setid( weathertheme, "NEW" );
    initThemeValues();
    m_ThemeList->Append( _T("NEW"), weathertheme );
  }
  m_ThemeList->SetStringSelection( _T("NEW") );
  m_ThemeList->SetFirstItem( _T("NEW") );
  initThemeValues();
}


void WeatherDlg::onThemeModify( wxCommandEvent& event ) {
  if( m_Props == NULL ) {
    return;
  }

  evaluateTheme();
}


void WeatherDlg::onThemeDelete( wxCommandEvent& event ) {
  if( m_Props == NULL ) {
    return;
  }

  if( m_ThemeList->GetSelection() != wxNOT_FOUND ) {
    iONode weathertheme = (iONode)m_ThemeList->GetClientData(m_ThemeList->GetSelection());
    if( weathertheme != NULL ) {
      NodeOp.removeChild(m_Props, weathertheme);
      m_ThemeList->Clear();
      initThemeIndex();
      initThemeValues();
      // ToDo: Clean up node?
    }
  }
}

void WeatherDlg::onActions( wxCommandEvent& event ) {
  if( m_Props == NULL ) {
    return;
  }

  ActionsCtrlDlg*  dlg = new ActionsCtrlDlg(this, m_Props, "sunrise,noon,sunset" );

  if( wxID_OK == dlg->ShowModal() ) {
    // TODO: inform
  }

  dlg->Destroy();
}

void WeatherDlg::onColorCellSelect( wxGridEvent& event ) {
  m_RGBWPanel->setWeather(m_Props, event.GetRow(), m_ColorWhite->IsChecked(), m_ColorBrightness->IsChecked(), m_ColorSaturation->IsChecked(), m_ColorWhite2->IsChecked());
  event.Skip();
}
void WeatherDlg::onColorLabelClick( wxGridEvent& event ) {
  m_RGBWPanel->setWeather(m_Props, event.GetRow(), m_ColorWhite->IsChecked(), m_ColorBrightness->IsChecked(), m_ColorSaturation->IsChecked(), m_ColorWhite2->IsChecked());
  event.Skip();
}
void WeatherDlg::onColorCellChanged( wxGridEvent& event ) {
  evaluate();
  m_RGBWPanel->setWeather(m_Props, event.GetRow(), m_ColorWhite->IsChecked(), m_ColorBrightness->IsChecked(), m_ColorSaturation->IsChecked(), m_ColorWhite2->IsChecked());
  event.Skip();
}


/*
0,26,64,0
0,26,64,0
0,26,64,0
0,26,64,0
0,26,64,0
0,33,64,0
0,43,64,0
20,59,64,13
77,77,59,59
140,77,38,135
186,51,15,199
209,26,8,242
232,10,0,255
242,0,0,255
250,0,0,255
252,0,0,245
252,0,0,224
247,0,0,201
240,0,0,171
230,0,0,140
212,5,8,110
179,10,13,69
128,20,59,33
26,26,64,0
 */

void WeatherDlg::onColorImport( wxCommandEvent& event ) {
  wxFileDialog* fdlg = new wxFileDialog(this, _T("Weather color"), wxString(".",wxConvUTF8), _T(""),
      _T("CSV files (*.csv)|*.csv"), wxFD_OPEN);
  if( fdlg->ShowModal() == wxID_OK ) {
    if( FileOp.exist(fdlg->GetPath().mb_str(wxConvUTF8)) ) {
      iOFile f = FileOp.inst( fdlg->GetPath().mb_str(wxConvUTF8), OPEN_READONLY );
      char* buffer = (char*)allocMem( FileOp.size( f ) +1 );
      int hour = 0;
      while( FileOp.readStr(f, buffer) && hour < 24) {
        iOStrTok tok = StrTokOp.inst(buffer, ',');
        if( StrTokOp.hasMoreTokens(tok) )
          m_ColorGrid->SetCellValue(hour, 0, wxString( StrTokOp.nextToken(tok), wxConvUTF8 ) );
        if( StrTokOp.hasMoreTokens(tok) )
          m_ColorGrid->SetCellValue(hour, 1, wxString( StrTokOp.nextToken(tok), wxConvUTF8 ) );
        if( StrTokOp.hasMoreTokens(tok) )
          m_ColorGrid->SetCellValue(hour, 2, wxString( StrTokOp.nextToken(tok), wxConvUTF8 ) );
        if( StrTokOp.hasMoreTokens(tok) )
          m_ColorGrid->SetCellValue(hour, 3, wxString( StrTokOp.nextToken(tok), wxConvUTF8 ) );
        if( StrTokOp.hasMoreTokens(tok) )
          m_ColorGrid->SetCellValue(hour, 4, wxString( StrTokOp.nextToken(tok), wxConvUTF8 ) );
        else
          m_ColorGrid->SetCellValue(hour, 4, wxT("255") );
        if( StrTokOp.hasMoreTokens(tok) )
          m_ColorGrid->SetCellValue(hour, 5, wxString( StrTokOp.nextToken(tok), wxConvUTF8 ) );
        else
          m_ColorGrid->SetCellValue(hour, 5, wxT("250") );
        if( StrTokOp.hasMoreTokens(tok) )
          m_ColorGrid->SetCellValue(hour, 6, wxString( StrTokOp.nextToken(tok), wxConvUTF8 ) );
        else
          m_ColorGrid->SetCellValue(hour, 6, wxT("0") );
        StrTokOp.base.del(tok);
        hour++;
      }
      FileOp.base.del( f );
    }
  }
  fdlg->Destroy();
  m_ColorGrid->Refresh();
  evaluate();
  m_RGBWPanel->Refresh();
}


void WeatherDlg::onColorExport( wxCommandEvent& event ) {
  wxFileDialog* fdlg = new wxFileDialog(this, _T("Weather color"), wxString(".",wxConvUTF8), _T(""),
      _T("CSV files (*.csv)|*.csv"), wxFD_SAVE);
  if( fdlg->ShowModal() == wxID_OK ) {
    wxString path = fdlg->GetPath();

    if( !path.Contains( _T(".csv") ) )
      path.Append( _T(".csv") );

    iOFile f = FileOp.inst( path.mb_str(wxConvUTF8), OPEN_WRITE );
    if( f != NULL ) {
      for( int hour = 0; hour < 24; hour++) {
        FileOp.fmt( f, "%d,%d,%d,%d,%d,%d\n",
            atoi(m_ColorGrid->GetCellValue(hour, 0).mb_str(wxConvUTF8)),
            atoi(m_ColorGrid->GetCellValue(hour, 1).mb_str(wxConvUTF8)),
            atoi(m_ColorGrid->GetCellValue(hour, 2).mb_str(wxConvUTF8)),
            atoi(m_ColorGrid->GetCellValue(hour, 3).mb_str(wxConvUTF8)),
            atoi(m_ColorGrid->GetCellValue(hour, 4).mb_str(wxConvUTF8)),
            atoi(m_ColorGrid->GetCellValue(hour, 5).mb_str(wxConvUTF8)),
            atoi(m_ColorGrid->GetCellValue(hour, 6).mb_str(wxConvUTF8))
            );
      }
      FileOp.base.del( f );
    }
  }
  fdlg->Destroy();
}


void WeatherDlg::onColorCellLeftDClick( wxGridEvent& event ) {
  int col = event.GetCol();
  int row = event.GetRow();
  int val = atoi(m_ColorGrid->GetCellValue(row, col).mb_str(wxConvUTF8));

  if( event.AltDown())
    val--;
  else
    val++;

  if( val > 255 )
    val = 255;
  if( val < 0 )
    val = 0;
  m_ColorGrid->SetCellValue(row, col, wxString::Format(wxT("%d"), val));

  evaluate();
  m_RGBWPanel->setWeather(m_Props, event.GetRow(), m_ColorWhite->IsChecked(), m_ColorBrightness->IsChecked(), m_ColorSaturation->IsChecked(), m_ColorWhite2->IsChecked());
}


/*
 * https://forums.wxwidgets.org/viewtopic.php?f=1&t=27501&p=117842&hilit=saturation#p117842
//////////////////////////////////////////////////
// Colour methods to convert HSL <-> RGB
//////////////////////////////////////////////////
float __min(float x, float y, float z)
{
   float m = x < y ? x : y;
   m = m < z ? m : z;
   return m;
}

float __max(float x, float y, float z)
{
   float m = x > y ? x : y;
   m = m > z ? m : z;
   return m;
}

void RGB_2_HSL(float r, float g, float b, float *h, float *s, float *l)
{
   float var_R = ( r / 255.0 );                     //RGB from 0 to 255
   float var_G = ( g / 255.0 );
   float var_B = ( b / 255.0 );

   float var_Min = __min( var_R, var_G, var_B );    //Min. value of RGB
   float var_Max = __max( var_R, var_G, var_B );    //Max. value of RGB
   float del_Max = var_Max - var_Min;             //Delta RGB value

   *l = ( var_Max + var_Min ) / 2.0;

   if ( del_Max == 0 ) {                   //This is a gray, no chroma...
      *h = 0;                                //HSL results from 0 to 1
      *s = 0;
   } else {                                 //Chromatic data...
      if ( *l < 0.5 ) *s = del_Max / ( var_Max + var_Min );
      else *s = del_Max / ( 2.0 - var_Max - var_Min );

      float del_R = ( ( ( var_Max - var_R ) / 6.0 ) + ( del_Max / 2.0 ) ) / del_Max;
      float del_G = ( ( ( var_Max - var_G ) / 6.0 ) + ( del_Max / 2.0 ) ) / del_Max;
      float del_B = ( ( ( var_Max - var_B ) / 6.0 ) + ( del_Max / 2.0 ) ) / del_Max;

      if      ( var_R == var_Max ) *h = del_B - del_G;
      else if ( var_G == var_Max ) *h = ( 1.0 / 3.0 ) + del_R - del_B;
      else if ( var_B == var_Max ) *h = ( 2.0 / 3.0 ) + del_G - del_R;

      if ( *h < 0 ) *h += 1;
      if ( *h > 1 ) *h -= 1;
   }
}

float Hue_2_RGB( float v1, float v2, float vH )             //Function Hue_2_RGB
{
   if ( vH < 0 ) vH += 1;
   if ( vH > 1 ) vH -= 1;
   if ( ( 6.0 * vH ) < 1 ) return ( v1 + ( v2 - v1 ) * 6.0 * vH );
   if ( ( 2.0 * vH ) < 1 ) return ( v2 );
   if ( ( 3.0 * vH ) < 2 ) return ( v1 + ( v2 - v1 ) * ( ( 2.0 / 3.0 ) - vH ) * 6.0 );
   return ( v1 );
}

void HSL_2_RGB(float h, float s, float l, float *r, float *g, float *b)
{
   if ( s == 0 ) {                     //HSL from 0 to 1
      *r = l * 255.0;                      //RGB results from 0 to 255
      *g = l * 255.0;
      *b = l * 255.0;
   } else {
      float var_2;
      if ( l < 0.5 ) var_2 = l * ( 1.0 + s );
      else           var_2 = ( l + s ) - ( s * l );

      float var_1 = 2.0 * l - var_2;

      *r = 255.0 * Hue_2_RGB( var_1, var_2, h + ( 1.0 / 3.0 ) );
      *g = 255.0 * Hue_2_RGB( var_1, var_2, h );
      *b = 255.0 * Hue_2_RGB( var_1, var_2, h - ( 1.0 / 3.0 ) );
   }
}
*/

void WeatherDlg::onColorLabelDClick( wxGridEvent& event ) {
  if( m_Props == NULL )
    return;
  int row = event.GetRow();
  int r = atoi(m_ColorGrid->GetCellValue(row, 0).mb_str(wxConvUTF8));
  int g = atoi(m_ColorGrid->GetCellValue(row, 1).mb_str(wxConvUTF8));
  int b = atoi(m_ColorGrid->GetCellValue(row, 2).mb_str(wxConvUTF8));

  wxColourData ColourData;
  ColourData.SetColour(wxColour(r,g,b));

  wxColourDialog* dlg = new wxColourDialog(this, &ColourData);
  if( wxID_OK == dlg->ShowModal() ) {
    wxColour &colour = dlg->GetColourData().GetColour();

    m_ColorGrid->SetCellValue(row, 0, wxString::Format(wxT("%d"), (int)colour.Red()) );
    m_ColorGrid->SetCellValue(row, 1, wxString::Format(wxT("%d"), (int)colour.Green()) );
    m_ColorGrid->SetCellValue(row, 2, wxString::Format(wxT("%d"), (int)colour.Blue()) );
    evaluate();
    m_RGBWPanel->setWeather(m_Props, event.GetRow(), m_ColorWhite->IsChecked(), m_ColorBrightness->IsChecked(), m_ColorSaturation->IsChecked(), m_ColorWhite2->IsChecked());
  }
  dlg->Destroy();
}

void WeatherDlg::onColorWhite( wxCommandEvent& event ) {
  if( m_Props == NULL )
    return;

  wWeather.setusewhite( m_Props, m_ColorWhite->IsChecked()?True:False);
  if( m_ColorWhite->IsChecked() )
    ShowCol(3);
  else
    HideCol(3);
  m_RGBWPanel->setWeather(m_Props, m_SelectedRow, m_ColorWhite->IsChecked(), m_ColorBrightness->IsChecked(), m_ColorSaturation->IsChecked(), m_ColorWhite2->IsChecked());
}

void WeatherDlg::onColorWhite2( wxCommandEvent& event ) {
  if( m_Props == NULL )
    return;

  wWeather.setusewhite2( m_Props, m_ColorWhite2->IsChecked()?True:False);
  if( m_ColorWhite2->IsChecked() )
    ShowCol(6);
  else
    HideCol(6);
  m_RGBWPanel->setWeather(m_Props, m_SelectedRow, m_ColorWhite->IsChecked(), m_ColorBrightness->IsChecked(), m_ColorSaturation->IsChecked(), m_ColorWhite2->IsChecked());
}

void WeatherDlg::onColorBrightness( wxCommandEvent& event ) {
  if( m_Props == NULL )
    return;

  wWeather.setusebri( m_Props, m_ColorBrightness->IsChecked()?True:False);
  if( m_ColorBrightness->IsChecked() )
    ShowCol(4);
  else
    HideCol(4);
  m_RGBWPanel->setWeather(m_Props, m_SelectedRow, m_ColorWhite->IsChecked(), m_ColorBrightness->IsChecked(), m_ColorSaturation->IsChecked(), m_ColorWhite2->IsChecked());
}

void WeatherDlg::onColorSaturation( wxCommandEvent& event ) {
  if( m_Props == NULL )
    return;

  wWeather.setusesat( m_Props, m_ColorSaturation->IsChecked()?True:False);
  if( m_ColorSaturation->IsChecked() )
    ShowCol(5);
  else
    HideCol(5);
  m_RGBWPanel->setWeather(m_Props, m_SelectedRow, m_ColorWhite->IsChecked(), m_ColorBrightness->IsChecked(), m_ColorSaturation->IsChecked(), m_ColorWhite2->IsChecked());
}

void WeatherDlg::ShowCol(int col) {
  //m_ColorGrid->ShowCol(col); // wx 3.0
  m_ColorGrid->SetColSize(col, 50);
  m_ColorGrid->AutoSizeColumn(col);
  m_ColorGrid->ForceRefresh();
}


void WeatherDlg::HideCol(int col) {
  //m_ColorGrid->HideCol(col); // wx 3.0
  m_ColorGrid->SetColSize(col, 0);
  m_ColorGrid->ForceRefresh();
}


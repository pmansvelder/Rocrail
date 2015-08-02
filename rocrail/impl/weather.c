/*
 Rocrail - Model Railroad Software

Copyright (c) 2002-2015 Robert Jan Versluis, Rocrail.net

 All rights reserved.
*/


#include "rocrail/impl/weather_impl.h"

#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#include "rocs/public/mem.h"
#include "rocs/public/trace.h"
#include "rocs/public/node.h"
#include "rocs/public/thread.h"
#include "rocs/public/strtok.h"
#include "rocs/public/list.h"
#include "rocs/public/system.h"

#include "rocrail/public/app.h"
#include "rocrail/public/control.h"
#include "rocrail/public/output.h"
#include "rocrail/public/model.h"

#include "rocrail/wrapper/public/RocRail.h"
#include "rocrail/wrapper/public/Weather.h"
#include "rocrail/wrapper/public/Sunrise.h"
#include "rocrail/wrapper/public/Noon.h"
#include "rocrail/wrapper/public/Sunset.h"
#include "rocrail/wrapper/public/Night.h"
#include "rocrail/wrapper/public/Output.h"
#include "rocrail/wrapper/public/Color.h"
#include "rocrail/wrapper/public/WeatherTheme.h"
#include "rocrail/wrapper/public/Ctrl.h"
#include "rocrail/wrapper/public/ActionCtrl.h"
#include "rocrail/wrapper/public/WeatherColor.h"
#include "rocrail/wrapper/public/SysCmd.h"


static int instCnt = 0;

static void __doDaylight(iOWeather weather, int hour, int min, Boolean shutdown, Boolean clearTheme );

/** ----- OBase ----- */
static void __del( void* inst ) {
  if( inst != NULL ) {
    iOWeatherData data = Data(inst);
    /* Cleanup data->xxx members...*/
    
    freeMem( data );
    freeMem( inst );
    instCnt--;
  }
  return;
}

static const char* __name( void ) {
  return name;
}

static unsigned char* __serialize( void* inst, long* size ) {
  return NULL;
}

static void __deserialize( void* inst,unsigned char* bytestream ) {
  return;
}

static char* __toString( void* inst ) {
  return NULL;
}

static int __count( void ) {
  return instCnt;
}

static struct OBase* __clone( void* inst ) {
  return NULL;
}

static Boolean __equals( void* inst1, void* inst2 ) {
  return False;
}

static void* __properties( void* inst ) {
  return NULL;
}

static const char* __id( void* inst ) {
  return NULL;
}

static void* __event( void* inst, const void* evt ) {
  iOWeatherData data = Data(inst);
  iONode     evtNode = (iONode)evt;
  if( evtNode == NULL )
    return NULL;

  TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "event for weather %s: %s %s",
      wWeather.getid(data->props), NodeOp.getName(evtNode), wSysCmd.getcmd(evtNode) );

  if( StrOp.equals( wSysCmd.name(), NodeOp.getName(evtNode) ) ) {
    const char* cmd = wSysCmd.getcmd(evtNode);
    iOControl control = AppOp.getControl();

    if( StrOp.equals( wSysCmd.ebreak, cmd ) ) {
      data->ebreak = True;
      long t = ControlOp.getTime(control);
      struct tm* ltm = localtime( &t );
      int hour = ltm->tm_hour;
      int min  = ltm->tm_min;
      __doDaylight(inst, hour, min, False, False );
    }
    else if( StrOp.equals( wSysCmd.go, cmd ) || StrOp.equals( wSysCmd.stop, cmd ) ) {
      data->ebreak = False;
      long t = ControlOp.getTime(control);
      struct tm* ltm = localtime( &t );
      int hour = ltm->tm_hour;
      int min  = ltm->tm_min;
      __doDaylight(inst, hour, min, False, False );
    }
  }

  return NULL;
}

/** ----- OWeather ----- */

static void __checkAction( iOWeather inst, const char* state ) {
  iOWeatherData data   = Data(inst);
  iOModel       model  = AppOp.getModel();
  iONode        action = wWeather.getactionctrl( data->props );

  /* loop over all actions */
  while( action != NULL ) {
    /* Set the callerID. */
    wActionCtrl.setcallerid(action, wWeather.getid(data->props) );

    if( StrOp.len(wActionCtrl.getstate( action )) == 0 || StrOp.equals(state, wActionCtrl.getstate( action )) )
    {
      iOAction Action = ModelOp.getAction(model, wActionCtrl.getid( action ));
      if( Action != NULL ) {
        wActionCtrl.setparam(action, state);
        ActionOp.exec(Action, action);
      }
    }
    else {
      TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "%s action state does not match: [%s-%s]",
          wWeather.getid(data->props), wActionCtrl.getstate( action ), state );
    }
    action = wWeather.nextactionctrl( data->props, action );
  }
}


static void __doInitialize(iOWeather weather, Boolean day, Boolean night) {
  iOWeatherData data = Data(weather);
  iOModel model = AppOp.getModel();

  if( data->props != NULL ) {
    iONode nightProps = wWeather.getnight(data->props);


    if( day ) {
      TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "initialize all day lamps" );
      iOStrTok tok = StrTokOp.inst( wWeather.getoutputs(data->props), ',' );
      while( StrTokOp.hasMoreTokens(tok) ) {
        const char* id = StrTokOp.nextToken(tok);
        iOOutput output = ModelOp.getOutput(model, id);
        if( output != NULL ) {
          iONode cmd = NodeOp.inst( wOutput.name(), NULL, ELEMENT_NODE);
          wOutput.setaddr(cmd, wOutput.getaddr(OutputOp.base.properties(output)));
          wOutput.setvalue(cmd, 0);
          wOutput.setcmd(cmd, wOutput.off);
          OutputOp.cmd(output, cmd, True);
        }
      };
      StrTokOp.base.del(tok);
    }

    if( night && nightProps != NULL ) {
      TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "initialize all night lamps" );
      iOStrTok tok = StrTokOp.inst( wNight.getoutputs(nightProps), ',' );
      while( StrTokOp.hasMoreTokens(tok) ) {
        const char* id = StrTokOp.nextToken(tok);
        iOOutput output = ModelOp.getOutput(model, id);
        if( output != NULL ) {
          iONode cmd = NodeOp.inst( wOutput.name(), NULL, ELEMENT_NODE);
          wOutput.setaddr(cmd, wOutput.getaddr(OutputOp.base.properties(output)));
          wOutput.setvalue(cmd, 0);
          wOutput.setcmd(cmd, wOutput.off);
          OutputOp.cmd(output, cmd, True);
        }
      };
      StrTokOp.base.del(tok);
    }
  }
}


static Boolean __getColor4Time(iONode color[], int hour, int min, float* r, float* g, float* b, float* w, float* bri, float* sat, float* w2) {
  int fromHour = hour * 2 + min / 30;
  int toHour   = fromHour + 1;

  if( toHour > 47 ) {
    toHour -= 48;
  }

  if( color[fromHour] != NULL && color[toHour] != NULL ) {
    if( wWeatherColor.getred(color[fromHour]) <= wWeatherColor.getred(color[toHour]) ) {
      /* increase */
      float dif = wWeatherColor.getred(color[toHour]) - wWeatherColor.getred(color[fromHour]);
      *r = wWeatherColor.getred(color[fromHour]) + (dif * (min%30)) / 30;
    }
    else {
      /* decrease */
      float dif = wWeatherColor.getred(color[fromHour]) - wWeatherColor.getred(color[toHour]);
      *r = wWeatherColor.getred(color[fromHour]) - (dif * (min%30)) / 30;
    }

    if( wWeatherColor.getgreen(color[fromHour]) <= wWeatherColor.getgreen(color[toHour]) ) {
      /* increase */
      float dif = wWeatherColor.getgreen(color[toHour]) - wWeatherColor.getgreen(color[fromHour]);
      *g = wWeatherColor.getgreen(color[fromHour]) + (dif * (min%30)) / 30;
    }
    else {
      /* decrease */
      float dif = wWeatherColor.getgreen(color[fromHour]) - wWeatherColor.getgreen(color[toHour]);
      *g = wWeatherColor.getgreen(color[fromHour]) - (dif * (min%30)) / 30;
    }

    if( wWeatherColor.getblue(color[fromHour]) <= wWeatherColor.getblue(color[toHour]) ) {
      /* increase */
      float dif = wWeatherColor.getblue(color[toHour]) - wWeatherColor.getblue(color[fromHour]);
      *b = wWeatherColor.getblue(color[fromHour]) + (dif * (min%30)) / 30;
    }
    else {
      /* decrease */
      float dif = wWeatherColor.getblue(color[fromHour]) - wWeatherColor.getblue(color[toHour]);
      *b = wWeatherColor.getblue(color[fromHour]) - (dif * (min%30)) / 30;
    }

    if( wWeatherColor.getwhite(color[fromHour]) <= wWeatherColor.getwhite(color[toHour]) ) {
      /* increase */
      float dif = wWeatherColor.getwhite(color[toHour]) - wWeatherColor.getwhite(color[fromHour]);
      *w = wWeatherColor.getwhite(color[fromHour]) + (dif * (min%30)) / 30;
    }
    else {
      /* decrease */
      float dif = wWeatherColor.getwhite(color[fromHour]) - wWeatherColor.getwhite(color[toHour]);
      *w = wWeatherColor.getwhite(color[fromHour]) - (dif * (min%30)) / 30;
    }

    if( wWeatherColor.getwhite2(color[fromHour]) <= wWeatherColor.getwhite2(color[toHour]) ) {
      /* increase */
      float dif = wWeatherColor.getwhite2(color[toHour]) - wWeatherColor.getwhite2(color[fromHour]);
      *w2 = wWeatherColor.getwhite2(color[fromHour]) + (dif * (min%30)) / 30;
    }
    else {
      /* decrease */
      float dif = wWeatherColor.getwhite2(color[fromHour]) - wWeatherColor.getwhite2(color[toHour]);
      *w2 = wWeatherColor.getwhite2(color[fromHour]) - (dif * (min%30)) / 30;
    }

    if( wWeatherColor.getbri(color[fromHour]) <= wWeatherColor.getbri(color[toHour]) ) {
      /* increase */
      float dif = wWeatherColor.getbri(color[toHour]) - wWeatherColor.getbri(color[fromHour]);
      *bri = wWeatherColor.getbri(color[fromHour]) + (dif * (min%30)) / 30;
    }
    else {
      /* decrease */
      float dif = wWeatherColor.getbri(color[fromHour]) - wWeatherColor.getbri(color[toHour]);
      *bri = wWeatherColor.getbri(color[fromHour]) - (dif * (min%30)) / 30;
    }

    if( wWeatherColor.getsat(color[fromHour]) <= wWeatherColor.getsat(color[toHour]) ) {
      /* increase */
      float dif = wWeatherColor.getsat(color[toHour]) - wWeatherColor.getsat(color[fromHour]);
      *sat = wWeatherColor.getsat(color[fromHour]) + (dif * (min%30)) / 30;
    }
    else {
      /* decrease */
      float dif = wWeatherColor.getsat(color[fromHour]) - wWeatherColor.getsat(color[toHour]);
      *sat = wWeatherColor.getsat(color[fromHour]) - (dif * (min%30)) / 30;
    }


    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "color4Time fromIdx=%d toIdx=%d red=%d green=%d blue=%d white=%d bri=%d sat=%d",
        fromHour, toHour, (int)*r, (int)*g, (int)*b, (int)*w, (int)*bri, (int)*sat);
    return True;
  }

  TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "color4Time fromIdx=%d(%s) toIdx=%d(%s)",
      fromHour, color[fromHour]==NULL?"null":"ok", toHour, color[toHour]==NULL?"null":"ok");


  return False;
}


static void __doDaylight(iOWeather weather, int hour, int min, Boolean shutdown, Boolean clearTheme ) {
  iOWeatherData data = Data(weather);
  iOModel model     = AppOp.getModel();
  iOList  list      = NULL;
  iOList  nightList = NULL;
  int     minutes   = hour * 60 + min;

  if( data->props == NULL ) {
    return;
  }

  list      = ListOp.inst();
  nightList = ListOp.inst();

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "do daylight at %02d:%02d (%d) on %s (shutdown=%s)",
      hour, min, minutes, wWeather.getoutputs(data->props), shutdown?"true":"false" );

  iOStrTok tok = StrTokOp.inst( wWeather.getoutputs(data->props), ',' );
  while( StrTokOp.hasMoreTokens(tok) ) {
    const char* id = StrTokOp.nextToken(tok);
    iOOutput output = ModelOp.getOutput(model, id);
    if( output != NULL ) {
      ListOp.add(list, (obj)output);
    }
  };
  StrTokOp.base.del(tok);

  tok = StrTokOp.inst( wWeather.getdeactivate(data->props), ',' );
  while( StrTokOp.hasMoreTokens(tok) ) {
    const char* id = StrTokOp.nextToken(tok);
    iOOutput output = ModelOp.getOutput(model, id);
    if( output != NULL ) {
      iONode cmd   = NodeOp.inst( wOutput.name(), NULL, ELEMENT_NODE);
      wOutput.setaddr(cmd, wOutput.getaddr(OutputOp.base.properties(output)));
      wOutput.setcmd(cmd, wOutput.off);
      OutputOp.cmd(output, cmd, True);
    }
  };
  StrTokOp.base.del(tok);

  if( ListOp.size(list) > 0 ) {
    iONode sunriseProps = wWeather.getsunrise(data->props);
    iONode noonProps    = wWeather.getnoon(data->props);
    iONode sunsetProps  = wWeather.getsunset(data->props);
    iONode nightProps   = wWeather.getnight(data->props);
    iONode colorProps[48] = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
                             NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
                             NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
                             NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};

    iONode color = wWeather.getweathercolor(data->props);
    while(color != NULL) {
      int hour = wWeatherColor.gethour(color);
      int min  = wWeatherColor.getminute(color);
      if( hour < 24 && hour >= 0 && min < 60 && min >= 0) {
        colorProps[(hour*2)+(min/30)] = color;
      }
      color = wWeather.nextweathercolor(data->props, color);
    }


    if( sunriseProps == NULL ) {
      sunriseProps = NodeOp.inst(wSunrise.name(), data->props, ELEMENT_NODE );
      NodeOp.addChild(data->props, sunriseProps);
      wSunrise.sethour(sunriseProps, 6) ;
    }
    if( noonProps == NULL ) {
      noonProps = NodeOp.inst(wNoon.name(), data->props, ELEMENT_NODE );
      NodeOp.addChild(data->props, noonProps);
      wNoon.sethour(sunsetProps, 12) ;
    }
    if( sunsetProps == NULL ) {
      sunsetProps = NodeOp.inst(wSunset.name(), data->props, ELEMENT_NODE );
      NodeOp.addChild(data->props, sunsetProps);
      wSunset.sethour(sunsetProps, 18) ;
    }
    if( nightProps == NULL ) {
      nightProps = NodeOp.inst(wNight.name(), data->props, ELEMENT_NODE );
      NodeOp.addChild(data->props, nightProps);
    }

    iOStrTok tok = StrTokOp.inst( wNight.getoutputs(nightProps), ',' );
    while( StrTokOp.hasMoreTokens(tok) ) {
      const char* id = StrTokOp.nextToken(tok);
      iOOutput output = ModelOp.getOutput(model, id);
      if( output != NULL ) {
        ListOp.add(nightList, (obj)output);
      }
    };
    StrTokOp.base.del(tok);



    int sunrise = (wSunrise.gethour(sunriseProps) + data->starthour) * 60 + wSunrise.getminute(sunriseProps) + data->startminute;
    int noon    = (wNoon.gethour(noonProps) + data->starthour) * 60 + wNoon.getminute(noonProps) + data->startminute;
    int sunset  = (wSunset.gethour(sunsetProps) + data->starthour) * 60 + wSunset.getminute(sunsetProps) + data->startminute;

    int sunriseRed   = wSunrise.getred(sunriseProps);
    int sunriseGreen = wSunrise.getgreen(sunriseProps);
    int sunriseBlue  = wSunrise.getblue(sunriseProps);

    int sunsetRed   = wSunset.getred(sunsetProps);
    int sunsetGreen = wSunset.getgreen(sunsetProps);
    int sunsetBlue  = wSunset.getblue(sunsetProps);

    float maxbri       = wWeather.getmaxbri(data->props);
    int   minbri       = wWeather.getminbri(data->props);
    float percent      = 0.0;
    float brightness   = 0.0;
    float saturation   = 250.0;
    float red          = wNoon.getred(noonProps);
    float green        = wNoon.getgreen(noonProps);
    float blue         = wNoon.getblue(noonProps);
    float white        = wNoon.getwhite(noonProps);
    float white2       = 0.0;
    float noonRed      = wNoon.getred(noonProps);
    float noonGreen    = wNoon.getgreen(noonProps);
    float noonBlue     = wNoon.getblue(noonProps);
    float colorsliding = wWeather.getcolorsliding(data->props);

    int daylight  = sunset - sunrise;

    Boolean adjustBri = (data->themedim > 0 || clearTheme) ? True:False;

    Boolean color4time = False;

    if( wWeather.iscolortable(data->props) ) {
      float bri = 0.0;
      float sat = 0.0;
      color4time = __getColor4Time(colorProps, hour, min, &red, &green, &blue, &white, &bri, &sat, &white2);
      if(color4time) {
        brightness = bri;
        saturation = sat;
        adjustBri = True;
      }
    }


    if( minutes == sunrise )
      __checkAction(weather, "sunrise");
    if( minutes == noon )
      __checkAction(weather, "noon");
    if( minutes == sunset )
      __checkAction(weather, "sunset");



    /* AM */
    if( !color4time && minutes <= noon && minutes >= sunrise) {
      float range = noon - sunrise;
      percent = (100.0 / range) * (float)(minutes - sunrise);
      float l_brightness = (percent * maxbri) / 100.0;
      brightness = l_brightness;
      adjustBri = True;

      if( !color4time && minutes <= sunrise + colorsliding ) {
        float pMinutes = minutes - sunrise;
        float redDif = (noonRed - sunriseRed) / colorsliding;
        red = sunriseRed + redDif * pMinutes;
        float greenDif = (noonGreen - sunriseGreen) / colorsliding;
        green = sunriseGreen + greenDif * pMinutes;
        float blueDif = (noonBlue - sunriseBlue) / colorsliding;
        blue = sunriseBlue + blueDif * pMinutes;
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "pMinutes=%d sunrise=%d minutes=%d redDif=%d greenDif=%d blueDif=%d",
            (int)pMinutes, sunrise, minutes, (int)redDif, (int)greenDif, (int)blueDif);
      }
    }

    /* PM */
    if( !color4time && minutes > noon && minutes <= sunset) {
      float range = sunset - noon;
      percent = 100.0 - ((100.0 / range) * (float)(minutes - noon));
      float l_brightness = (percent * maxbri) / 100.0;
      brightness = l_brightness;
      adjustBri = True;

      if( !color4time && minutes >= sunset - colorsliding ) {
        float pMinutes = sunset - minutes;
        float redDif = (noonRed - sunsetRed) / colorsliding;
        red = sunsetRed + redDif * pMinutes;
        float greenDif = (noonGreen - sunsetGreen) / colorsliding;
        green = sunsetGreen + greenDif * pMinutes;
        float blueDif = (noonBlue - sunsetBlue) / colorsliding;
        blue = sunsetBlue + blueDif * pMinutes;
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "pMinutes=%d sunset=%d minutes=%d redDif=%d greenDif=%d blueDif=%d",
            (int)pMinutes, sunset, minutes, (int)redDif, (int)greenDif, (int)blueDif);
      }
    }

    if( data->themedim > 0 ) {
      float l_brightness = brightness;
      float l_percentDim = data->themedim;
      float l_briPercent = l_brightness / 100.0;
      brightness = l_brightness - (l_briPercent * l_percentDim);
    }

    if( data->ebreak ) {
      brightness = 255;
      red = 255;
      green = 255;
      blue = 255;
      white = 255;
      white2 = 255;
    }

    if(adjustBri || shutdown) {
      int LAMPS = ListOp.size(list);
      float segment = 180.0 / (float)(LAMPS-1);
      int n = 0;
      float angle = (180.0 / (float)(sunset - sunrise)) * (minutes-sunrise);
      for( n = 0; n < LAMPS; n++) {
        iOOutput output = (iOOutput)ListOp.get(list, n);
        int lampBri = 0;

        float lampangle = segment * n;
        if( lampangle + segment > angle && lampangle < angle + segment ) {
          float lampPercent = 0.0;

          if( angle < lampangle )
            lampPercent = 100.0 - ((100.0 / segment) * (lampangle - angle));
          else
            lampPercent = 100.0 - ((100.0 / segment) * (angle - lampangle));

          lampBri = (lampPercent * brightness) / 100.0;
        }
        else {
          lampBri = 0;
        }

        if( !wWeather.isslidingdaylight(data->props) || LAMPS < 2 || data->ebreak ) {
          lampBri = brightness;
          if( lampBri < minbri )
            lampBri = minbri;
        }

        iONode cmd = NodeOp.inst( wOutput.name(), NULL, ELEMENT_NODE);
        iONode color = NodeOp.inst( wColor.name(), cmd, ELEMENT_NODE);
        NodeOp.addChild( cmd, color );
        wColor.setred(color, red);
        wColor.setgreen(color, green);
        wColor.setblue(color, blue);
        wColor.setwhite(color, white);
        wColor.setwhite2(color, white2);
        wColor.setsaturation(color, saturation);
        wOutput.setaddr(cmd, wOutput.getaddr(OutputOp.base.properties(output)));
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
            "lamp %s brightness=%d(of %.2f), lampAngle=%.2f sunAngle=%.2f dayminutes=%d sunrise=%d sunset=%d",
            wOutput.getid(OutputOp.base.properties(output)), lampBri, brightness, lampangle, angle, minutes, sunrise, sunset );
        wOutput.setvalue(cmd, lampBri);
        wOutput.setcmd(cmd, shutdown?wOutput.off:wOutput.value);
        OutputOp.cmd(output, cmd, True);

        ThreadOp.sleep(10);
      }
      adjustBri = False;
    }
    else {
      __doInitialize(weather, True, False );
    }

    float nightsliding = wWeather.getnightsliding(data->props);
    if( minutes+nightsliding > sunset || minutes-nightsliding < sunrise || shutdown ) {
      /* Night. */
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "do nightlight at %02d:%02d (%d) on %s", hour, min, minutes, wNight.getoutputs(nightProps) );
      if( ListOp.size(nightList) > 0 ) {
        int LAMPS = ListOp.size(nightList);
        int bri = wNight.getbri(nightProps);
        int n = 0;

        if( minutes+nightsliding > sunset ) {
          if( sunset - minutes > 0 ) {
            float l_bri = bri;
            l_bri = l_bri - ((l_bri / nightsliding) * (sunset - minutes));
            bri = (int)l_bri;
          }
        }

        if( minutes-nightsliding < sunrise ) {
          if( minutes - sunrise > 0 ) {
            float l_bri = bri;
            l_bri = l_bri - ((l_bri / nightsliding) * (minutes - sunrise));
            bri = (int)l_bri;
          }
        }

        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "night brightness=%d of %d", bri, wNight.getbri(nightProps));

        for( n = 0; n < LAMPS; n++) {
          iOOutput output = (iOOutput)ListOp.get(nightList, n);
          iONode cmd   = NodeOp.inst( wOutput.name(), NULL, ELEMENT_NODE);
          iONode color = NodeOp.inst( wColor.name(), cmd, ELEMENT_NODE);
          NodeOp.addChild( cmd, color );
          wOutput.setaddr(cmd, wOutput.getaddr(OutputOp.base.properties(output)));
          wOutput.setvalue(cmd, bri);
          wColor.setred(color, wNight.getred(nightProps));
          wColor.setgreen(color, wNight.getgreen(nightProps));
          wColor.setblue(color, wNight.getblue(nightProps));
          wColor.setwhite(color, wNight.getwhite(nightProps));
          wOutput.setcmd(cmd, shutdown?wOutput.off:wOutput.value);
          OutputOp.cmd(output, cmd, True);
        }
      }
    }
    else {
      __doInitialize(weather, False, True );
    }

  }
  ListOp.base.del(list);
  ListOp.base.del(nightList);

}


static void __checkWeatherThemes(iOWeather weather, int hour, int min ) {
  iOWeatherData data = Data(weather);
  iOModel model = AppOp.getModel();

  if( data->props == NULL ) {
    return;
  }

  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "check weather themes at %02d:%02d theme=%X %d", hour, min, data->theme, data->themetimerrand );

  if( data->theme == NULL ) {
    /* select a theme... */
    iONode theme = wWeather.getweathertheme(data->props);
    while( theme != NULL ) {
      if( (hour == wWeatherTheme.gethour(theme) && min == wWeatherTheme.getminute(theme)) ||
          (data->themetimerrand <= 0 && wWeatherTheme.israndom(theme)) || (data->requestedTheme != NULL && StrOp.equals(data->requestedTheme, wWeatherTheme.getid(theme) )) ) {
        data->theme = theme;
        data->themeduration = 0;
        data->themetimer1 = 0;
        data->themedim = 0;
        data->themestartup = True;
        data->themeshutdown = False;
        if( data->requestedTheme != NULL && StrOp.equals(data->requestedTheme, wWeatherTheme.getid(theme) ) ) {
          StrOp.free(data->requestedTheme);
          data->requestedTheme = NULL;
        }
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "activating theme [%s]", wWeatherTheme.getid(theme) );
          __checkAction(weather, wWeatherTheme.getid(theme));
        break;
      }
      theme = wWeather.nextweathertheme(data->props, theme);
    }
  }
  else {
    data->themeduration++;
  }

  if( data->theme != NULL &&  data->themeduration > (wWeatherTheme.getduration(data->theme)*10) && !data->themeshutdown ) {
    /*data->theme = NULL;
    data->themedim = 0;*/
    data->themetimer1 = 0;
    data->themeshutdown = True;
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "deactivate theme [%s]", wWeatherTheme.getid(data->theme) );
  }

  data->themedimtimer++;

  if( data->theme != NULL ) {
    if( data->themestartup && data->themedim < wWeatherTheme.getdim(data->theme) ) {
      if( data->themedimtimer >= 10 ) {
        float dim = wWeatherTheme.getdim(data->theme);
        dim /= 10.0;
        if( dim < 1 )
          dim = 1;
        data->themedim += (int)dim;
        TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "***** themedim=%d", data->themedim );
        __doDaylight(weather, hour, min, False, False);
      }
    }
    else if( data->themestartup ) {
      data->themestartup = False;
    }

    if( data->themeshutdown && data->themedim > 0 ) {
      if( data->themedimtimer >= 10 ) {
        float dim = wWeatherTheme.getdim(data->theme);
        dim /= 10.0;
        if( dim < 1 )
          dim = 1;
        data->themedim -= (int)dim;
        if( data->themedim <= 0) {
          data->themedim = 0;
          data->themeshutdown = False;
          data->theme = NULL;
        }
        TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "***** themedim=%d", data->themedim );
        __doDaylight(weather, hour, min, False, False);
      }
    }
    else if( data->themeshutdown ) {
      data->themeshutdown = False;
      data->theme = NULL;
    }

    if( data->theme != NULL && !data->themestartup && !data->themeshutdown && data->themetimer1 <= 0 ) {
      /* play it... */
      iOStrTok tok = StrTokOp.inst( wWeatherTheme.getoutputs(data->theme), ',' );
      while( StrTokOp.hasMoreTokens(tok) ) {
        const char* id = StrTokOp.nextToken(tok);
        iOOutput output = ModelOp.getOutput(model, id);
        if( output != NULL ) {
          int i = 0;
          int cnt = rand() % 10;
          for( i = 0; i < cnt; i++ ) {
            iONode cmd = NodeOp.inst( wOutput.name(), NULL, ELEMENT_NODE);
            iONode color = NodeOp.inst( wColor.name(), NULL, ELEMENT_NODE);
            NodeOp.addChild(cmd, color);
            wColor.setred(color, 50);
            wColor.setgreen(color, 50);
            wColor.setblue(color, 255);
            wOutput.setaddr(cmd, wOutput.getaddr(OutputOp.base.properties(output)));
            wOutput.setvalue(cmd, 255);
            wOutput.setcmd(cmd, wOutput.value);
            OutputOp.cmd(output, cmd, False);
            ThreadOp.sleep(rand()%100);

            cmd = NodeOp.inst( wOutput.name(), NULL, ELEMENT_NODE);
            color = NodeOp.inst( wColor.name(), NULL, ELEMENT_NODE);
            NodeOp.addChild(cmd, color);
            wOutput.setaddr(cmd, wOutput.getaddr(OutputOp.base.properties(output)));
            wOutput.setvalue(cmd, 0);
            wOutput.setcmd(cmd, wOutput.value);
            OutputOp.cmd(output, cmd, False);
            ThreadOp.sleep(rand()%100);
          }

          iONode cmd = NodeOp.inst( wOutput.name(), NULL, ELEMENT_NODE);
          wOutput.setaddr(cmd, wOutput.getaddr(OutputOp.base.properties(output)));
          wOutput.setvalue(cmd, 255);
          wOutput.setcmd(cmd, wOutput.value);
          OutputOp.cmd(output, cmd, False);

          if( wWeatherTheme.getsoundrandomrate(data->theme) == 0 ) {
            cnt = 0;
          }

          if( cnt == wWeatherTheme.getsoundrandomrate(data->theme) && StrOp.len(wWeatherTheme.getsound(data->theme)) > 0 ) {
            if( data->themesoundtimer <= 0 ) {
              if( StrOp.len(wWeatherTheme.getsound(data->theme)) > 0 ) {
                char* s = StrOp.fmt("%s \"%s%c%s\"", wRocRail.getsoundplayer(AppOp.getIni()),
                    wRocRail.getsoundpath(AppOp.getIni()), SystemOp.getFileSeparator(), wWeatherTheme.getsound(data->theme) );
                SystemOp.system( s, True, False );
                StrOp.free(s);
              }
              if( StrOp.len(wWeatherTheme.getsoundoutput(data->theme)) > 0 ) {
                iOOutput output = ModelOp.getOutput(model, wWeatherTheme.getsoundoutput(data->theme));
                if( output != NULL ) {
                  iONode cmd = NodeOp.inst( wOutput.name(), NULL, ELEMENT_NODE);
                  wOutput.setaddr(cmd, wOutput.getaddr(OutputOp.base.properties(output)));
                  wOutput.setcmd(cmd, wOutput.on);
                  OutputOp.cmd(output, cmd, False);
                  ThreadOp.sleep(100);
                  cmd = NodeOp.inst( wOutput.name(), NULL, ELEMENT_NODE);
                  wOutput.setaddr(cmd, wOutput.getaddr(OutputOp.base.properties(output)));
                  wOutput.setcmd(cmd, wOutput.off);
                  OutputOp.cmd(output, cmd, False);
                }
              }
              data->themesoundtimer = 10;
            }
          }


          cmd = NodeOp.inst( wOutput.name(), NULL, ELEMENT_NODE);
          wOutput.setaddr(cmd, wOutput.getaddr(OutputOp.base.properties(output)));
          wOutput.setvalue(cmd, 0);
          wOutput.setcmd(cmd, wOutput.value);
          OutputOp.cmd(output, cmd, False);
        }
      };
      StrTokOp.base.del(tok);

      data->themetimer1 = rand()%wWeatherTheme.getduration(data->theme);
      data->themesoundtimer--;
    }
    else {
      data->themetimer1--;
      data->themesoundtimer--;
    }

  }

  if( data->themetimerrand <= 0 )
    data->themetimerrand = rand()%600;
  else
    data->themetimerrand--;

  if( data->themedimtimer >= 10 )
    data->themedimtimer = 0;
}


static void __makeWeather( void* threadinst ) {
  iOThread        th = (iOThread)threadinst;
  iOWeather  weather = (iOWeather)ThreadOp.getParm(th);
  iOWeatherData data = Data(weather);

  iOControl control = AppOp.getControl();
  int lastMin = 0;
  int loopCnt = 10;
  int hour    = 0;
  int min     = 0;

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "make weather started..." );
  ThreadOp.sleep(1000);
  __doInitialize(weather, True, True );

  data->themetimerrand = rand()%600;

  while( data->run ) {
    long t = ControlOp.getTime(control);
    struct tm* ltm = localtime( &t );
    hour = ltm->tm_hour;
    min  = ltm->tm_min;

    if( loopCnt >= 10 ) {
      loopCnt = 0;

      if( lastMin != min ) {
        if( wCtrl.isweather(wRocRail.getctrl(AppOp.getIni())) ) {
          TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "weather time is %02d:%02d", hour, min );
          __doDaylight(weather, hour, min, False, False );
        }
        lastMin = min;
      }
    }
    else {
      loopCnt++;
    }
    if( wCtrl.isweather(wRocRail.getctrl(AppOp.getIni())) ) {
      __checkWeatherThemes(weather, hour, min );
    }
    ThreadOp.sleep(100);
  }
  __doDaylight(weather, hour, min, wWeather.isoffatshutdown(data->props), False );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "make weather ended..." );

}

static void _halt( iOWeather inst ) {
  iOWeatherData data = Data(inst);
  data->run = False;
  ThreadOp.sleep(120);
}


static void _setWeather( iOWeather inst, const char* id, const char* param ) {
  iOWeatherData data = Data(inst);
  iOModel   model   = AppOp.getModel();
  iOControl control = AppOp.getControl();

  if( id != NULL && StrOp.len(id) > 0 ) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "use weather [%s] with parameter [%s]", id, param!=NULL?param:"-" );
    data->props = ModelOp.getWeather(model, id);

    if( data->props == NULL ) {
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "weather [%s] not found", id );
    }

    if( data->props != NULL && param != NULL ) {
      iONode clone = NULL;
      Boolean relativetime = wWeather.isrelativetime(data->props);
      iONode  sunriseProps = wWeather.getsunrise(data->props);
      iONode  noonProps    = wWeather.getnoon(data->props);
      iONode  sunsetProps  = wWeather.getsunset(data->props);
      /* 360,720,1080 */
      int idx = 0;
      iOStrTok tok = StrTokOp.inst( param, ',' );
      while( StrTokOp.hasMoreTokens(tok) ) {
        const char* sMinutes = StrTokOp.nextToken(tok);
        int minutes = atoi(sMinutes);
        if( idx == 0 && sunriseProps != NULL ) {
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "set sunrise time to [%d:%02d]", minutes / 60, minutes % 60 );
          wSunrise.sethour( sunriseProps, minutes / 60 );
          wSunrise.setminute( sunriseProps, minutes % 60 );
        }
        else if( idx == 1 && noonProps != NULL ) {
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "set noon time to [%d:%02d]", minutes / 60, minutes % 60 );
          wNoon.sethour( noonProps, minutes / 60 );
          wNoon.setminute( noonProps, minutes % 60 );
        }
        else if( idx == 2 && sunsetProps != NULL ) {
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "set sunset time to [%d:%02d]", minutes / 60, minutes % 60 );
          wSunset.sethour( sunsetProps, minutes / 60 );
          wSunset.setminute( sunsetProps, minutes % 60 );
        }
        else if( idx == 3 ) {
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "set relative to [%s]", sMinutes );
          relativetime = StrOp.equals("relative", sMinutes);
        }
        idx++;
      };
      StrTokOp.base.del(tok);

      clone = (iONode)NodeOp.base.clone( data->props );
      AppOp.broadcastEvent( clone );

      if( relativetime ) {
        long t = ControlOp.getTime(control);
        struct tm* ltm = localtime( &t );
        data->starthour   = ltm->tm_hour;
        data->startminute = ltm->tm_min;
      }
      else {
        data->starthour   = 0;
        data->startminute = 0;
      }
    }
  }
  else {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "reset weather" );
    data->props = NULL;
  }
}


static void _setTheme( iOWeather inst, const char* id ) {
  iOWeatherData data = Data(inst);
  if( data->requestedTheme != NULL )
    StrOp.free(data->requestedTheme);
  if( id != NULL && StrOp.len(id) > 0 ) {
    data->requestedTheme = StrOp.dup(id);
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "use weather theme [%s]", id );
  }
  else
    data->requestedTheme = NULL;
}


static Boolean _isWeather( iOWeather inst, const char* id ) {
  iOWeatherData data = Data(inst);
  if( data->props == NULL )
    return False;

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "is weather [%s] ? [%s]", id, wWeather.getid(data->props) );
  return StrOp.equals(id, wWeather.getid(data->props));
}


/**  */
static struct OWeather* _inst( iONode ini ) {
  iOWeather __Weather = allocMem( sizeof( struct OWeather ) );
  iOWeatherData data = allocMem( sizeof( struct OWeatherData ) );
  MemOp.basecpy( __Weather, &WeatherOp, 0, sizeof( struct OWeather ), data );

  /* Initialize data->xxx members... */
  data->props = ini;
  data->run = True;

  data->makeWeather = ThreadOp.inst( "makeWeather", __makeWeather, __Weather );
  ThreadOp.start( data->makeWeather );

  ModelOp.addSysEventListener( AppOp.getModel(), (obj)__Weather );

  instCnt++;
  return __Weather;
}


/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocrail/impl/weather.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/

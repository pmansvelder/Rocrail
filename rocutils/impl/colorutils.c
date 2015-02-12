/** ------------------------------------------------------------
  * A U T O   G E N E R A T E D  (First time only!)
  * Generator: Rocs ogen (build Feb  9 2015 13:17:37)
  * Module: rocutils
  * XML: 
  * XML: 
  * Object: ColorUtils
  * Date: Thu Feb 12 13:40:28 2015
  * ------------------------------------------------------------
  * $Source$
  * $Author$
  * $Date$
  * $Revision$
  * $Name$
  */

#include "rocutils/impl/colorutils_impl.h"

#include "rocs/public/mem.h"
#include <math.h> 

static int instCnt = 0;

/** ----- OBase ----- */
static void __del( void* inst ) {
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
  return NULL;
}

/** ----- OColorUtils ----- */


/**  */
static void _HSVtoRGB( double hsv_hue, double hsv_saturation, double hsv_value, int* r, int* g, int* b ) {
    double red, green, blue;

    if ( hsv_saturation == 0.0 )
    {
        /* Grey */
        red   = hsv_value;
        green = hsv_value;
        blue  = hsv_value;
    }
    else /* not grey */
    {
        double hue = hsv_hue * 6.0;      /* sector 0 to 5 */
        int i = (int)floor(hue);
        double f = hue - i;          /* fractional part of h */
        double p = hsv_value * (1.0 - hsv_saturation);

        switch (i)
        {
            case 0:
                red = hsv_value;
                green = hsv_value * (1.0 - hsv_saturation * (1.0 - f));
                blue = p;
                break;

            case 1:
                red = hsv_value * (1.0 - hsv_saturation * f);
                green = hsv_value;
                blue = p;
                break;

            case 2:
                red = p;
                green = hsv_value;
                blue = hsv_value * (1.0 - hsv_saturation * (1.0 - f));
                break;

            case 3:
                red = p;
                green = hsv_value * (1.0 - hsv_saturation * f);
                blue = hsv_value;
                break;

            case 4:
                red = hsv_value * (1.0 - hsv_saturation * (1.0 - f));
                green = p;
                blue = hsv_value;
                break;

            default:    /* case 5: */
                red = hsv_value;
                green = p;
                blue = hsv_value * (1.0 - hsv_saturation * f);
                break;
        }
    }


    *r = (int)(red   * 255.0);
    *g = (int)(green * 255.0);
    *b = (int)(blue  * 255.0);
}


/**  */
static void _RGBtoHSV( int r, int g, int b, double* h, double* s, double* v ) {
    const double red   = r / 255.0;
    const double green = g / 255.0;
    const double blue  = b / 255.0;

    /* find the min and max intensity (and remember which one was it for the latter) */
    double minimumRGB = red;
    if ( green < minimumRGB )
        minimumRGB = green;
    if ( blue < minimumRGB )
        minimumRGB = blue;

    enum { RED, GREEN, BLUE } chMax = RED;
    double maximumRGB = red;
    if ( green > maximumRGB )
    {
        chMax = GREEN;
        maximumRGB = green;
    }
    if ( blue > maximumRGB )
    {
        chMax = BLUE;
        maximumRGB = blue;
    }

    const double value = maximumRGB;

    double hue = 0.0, saturation;
    const double deltaRGB = maximumRGB - minimumRGB;
    if ( deltaRGB == 0.0 )
    {
        /* Gray has no color */
        hue = 0.0;
        saturation = 0.0;
    }
    else
    {
        switch ( chMax )
        {
            case RED:
                hue = (green - blue) / deltaRGB;
                break;

            case GREEN:
                hue = 2.0 + (blue - red) / deltaRGB;
                break;

            case BLUE:
                hue = 4.0 + (red - green) / deltaRGB;
                break;
        }

        hue /= 6.0;

        if ( hue < 0.0 )
            hue += 1.0;

        saturation = deltaRGB / maximumRGB;
    }

    *h = hue;
    *s = saturation;
    *v = value;
}

static void _RGBtoXY(int R, int G, int B, float* x, float* y) {
  float red   = ( (float)R / 255.0 );
  float green = ( (float)G / 255.0 );
  float blue  = ( (float)B / 255.0 );

  if ( red > 0.04045 )
    red = pow( ( red + 0.055 ) / 1.055, 2.4);
  else
    red = red / 12.92;

  if ( green > 0.04045 )
    green = pow( ( green + 0.055 ) / 1.055, 2.4);
  else
    green = green / 12.92;

  if ( blue > 0.04045 )
    blue = pow( ( blue + 0.055 ) / 1.055, 2.4);
  else
    blue = blue / 12.92;

  float X = (float) (red * 0.649926 + green * 0.103455 + blue * 0.197109);
  float Y = (float) (red * 0.234327 + green * 0.743075 + blue * 0.022598);
  float Z = (float) (red * 0.000000 + green * 0.053077 + blue * 1.035763);

  *x = X / ( X + Y + Z );
  *y = Y / ( X + Y + Z );
}


/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocutils/impl/colorutils.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/

/*
 Rocrail - Model Railroad Software

Copyright (c) 2002-2015 Robert Jan Versluis, Rocrail.net

 


 All rights reserved.
*/

#ifndef DECODERBASE_H_
#define DECODERBASE_H_

class DecoderBase
{
public:
  virtual void setPortValue( int port, int value, int type ) = 0;
};

#endif /*DECODERBASE_H_*/

/*
 Rocrail - Model Railroad Software
 Copyright (c) 2002-2015 Robert Jan Versluis, Rocrail.net
 All rights reserved.
*/

#ifndef __ROCRAIL_ROCVIEW_ABOX_H
#define __ROCRAIL_ROCVIEW_ABOX_H

#include "rocabox/public/archivebox.h"

class ABox
{
public:
    /// Constructors
    ABox();
private:
    iOArchiveBox m_ArchiveBox;
};

#endif

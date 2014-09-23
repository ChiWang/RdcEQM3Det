/*
 *  $Id: DmpEvtHeader.h, 2014-08-08 21:59:44 DAMPE $
 *  Author(s):
 *    Chi WANG (chiwang@mail.ustc.edu.cn) 13/12/2013
*/

#ifndef DmpEvtHeader_H
#define DmpEvtHeader_H

#include "TObject.h"

class DmpEvtHeader : public TObject{
/*
 * DmpEvtHeader
 *
*/
public:
  DmpEvtHeader():fEventID(-1),fSecond(-1),fMillisecond(-1){}
  ~DmpEvtHeader(){}
  DmpEvtHeader &operator=(const DmpEvtHeader &r){
    fEventID = r.fEventID;
    fSecond = r.fSecond;
    fMillisecond = r.fMillisecond;
  }

  void  SetTime(char *time){
    fSecond = 0;
    for(size_t i=0;i<4;++i){        // 4 bytes second
      fSecond = fSecond *256 + (short)(unsigned char)time[i];
    }
    fMillisecond = (short)(unsigned char)time[4]*256 + (short)(unsigned char)time[5];          // 2 bytes millisecond
  }

public:
  short     fEventID;           // good event
  int       fSecond;            // time second
  short     fMillisecond;       // time millisecond

  ClassDef(DmpEvtHeader,1)
};

#endif


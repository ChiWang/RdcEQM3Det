/*
 *  $Id: DmpEvtHeader.h, 2014-10-09 23:08:50 DAMPE $
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
  DmpEvtHeader();
  ~DmpEvtHeader();
  DmpEvtHeader &operator=(const DmpEvtHeader &r);
  void Reset();
  void LoadFrom(const DmpEvtHeader &r);
  void LoadFrom(const DmpEvtHeader *&r);

public:
  bool IsGoodEvent(const short &id=99)const;
  bool StkFake()const{return (fStkStatus&kFakeData);}

public:
  enum {
    // errors, taken bits 15~4
    kCRCError  = 0x2000,        // for Fee
    kPackageFlagError = 0x1000,     // for Fee
    kTriggerFlagError = 0x0100,     // for Fee
    kTriggerNotMatch = 0x0200,      // for sub-detector, or whole detector
    // below is not a error for current event:    taken bits 0~3
    kFakeData  = 0x0004,
    kTriggerSkipped = 0x0002,   // it's not a error of current event
  };
  short     fEventID;           // good event
  int       fSecond;            // time second
  short     fMillisecond;       // time millisecond
  unsigned short fPsdStatus;
  unsigned short fStkStatus;
  unsigned short fBgoStatus;
  unsigned short fNudStatus;

  ClassDef(DmpEvtHeader,1)
};

#endif


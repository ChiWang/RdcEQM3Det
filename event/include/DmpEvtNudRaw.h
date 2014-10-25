/*
 *  $Id: DmpEvtNudRaw.h, 2014-10-24 17:21:22 DAMPE $
 *  Author(s):
 *    Chi WANG (chiwang@mail.ustc.edu.cn) 24/04/2014
*/

#ifndef DmpEvtNudRaw_H
#define DmpEvtNudRaw_H

#include "DmpFeeNavig.h"

//-------------------------------------------------------------------
class DmpEvtNudRaw : public TObject{
/*
 *  DmpEvtNudRaw
 *
 *      this class is used to save output of Raw for Nud
 *
 *   one object mean one Nud event
 *
 */
public:
  DmpEvtNudRaw();
  ~DmpEvtNudRaw();
  DmpEvtNudRaw &operator=(const DmpEvtNudRaw &r);
  void  Reset();
  void  LoadFrom(DmpEvtNudRaw *r);

public:
  DmpERunMode::Type GetRunMode()const;
  short GetTrigger()const;

public:
  DmpFeeNavig   fFeeNavig;
  //std::map<short,double>    fADC;   // key is channel ID: 0~3; value is ADC count
  short         fChannelID[4];
  double        fADC[4];

  ClassDef(DmpEvtNudRaw,1)
};

#endif


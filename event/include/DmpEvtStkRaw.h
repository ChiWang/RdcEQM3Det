#ifndef DmpEvtStkRaw_H
#define DmpEvtStkRaw_H

#include "DmpFeeNavig.h"
#include <map>

//-------------------------------------------------------------------
class DmpEvtStkRaw : public TObject{
/*
 *  DmpEvtStkRaw
 *
 *      this class is used to save output of Raw for Stk
 *
 *   one object mean one Stk event
 *
 */
public:
  DmpEvtStkRaw();
  ~DmpEvtStkRaw();
  DmpEvtStkRaw &operator=(const DmpEvtStkRaw &r);
  void  Reset();
  void  LoadFrom(DmpEvtStkRaw *r);

public:
  DmpERunMode::Type GetRunMode(const short &i=99)const;
  short GetTrigger(const short &i=99)const;

public:
  std::vector<DmpFeeNavig>  fFeeNavig;
  std::map<long,double>    fADC;      // key is global dynode ID, value is adc count

  ClassDef(DmpEvtStkRaw,1)
};

#endif


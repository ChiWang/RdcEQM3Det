/*
 *  $Id: DmpEvtBgoRaw.h, 2014-10-09 21:39:18 DAMPE $
 *  Author(s):
 *    Chi WANG (chiwang@mail.ustc.edu.cn) 24/04/2014
*/

#ifndef DmpEvtBgoRaw_H
#define DmpEvtBgoRaw_H

//#include <map>
#include "DmpFeeNavig.h"

//-------------------------------------------------------------------
class DmpEvtBgoRaw : public TObject{
/*
 *  DmpEvtBgoRaw
 *
 *      this class is used to save output of Raw for Bgo
 *
 *   one object mean one Bgo event
 *
 */
public:
  DmpEvtBgoRaw();
  ~DmpEvtBgoRaw();
  DmpEvtBgoRaw &operator=(const DmpEvtBgoRaw &r);
  void  Reset();
  void  LoadFrom(DmpEvtBgoRaw *r);

public:
  DmpERunMode::Type GetRunMode(const short &i=99)const;
  bool  TriggersMatch()const;
  short GetTrigger(const short &i=99)const;

public:
  std::vector<DmpFeeNavig>  fFeeNavig;
  //std::map<short,double>    fADC;      // key is global dynode ID, value is adc count
  std::vector<short>    fGlobalDyID;
  std::vector<double>   fADC;

  ClassDef(DmpEvtBgoRaw,1)
};

#endif


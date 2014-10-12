/*
 *  $Id: DmpEvtBgoRaw.cc, 2014-10-10 22:48:41 DAMPE $
 *  Author(s):
 *    Chi WANG (chiwang@mail.ustc.edu.cn) 24/04/2014
*/

#include "DmpEvtBgoRaw.h"
#include "DmpLog.h"
ClassImp(DmpEvtBgoRaw)

//-------------------------------------------------------------------
DmpEvtBgoRaw::DmpEvtBgoRaw()
{
}

//-------------------------------------------------------------------
DmpEvtBgoRaw::~DmpEvtBgoRaw(){
  Reset();
}

//-------------------------------------------------------------------
DmpEvtBgoRaw& DmpEvtBgoRaw::operator=(const DmpEvtBgoRaw &r){
  Reset();
  fFeeNavig = r.fFeeNavig;
  fADC = r.fADC;
}

//-------------------------------------------------------------------
void DmpEvtBgoRaw::Reset(){
  fFeeNavig.clear();
  fADC.clear();
}

//-------------------------------------------------------------------
void DmpEvtBgoRaw::LoadFrom(DmpEvtBgoRaw *r){
  Reset();
  fFeeNavig = r->fFeeNavig;
  fADC = r->fADC;
}

//-------------------------------------------------------------------
DmpERunMode::Type DmpEvtBgoRaw::GetRunMode(const short &index)const{
  DmpERunMode::Type a = DmpERunMode::kUnknow;
  if(fFeeNavig.size() != 0){
    if(index == 99){
      a = fFeeNavig.at(0).RunMode;
      for(size_t i=1;i<fFeeNavig.size();++i){
        if( a != fFeeNavig.at(i).RunMode){
          a = DmpERunMode::kMixed;
          break;
        }
      }
    }else{
      a = fFeeNavig.at(index).RunMode;
    }
  }
  return a;
}

//-------------------------------------------------------------------
short DmpEvtBgoRaw::GetTrigger(const short &index)const{
  short trig = -1;
  if(fFeeNavig.size() != 0){
    if(index == 99){  // check all Fee
      trig = fFeeNavig.at(0).Trigger;
      for(size_t i=1;i<fFeeNavig.size();++i){
        if(trig != fFeeNavig.at(i).Trigger){
          trig = -1;
          break;
        }
      }
    }else{
      trig = fFeeNavig.at(index).Trigger;
    }
  }
  return trig;
}



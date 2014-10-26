/*
 *  $Id: DmpEvtBgoRaw.cc, 2014-10-10 22:48:41 DAMPE $
 *  Author(s):
 *    Chi WANG (chiwang@mail.ustc.edu.cn) 24/04/2014
*/

#include "DmpEvtBgoRaw.h"
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
  fGlobalDynodeID = r.fGlobalDynodeID;
  fADC = r.fADC;
}

//-------------------------------------------------------------------
void DmpEvtBgoRaw::Reset(){
  fFeeNavig.clear();
  fGlobalDynodeID.clear();
  fADC.clear();
}

//-------------------------------------------------------------------
void DmpEvtBgoRaw::LoadFrom(DmpEvtBgoRaw *r){
  Reset();
  fFeeNavig = r->fFeeNavig;
  fGlobalDynodeID = r->fGlobalDynodeID;
  fADC = r->fADC;
}

//-------------------------------------------------------------------
DmpERunMode::Type DmpEvtBgoRaw::GetRunMode(const short &index)const{
  DmpERunMode::Type a = DmpERunMode::kUnknow;
  if(fFeeNavig.size() != 0){
    if(index == 99){
      a = fFeeNavig.at(0).GetRunMode();
      for(size_t i=1;i<fFeeNavig.size();++i){
        if( a != fFeeNavig.at(i).GetRunMode()){
          a = DmpERunMode::kMixed;
          break;
        }
      }
    }else{
      a = fFeeNavig.at(index).GetRunMode();
    }
  }
  return a;
}

//-------------------------------------------------------------------
bool DmpEvtBgoRaw::TriggersMatch()const{
  bool tmp = true;
  short trig = fFeeNavig.at(0).GetTrigger();
  for(size_t i=1;i<fFeeNavig.size();++i){
    if(trig != fFeeNavig.at(i).GetTrigger()){
      tmp = false;
      break;
    }
  }
  return tmp;
}

//-------------------------------------------------------------------
short DmpEvtBgoRaw::GetTrigger(const short &index)const{
  short trig = -1;
  if(index == 99){  // check all Fee
    if(TriggersMatch()){
      trig = fFeeNavig.at(0).GetTrigger();
    }
  }else{
    trig = fFeeNavig.at(index).GetTrigger();
  }
  return trig;
}



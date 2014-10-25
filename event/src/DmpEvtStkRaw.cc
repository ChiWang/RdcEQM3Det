#include "DmpEvtStkRaw.h"
ClassImp(DmpEvtStkRaw)

//-------------------------------------------------------------------
DmpEvtStkRaw::DmpEvtStkRaw()
{
}

//-------------------------------------------------------------------
DmpEvtStkRaw::~DmpEvtStkRaw(){
  Reset();
}

//-------------------------------------------------------------------
DmpEvtStkRaw& DmpEvtStkRaw::operator=(const DmpEvtStkRaw &r){
  Reset();
  fFeeNavig = r.fFeeNavig;
  fADC = r.fADC;
}

//-------------------------------------------------------------------
void DmpEvtStkRaw::Reset(){
  fFeeNavig.clear();
  fADC.clear();
}

//-------------------------------------------------------------------
void DmpEvtStkRaw::LoadFrom(DmpEvtStkRaw *r){
  Reset();
  fFeeNavig = r->fFeeNavig;
  fADC = r->fADC;
}

//-------------------------------------------------------------------
DmpERunMode::Type DmpEvtStkRaw::GetRunMode(const short &index)const{
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
short DmpEvtStkRaw::GetTrigger(const short &index)const{
  short trig = -1;
  if(fFeeNavig.size() != 0){
    if(index == 99){  // check all Fee
      trig = fFeeNavig.at(0).GetTrigger();
      for(size_t i=1;i<fFeeNavig.size();++i){
        if(trig != fFeeNavig.at(i).GetTrigger()){
          trig = -1;
          break;
        }
      }
    }else{
      trig = fFeeNavig.at(index).GetTrigger();
    }
  }
  return trig;
}



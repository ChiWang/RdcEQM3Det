/*
 *  $Id: DmpEvtNudRaw.cc, 2014-10-24 17:21:20 DAMPE $
 *  Author(s):
 *    Chi WANG (chiwang@mail.ustc.edu.cn) 24/04/2014
*/

#include "DmpEvtNudRaw.h"

//-------------------------------------------------------------------
DmpEvtNudRaw::DmpEvtNudRaw(){
}

//-------------------------------------------------------------------
DmpEvtNudRaw::~DmpEvtNudRaw(){
  Reset();
}

//-------------------------------------------------------------------
DmpEvtNudRaw& DmpEvtNudRaw::operator=(const DmpEvtNudRaw &r){
  Reset();
  fFeeNavig = r.fFeeNavig;
  for(size_t i=0;i<4;++i){
    fChannelID[i] = r.fChannelID[i];
    fADC[i] = r.fADC[i];
  }
}

//-------------------------------------------------------------------
void DmpEvtNudRaw::Reset(){
}

//-------------------------------------------------------------------
void DmpEvtNudRaw::LoadFrom(DmpEvtNudRaw *r){
  Reset();
  fFeeNavig = r->fFeeNavig;
  for(size_t i=0;i<4;++i){
    fChannelID[i] = r->fChannelID[i];
    fADC[i] = r->fADC[i];
  }
}

//-------------------------------------------------------------------
DmpERunMode::Type DmpEvtNudRaw::GetRunMode()const{
  return fFeeNavig.GetRunMode();
}

//-------------------------------------------------------------------
short DmpEvtNudRaw::GetTrigger()const{
  return fFeeNavig.GetTrigger();
}


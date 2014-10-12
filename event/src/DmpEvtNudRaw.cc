/*
 *  $Id: DmpEvtNudRaw.cc, 2014-10-09 21:06:52 DAMPE $
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
  fADC = r.fADC;
}

//-------------------------------------------------------------------
void DmpEvtNudRaw::Reset(){
  fADC.clear();
}

//-------------------------------------------------------------------
void DmpEvtNudRaw::LoadFrom(DmpEvtNudRaw *r){
  Reset();
  fFeeNavig = r->fFeeNavig;
  fADC = r->fADC;
}

//-------------------------------------------------------------------
DmpERunMode::Type DmpEvtNudRaw::GetRunMode()const{
  return fFeeNavig.RunMode;
}

//-------------------------------------------------------------------
short DmpEvtNudRaw::GetTrigger()const{
  return fFeeNavig.Trigger;
}


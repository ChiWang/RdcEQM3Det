/*
 *  $Id: DmpEvtHeader.cc, 2014-10-10 18:53:39 DAMPE $
 *  Author(s):
 *    Chi WANG (chiwang@mail.ustc.edu.cn) 09/10/2014
*/

#include "DmpEDetectorID.h"
#include "DmpEvtHeader.h"

ClassImp(DmpEvtHeader)

//-------------------------------------------------------------------
DmpEvtHeader::DmpEvtHeader()
 :fEventID(-1),
  fSecond(-1),
  fMillisecond(-1),
  fPsdStatus(0x0000),
  fStkStatus(0x0000),
  fBgoStatus(0x0000),
  fNudStatus(0x0000)
{
}

//-------------------------------------------------------------------
DmpEvtHeader::~DmpEvtHeader(){

}

//-------------------------------------------------------------------
DmpEvtHeader& DmpEvtHeader::operator=(const DmpEvtHeader &r){
  fEventID = r.fEventID;
  fSecond = r.fSecond;
  fMillisecond = r.fMillisecond;
  fPsdStatus = r.fPsdStatus;
  fStkStatus = r.fStkStatus;
  fBgoStatus = r.fBgoStatus;
  fNudStatus = r.fNudStatus;
}

//-------------------------------------------------------------------
void DmpEvtHeader::Reset(){
  // NOTE: Do NOT need to reset others
  fSecond = 0;
  fMillisecond = 0;
  fPsdStatus = 0x0000;
  fStkStatus = 0x0000;
  fBgoStatus = 0x0000;
  fNudStatus = 0x0000;
}

//-------------------------------------------------------------------
void DmpEvtHeader::LoadFrom(const DmpEvtHeader &r){
  fEventID = r.fEventID;
  fSecond = r.fSecond;
  fMillisecond = r.fMillisecond;
  fPsdStatus = r.fPsdStatus;
  fStkStatus = r.fStkStatus;
  fBgoStatus = r.fBgoStatus;
  fNudStatus = r.fNudStatus;
}

//-------------------------------------------------------------------
void DmpEvtHeader::LoadFrom(DmpEvtHeader *&r){
  fEventID = r->fEventID;
  fSecond = r->fSecond;
  fMillisecond = r->fMillisecond;
  fPsdStatus = r->fPsdStatus;
  fStkStatus = r->fStkStatus;
  fBgoStatus = r->fBgoStatus;
  fNudStatus = r->fNudStatus;
}

//-------------------------------------------------------------------
bool DmpEvtHeader::IsGoodEvent(const short &id)const{
  bool v = true;
  if(id == DmpEDetectorID::kPsd){
    v = not (fPsdStatus & 0xfff0);
  }else if(id == DmpEDetectorID::kStk){
    v = not (fStkStatus & 0xfff0);
  }else if(id == DmpEDetectorID::kBgo){
    v = not (fBgoStatus & 0xfff0);
  }else if(id == DmpEDetectorID::kNud){
    v = not (fNudStatus & 0xfff0);
  }else if(id == 99){
    v = not ((fPsdStatus|fStkStatus|fBgoStatus|fNudStatus) & 0xfff0);
  }
  return v;
}



/*
 *  $Id: DmpEvtHeader.cc, 2014-10-24 00:33:41 DAMPE $
 *  Author(s):
 *    Chi WANG (chiwang@mail.ustc.edu.cn) 09/10/2014
*/

#include "DmpEDetectorID.h"
#include "DmpEvtHeader.h"

ClassImp(DmpEvtHeader)

//-------------------------------------------------------------------
DmpEvtHeader::DmpEvtHeader(){
  Reset();
  fTmpDeltaTime = 0;
}

//-------------------------------------------------------------------
DmpEvtHeader::~DmpEvtHeader(){
}

//-------------------------------------------------------------------
DmpEvtHeader& DmpEvtHeader::operator=(const DmpEvtHeader &r){
  fTrigger = r.fTrigger;
  fDeltaTime = r.fDeltaTime;
  fSecond = r.fSecond;
  fMillisecond = r.fMillisecond;
  fTriggerStatus = r.fTriggerStatus;
  fPsdStatus = r.fPsdStatus;
  fStkStatus = r.fStkStatus;
  fBgoStatus = r.fBgoStatus;
  fNudStatus = r.fNudStatus;
}

//-------------------------------------------------------------------
void DmpEvtHeader::Reset(){
  fTmpDeltaTime = fSecond*1000+fMillisecond;
  fTrigger = 0x0000;
  fDeltaTime = 99.99;
  fSecond = 0;
  fMillisecond = 0;
  fTriggerStatus.reset();
  fPsdStatus.reset();
  fStkStatus.reset();
  fBgoStatus.reset();
  fNudStatus.reset();
}

//-------------------------------------------------------------------
void DmpEvtHeader::LoadFrom(DmpEvtHeader *r){
  fTrigger = r->fTrigger;
  fDeltaTime = r->fDeltaTime;
  fSecond = r->fSecond;
  fMillisecond = r->fMillisecond;
  fTriggerStatus = r->fTriggerStatus;
  fPsdStatus = r->fPsdStatus;
  fStkStatus = r->fStkStatus;
  fBgoStatus = r->fBgoStatus;
  fNudStatus = r->fNudStatus;
}

//-------------------------------------------------------------------
bool DmpEvtHeader::IsGoodEvent(const short &id)const{
  bool v = true;
    // fake data is good for this event
  if(id == DmpEDetectorID::kPsd){
    v = (fPsdStatus.count()==1 && fPsdStatus.test(6))?true:false;
  }else if(id == DmpEDetectorID::kStk){
    v = (fStkStatus.count()==1 && fStkStatus.test(6))?true:false;
  }else if(id == DmpEDetectorID::kBgo){
    v = (fBgoStatus.count()==1 && fBgoStatus.test(6))?true:false;
  }else if(id == DmpEDetectorID::kNud){
    v = (fNudStatus.count()==1 && fNudStatus.test(6))?true:false;
  }else if(id == 99){
    v = (IsGoodEvent(DmpEDetectorID::kPsd) && IsGoodEvent(DmpEDetectorID::kStk) && IsGoodEvent(DmpEDetectorID::kBgo) && (IsGoodEvent(DmpEDetectorID::kNud)));
  }
  return v;
}

//-------------------------------------------------------------------
bool DmpEvtHeader::IsFakeData(const short &id)const{
  bool v = false;
  if(id == DmpEDetectorID::kPsd){
    v = fPsdStatus[6];
  }else if(id == DmpEDetectorID::kStk){
    v = fStkStatus[6];
  }else if(id == DmpEDetectorID::kBgo){
    v = fBgoStatus[6];
  }else if(id == DmpEDetectorID::kNud){
    v = fNudStatus[6];
  }else{
    throw;
  }
  return v;
}

//-------------------------------------------------------------------
bool DmpEvtHeader::TriggersMatch(const short &id)const{
  bool v = true;
  if(id == DmpEDetectorID::kPsd){
    v = not fPsdStatus[0];
  }else if(id == DmpEDetectorID::kStk){
    v = not fStkStatus[0];
  }else if(id == DmpEDetectorID::kBgo){
    v = not fBgoStatus[0];
  }else if(id == DmpEDetectorID::kNud){
    v = not fNudStatus[0];
  }else if(99 == id){
    v = not (fPsdStatus[1] || fBgoStatus[1] || fNudStatus[1] || fStkStatus[1]);
  }else{
    throw;
  }
  return v;
}

//-------------------------------------------------------------------
short DmpEvtHeader::ChoosedTriggerType(const short &class_id)const{
  short id = -1;
  if(4 == class_id){
    id = (fTriggerStatus.to_ulong()>>27) & 0x07;
  }else if(3 == class_id){
    id = (fTriggerStatus.to_ulong()>>22) & 0x1f;
  }else if(2 == class_id){
    id = (fTriggerStatus.to_ulong()>>20) & 0x03;
  }else if(1 == class_id){
    id = (fTriggerStatus.to_ulong()>>18) & 0x03;
  }else if(0 == class_id){
    id = (fTriggerStatus.to_ulong()>>16) & 0x03;
  }
  return id;
}

//-------------------------------------------------------------------
void DmpEvtHeader::SetTriggerChoose(const unsigned short &choosing){
  std::bitset<16> tmp = choosing;
  for(size_t i=0;i<14;++i){ // bit 31, bit 30 are not for class type
    if(tmp.test(i)){
      fTriggerStatus.set(16+i);
    }
  }
}

//-------------------------------------------------------------------
void DmpEvtHeader::SetTriggerGenerate(const unsigned char &g){
  std::bitset<8> tmp = g;
  for(size_t i=0;i<7;++i){  // bit 15 is not used
    if(tmp.test(i)){
      fTriggerStatus.set(8+i);
    }
  }
}

//-------------------------------------------------------------------
void DmpEvtHeader::SetTriggerEnable(const unsigned char &e){
  std::bitset<8> tmp = e;
  for(size_t i=0;i<7;++i){  // bit 7 is not used
    if(tmp.test(i)){
      fTriggerStatus.set(i);
    }
  }
}

//-------------------------------------------------------------------
void DmpEvtHeader::SetDeltaTime(const float &v){
  fTmpDeltaTime = fSecond*1000 + fMillisecond - fTmpDeltaTime;
  if(6 < fTmpDeltaTime){
    fDeltaTime = fTmpDeltaTime>30000?30000:((fTmpDeltaTime/6)*6+v);
  }else{
    fDeltaTime = v;
  }
}

//-------------------------------------------------------------------
void DmpEvtHeader::SetTag(const short &id,TagType tagType){
  if(id == DmpEDetectorID::kPsd){
    fPsdStatus.set(tagType);
  }else if(id == DmpEDetectorID::kStk){
    fStkStatus.set(tagType);
  }else if(id == DmpEDetectorID::kBgo){
    fBgoStatus.set(tagType);
  }else if(id == DmpEDetectorID::kNud){
    fNudStatus.set(tagType);
  }else{
    throw;
  }
}



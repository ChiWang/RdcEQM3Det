/*
 *  $Id: ProcessThisEventNud.cc, 2014-10-08 18:10:34 DAMPE $
 *  Author(s):
 *    Chi WANG (chiwang@mail.ustc.edu.cn) 09/03/2014
 *    Yifeng WEI (weiyf@mail.ustc.edu.cn) 24/04/2014
*/

#include "DmpDataBuffer.h"
#include "DmpAlgHex2Root.h"
#include "DmpParameterNud.h"

//-------------------------------------------------------------------
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
bool DmpAlgHex2Root::InitializeNud(){
  DmpLogInfo<<"Initialize \tNud"<<DmpLogEndl;
  fEvtNud = new DmpEvtNudRaw();
  gDataBuffer->RegisterObject("Event/Rdc/Nud",fEvtNud,"DmpEvtNudRaw");
  return true;
}

//-------------------------------------------------------------------
bool DmpAlgHex2Root::ProcessThisEventNud(const long &id){
  if(fNudBuf.find(id) == fNudBuf.end()){
  std::cout<<"DEBUG: "<<__FILE__<<"("<<__LINE__<<") not find "<<id<<std::endl;
    return false;
  }
  short lastTrigger = fEvtNud->GetTrigger();
  fEvtNud->Reset();
  fEvtNud->fFeeNavig = fNudBuf[id]->Navigator;
  for(size_t c=0;c<4;++c){        // 4 channels of Nud
    short v = (short)((fNudBuf[id]->Signal[2*c]<<8) | (fNudBuf[id]->Signal[2*c+1]&0x00ff));
    fEvtNud->fADC.insert(std::make_pair(c,v));
  }
  short currentTrigger = fEvtNud->GetTrigger();
  if(-1 != lastTrigger && (lastTrigger+1 & currentTrigger) != currentTrigger){
    DmpLogWarning<<"Nud trigger not continuous:\t"<<lastTrigger<<" ---> "<<currentTrigger<<DmpLogEndl;
    fEvtHeader->fNudStatus = fEvtHeader->fNudStatus | DmpEvtHeader::kTriggerSkipped;
  }
  CheckFeeFlag_Nud();
  return true;
}

//-------------------------------------------------------------------
void DmpAlgHex2Root::CheckFeeFlag_Nud(){
  if(false ==  fEvtNud->fFeeNavig.CRCFlag){
    DmpLogError<<"Fee(Nud)_0x"<<std::hex<<fEvtNud->fFeeNavig.FeeID<<std::dec<<"\tCRC error";PrintTime();
    fEvtHeader->fNudStatus = DmpEvtHeader::kCRCError;
  }
  if(DmpETriggerFlag::kCheckWrong == fEvtNud->fFeeNavig.TriggerFlag){
    DmpLogError<<"Fee(Nud)_0x"<<std::hex<<fEvtNud->fFeeNavig.FeeID<<std::dec<<"\ttrigger flag error";PrintTime();
    fEvtHeader->fNudStatus = fEvtHeader->fNudStatus | DmpEvtHeader::kTriggerFlagError;
  }
  if(DmpEPackageFlag::kGood != fEvtNud->fFeeNavig.PackageFlag){
    DmpLogError<<"Fee(Nud)_0x"<<std::hex<<fEvtNud->fFeeNavig.FeeID<<"\tpackage flag error:\t"<<fEvtNud->fFeeNavig.PackageFlag<<std::dec;PrintTime();
    fEvtHeader->fNudStatus = fEvtHeader->fNudStatus | DmpEvtHeader::kPackageFlagError;
  }
}


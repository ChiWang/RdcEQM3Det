/*
 *  $Id: ProcessThisEventNud.cc, 2014-10-22 21:53:50 DAMPE $
 *  Author(s):
 *    Chi WANG (chiwang@mail.ustc.edu.cn) 09/03/2014
 *    Yifeng WEI (weiyf@mail.ustc.edu.cn) 24/04/2014
*/

#include "DmpDataBuffer.h"
#include "DmpAlgHex2Root.h"

//-------------------------------------------------------------------
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
bool DmpAlgHex2Root::InitializeNud(){
  DmpLogInfo<<"Initialize \tNud"<<DmpLogEndl;
  fEvtNud = new DmpEvtNudRaw();
  gDataBuffer->RegisterObject("Event/Rdc/Nud",fEvtNud);
  return true;
}

//-------------------------------------------------------------------
bool DmpAlgHex2Root::ProcessThisEventNud(const long &id){
  if(fNudBuf.find(id) == fNudBuf.end()){
    DmpLogError<<" (Nud) not find event "<<id<<DmpLogEndl;
    return false;
  }
  DmpLogDebug<<DmpLogEndl;
  fEvtNud->Reset();
  fEvtNud->fFeeNavig = fNudBuf[id]->Navigator;
  for(size_t c=0;c<4;++c){        // 4 channels of Nud
    short v = (short)((fNudBuf[id]->Signal[2*c]<<8) | (fNudBuf[id]->Signal[2*c+1]&0x00ff));
    fEvtNud->fChannelID[c] = c;
    fEvtNud->fADC[c] =v;
  }
    // flag check
    short feeID = fEvtNud->fFeeNavig.GetFeeID();
    if(fEvtNud->fFeeNavig.CRCError()){
      DmpLogError<<"Fee(Nud)_0x"<<std::hex<<feeID<<std::dec<<"\tCRC error";PrintTime();
      fEvtHeader->SetTag(DmpEDetectorID::kNud,DmpEvtHeader::tag_CRCError);
    }
    if(fEvtNud->fFeeNavig.ReceivedTriggerCheck_Wrong()){
      DmpLogError<<"Fee(Nud)_0x"<<std::hex<<feeID<<std::dec<<"\treceived trigger check signal from trigger system, check error";PrintTime();
      fEvtHeader->SetTag(DmpEDetectorID::kNud,DmpEvtHeader::tag_TrigFlagError);
    }
    if(fEvtNud->fFeeNavig.PackageFlagError()){
      DmpLogError<<"Fee(Nud)_0x"<<std::hex<<feeID<<"\tpackage flag error: "<<fEvtNud->fFeeNavig.GetPackageFlag()<<std::dec;PrintTime();
      fEvtHeader->SetTag(DmpEDetectorID::kNud,DmpEvtHeader::tag_PkgFlagError);
    }
  // trigger match trigger system
  if(fEvtHeader->GetTrigger() != fEvtNud->GetTrigger()){
    fEvtHeader->SetTag(DmpEDetectorID::kNud,DmpEvtHeader::tag_TrigNotMatchTrigSys);
    DmpLogWarning<<"Nud trigger not match trigger system.\t Nud = "<<fEvtNud->GetTrigger()<<" trigger system = "<<fEvtHeader->GetTrigger();PrintTime();
  }
  DmpLogDebug<<DmpLogEndl;
  return true;
}


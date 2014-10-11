/*
 *  $Id: ProcessThisEventPsd.cc, 2014-10-09 21:43:40 DAMPE $
 *  Author(s):
 *    Chi WANG (chiwang@mail.ustc.edu.cn) 09/03/2014
*/

#include "DmpEvtHeader.h"
#include "DmpEvtPsdRaw.h"
#include "DmpMetadata.h"
#include "DmpDataBuffer.h"
#include "DmpAlgHex2Root.h"
#include "DmpPsdBase.h"
#include "DmpParameterPsd.h"

//-------------------------------------------------------------------
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
bool DmpAlgHex2Root::InitializePsd(){
  DmpLogInfo<<"Initialize\tPsd: Setting connector"<<DmpLogEndl;
// *
// *  TODO: setup connector
// *
  //-------------------------------------------------------------------
  fEvtPsd = new DmpEvtPsdRaw();
  gDataBuffer->RegisterObject("Event/Rdc/Psd",fEvtPsd,"DmpEvtPsdRaw");
  return true;
}

//-------------------------------------------------------------------
bool DmpAlgHex2Root::ProcessThisEventPsd(const long &id){
  if(fPsdBuf.find(id) == fPsdBuf.end()){
  std::cout<<"DEBUG: "<<__FILE__<<"("<<__LINE__<<") not find "<<id<<std::endl;
    return false;
  }
  short lastTrigger = fEvtPsd->GetTrigger();
  fEvtPsd->Reset();
  for(size_t i=0;i<DmpParameterPsd::kFeeNo;++i){
    fEvtPsd->fFeeNavig.push_back(fPsdBuf[id][i]->Navigator);
    short feeID = fPsdBuf[id][i]->Navigator.FeeID;
    if(DmpERunMode::kCompress == fPsdBuf[id][i]->Navigator.RunMode){
      short nChannel = fPsdBuf[id][i]->Signal.size()/3;
      for(size_t c=0;c<nChannel;++c){
        short channelID = (short)(unsigned char)fPsdBuf[id][i]->Signal[3*c];
        short v = (short)((fPsdBuf[id][i]->Signal[3*c+1]<<8) | (fPsdBuf[id][i]->Signal[3*c+2]&0x00ff));
        fEvtPsd->fADC.insert(std::make_pair(fMapPsd[feeID*1000+channelID],v));
      }
    }else if(DmpERunMode::kOriginal == fPsdBuf[id][i]->Navigator.RunMode || DmpERunMode::kCalDAC == fPsdBuf[id][i]->Navigator.RunMode){
      short nChannel = fPsdBuf[id][i]->Signal.size()/2;
      for(size_t c=0;c<nChannel;++c){
        short v = (short)((fPsdBuf[id][i]->Signal[2*c]<<8) | (fPsdBuf[id][i]->Signal[2*c+1]&0x00ff));
        fEvtPsd->fADC.insert(std::make_pair(fMapPsd[feeID*1000+c],v));
      }
    }else{
      DmpLogError<<" Wrong mode... FeeID = "<<std::hex<<fPsdBuf[id][i]->Navigator.FeeID<<"\tmode "<<fPsdBuf[id][i]->Navigator.RunMode<<std::dec; PrintTime();
    }
  }
  short currentTrigger = fEvtPsd->GetTrigger();
  if(currentTrigger == -1){
    DmpLogError<<"Fee(Psd) triggers not match";PrintTime();
    for(short i=0;i<fEvtPsd->fFeeNavig.size();++i){
      DmpLogCout<<"\tFee_0x"<<std::hex<<fEvtPsd->fFeeNavig[i].FeeID<<",\t trigger = "<<fEvtPsd->fFeeNavig[i].Trigger<<std::dec<<DmpLogEndl;
    }
    fEvtHeader->fPsdStatus = fEvtHeader->fPsdStatus | DmpEvtHeader::kTriggerNotMatch;
  }
  if(-1 != lastTrigger && (lastTrigger+1 & currentTrigger) != currentTrigger){
    DmpLogWarning<<"Psd trigger not continuous:\t"<<lastTrigger<<" ---> "<<currentTrigger;PrintTime();
    fEvtHeader->fPsdStatus = fEvtHeader->fPsdStatus | DmpEvtHeader::kTriggerSkipped;
  }
  CheckFeeFlag_Psd();
  return true;
}

//-------------------------------------------------------------------
void DmpAlgHex2Root::CheckFeeFlag_Psd(){
  for(size_t i=0;i<fEvtPsd->fFeeNavig.size();++i){
    if(false ==  fEvtPsd->fFeeNavig[i].CRCFlag){
      DmpLogError<<"Fee(Psd)_0x"<<std::hex<<fEvtPsd->fFeeNavig[i].FeeID<<std::dec<<"\tCRC error";PrintTime();
      fEvtHeader->fPsdStatus = DmpEvtHeader::kCRCError;
    }
    if(DmpETriggerFlag::kCheckWrong == fEvtPsd->fFeeNavig[i].TriggerFlag){
      DmpLogError<<"Fee(Psd)_0x"<<std::hex<<fEvtPsd->fFeeNavig[i].FeeID<<std::dec<<"\ttrigger flag error";PrintTime();
      fEvtHeader->fPsdStatus = fEvtHeader->fPsdStatus | DmpEvtHeader::kTriggerFlagError;
    }
    if(DmpEPackageFlag::kGood != fEvtPsd->fFeeNavig[i].PackageFlag){
      DmpLogError<<"Fee(Psd)_0x"<<std::hex<<fEvtPsd->fFeeNavig[i].FeeID<<"\tpackage flag error:\t"<<fEvtPsd->fFeeNavig[i].PackageFlag<<std::dec;PrintTime();
      fEvtHeader->fPsdStatus = fEvtHeader->fPsdStatus | DmpEvtHeader::kPackageFlagError;
    }
  }
}


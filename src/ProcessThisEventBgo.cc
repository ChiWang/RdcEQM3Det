/*
 *  $Id: ProcessThisEventBgo.cc, 2014-10-24 14:52:18 DAMPE $
 *  Author(s):
 *    Chi WANG (chiwang@mail.ustc.edu.cn) 09/03/2014
 *    Yifeng WEI (weiyf@mail.ustc.edu.cn) 24/04/2014
*/

#include "DmpDataBuffer.h"
#include "DmpAlgHex2Root.h"
#include "DmpBgoBase.h"

//-------------------------------------------------------------------
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
bool DmpAlgHex2Root::InitializeBgo(){
  DmpLogInfo<<"Initialize\tBgo: Setting connector"<<DmpLogEndl;
  // setup connector
  short feeID=0, channelNo=0, channelID=0, layerID=0, barID=0, sideID=0, dyID=0;
  boost::filesystem::directory_iterator end_iter;
  for(boost::filesystem::directory_iterator iter(fMetadata->GetValue("Bgo/Connector"));iter!=end_iter;++iter){
    if(iter->path().extension() != ".cnct") continue;
    ifstream cnctFile(iter->path().string().c_str());
    if(not cnctFile.good()){
      DmpLogError<<"\t\treading cnct file ("<<iter->path().string()<<") failed"<<DmpLogEndl;
      cnctFile.close();
      return false;
    }
    cnctFile>>feeID;
    cnctFile>>channelNo;
    DmpLogInfo<<"\tReading cnct file: "<<iter->path().string()<<"\tDone. ID = "<<feeID<<"\tN_channel = "<<channelNo<<DmpLogEndl;
    for(short s=0;s<channelNo;++s){
      cnctFile>>channelID;
      cnctFile>>layerID;
      cnctFile>>barID;
      cnctFile>>sideID;
      cnctFile>>dyID;
      fMapBgo.insert(std::make_pair(feeID*1000+channelID,DmpBgoBase::ConstructGlobalDynodeID(layerID,barID,sideID,dyID)));
    }
    cnctFile.close();
  }
  //-------------------------------------------------------------------
  fEvtBgo = new DmpEvtBgoRaw();
  gDataBuffer->RegisterObject("Event/Rdc/Bgo",fEvtBgo,"DmpEvtBgoRaw");
  return true;
}

//-------------------------------------------------------------------
bool DmpAlgHex2Root::ProcessThisEventBgo(const long &id){
  if(fBgoBuf.find(id) == fBgoBuf.end()){
  std::cout<<"DEBUG: "<<__FILE__<<"("<<__LINE__<<") not find "<<id<<std::endl;
    return false;
  }
std::cout<<"DEBUG: "<<__FILE__<<"("<<__LINE__<<")"<<std::endl;
  fEvtBgo->Reset();
  short nFee = fBgoBuf[id].size();
  for(short i=0;i<nFee;++i){
    fEvtBgo->fFeeNavig.push_back(fBgoBuf[id][i]->Navigator);
    short feeID = fBgoBuf[id][i]->Navigator.GetFeeID();
    DmpERunMode::Type runMode = fBgoBuf[id][i]->Navigator.GetRunMode();
    if(DmpERunMode::kCompress == runMode){
      short nChannel = fBgoBuf[id][i]->Signal.size()/3;
      for(size_t c=0;c<nChannel;++c){
        short channelID = (short)(unsigned char)fBgoBuf[id][i]->Signal[3*c];
        channelID += feeID*1000;
        short v = (short)((fBgoBuf[id][i]->Signal[3*c+1]<<8) | (fBgoBuf[id][i]->Signal[3*c+2]&0x00ff));
        //fEvtBgo->fADC.insert(std::make_pair(fMapBgo.at(feeID*1000+channelID),v));
        if(fMapBgo.find(channelID) == fMapBgo.end()){
          continue;
        }
        fEvtBgo->fGlobalDynodeID.push_back(fMapBgo.at(channelID));
        fEvtBgo->fADC.push_back(v);
      }
    }else if(DmpERunMode::kOriginal == runMode || DmpERunMode::kCalDAC == runMode){
      short nChannel = fBgoBuf[id][i]->Signal.size()/2;
      for(size_t c=0;c<nChannel;++c){
        short v = (short)((fBgoBuf[id][i]->Signal[2*c]<<8) | (fBgoBuf[id][i]->Signal[2*c+1]&0x00ff));
        //fEvtBgo->fADC.insert(std::make_pair(fMapBgo.at(feeID*1000+c),v));
        fEvtBgo->fGlobalDynodeID.push_back(fMapBgo.at(feeID*1000+c));
        fEvtBgo->fADC.push_back(v);
      }
    }else{
      DmpLogError<<" Wrong mode... FeeID = "<<std::hex<<feeID<<"\tmode "<<runMode<<std::dec; PrintTime();
    }
    // flag check
    if(fEvtBgo->fFeeNavig[i].CRCError()){
      DmpLogError<<"Fee(Bgo)_0x"<<std::hex<<feeID<<std::dec<<"\tCRC error";PrintTime();
      fEvtHeader->SetTag(DmpEDetectorID::kBgo,DmpEvtHeader::tag_CRCError);
    }
    if(fEvtBgo->fFeeNavig[i].ReceivedTriggerCheck_Wrong()){
      DmpLogError<<"Fee(Bgo)_0x"<<std::hex<<feeID<<std::dec<<"\treceived trigger check signal from trigger system, check error";PrintTime();
      fEvtHeader->SetTag(DmpEDetectorID::kBgo,DmpEvtHeader::tag_TrigFlagError);
    }
    if(fEvtBgo->fFeeNavig[i].PackageFlagError()){
      DmpLogError<<"Fee(Bgo)_0x"<<std::hex<<feeID<<"\tpackage flag error: "<<fEvtBgo->fFeeNavig[i].GetPackageFlag()<<std::dec;PrintTime();
      fEvtHeader->SetTag(DmpEDetectorID::kBgo,DmpEvtHeader::tag_PkgFlagError);
    }
  }
  // inside triggers match
  if(not fEvtBgo->TriggersMatch()){
    fEvtHeader->SetTag(DmpEDetectorID::kBgo,DmpEvtHeader::tag_TrigNotMatchInSubDet);
    DmpLogError<<"Triggers of Bgo Fee not match\t";PrintTime();
    for(size_t i=0;i<nFee;++i){
      DmpLogCout<<"\tFeeID = "<<std::hex<<fEvtBgo->fFeeNavig[i].GetFeeID()<<"\ttrigger = "<<fEvtBgo->fFeeNavig[i].GetTrigger()<<std::dec<<DmpLogEndl;
    }
  }
  // trigger match trigger system
  if(fEvtHeader->GetTrigger() != fEvtBgo->GetTrigger()){
    fEvtHeader->SetTag(DmpEDetectorID::kBgo,DmpEvtHeader::tag_TrigNotMatchTrigSys);
    DmpLogWarning<<"Bgo trigger not match trigger system.\t Bgo = "<<fEvtBgo->GetTrigger()<<" trigger system = "<<fEvtHeader->GetTrigger(); PrintTime();
  }
std::cout<<"DEBUG: "<<__FILE__<<"("<<__LINE__<<")"<<std::endl;
  return true;
}



/*
 *  $Id: ProcessThisEventBgo.cc, 2014-08-31 22:47:11 DAMPE $
 *  Author(s):
 *    Chi WANG (chiwang@mail.ustc.edu.cn) 09/03/2014
 *    Yifeng WEI (weiyf@mail.ustc.edu.cn) 24/04/2014
*/

#include "DmpEvtBgoRaw.h"
#include "DmpDataBuffer.h"
#include "DmpAlgHex2Root.h"
#include "DmpBgoBase.h"
#include "DmpParameterBgo.h"

//-------------------------------------------------------------------
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
bool DmpAlgHex2Root::InitializeBgo(){
  if(fCNCTPathBgo == "NO"){
    DmpLogInfo<<"No set connector:\tBgo"<<DmpLogEndl;
    return true;
  }else{
    DmpLogInfo<<"Setting connector:\tBgo"<<DmpLogEndl;
  }
  // setup connector
  short feeID=0, channelNo=0, channelID=0, layerID=0, barID=0, sideID=0, dyID=0;
  boost::filesystem::directory_iterator end_iter;
  for(boost::filesystem::directory_iterator iter(fCNCTPathBgo);iter!=end_iter;++iter){
    if(iter->path().extension() != ".cnct") continue;
    ifstream cnctFile(iter->path().string().c_str());
    if (not cnctFile.good()){
      DmpLogError<<"\t\treading cnct file ("<<iter->path().string()<<") failed"<<DmpLogEndl;
      cnctFile.close();
      return false;
    }
    cnctFile>>feeID;
    cnctFile>>channelNo;
    for(short s=0;s<channelNo;++s){
      cnctFile>>channelID;
      cnctFile>>layerID;
      cnctFile>>barID;
      cnctFile>>sideID;
      cnctFile>>dyID;
      fMapBgo.insert(std::make_pair(feeID*1000+channelID,DmpBgoBase::ConstructGlobalDynodeID(layerID,barID,sideID,dyID)));
    }
    cnctFile.close();
    DmpLogInfo<<"\tReading cnct file: "<<iter->path().string()<<"\tDone. ID = "<<feeID<<"\tN_channel = "<<channelNo<<DmpLogEndl;
  }
  //-------------------------------------------------------------------
  fEvtBgo = new DmpEvtBgoRaw();
  if(not gDataBuffer->RegisterObject("Event/Rdc/Bgo",fEvtBgo,"DmpEvtBgoRaw")){
    return false;
  }
  return true;
}

//-------------------------------------------------------------------
bool DmpAlgHex2Root::ProcessThisEventBgo(const long &id){
  if(fBgoBuf.find(id) == fBgoBuf.end()){
  std::cout<<"DEBUG: "<<__FILE__<<"("<<__LINE__<<") not find "<<id<<std::endl;
    return false;
  }
  fEvtBgo->Reset();
  for(size_t i=0;i<DmpParameterBgo::kFeeNo;++i){
    fEvtBgo->SetFeeNavigator(fBgoBuf[id][i]->Navigator);
    short feeID = fBgoBuf[id][i]->Navigator.FeeID;
    if(DmpERunMode::kCompress == fBgoBuf[id][i]->Navigator.RunMode){
      short nChannel = fBgoBuf[id][i]->Signal.size()/3;
      for(size_t c=0;c<nChannel;++c){
        short channelID = (short)(unsigned char)fBgoBuf[id][i]->Signal[3*c];
        short v = (short)((fBgoBuf[id][i]->Signal[3*c+1]<<8) | (fBgoBuf[id][i]->Signal[3*c+2]&0x00ff));
        fEvtBgo->AppendSignal(fMapBgo[feeID*1000+channelID],v);
      }
    }else if(DmpERunMode::kOriginal == fBgoBuf[id][i]->Navigator.RunMode || DmpERunMode::kCalDAC == fBgoBuf[id][i]->Navigator.RunMode){
      short nChannel = fBgoBuf[id][i]->Signal.size()/2;
      for(size_t c=0;c<nChannel;++c){
        short v = (short)((fBgoBuf[id][i]->Signal[2*c]<<8) | (fBgoBuf[id][i]->Signal[2*c+1]&0x00ff));
        fEvtBgo->AppendSignal(fMapBgo[feeID*1000+c],v);
      }
    }else{
      DmpLogError<<" Wrong mode... FeeID = "<<std::hex<<fBgoBuf[id][i]->Navigator.FeeID<<"\tmode "<<fBgoBuf[id][i]->Navigator.RunMode<<std::dec<<std::endl;
    }
  }
  fEvtBgo->GenerateStatus();
  return true;
}



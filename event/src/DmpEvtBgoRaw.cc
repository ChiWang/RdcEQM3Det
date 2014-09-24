/*
 *  $Id: DmpEvtBgoRaw.cc, 2014-09-11 16:35:39 DAMPE $
 *  Author(s):
 *    Chi WANG (chiwang@mail.ustc.edu.cn) 24/04/2014
*/

#include <iostream>

#include "DmpEvtBgoRaw.h"
#include "DmpEFeeFlags.h"

//-------------------------------------------------------------------
DmpEvtBgoRaw::DmpEvtBgoRaw()
 :fTrigger(-1),fRunMode(-1),fIsGood(true)
{
}

//-------------------------------------------------------------------
DmpEvtBgoRaw::~DmpEvtBgoRaw(){
}

//-------------------------------------------------------------------
void DmpEvtBgoRaw::Reset(){
  fFeeNavig.clear();
  fGlobalDynodeID.clear();
  fADC.clear();
  fIsGood = true;
}

//-------------------------------------------------------------------
void DmpEvtBgoRaw::AppendSignal(const short &gid,const short &v){
  //short n= fGlobalDynodeID.size();
  //for(size_t i=0;i<n;++i){
  //  if(gid == fGlobalDynodeID[i]){
  //    std::cout<<"Error: exist this gid"<<std::endl;
  //    return;
  //  }
  //}
  fGlobalDynodeID.push_back(gid);
  fADC.push_back(v);
}

//-------------------------------------------------------------------
void DmpEvtBgoRaw::GenerateStatus(){
//-------------------------------------------------------------------
  short lastTrigger = fTrigger;
  fTrigger = fFeeNavig[0].Trigger;
  if(-1 != lastTrigger && (lastTrigger+1 & fTrigger) != fTrigger){
      std::cout<<" Bgo trigger not continuous:\t"<<lastTrigger<<"->"<<fTrigger<<std::endl;
  }
//-------------------------------------------------------------------
  fRunMode = fFeeNavig[0].RunMode;
  for(size_t i=1;i<fFeeNavig.size();++i){
    if(fRunMode != fFeeNavig[i].RunMode){
      fRunMode = DmpERunMode::kUnknow;
      //fIsGood = false;  // yes! Some good event run mode not match, too
    }
  }
//-------------------------------------------------------------------
  for(size_t i=0;i<fFeeNavig.size();++i){
    if(false ==  fFeeNavig[i].CRCFlag){
      fIsGood = false;
      break;
    }
    if(DmpETriggerFlag::kCheckWrong == fFeeNavig[i].TriggerFlag){
      fIsGood = false;
      break;
    }
    if(DmpEPackageFlag::kGood != fFeeNavig[i].PackageFlag){
      fIsGood = false;
      break;
    }
  }
}


/*
 *  $Id: ProcessThisEventNud.cc, 2014-08-31 22:47:13 DAMPE $
 *  Author(s):
 *    Chi WANG (chiwang@mail.ustc.edu.cn) 09/03/2014
 *    Yifeng WEI (weiyf@mail.ustc.edu.cn) 24/04/2014
*/

#include "DmpEvtNudRaw.h"
#include "DmpDataBuffer.h"
#include "DmpAlgHex2Root.h"
#include "DmpParameterNud.h"

//-------------------------------------------------------------------
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
bool DmpAlgHex2Root::InitializeNud(){
  return true;
}

//-------------------------------------------------------------------
bool DmpAlgHex2Root::ProcessThisEventNud(const long &id){
  if(fNudBuf.find(id) == fNudBuf.end()){
  std::cout<<"DEBUG: "<<__FILE__<<"("<<__LINE__<<") not find "<<id<<std::endl;
    return false;
  }
  //fEvtNud->Reset();
  return true;
}



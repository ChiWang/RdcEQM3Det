/*
 *  $Id: ProcessThisEventPsd.cc, 2014-08-31 22:47:09 DAMPE $
 *  Author(s):
 *    Chi WANG (chiwang@mail.ustc.edu.cn) 09/03/2014
 *    Yifeng WEI (weiyf@mail.ustc.edu.cn) 24/04/2014
*/

#include "DmpEvtPsdRaw.h"
#include "DmpDataBuffer.h"
#include "DmpAlgHex2Root.h"
#include "DmpParameterPsd.h"

//-------------------------------------------------------------------
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
bool DmpAlgHex2Root::InitializePsd(){
  return true;
}

//-------------------------------------------------------------------
bool DmpAlgHex2Root::ProcessThisEventPsd(const long &id){
  if(fPsdBuf.find(id) == fPsdBuf.end()){
  std::cout<<"DEBUG: "<<__FILE__<<"("<<__LINE__<<") not find "<<id<<std::endl;
    return false;
  }
  //fEvtPsd->Reset();
  return true;
}



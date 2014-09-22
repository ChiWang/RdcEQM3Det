/*
 *  $Id: ReadDataIntoDataBuffer.cc, 2014-08-31 22:46:51 DAMPE $
 *  Author(s):
 *    Chi WANG (chiwang@mail.ustc.edu.cn) 08/08/2014
*/

#include "DmpAlgHex2Root.h"
#include "DmpEDetectorID.h"
#include "DmpCore.h"
#include "DmpParameterBgo.h"
#include "DmpParameterPsd.h"
#include "DmpParameterNud.h"
#include "DmpParameterStk.h"

bool DmpAlgHex2Root::ReadDataIntoDataBuffer(){
  static short s_LastPkgID = -1, s_LastFeeTrg = -1, s_CurrentFeeTrg = 0;
  static short s_TotalFeeNo = DmpParameterBgo::kFeeNo+DmpParameterNud::kFeeNo+DmpParameterPsd::kFeeNo+DmpParameterStk::kTrbNo;
  unsigned int scientificHeader = 0;         // 4 bytes 0xe225 0813
  fFile.read((char*)(&scientificHeader),4);
  while(0xe2250813 != htobe32(scientificHeader)){
    if(fFile.tellg() == -1){
      return false;
    }
    fFile.seekg((int)fFile.tellg()-3,std::ios::beg);
    fFile.read((char*)(&scientificHeader),4);
  }
  int endOfLastHeader = fFile.tellg();
  unsigned short packetID = 0;
  fFile.read((char*)(&packetID),2);
  packetID = htobe16(packetID) & 0x3fff;    // only bits 0~13
  unsigned short dataLength = 0;
  fFile.read((char*)(&dataLength),2);
  dataLength = htobe16(dataLength);
  char time[8];
  fFile.read(time,8);
  if(fFile.tellg() == -1){  // data length right
    return false;
  }
  _HeaderNavig *newEvt = new _HeaderNavig(dataLength,&time[2]);
  fHeaderBuf.insert(std::make_pair(fGoodRawEventID,newEvt));
  if((s_LastPkgID != -1) && (((packetID-1)&s_LastPkgID) != s_LastPkgID)){
    DmpLogWarning<<"Scientific data package count not continuous...\tLast/Current: "<<s_LastPkgID<<"/"<<packetID;  PrintTime();
  }
  s_LastPkgID = packetID;
  if(CheckE2250813DataLength(dataLength)){    // will find next 0xe2250813 as expected
    for(short i=0;i<s_TotalFeeNo;++i){
      unsigned short feeHeader = 0;
      fFile.read((char*)(&feeHeader),2);
      if(0xeb90 == htobe16(feeHeader)){
        fFile.seekg((int)fFile.tellg()+2,std::ios::beg);    // NOTE: skip 1 byte pkgFlag, 1 byte runMode_feeID
        fFile.read((char*)(&dataLength),2);
        dataLength= htobe16(dataLength);
        if(CheckEb90DataLength(dataLength)){
          fFile.seekg((int)fFile.tellg()-4,std::ios::beg);  // 4 = 2 bytes data length + skipped 2 bytes
          char data[dataLength];    // NOTE: include 1 pkgFlag, include 1 byte runMode_feeID, include 2 bytes of data length, include all scientific data, exclude 2 bytes of CRC
          fFile.read(data,dataLength);
          unsigned short crc = 0;
          fFile.read((char*)(&crc),2);
          crc= htobe16(crc);
          _FeeData *newFee = new _FeeData(data,dataLength,crc);
          //DmpLogInfo<<"Fee ID 0x"<<std::hex<<newFee->Navigator.FeeID<<", Mode "<<newFee->Navigator.RunMode<<std::dec<<DmpLogEndl;
          short detectorID = (newFee->Navigator.FeeID>>4)&0x03;
          if(DmpEDetectorID::kStk == detectorID){
            delete newFee;
            continue;
          }
          if(i==0){ // trigger check
            s_CurrentFeeTrg = newFee->Navigator.Trigger;
            if((s_LastFeeTrg != -1) && ((s_CurrentFeeTrg&(s_LastFeeTrg+1)) != s_CurrentFeeTrg)){    // trigger continuous
              DmpLogWarning<<"Fee trigger not continuous..."<<DmpLogEndl;
            }
            s_LastFeeTrg = s_CurrentFeeTrg;
          }else{
            if(newFee->Navigator.Trigger != s_CurrentFeeTrg){    // trigger match
              DmpLogError<<" trigger_0 = "<<std::hex<<s_CurrentFeeTrg<<"\t trigger_"<<i<<" = "<<newFee->Navigator.Trigger<<std::dec<<DmpLogEndl;
              delete newFee;
              Exception(endOfLastHeader,"Trigger not match");
              return false;
            }
          }
          if(newFee->Navigator.CRCFlag){   // CRC check
            if(DmpEDetectorID::kBgo == detectorID){
              fBgoBuf[fGoodRawEventID].push_back(newFee);
            }else if(DmpEDetectorID::kPsd == detectorID){
              fPsdBuf[fGoodRawEventID].push_back(newFee);
            }else if(DmpEDetectorID::kNud == detectorID){
              fNudBuf[fGoodRawEventID] = newFee;
            }else if(DmpEDetectorID::kStk == detectorID){
              // *
              // *  TODO: 
              // *
            }else{
              Exception(endOfLastHeader,"Fee type error");
              return false;
            }
          }else{
            Exception(endOfLastHeader,"CRC error");
            return false;
          }
        }else{
          Exception(endOfLastHeader,"Data length error [0xeb90]");
          return false;
        }
      }else{
        Exception(endOfLastHeader,"Not find 0xeb90");
        return false;
      }
    }
  }else{
    Exception(endOfLastHeader,"Data length error [0xe2250813]");
    return false;
  }
  // check buffer, has current event? TODO: for Stk?
  fEventInBuf.push_back(fGoodRawEventID);
  ++fGoodRawEventID;
  return true;
}

//-------------------------------------------------------------------
bool DmpAlgHex2Root::CheckE2250813DataLength(const int &n){
  int skipPoint = fFile.tellg();
  fFile.seekg(skipPoint+n+1-8,std::ios::beg);   // time: 8 bytes. need 1
  unsigned int scientificHeader = 0;            // 4 bytes 0xe225 0813
  fFile.read((char*)(&scientificHeader),4);
  if(0xe2250813 != htobe32(scientificHeader)){
    return false;
  }
  fFile.seekg(skipPoint,std::ios::beg);
  return true;
}

//-------------------------------------------------------------------
bool DmpAlgHex2Root::CheckEb90DataLength(const int &n){
  int skipPoint = fFile.tellg();
  fFile.seekg(skipPoint+n-2,std::ios::beg);   // NOTE:  n include 2 bytes which is data length
  unsigned short header = 0;         // 4 bytes 0xe225 0813
  fFile.read((char*)(&header),2);
  fFile.seekg(skipPoint,std::ios::beg);
  if(0xeb90 != htobe16(header) && 0xe225 != htobe16(header)){   // the last Fee will read 0xe225
    return false;
  }
  return true;
}

//-------------------------------------------------------------------
void DmpAlgHex2Root::Exception(const int &endOfLastE2250813,const std::string &e){
  DmpLogError<<e<<"\tEvent ID: "<<gCore->GetCurrentEventID(); PrintTime();
  fFile.seekg(endOfLastE2250813,std::ios::beg);
  unsigned int scientificHeader = 0;         // 4 bytes 0xe225 0813
  fFile.read((char*)(&scientificHeader),4);
  while(0xe2250813 != htobe32(scientificHeader)){
    if(fFile.tellg() == -1){
      return;
    }
    fFile.seekg((int)fFile.tellg()-3,std::ios::beg);
    fFile.read((char*)(&scientificHeader),4);
  }
  int nBytes = (int)fFile.tellg() - endOfLastE2250813;
  char *errorData = new char[nBytes];
  fFile.seekg(endOfLastE2250813-4,std::ios::beg);
  fFile.read(errorData,nBytes);
  fOutError.write(errorData,nBytes);
  delete[] errorData;
  EraseBuffer(fGoodRawEventID);
  std::cout<<std::endl;
}



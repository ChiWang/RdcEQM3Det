/*
 *  $Id: ReadDataIntoDataBuffer.cc, 2014-10-22 18:29:36 DAMPE $
 *  Author(s):
 *    Chi WANG (chiwang@mail.ustc.edu.cn) 08/08/2014
*/

#include "DmpAlgHex2Root.h"
#include "DmpEDetectorID.h"
#include "DmpCore.h"

bool DmpAlgHex2Root::ReadDataIntoDataBuffer(){
  static short s_LastPkgID = -1;
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
      unsigned short feeHeader = 0;
      fFile.read((char*)(&feeHeader),2);
      while(0xeb90 == htobe16(feeHeader)){
        fFile.seekg((int)fFile.tellg()+1,std::ios::beg);    // NOTE: skip 1 byte pkgFlag
        short feeID = 0;
        fFile.read((char*)(&feeID),1);                 // NOTE: get Fee ID
        DmpLogDebug<<std::hex<<"Reading feeID = "<<feeID<<std::dec<<std::endl;
        if((feeID&0x003c) == 0x0030){     // for data of trigger system
          fFile.seekg((int)fFile.tellg()-2,std::ios::beg);  // to the end of eb90. 2:  read 1 byte + skipped 1 byte
          dataLength = 10;  // 10 bytes data of trigger, from the end of eb90, to the end of Check-Sum
          char data[dataLength];
          fFile.read(data,dataLength);
          _TriggerData *newTrig = new _TriggerData(data,dataLength);
          fTriggerBuf[fGoodRawEventID] = newTrig;
        }else{
          fFile.read((char*)(&dataLength),2);
          dataLength= htobe16(dataLength);
          if(CheckEb90DataLength(dataLength)){
            fFile.seekg((int)fFile.tellg()-4,std::ios::beg);  // to the end of eb90. 4 = 2 bytes data length + skipped 1 byte pkgFlag + 1 byte feeID
            dataLength += 2;    // NOTE: include: 1 byte pkgFlag, 1 byte runMode_feeID, 2 bytes data length, all scientific data, ADN 2 bytes of CRC
            char data[dataLength];
            fFile.read(data,dataLength);
            _FeeData *newFee = new _FeeData(data,dataLength);
            //DmpLogInfo<<"Fee ID 0x"<<std::hex<<newFee->Navigator.FeeID<<", Mode "<<newFee->Navigator.RunMode<<std::dec<<DmpLogEndl;
            DmpEDetectorID::Type detID = newFee->Navigator.GetDetectorID();
            if(DmpEDetectorID::kBgo == detID){
              fBgoBuf[fGoodRawEventID].push_back(newFee);
            }else if(DmpEDetectorID::kPsd == detID){
              fPsdBuf[fGoodRawEventID].push_back(newFee);
            }else if(DmpEDetectorID::kNud == detID){
              fNudBuf[fGoodRawEventID] = newFee;
            }else if(DmpEDetectorID::kStk == detID){
              fStkBuf[fGoodRawEventID].push_back(newFee);
            }else{
              Exception(endOfLastHeader,"Fee type error");
              return false;
            }
          }else{
            Exception(endOfLastHeader,"Data length error [0xeb90]");
            return false;
          }
        }
        fFile.read((char*)(&feeHeader),2);
      }
      if(0xe225 == htobe16(feeHeader)){
        fFile.seekg((int)fFile.tellg()-2,std::ios::beg);
      }else{
        Exception(endOfLastHeader,"Not find next 0xeb90");
        return false;
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
  fFile.seekg(skipPoint,std::ios::beg);
  if(0xe2250813 != htobe32(scientificHeader)){
    return false;
  }
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



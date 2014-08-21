/*
 *  $Id: DmpRdcAlgEQM.cc, 2014-08-13 10:57:04 DAMPE $
 *  Author(s):
 *    Chi WANG (chiwang@mail.ustc.edu.cn) 27/05/2014
*/

#include <stdlib.h>     // getenv()
#include "DmpEvtHeader.h"
#include "DmpDataBuffer.h"
#include "DmpRdcAlgEQM.h"
#include "DmpParameterBgo.h"
#include "DmpParameterPsd.h"
#include "DmpParameterNud.h"

//-------------------------------------------------------------------
DmpRdcAlgEQM::DmpRdcAlgEQM()
 :DmpVAlg("Rdc/EQM"),fInDataName("NO"),fEvtHeader(0),
  fCNCTPathBgo("NO"),fEvtBgo(0),
  fCNCTPathPsd("NO"),fEvtPsd(0),
  fCNCTPathNud("NO"),fEvtNud(0)
  //fCNCTPathStk("NO"),fEvtStk(0)
{
  OptMap.insert(std::make_pair("BinaryFile",    0));
  OptMap.insert(std::make_pair("Bgo/Connector", 1));
  OptMap.insert(std::make_pair("Psd/Connector", 4));
  OptMap.insert(std::make_pair("Stk/Connector", 5));
  OptMap.insert(std::make_pair("Nud/Connector", 7));
}

//-------------------------------------------------------------------
DmpRdcAlgEQM::~DmpRdcAlgEQM(){
}

//-------------------------------------------------------------------
void DmpRdcAlgEQM::Set(const std::string &type, const std::string &argv){
// *
// *  TODO: if release, use DMPSWSYS
// *
  std::string prefix = (std::string)getenv("DMPSWWORK")+"/share/Connector/";
  //std::string prefix = (std::string)getenv("DMPSWSYS")+"/share/Connector/";
  switch (OptMap[type]){
    case 0: // BinaryFile
    {
      fInDataName = argv;
      break;
    }
    case 1: // Connector/Bgo
    {
      fCNCTPathBgo = prefix+argv;
      break;
    }
    case 4:
    {// Psd/Connector
      fCNCTPathPsd = prefix+argv;
      break;
    }
    case 5:
    {// Stk/Connector
      //fCNCTPathStk = prefix+argv;
      break;
    }
    case 7:
    {// Nud/Connector
      fCNCTPathNud = prefix+argv;
      break;
    }
    default:
    {
      DmpLogWarning<<"No argument type: "<<type<<DmpLogEndl;
    }
  }
}

//-------------------------------------------------------------------
bool DmpRdcAlgEQM::Initialize(){
  fFile.open(fInDataName.c_str(),std::ios::in|std::ios::binary);
  if(not fFile.good()){
    DmpLogError<<"Open "<<fInDataName<<" failed"<<DmpLogEndl;
    return false;
  }else{
    std::string name = "Error_"+fInDataName.filename().string();
    DmpLogInfo<<"Reading "<<fInDataName.string()<<"\tError data in "<<name<<DmpLogEndl;
    fOutError.open(name.c_str(),std::ios::out|std::ios::binary);
  }
  fTotalFeeNo = DmpParameterBgo::kFeeNo+DmpParameterNud::kFeeNo+DmpParameterPsd::kFeeNo;
  fEvtHeader = new DmpEvtHeader();
  if(not gDataBuffer->RegisterObject("Event/Rdc/EventHeader",fEvtHeader,"DmpEvtHeader")){
    return false;
  }
  bool bgo = InitializeBgo();
  bool psd = InitializePsd();
  bool nud = InitializeNud();
  return (bgo&&psd&&nud);
}

//-------------------------------------------------------------------
#include "DmpCore.h"
bool DmpRdcAlgEQM::ProcessThisEvent(){
  while(fEventInBuf.size() == 0){
    if(fFile.eof()){
      DmpLogInfo<<"Reach the end of "<<fInDataName<<DmpLogEndl;
      gCore->TerminateRun();
      return false;
    }
    ReadDataIntoDataBuffer();
  }
  bool header = ProcessThisEventHeader(*fEventInBuf.begin());
  bool bgo = ProcessThisEventBgo(*fEventInBuf.begin());
  bool psd = ProcessThisEventPsd(*fEventInBuf.begin());
  bool nud = ProcessThisEventNud(*fEventInBuf.begin());
  fEventInBuf.erase(fEventInBuf.begin());
  return (header && bgo && psd && nud);
}

//-------------------------------------------------------------------
bool DmpRdcAlgEQM::Finalize(){
  fFile.close();
  fOutError.close();
  return true;
}

//-------------------------------------------------------------------
bool DmpRdcAlgEQM::ProcessThisEventHeader(const long &id){
  if(fHeaderBuf.find(id) == fHeaderBuf.end()){
    return false;
  }
  fEvtHeader->Reset();
  fEvtHeader->SetEventID(gCore->GetCurrentEventID());
  fEvtHeader->SetTime(&fHeaderBuf[id].Time[2]);
  fHeaderBuf.erase(id);
  return true;
}

//-------------------------------------------------------------------
void DmpRdcAlgEQM::PrintTime()const{
  std::cout<<"  Time:";
  for(size_t i=2;i<8;++i){
    std::cout<<std::hex<<"  "<<(short)(unsigned char)fHeaderBuf.end()->second.Time[i];
  }
  std::cout<<std::dec<<std::endl;
}

//-------------------------------------------------------------------
_FeeData::_FeeData(const _FeeData &r){
  Navigator = r.Navigator;
  short n=r.Signal.size();
  for(short i=0;i<n;++i){
    Signal.push_back(r.Signal[i]);
  }
}

//-------------------------------------------------------------------
unsigned short crc16_ccitt_tableH[256]={
 0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7,
 0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef,
 0x1231, 0x0210, 0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6,
 0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de,
 0x2462, 0x3443, 0x0420, 0x1401, 0x64e6, 0x74c7, 0x44a4, 0x5485,
 0xa56a, 0xb54b, 0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d,
 0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6, 0x5695, 0x46b4,
 0xb75b, 0xa77a, 0x9719, 0x8738, 0xf7df, 0xe7fe, 0xd79d, 0xc7bc,
 0x48c4, 0x58e5, 0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823,
 0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a, 0xb92b,
 0x5af5, 0x4ad4, 0x7ab7, 0x6a96, 0x1a71, 0x0a50, 0x3a33, 0x2a12,
 0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a,
 0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41,
 0xedae, 0xfd8f, 0xcdec, 0xddcd, 0xad2a, 0xbd0b, 0x8d68, 0x9d49,
 0x7e97, 0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70,
 0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a, 0x9f59, 0x8f78,
 0x9188, 0x81a9, 0xb1ca, 0xa1eb, 0xd10c, 0xc12d, 0xf14e, 0xe16f,
 0x1080, 0x00a1, 0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067,
 0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e,
 0x02b1, 0x1290, 0x22f3, 0x32d2, 0x4235, 0x5214, 0x6277, 0x7256,
 0xb5ea, 0xa5cb, 0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d,
 0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
 0xa7db, 0xb7fa, 0x8799, 0x97b8, 0xe75f, 0xf77e, 0xc71d, 0xd73c,
 0x26d3, 0x36f2, 0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634,
 0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9, 0xb98a, 0xa9ab,
 0x5844, 0x4865, 0x7806, 0x6827, 0x18c0, 0x08e1, 0x3882, 0x28a3,
 0xcb7d, 0xdb5c, 0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a,
 0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0, 0x2ab3, 0x3a92,
 0xfd2e, 0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b, 0x9de8, 0x8dc9,
 0x7c26, 0x6c07, 0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1,
 0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8,
 0x6e17, 0x7e36, 0x4e55, 0x5e74, 0x2e93, 0x3eb2, 0x0ed1, 0x1ef0
};
_FeeData::_FeeData(char *data,const short &bytes,const short &feeID,const short &runMode,const short &trigger,const short &trgFlag,const char &pkgFlag,const bool &crc):Navigator(feeID,runMode,trigger,trgFlag,pkgFlag,crc){
  unsigned short crc_cal = 0xffff;
  for(short i=0;i<bytes;++i){
    crc_cal = (crc_cal<<8) ^ crc16_ccitt_tableH[((crc_cal>>8) ^ data[i]) & 0xff];
    Signal.push_back(data[i]);
  }
  unsigned short crc_in = (short)(unsigned char)Signal[Signal.size()-2]*256+(short)(unsigned char)Signal[Signal.size()-1];
  if(crc_cal != crc_in){
    Navigator.CRCFlag = false;
  }
std::cout<<"DEBUG: "<<__FILE__<<"("<<__LINE__<<")\tCRC_cal = "<<crc_cal<<" read = "<<crc_in<<"\tbytes = "<<bytes<<" size = "<<Signal.size()<<std::endl;
  Signal.erase(Signal.begin(),Signal.begin()+2);  // 2 bytes for data length
  Signal.erase(Signal.end()-2,Signal.end());      // 2 bytes for CRC
  Navigator.Trigger = (short)(unsigned char)Signal[Signal.size()-1];
  Navigator.TriggerFlag = (short)(unsigned char)Signal[Signal.size()-2]>>4;
  Signal.erase(Signal.end()-2,Signal.end());      // 2 bytes for trigger
std::cout<<"DEBUG: "<<__FILE__<<"("<<__LINE__<<")\tsize = "<<Signal.size()<<std::endl;
}

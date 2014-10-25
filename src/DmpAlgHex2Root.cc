/*
 *  $Id: DmpAlgHex2Root.cc, 2014-10-24 01:51:47 DAMPE $
 *  Author(s):
 *    Chi WANG (chiwang@mail.ustc.edu.cn) 27/05/2014
*/

#include <stdlib.h>     // getenv()
//#include "boost/lexical_cast.hpp"
#include "DmpDataBuffer.h"
#include "DmpAlgHex2Root.h"
#include "DmpCore.h"

//-------------------------------------------------------------------
DmpAlgHex2Root::DmpAlgHex2Root()
 :DmpVAlg("Rdc/Hex2Root/EQM"),fGoodRawEventID(0),fEvtHeader(0),fMetadata(0),
  fEvtBgo(0),fEvtPsd(0),fEvtNud(0),fEvtStk(0)
{
  fMetadata = new DmpMetadata();
  gDataBuffer->RegisterObject("Metadata/Rdc/JobOpt",fMetadata,"DmpMetadata");
  std::string prefix = (std::string)getenv("DMPSWSYS")+"/share/Connector/";
  fMetadata->SetOption("Bgo/Connector",prefix+"Bgo/EQM");
  fMetadata->SetOption("Psd/Connector",prefix+"Psd/EQM");
  fMetadata->SetOption("Stk/Connector",prefix+"Stk/EQM");
  //gRootIOSvc->Set("Output/Key","rdc");
}

//-------------------------------------------------------------------
DmpAlgHex2Root::~DmpAlgHex2Root(){
}

//-------------------------------------------------------------------
void DmpAlgHex2Root::Set(const std::string &type, const std::string &argv){
  fMetadata->SetOption(type,argv);
}

//-------------------------------------------------------------------
bool DmpAlgHex2Root::Initialize(){
  fFile.open(gRootIOSvc->GetInputFileName().c_str(),std::ios::in|std::ios::binary);
  if(not fFile.good()){
    DmpLogError<<"Open "<<gRootIOSvc->GetInputFileName()<<" failed"<<DmpLogEndl;
    return false;
  }else{
    std::string name = gRootIOSvc->GetOutputPath()+gRootIOSvc->GetInputStem()+".error";
    fOutError.open(name.c_str(),std::ios::out|std::ios::binary);
  }
  fEvtHeader = new DmpEvtHeader();
  gDataBuffer->RegisterObject("Event/Rdc/EventHeader",fEvtHeader,"DmpEvtHeader");
  bool psd = InitializePsd();
  bool stk = InitializeStk();
  bool bgo = InitializeBgo();
  bool nud = InitializeNud();
  return (bgo&&psd&&nud&&stk);
}

//-------------------------------------------------------------------
bool DmpAlgHex2Root::ProcessThisEvent(){
// *
// *  TODO: 
// *
  while(fEventInBuf.size() == 0){
    if(fFile.eof() || (fFile.tellg() == -1)){
      DmpLogInfo<<"Reach the end of "<<gRootIOSvc->GetInputFileName()<<DmpLogEndl;
      gCore->TerminateRun();
      return false;
    }
    ReadDataIntoDataBuffer();
  }
  long eventID = gCore->GetCurrentEventID();
  bool header = ProcessThisEventHeader(eventID);
  bool bgo = ProcessThisEventBgo(eventID);
  bool psd = ProcessThisEventPsd(eventID);
  bool nud = ProcessThisEventNud(eventID);
  bool stk = ProcessThisEventStk(eventID);
  EraseBuffer(eventID);
  return (header && bgo && psd && nud && stk);
}

//-------------------------------------------------------------------
bool DmpAlgHex2Root::Finalize(){
  fFile.close();
  fOutError.close();
  return true;
}

//-------------------------------------------------------------------
bool DmpAlgHex2Root::ProcessThisEventHeader(const long &id){
  if(fHeaderBuf.find(id) == fHeaderBuf.end()){
    return false;
  }
  fEvtHeader->Reset();
  int s=0;
  for(size_t i=0;i<4;++i){        // 4 bytes second
    s = s*256 + (short)(unsigned char)fHeaderBuf[id]->Time[i];
  }
  fEvtHeader->SetTime(s,((short)(unsigned char)fHeaderBuf[id]->Time[4]*256 + (short)(unsigned char)fHeaderBuf[id]->Time[5]));

  return ProcessThisEventTrigger(id);
}

//-------------------------------------------------------------------
bool DmpAlgHex2Root::ProcessThisEventTrigger(const long &id){
  if(fTriggerBuf.find(id) == fTriggerBuf.end()){
    return true;
  }
  // Check_sum
  static short nSize = fTriggerBuf[id]->Data.size();
  unsigned short read_sum = (((unsigned short)(unsigned char)fTriggerBuf[id]->Data[nSize-2])<<8) + (unsigned short)(unsigned char)fTriggerBuf[id]->Data[nSize-1];
  unsigned short cal_sum = 0;
  for(size_t i=0;i<nSize-2;++i){
    cal_sum += (unsigned short)(unsigned char)fTriggerBuf[id]->Data[i];
  }
  if(cal_sum != read_sum){
    DmpLogError<<"trigger system data sum_check error:  read/cal: "<<std::hex<<read_sum<<" / "<<cal_sum<<std::dec; PrintTime();
    fEvtHeader->SetTriggerSumCheckError();
  }
  // generated and enabled
  unsigned short choosing;
  choosing = ((unsigned short)(unsigned char)(fTriggerBuf[id]->Data[0]))<<8 + (fTriggerBuf[id]->Data[1] & 0x00c0) + (fTriggerBuf[id]->Data[1]&0x0003)<<4 + (fTriggerBuf[id]->Data[2]>>4 & 0x0008) + (fTriggerBuf[id]->Data[3]>>5 &0x0004);
  choosing =  (choosing>>2 & 0x3fff);
  fEvtHeader->SetTriggerChoose(choosing);
  unsigned char generate,enable;
  generate = (unsigned char)fTriggerBuf[id]->Data[2];
  fEvtHeader->SetTriggerGenerate(generate);
  enable = (unsigned char)fTriggerBuf[id]->Data[3];
  fEvtHeader->SetTriggerEnable(enable);
  // trigger ID
  short trig = (((short)(unsigned char)(fTriggerBuf[id]->Data[4]&0x000f))<<8) + (short)(unsigned char)fTriggerBuf[id]->Data[5];   // NOTE: only save 2^12
  fEvtHeader->SetTrigger(trig);
  // delta time
  float deltT  = ((((unsigned short)(unsigned char)(fTriggerBuf[id]->Data[6]))<<8) + (unsigned short)(unsigned char)(fTriggerBuf[id]->Data[7]))*0.0001;       // unit: millisecond
  fEvtHeader->SetDeltaTime(deltT);
  return true;
}

//-------------------------------------------------------------------
void DmpAlgHex2Root::PrintTime()const{
  DmpLogCout<<"\t\tTime:";
  for(size_t i=0;i<6;++i){
    DmpLogCout<<std::hex<<(short)(unsigned char)(--fHeaderBuf.end())->second->Time[i];
  }
  DmpLogCout<<std::dec<<DmpLogEndl;
}

//-------------------------------------------------------------------
void DmpAlgHex2Root::EraseBuffer(const long &id){
  if(fHeaderBuf.find(id) != fHeaderBuf.end()){
    delete fHeaderBuf[id];
    fHeaderBuf.erase(id);
  }
  if(fNudBuf.find(id) != fNudBuf.end()){
    delete fNudBuf[id];
    fNudBuf.erase(id);
  }
  if(fTriggerBuf.find(id) != fTriggerBuf.end()){
    delete fTriggerBuf[id];
    fTriggerBuf.erase(id);
  }
  if(fBgoBuf.find(id) != fBgoBuf.end()){
    for(size_t i=0;i<fBgoBuf[id].size();++i){
      delete fBgoBuf[id][i];
    }
    fBgoBuf.erase(id);
  }
  if(fPsdBuf.find(id) != fPsdBuf.end()){
    for(size_t i=0;i<fPsdBuf[id].size();++i){
      delete fPsdBuf[id][i];
    }
    fPsdBuf.erase(id);
  }
  if(fStkBuf.find(id) != fStkBuf.end()){
    for(size_t i=0;i<fStkBuf[id].size();++i){
      delete fStkBuf[id][i];
    }
    fStkBuf.erase(id);
  }
  for(size_t i=0;i<fEventInBuf.size();++i){
    if(fEventInBuf[i] == id){
      fEventInBuf.erase(fEventInBuf.begin()+i);
    }
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

_FeeData::_FeeData(char *data,const short &bytes){  // bytes: from the end of eb90 to the end of CRC
  unsigned short crc_cal = 0xffff;
  for(short i=0;i<bytes-2;++i){ // CRC check, from the end of eb90, to the beginning of CRC
    crc_cal = (crc_cal<<8) ^ crc16_ccitt_tableH[((crc_cal>>8) ^ (unsigned char)data[i]) & 0xff];
    Signal.push_back(data[i]);      // Signal not has CRC
  }
  unsigned short crc_read = (((unsigned short)(unsigned char)data[bytes-2])<<8) + (unsigned short)(unsigned char)data[bytes-1];
  if(not (Signal[0]&0x0080)){
    Navigator.SetTag(DmpFeeNavig::tag_FeePowerOff);
  }
  if(not (Signal[0]&0x0040)){
    Navigator.SetTag(DmpFeeNavig::tag_HThresholdError);
  }
  if(not (Signal[0]&0x0020)){
    Navigator.SetTag(DmpFeeNavig::tag_LThresholdError);
  }
  if(not (Signal[0]&0x0010)){
    Navigator.SetTag(DmpFeeNavig::tag_SettingTAWhileDataTaking);
  }
  if(not (Signal[0]&0x0008)){
    Navigator.SetTag(DmpFeeNavig::tag_AD976Error);
  }
  Navigator.SetRunModeFeeID((unsigned char)Signal[1]);  // before Signal[0], since waring need FeeID
  Signal.erase(Signal.begin(),Signal.begin()+4);    // NOTE: 1 byte pkgFlag, 1 byte runMode_feeID, 2 bytes for data length
  if(Navigator.GetDetectorID() == DmpEDetectorID::kStk){      // exclude Stk, it's not trigger for stk at here
// *
// *  TODO:  check sum
// *
    Signal.erase(Signal.end()-2,Signal.end());        // 2 bytes for sum check
  }
  if(Signal[Signal.size()-2] & 0x0020){
    Navigator.SetTag(DmpFeeNavig::tag_ReceivedTrigCheck_Error);
  }
  Navigator.SetTrigger((((short)(unsigned char)(Signal[Signal.size()-2]&0x000f))<<8) + (unsigned short)(unsigned char)Signal[Signal.size()-1]);
  Signal.erase(Signal.end()-2,Signal.end());        // 2 bytes for trigger
  if(crc_cal != crc_read){
    Navigator.SetTag(DmpFeeNavig::tag_CRCError);
  }
}



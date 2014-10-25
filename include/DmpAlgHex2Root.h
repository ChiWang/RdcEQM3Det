/*
 *  $Id: DmpAlgHex2Root.h, 2014-10-24 01:48:35 DAMPE $
 *  Author(s):
 *    Chi WANG (chiwang@mail.ustc.edu.cn) 27/05/2014
*/

#ifndef DmpAlgHex2Root_H
#define DmpAlgHex2Root_H

#include <fstream>
#include <map>

#include "DmpFeeNavig.h"
#include "DmpEvtPsdRaw.h"
#include "DmpEvtNudRaw.h"
#include "DmpEvtStkRaw.h"
#include "DmpEvtHeader.h"
#include "DmpMetadata.h"
#include "DmpVAlg.h"

//-------------------------------------------------------------------
struct _HeaderNavig{
  _HeaderNavig():DataLength(0xffff){
    for(size_t i=0;i<6;++i){
      Time[i] = 0x00;
    }
  }
  _HeaderNavig(const unsigned short &dl,char *t){
    DataLength = dl;
    for(size_t i=0;i<6;++i){
      Time[i] = t[i];
    }
  }
  unsigned short     DataLength;
  char      Time[6];
};

//-------------------------------------------------------------------
struct _TriggerData{
  _TriggerData(char *data,const short &bytes){for(size_t i=0;i<bytes;++i) Data.push_back(data[i]);}
  _TriggerData(const _TriggerData &r){Data = r.Data;}
  _TriggerData(){}
  ~_TriggerData(){}
  std::vector<char> Data;
};

//-------------------------------------------------------------------
struct _FeeData{
  _FeeData(char *data,const short &bytes);
  _FeeData(const _FeeData &r){Navigator = r.Navigator;  Signal = r.Signal;}
  _FeeData(){}
  ~_FeeData(){}
  DmpFeeNavig  Navigator;
  std::vector<char> Signal;
  /*
   * Signal:    for Bgo, Psd, Nud, Stk
   *        from the end of data length, to the beginning of trigger (not include trigger)
   *
   */
};

//-------------------------------------------------------------------
class DmpAlgHex2Root : public DmpVAlg{
/*
 *  DmpAlgHex2Root
 *
 */
public:
  DmpAlgHex2Root();
  ~DmpAlgHex2Root();

  void Set(const std::string&,const std::string&);
  bool Initialize();
  bool ProcessThisEvent();
  bool Finalize();

private:    // for all
  bool ReadDataIntoDataBuffer();    // read one e2250813
  /* 
   *    1. return true:
   *        1.1     read one 0xe2250813 into data buffer
   *        1.2     throw one 0xe2250813 into Exception()
   *    2. retrun false:
   *        reach the end of input file stream
   */
  bool CheckE2250813DataLength(const int &nBytes);
  bool CheckEb90DataLength(const int &nBytes);
  void Exception(const int &b,const std::string &e);     // throw whole data of e2250813 into fOutError
  void EraseBuffer(const long &id);

  long              fGoodRawEventID;    // good event id, find from raw data(one e2250813 or many e2250813)
  std::ifstream     fFile;          // in data stream
  std::ofstream     fOutError;      // save error datas into Error_fInDataName
  std::vector<long> fEventInBuf;    // Event ID: Event in Buffer
  std::map<long,_HeaderNavig*>      fHeaderBuf;
  std::map<long,_TriggerData*>      fTriggerBuf;
  std::map<long,std::vector<_FeeData*> >    fBgoBuf;
  std::map<long,std::vector<_FeeData*> >    fPsdBuf;
  std::map<long,std::vector<_FeeData*> >    fStkBuf;
  std::map<long,_FeeData*>      fNudBuf;

private:
  DmpMetadata       *fMetadata;     // metadata of simulation
  DmpEvtHeader      *fEvtHeader;    // save me
  bool ProcessThisEventHeader(const long &id);    // convert event header
  bool ProcessThisEventTrigger(const long &id);
  void PrintTime()const;

private:    // Bgo
  DmpEvtBgoRaw      *fEvtBgo;       // Bgo outdata
  std::map<short,short> fMapBgo;    // map of Bgo connector
  bool InitializeBgo();
  bool ProcessThisEventBgo(const long &id);

private:    // Psd
  DmpEvtPsdRaw      *fEvtPsd;       // Psd outdata
  std::map<short,short> fMapPsd;    // map of Psd connector
  bool InitializePsd();
  bool ProcessThisEventPsd(const long &id);
  //void CheckFeeFlag_Psd();

private:    // Nud
  DmpEvtNudRaw      *fEvtNud;       // Nud outdata
  bool InitializeNud();
  bool ProcessThisEventNud(const long &id);
  //void CheckFeeFlag_Nud();

private:    // Stk
  DmpEvtStkRaw      *fEvtStk;       // Stk outdata
  bool InitializeStk();
  bool ProcessThisEventStk(const long &id);
  //void CheckFeeFlag_Stk();
};


#endif



/*
 *  $Id: DmpEvtHeader.h, 2014-10-24 01:42:59 DAMPE $
 *  Author(s):
 *    Chi WANG (chiwang@mail.ustc.edu.cn) 13/12/2013
*/

#ifndef DmpEvtHeader_H
#define DmpEvtHeader_H

#include "TObject.h"
#include <bitset>

class DmpEvtHeader : public TObject{
/*
 * DmpEvtHeader
 *
*/
public:
  typedef std::bitset<32>   DmpTriggerStatus;
  /*
   *    fTriggerStatus
   *    bits:   31~16 (choosed type)
   *        bit:  31 == 0: not used
   *        bit:  30 == 1: data of trigger system error
   *        bits: 29~27     class-4
   *        bits: 26~22     class-3
   *        bits: 21,20     class-2
   *        bits: 19,18     class-1
   *        bits: 17,16     class-0
   *    bits:   15~8 (genrated signal)
   *        bit 15 == 0 not used
   *        bit 14  inject an external signal into trigger system
   *        bit 13  period
   *        bit 12  class-4
   *        bit 11  class-3
   *        bit 10  class-2
   *        bit 9   class-1
   *        bit 8   class-0
   *    bits:   7~0 (enable output of generated signal)
   *        bit 7 == 0 not used
   *        bit 6   external
   *        bit 5   period
   *        bit 4   class-4
   *        bit 3   class-3
   *        bit 2   class-2
   *        bit 1   class-1
   *        bit 0   class-0
   */
  typedef std::bitset<8>    DmpSubDetStatus;
  /*
   *  bit 7 == 0:   not used
   *  bit 6 == 1:   fake data
   *  bit 5 == 1:   CRC error
   *  bit 4 == 1:   package flag error
   *  bit 3 == 1:   trigger flag error
   *  bit 2 == 1:   sum check error // noly for Stk
   *  bit 1 == 1:   trigger is not the same as trigger system
   *  bit 0 == 1:   sub-det's triggers(from fee) are not the same
   */
  enum TagType
  {
    tag_FakeData = 6,
    tag_CRCError = 5,
    tag_PkgFlagError = 4,
    tag_TrigFlagError = 3,
    tag_SumCheckError = 2,
    tag_TrigNotMatchTrigSys = 1,
    tag_TrigNotMatchInSubDet = 0,
  };

public:
  DmpEvtHeader();
  ~DmpEvtHeader();
  DmpEvtHeader &operator=(const DmpEvtHeader &r);
  void Reset();
  void LoadFrom(DmpEvtHeader *r);

public: // Get functions
  bool IsGoodEvent(const short &id=99)const;
  short GetTrigger()const{return fTrigger;} // ONLY trigger of trigger system
  bool IsFakeData(const short &id)const;
  bool TriggerDataCheckRight()const{return (not fTriggerStatus[30]);}
  bool TriggersMatch(const short &id=99)const;  // if the argument is sub-det id, just check triggers of all fee of this sub-det
  bool TriggerValid()const {return (TriggerDataCheckRight()&&TriggersMatch());}
  short ChoosedTriggerType(const short &class_id)const; // class_id = 4~0
  bool InjectedExternalTrigger()const{return fTriggerStatus[14];}
  bool GeneratedPeriodTrigger()const{return fTriggerStatus[13];}
  bool GeneratedTrigger(const short &class_id)const{return fTriggerStatus[8+class_id];}
  bool EnabledExternalTrigger()const{return fTriggerStatus[6];}
  bool EnabledPeriodTrigger()const{return fTriggerStatus[5];}
  bool EnabledTrigger(const short &class_id)const{return fTriggerStatus[class_id];}

public: // Set functions
  void SetTriggerSumCheckError(){fTriggerStatus.set(30);}
  void SetTriggerChoose(const unsigned short &choosing);
  void SetTriggerGenerate(const unsigned char &g);
  void SetTriggerEnable(const unsigned char &e);
  void SetTrigger(const short &v){fTrigger = v;}
  void SetDeltaTime(const float &v);
  void SetTime(const int &s,const short &ms){fSecond = s;fMillisecond = ms;}
  void SetTag(const short &detectorID,TagType tagType);

private:
  short     fTrigger;       //bits: 15~12, not used. bits: 0~11, trigger ID(sub-detector has 2^12)
  float     fDeltaTime;     // delta time of 2 triggers (combine delta time from trigger system, and fTmpDeltaTime), unit: millisecond
  int       fSecond;        // time second
  short     fMillisecond;   // time millisecond
  DmpTriggerStatus  fTriggerStatus;     // record all trigger information
  DmpSubDetStatus   fPsdStatus;
  DmpSubDetStatus   fStkStatus;
  DmpSubDetStatus   fBgoStatus;
  DmpSubDetStatus   fNudStatus;
  int       fTmpDeltaTime;    //! update it by fSecond and fMillisecond. NOt save me

  ClassDef(DmpEvtHeader,1)
};

#endif


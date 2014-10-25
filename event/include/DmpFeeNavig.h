/*
 *  $Id: DmpFeeNavig.h, 2014-10-24 03:28:29 DAMPE $
 *  Author(s):
 *    Chi WANG (chiwang@mail.ustc.edu.cn) 07/08/2014
*/

#ifndef DmpFeeNavig_H
#define DmpFeeNavig_H

#include <bitset>

#include "TObject.h"
#include "DmpEFeeFlags.h"
#include "DmpEDetectorID.h"

//-------------------------------------------------------------------
class DmpFeeNavig : public TObject{
/*
 *  navigator of Fee, no ADC counts in this class
 *
 */
public:
typedef std::bitset<8> DmpFeeStatus;
/*
 *  bit 7 == 1      CRC error
 *  bit 6 == 1      Sum check error // only for Stk
 *  // bit 5~0:
 *    for Stk: fragment counts
 *    for the other sub-det: package flag
 *  bit 5 == 1      received a trigger check signal from trigger system, but checking wrong
 *  bit 4 == 1      Fee power off while data taking
 *  bit 3 == 1      setting error of high compress threshold    // only for Psd
 *  bit 2 == 1      setting error of low compress threshold     // only for Psd
 *  bit 1 == 1      setting TA threshold while the current data taking // only for Bgo
 *  bit 0 == 1      AD976 error
 */
enum TagType
{
  tag_CRCError = 7,
  tag_SumCheckError = 6,
  tag_ReceivedTrigCheck_Error = 5,
  tag_FeePowerOff = 4,
  tag_HThresholdError = 3,
  tag_LThresholdError = 2,
  tag_SettingTAWhileDataTaking = 1,
  tag_AD976Error = 0,
};
typedef std::bitset<8> DmpFeeModeID;        // fee run mode and id
/*
 *  bits 7,6:  run mode
 *       0 0    original
 *       0 1    compress
 *       1 0    CalDAC
 *       1 1    not used
 *  bits 5,4:  detector ID
 *       0 0    Stk
 *       0 1    Bgo
 *       1 0    Psd
 *       1 1    Nud
 *  bits 3~0:  fee id of this detector
 *      3,2:  side: +X: 00; -X: 01; +Y: 10; -Y: 11
 *      1,0:  count of fee, from top to bottom
 */

public:
  DmpFeeNavig():Trigger(-1){RunMode_ID.reset();Status.reset();}
  DmpFeeNavig(const DmpFeeNavig &r){
    RunMode_ID = r.RunMode_ID;
    Trigger = r.Trigger;
    Status = r.Status;
  }
  DmpFeeNavig& operator=(const DmpFeeNavig &r){
    RunMode_ID = r.RunMode_ID;
    Trigger = r.Trigger;
    Status = r.Status;
  }
  DmpFeeNavig(DmpFeeModeID rid,const short &trigger,DmpFeeStatus s):RunMode_ID(rid),Trigger(trigger),Status(s){}

public: // Get functions
  DmpERunMode::Type     GetRunMode()const{return (DmpERunMode::Type)(RunMode_ID.to_ulong()>>6);}
  DmpEDetectorID::Type  GetDetectorID()const{return (DmpEDetectorID::Type)(RunMode_ID.to_ulong()>>4 & 0x03);}
  short GetFeeID()const{return (RunMode_ID.to_ulong() & 0x3f);}
  short GetTrigger()const{return Trigger;}
  bool ValidFeeData()const{
    bool v = true;
    if(DmpEDetectorID::kStk == GetDetectorID()){
      if(Status[7] || Status[6]){
        v = false;
      }
    }else{
      if(Status.count()){
        v = false;
      }
    }
    return v;
  }
  bool CRCError()const{return Status[7];}
  bool SumCheckError()const{return Status[6];}
  short GetFragmentCount()const{return (Status.to_ulong() & 0x003f);}    // only for stk
  bool ReceivedTriggerCheck_Wrong()const{return Status[5];}
  bool PowerOffWhileDataTaking()const{return Status[4];}
  bool HighThresholdRAMError()const{return Status[3];}
  bool LowThresholdRAMError()const{return Status[2];}
  bool SettigTAWhileDataTaking()const{return Status[1];}
  bool AD976Error()const{return Status[0];}
  bool PackageFlagError()const{return (PowerOffWhileDataTaking()||HighThresholdRAMError()||LowThresholdRAMError()||SettigTAWhileDataTaking()||AD976Error());}
  short GetPackageFlag()const{
    return (Status.to_ulong() & 0x001f);
  }

public: // Set functions
  void SetTag(TagType tagType){Status.set(tagType);}
  void SetRunModeFeeID(const unsigned char &v){RunMode_ID = v;}
  void SetTrigger(const short &t){Trigger = t;}

private:
  DmpFeeModeID  RunMode_ID;
  short         Trigger;    // 2^12
  DmpFeeStatus  Status;

  ClassDef(DmpFeeNavig,1)
};

#endif


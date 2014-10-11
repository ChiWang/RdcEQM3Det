/*
 *  $Id: DmpFeeNavig.h, 2014-08-10 20:57:03 DAMPE $
 *  Author(s):
 *    Chi WANG (chiwang@mail.ustc.edu.cn) 07/08/2014
*/

#ifndef DmpFeeNavig_H
#define DmpFeeNavig_H

#include "TObject.h"
#include "DmpEFeeFlags.h"

//-------------------------------------------------------------------
class DmpFeeNavig : public TObject{
/*
 *  navigator of Fee, no ADC counts in this class
 *
 */
public:
  DmpFeeNavig():FeeID(-1),RunMode(DmpERunMode::kUnknow),Trigger(-1),PackageFlag(0x00ff),TriggerFlag(-1),CRCFlag(true){}
  DmpFeeNavig(const DmpFeeNavig &r){
    FeeID = r.FeeID;
    RunMode = r.RunMode;
    Trigger = r.Trigger;
    TriggerFlag = r.TriggerFlag;
    PackageFlag = r.PackageFlag;
    CRCFlag = r.CRCFlag;
  }
  DmpFeeNavig& operator=(const DmpFeeNavig &r){
    FeeID = r.FeeID;
    RunMode = r.RunMode;
    Trigger = r.Trigger;
    TriggerFlag = r.TriggerFlag;
    PackageFlag = r.PackageFlag;
    CRCFlag = r.CRCFlag;
  }
  DmpFeeNavig(const short &feeID,const DmpERunMode::Type &runMode,const short &trigger,const short &trgFlag,const short &pkgFlag,const bool &crc):FeeID(feeID),RunMode(runMode),Trigger(trigger),TriggerFlag(trgFlag),PackageFlag(pkgFlag),CRCFlag(crc){}

  short FeeID;
  DmpERunMode::Type RunMode;
  short Trigger;
  short TriggerFlag;
  short PackageFlag;
  bool  CRCFlag;

  ClassDef(DmpFeeNavig,1)
};

#endif


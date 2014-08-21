/*
 *  $Id: DmpEvtRawPsd.h, 2014-08-20 14:19:01 DAMPE $
 *  Author(s):
 *    Chi WANG (chiwang@mail.ustc.edu.cn) 24/04/2014
*/

#ifndef DmpEvtRawPsd_H
#define DmpEvtRawPsd_H

#include <vector>
#include "DmpFeeNavig.h"

//-------------------------------------------------------------------
class DmpEvtRawPsd : public TObject{
/*
 *  DmpEvtRawPsd
 *
 *      this class is used to save output of Raw for Psd
 *
 *   one object mean one Psd event
 *
 */
public:
  DmpEvtRawPsd();
  ~DmpEvtRawPsd();
  void  Reset();
  void  SetFeeNavigator(const DmpFeeNavig &r){fFeeNavig = r;}
  void  AppendSignal(const short &gid,const short &v);
  void  GenerateStatus();

  bool  IsGoodEvent()const{return (fFeeNavig.CRCFlag && fFeeNavig.TriggerFlag && fFeeNavig.PackageFlag);}
  short GetTrigger()const{return fFeeNavig.Trigger;}
  short GetRunMode()const{return fFeeNavig.RunMode;}
  DmpFeeNavig GetFeeNavigator()const{return fFeeNavig;}
  short GetSignal(const short &gid)const;

private:
  DmpFeeNavig   fFeeNavig;
  std::vector<short>    fGlobalID;
  /*
   *    gid for bgo signal: short: bit 15~0
   *
   *    layer(0~1):     bits 14,13,12
   *        = (fGlobalID >> 12) & 0x0007
   *    strip(0~41):    bits 11,10,9,8,7,6
   *         = (fGlobalID >> 6) & 0x003f
   *    side(0,1):      bits 4
   *        = (fGlobal >> 4) & 0x0001
   *    dynode(5,8):  bits 3,2,1,0
   *        = fGlobal & 0x000f
   */
  std::vector<short>    fADC;

  ClassDef(DmpEvtRawPsd,1)
};

#endif



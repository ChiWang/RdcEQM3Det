/*
 *  $Id: DmpEvtBgoRaw.h, 2014-08-22 17:45:40 DAMPE $
 *  Author(s):
 *    Chi WANG (chiwang@mail.ustc.edu.cn) 24/04/2014
*/

#ifndef DmpEvtBgoRaw_H
#define DmpEvtBgoRaw_H

#include <vector>
#include "DmpFeeNavig.h"

//-------------------------------------------------------------------
class DmpEvtBgoRaw : public TObject{
/*
 *  DmpEvtBgoRaw
 *
 *      this class is used to save output of Raw for Bgo
 *
 *   one object mean one Bgo event
 *
 */
public:
  DmpEvtBgoRaw();
  ~DmpEvtBgoRaw();
  void  Reset();
  void  SetFeeNavigator(const DmpFeeNavig &s){fFeeNavig.push_back(s);}
  void  AppendSignal(const short &gid,const short &v);
  void  GenerateStatus();

public:
  short fTrigger;
  short fRunMode;
  bool  fIsGood;
  std::vector<DmpFeeNavig>  fFeeNavig;
  std::vector<short>    fGlobalDynodeID;
  /*
   *    short: bit 15~0
   *
   *    layer(0~13):    bits 14,13,12,11
   *        = (fGlobalDynodeID >> 11) & 0x000f
   *    bar(0~23):      bits 10,9,8,7,6
   *         = (fGlobalDynodeID >> 6) & 0x001f
   *    side(0,1):      bits 4
   *        = (fGlobal >> 4) & 0x0001
   *    dynode(2,5,8):  bits 3,2,1,0
   *        = fGlobal & 0x000f
   */
  std::vector<double>    fADC;

  ClassDef(DmpEvtBgoRaw,1)
};

#endif


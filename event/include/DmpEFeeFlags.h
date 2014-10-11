/*
 *  $Id: DmpEFeeFlags.h, 2014-08-10 22:10:53 DAMPE $
 *  Author(s):
 *    Chi WANG (chiwang@mail.ustc.edu.cn) 10/08/2014
*/

#ifndef DmpEFeeFlags_H
#define DmpEFeeFlags_H

//-------------------------------------------------------------------
namespace DmpEPackageFlag{
  enum Type{
    kPowerOk = 0x0080,        // bit 7 = 1, power not resetted. VA locked then power reset
    kHThresholdOk = 0x0040,   // bit 6 = 1, high threshold ok
    kLThresholdOk = 0x0020,   // bit 5 = 1, low threshold ok
    kTAOk = 0x0010,           // bit 4 = 1, TA ok
    kADC976Ok = 0x0008,       // bit 3 = 1, ADC 976 ok
    kGood = 0x00ff            // bit[0~2] = 111. reserved
  };
}

//-------------------------------------------------------------------
namespace DmpERunMode{
  enum Type{
    kUnknow = -1,
    kOriginal = 0,
    kCompress = 1,
    kCalDAC = 2,
    kMixed = 3  // this is for whole event. one event cantains many Fee, each Fee has a run mode, they may mixed
  };
}

//-------------------------------------------------------------------
namespace DmpETriggerFlag{
  enum Type{
    kNoReceiveTriggerCheck = 0,       // did not receive check signal
    kCheckRight = 1,    // received check signal, and checked right
    kCheckWrong = 2,    // received check signal, and checked, but wrong
    kUnknow = 3         // unknow
  };
}

#endif


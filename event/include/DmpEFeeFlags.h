/*
 *  $Id: DmpEFeeFlags.h, 2014-10-24 02:48:05 DAMPE $
 *  Author(s):
 *    Chi WANG (chiwang@mail.ustc.edu.cn) 10/08/2014
*/

#ifndef DmpEFeeFlags_H
#define DmpEFeeFlags_H

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


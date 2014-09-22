/*
 *  $Id: DmpBindingHex2Root.cc, 2014-08-31 22:47:32 DAMPE $
 *  Author(s):
 *    Chi WANG (chiwang@mail.ustc.edu.cn) 07/03/2014
*/

#include <boost/python.hpp>

#include "DmpAlgHex2Root.h"


BOOST_PYTHON_MODULE(libDmpHex2Root){
  using namespace boost::python;

  // default algorithm
  class_<DmpAlgHex2Root,boost::noncopyable,bases<DmpVAlg> >("DmpAlgHex2Root",init<>());
}



#ifndef DR_URQMD_H
#define DR_URQMD_H

#include "DataReaderEvent.h"
#include "DRBase.h"
#include "ListOfPDG.h"

class DRurqmd : public DRBase
{
public:
  DRurqmd(){};
  ~DRurqmd(){};

  DataReaderEvent* ReadURQMDEvent();

private:
  static const int skipLinesHeader = 12;
  static const int skipLinesEvent = 3;
};

#endif
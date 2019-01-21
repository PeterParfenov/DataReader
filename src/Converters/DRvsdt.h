// #ifdef UNIGEN_INSTALLED
#ifndef DR_VSDT_H
#define DR_VSDT_H

#include "vsdt.h"

#include "DataReaderEvent.h"
#include "DRBase.h"

class DRvsdt : virtual DRBase
{
public:
  DRvsdt(){};
  virtual ~DRvsdt(){};

  virtual DataReaderEvent* ReadVSDTEvent(VSDTEvent *const &vsdtEvent, TTree *const &tree, Long_t iEvent);

  ClassDef(DRvsdt,0);
};

#endif
// #endif

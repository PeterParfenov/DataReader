// #ifdef UNIGEN_INSTALLED
#ifndef DR_UNIGEN_H
#define DR_UNIGEN_H

#include "UEvent.h"
#include "UParticle.h"

#include "DataReaderEvent.h"
#include "DRBase.h"
#include "ListOfPDG.h"

class DRunigen : virtual DRBase
{
public:
  DRunigen(){};
  virtual ~DRunigen(){};

  virtual DataReaderEvent* ReadUNIGENEvent(UEvent *uEvent, TTree* tree, Long_t iEvent);
  static const int TimeStep = 1;

  ClassDef(DRunigen,0);
};

#endif
// #endif
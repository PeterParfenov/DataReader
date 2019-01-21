#include "DRBase.h"

std::string DRBase::GetLine()
{
  std::string str;
  char _buffer[256];
  if (fFileType.isASCII && fFileType.isGZ)
  {
    gzgets(iFile.GZ, _buffer, 256);
    str = (std::string)_buffer;
  }
  if (fFileType.isASCII && !fFileType.isGZ)
  {
    getline(iFile.ASCII, str);
  }
  return str;
}

Bool_t DRBase::OpenInputFile(TString _name)
{
  if (fFileType.isASCII && fFileType.isROOT)
  {
    std::cerr << "DRBase::OpenInputFile: Incopetent file type." << std::endl;
    return false;
  }
  if (fFileType.isASCII)
  {
    if (fFileType.isGZ)
    {
      std::cout << "DRBase::OpenInputFile: Input file type: GZIPPED ASCII" << std::endl;
      iFile.GZ = gzopen(_name.Data(), "rb");
      if (!iFile.GZ)
      {
        std::cerr << "DRBase::OpenInputFile: Attached file " << _name.Data() << " was not opened." << std::endl;
        return false;
      }
    }
    if (!fFileType.isGZ)
    {
      std::cout << "DRBase::OpenInputFile: Input file type: ASCII" << std::endl;
      iFile.ASCII.open(_name.Data());
      if (!iFile.ASCII.is_open())
      {
        std::cerr << "DRBase::OpenInputFile: Attached file " << _name.Data() << " was not opened." << std::endl;
        return false;
      }
    }
  }
  if (fFileType.isROOT)
  {
    std::cout << "DRBase::OpenInputFile: Input file type: ROOT" << std::endl;
    iFile.ROOT = new TFile(_name.Data(), "read");
    if (iFile.ROOT->IsZombie())
    {
      std::cerr << "DRBase::OpenInputFile: Attached file " << _name.Data() << " was not opened." << std::endl;
      return false;
    }
  }
  return true;
}

Bool_t DRBase::InitInputFile(TString _name)
{
  std::cout << "DRBase::InitInputFile: Processing." << std::endl;
  if (_name.Contains(".f14"))
  {
    fFileType.isASCII = true;
    fModelType.isURQMD = true;
  }
  if (_name.Contains(".gz"))
  {
    fFileType.isGZ = true;
  }
  if ((_name.Contains("phsd") || _name.Contains("PHSD")) && (!_name.Contains(".root")))
  {
    fFileType.isASCII = true;
    fModelType.isPHSD = true;
  }
  if (_name.Contains(".r12"))
  {
    fFileType.isASCII = true;
    fModelType.isLAQGSM = true;
  }
  if (_name.Contains(".root"))
  {
    fFileType.isROOT = true;
    if (_name.Contains("PHQMD") || _name.Contains("phqmd"))
    {
      fModelType.isPHQMD = true;
    }
    if (_name.Contains("vsdt") || _name.Contains("VSDT"))
    {
      fModelType.isVSDT = true;
    }
    if (_name.Contains("dcmqgsm") || _name.Contains("DCMQGSM"))
    {
      fModelType.isDCMQGSM = true;
    }
  }

  if (fModelType.isURQMD)
  {
    std::cout << "DRBase::InitInputFile: Input model type: UrQMD" << std::endl;
  }
  if (fModelType.isLAQGSM)
  {
    std::cout << "DRBase::InitInputFile: Input model type: LAQGSM" << std::endl;
  }
  if (fModelType.isPHSD)
  {
    std::cout << "DRBase::InitInputFile: Input model type: PHSD" << std::endl;
  }
  if (fModelType.isPHQMD)
  {
    std::cout << "DRBase::InitInputFile: Input model type: PHQMD" << std::endl;
  }
  if (fModelType.isDCMQGSM)
  {
    std::cout << "DRBase::InitInputFile: Input model type: DCMQGSM" << std::endl;
  }
  if (fModelType.isVSDT)
  {
    std::cout << "DRBase::InitInputFile: input model type: VSDT" << std::endl;
  }
  Bool_t result = OpenInputFile(_name);
  return result;
}

Bool_t DRBase::eof()
{
  if (fFileType.isASCII && fFileType.isGZ)
  {
    return (gzeof(iFile.GZ));
  }
  if (fFileType.isASCII && !fFileType.isGZ)
  {
    return (iFile.ASCII.eof());
  }
  return true;
}

void DRBase::SkipLine(Int_t nLines)
{
  // Skip lines
  for (Int_t j = 0; j < nLines - 1; j++)
  {
    GetLine();
  }
}

ClassImp(DRBase);

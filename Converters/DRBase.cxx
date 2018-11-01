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
    std::cerr << "DataReader::OpenInputFile: Incopetent file type." << std::endl;
    return false;
  }
  if (fFileType.isASCII)
  {
    if (fFileType.isGZ)
    {
      std::cout << "DataReader::OpenInputFile: Input file type: GZIPPED ASCII" << std::endl;
      iFile.GZ = gzopen(_name.Data(), "rb");
      if (!iFile.GZ)
      {
        std::cerr << "DataReader::OpenInputFile: Attached file " << _name.Data() << " was not opened." << std::endl;
        return false;
      }
    }
    if (!fFileType.isGZ)
    {
      std::cout << "DataReader::OpenInputFile: Input file type: ASCII" << std::endl;
      iFile.ASCII.open(_name.Data());
      if (!iFile.ASCII.is_open())
      {
        std::cerr << "DataReader::OpenInputFile: Attached file " << _name.Data() << " was not opened." << std::endl;
        return false;
      }
    }
  }
  if (fFileType.isROOT)
  {
    std::cout << "DataReader::OpenInputFile: Input file type: ROOT" << std::endl;
    iFile.ROOT = new TFile(_name.Data(), "read");
    if (iFile.ROOT->IsZombie())
    {
      std::cerr << "DataReader::OpenInputFile: Attached file " << _name.Data() << " was not opened." << std::endl;
      return false;
    }
  }
  return true;
}

Bool_t DRBase::InitInputFile(TString _name)
{
  std::cout << "DataReader::InitInputFile: Processing." << std::endl;
  if (_name.Contains(".f14"))
  {
    fFileType.isASCII = true;
    fModelType.isURQMD = true;
  }
  if (_name.Contains(".gz"))
  {
    fFileType.isGZ = true;
  }
  if (_name.Contains("phsd") || _name.Contains("PHSD"))
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
    if (_name.Contains("UNIGEN") || _name.Contains("PHQMD") || _name.Contains("unigen") || _name.Contains("phqmd"))
    {
      fModelType.isPHQMD = true;
    }
    if (_name.Contains("dcmqgsm") || _name.Contains("DCMQGSM"))
    {
      fModelType.isDCMQGSM = true;
    }
  }

  if (fModelType.isURQMD)
  {
    std::cout << "DataReader::InitInputFile: Input model type: UrQMD" << std::endl;
  }
  if (fModelType.isLAQGSM)
  {
    std::cout << "DataReader::InitInputFile: Input model type: LAQGSM" << std::endl;
  }
  if (fModelType.isPHSD)
  {
    std::cout << "DataReader::InitInputFile: Input model type: PHSD" << std::endl;
  }
  if (fModelType.isPHQMD)
  {
    std::cout << "DataReader::InitInputFile: Input model type: PHQMD" << std::endl;
  }
  if (fModelType.isDCMQGSM)
  {
    std::cout << "DataReader::InitInputFile: Input model type: DCMQGSM" << std::endl;
  }
  Bool_t result = OpenInputFile(_name);
  return result;
}

ClassImp(DRBase);
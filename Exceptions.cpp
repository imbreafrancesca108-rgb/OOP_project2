#include "Exceptions.h"

AmountException::AmountException (const std::string& message) :
    std::runtime_error(message) {}

OpenPath::OpenPath (const std::string& path):
    std::runtime_error("Path " +path+ " not found !"){}

SavePath::SavePath (const std::string& path)
    :std::runtime_error("Could not save to path "+path+" !"){}

CropException::CropException(const std::string& message):
    std::runtime_error(message){}

ResizeException::ResizeException(const std::string& message):
    std::runtime_error(message){}

OptionException::OptionException (const std::string& o)
    :std::runtime_error("Option "+o+" is not available !"){}

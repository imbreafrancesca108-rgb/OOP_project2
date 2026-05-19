#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H
#include<string>
#include <stdexcept>

class AmountException : public std::runtime_error {
public:
AmountException(const std::string& message="Invalid amount!") ;
};

class OpenPath: public std::runtime_error{
public:
    OpenPath(const std::string& path);
    };

class SavePath:public std::runtime_error{
public:
    SavePath(const std::string& path);
    };

class CropException:public std::runtime_error{
public:
    CropException(const std::string& message="Crop dimensions are larger than the original ones!");
    };
class ResizeException:public std::runtime_error{
public:
    ResizeException(const std::string& message="Resizing can't be done. Percentage is invalid!");
    };
class OptionException:public std::runtime_error{
public:
    OptionException(const std::string& o);};
#endif

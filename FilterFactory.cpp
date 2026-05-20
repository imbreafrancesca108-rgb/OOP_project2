#include "FilterFactory.h"
#include "Exceptions.h"
std::unique_ptr<ImageEditor> FilterFactory::create(const std::string& filter){
    if(filter=="GoldenHour") return std::make_unique<GoldenHour>();
    if(filter=="BlackAndWhite") return std::make_unique<BlackAndWhite>();
    if(filter=="OrangeAndTeal") return std::make_unique<OrangeAndTeal>();
    if(filter=="MoodyBlues") return std::make_unique<MoodyBlues>();
    if(filter=="Pastels") return std::make_unique<Pastels>();
    if(filter=="Retro") return std::make_unique<Retro>();

    if(filter=="Vintage") return std::make_unique<Vintage>();
    if(filter=="Winter") return std::make_unique<Winter>();
    if(filter=="Summer") return std::make_unique<Summer>();
    if(filter=="Spring") return std::make_unique<Spring>();
    if(filter=="Autumn") return std::make_unique<Autumn>();
    if(filter=="Cinematic") return std::make_unique<Cinematic>();

    throw OptionException(filter);
}

std::unique_ptr<ImageEditor> FilterFactory::create(const std::string& filter, double amount ){
    if(filter=="Resize" ){
    if(amount<=0) throw ResizeException();
    return std::make_unique<Resize>(amount);
    }
    if(amount<=0||amount>20) throw AmountException();

    if(filter=="Grain") return std::make_unique<Grain>(amount);
    if(filter=="Blur") return std::make_unique<Blur>(amount);
    if(filter=="Contrast") return std::make_unique<Contrast>(amount);

    throw OptionException(filter);
}

std::unique_ptr<ImageEditor> FilterFactory::create(const std::string& filter, int newWidth, int newHeight )
{
    if(filter=="Crop")
    {
        if(newWidth<=0||newHeight<=0) throw CropException();
        return std::make_unique<Crop>(newWidth,newHeight);
    }
    throw OptionException(filter);
}

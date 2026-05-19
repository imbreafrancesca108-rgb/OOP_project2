#ifndef IMAGEEDITOR_H
#define IMAGEEDITOR_H
#include <string>
#include "Image.h"

///abstract base class for all filters
class ImageEditor{
public:
    ImageEditor();
    virtual ~ImageEditor();  ///virtual destructor to ensure a clean-up for all derived classes

    ///virtual function: each derived class implements its own version
    virtual void applySettings(Image& Img)=0;
    virtual std::string getFilterName() const=0;
   };

class ColorFilters: virtual public ImageEditor{
public:
    ColorFilters() ;
    virtual ~ColorFilters();
    };

class DetailFilters: virtual public ImageEditor{
public:
    DetailFilters();
    virtual ~DetailFilters();
    };

class SizeFilters: virtual public ImageEditor{
public:
    SizeFilters() ;
    virtual ~SizeFilters();
    };
#endif


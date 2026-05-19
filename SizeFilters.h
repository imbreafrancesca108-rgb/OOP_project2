#ifndef SIZEFILTERS_H
#define SIZEFILTERS_H
#include <string>
#include "ImageEditor.h"

///SizeFilters: base class for 2 size filters
class Crop: public SizeFilters{
    int newWidth;
    int newHeight;
    public:
        Crop(int nw, int nh) ;
        ~Crop();
        void applySettings(Image& img) override;
        virtual std::string getFilterName() const override;

};

class Resize: public SizeFilters{
    double percentage;
    public:
        Resize(double p);
        ~Resize();
        void applySettings(Image& img) override;
        virtual std::string getFilterName() const override;
};
#endif

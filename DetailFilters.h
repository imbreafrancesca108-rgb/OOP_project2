#ifndef DETAILFILTERS_H
#define DETAILFILTERS_H
#include <iostream>
#include "ImageEditor.h"

///DetailFilters: base class for 3 detail filters

class Grain: public DetailFilters{
    protected:
    double amountGrain;
    public:
        Grain(double a); ///constructor
        ~Grain(); ///destructor
        void applySettings(Image& img) override;
        virtual std::string getFilterName() const override; ///polymorphism
};

class Blur: public DetailFilters{
    protected:
    double amountBlur;
    public:
        Blur(double a);
        ~Blur();
        void applySettings(Image& img) override;
        virtual std::string getFilterName() const override;
};

class Contrast: public DetailFilters{
    protected:
    double amountContrast;
    public:
        Contrast(double a) ;
        ~Contrast();
        void applySettings(Image& img) override;
        virtual std::string getFilterName() const override;
};
#endif

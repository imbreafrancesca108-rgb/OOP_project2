#ifndef COLORFILTERS_H
#define COLORFILTERS_H
#include <iostream>
#include "ImageEditor.h"

///ColorFilters: base class for 6 color filters

class BlackAndWhite: public ColorFilters{
public:
    BlackAndWhite();
    ~BlackAndWhite();
    void applySettings(Image& img) override ;
    virtual std::string getFilterName() const override;
};

class GoldenHour: public ColorFilters{
public:
    GoldenHour();
    ~GoldenHour() ;
    void applySettings(Image& img) override ;
    virtual std::string getFilterName() const override;
};

class Retro: public ColorFilters{
public:
    Retro();
    ~Retro() ;
    void applySettings(Image& img) override ;
    virtual std::string getFilterName() const override;
};

class OrangeAndTeal: public ColorFilters{
public:
    OrangeAndTeal();
    ~OrangeAndTeal() ;
    void applySettings(Image& img) override ;
    virtual std::string getFilterName() const override;
};

class Pastels: public ColorFilters{
public:
    Pastels();
    ~Pastels() ;
    void applySettings(Image& img) override ;
    virtual std::string getFilterName() const override;
};

class MoodyBlues:  public ColorFilters{
public:
    MoodyBlues();
    ~MoodyBlues();
    void applySettings(Image& img) override ;
    virtual std::string getFilterName() const override;
};
#endif

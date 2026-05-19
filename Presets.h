#ifndef PRESETS_H
#define PRESETS_H
#include <iostream>
#include "ColorFilters.h"
#include "SizeFilters.h"
#include "DetailFilters.h"

class Vintage: public DetailFilters, public ColorFilters{
public:
    Vintage() ;
    ~Vintage();
    void applySettings(Image& img) override;
    std::string getFilterName() const override;
};

class Cinematic: public DetailFilters, public ColorFilters{
public:
    Cinematic() ;
    ~Cinematic();
    void applySettings(Image& img) override;
    std::string getFilterName() const override;
};

class Summer: public DetailFilters, public ColorFilters{
public:
    Summer() ;
    ~Summer();
    void applySettings(Image& img) override;
    std::string getFilterName() const override;
};

class Spring: public DetailFilters, public ColorFilters{
public:
    Spring() ;
    ~Spring();
    void applySettings(Image& img) override;
    std::string getFilterName() const override;
};

class Winter: public DetailFilters, public ColorFilters{
public:
    Winter() ;
    ~Winter();
    void applySettings(Image& img) override;
    std::string getFilterName() const override;
};

class Autumn: public DetailFilters, public ColorFilters{
public:
    Autumn() ;
    ~Autumn();
    void applySettings(Image& img) override;
    std::string getFilterName() const override;
};




#endif


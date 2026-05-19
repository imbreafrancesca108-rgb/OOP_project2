#include "Presets.h"
#include "Image.h"
///all presets use virtual inheritance to inherit from both ColorFilters and DetailFilter
///virtual inheritance solves the diamond problem


Vintage::Vintage():  DetailFilters(), ColorFilters(){}
Vintage::~Vintage(){}

///applies b&w, adds a soft blur and some noticeable grain
void Vintage::applySettings(Image& img){
    BlackAndWhite bw;
    Blur b(0.55);
    Grain g(0.15);
    bw.applySettings(img);
    b.applySettings(img);
    g.applySettings(img);
}

std::string Vintage::getFilterName() const {
return "Vintage filter applied!";}

Cinematic::Cinematic():  DetailFilters(), ColorFilters(){}
Cinematic::~Cinematic(){}

///applies orange&teal and adds soft grain
void Cinematic::applySettings(Image& img){
    OrangeAndTeal ot;
    Grain g(0.12);
    ot.applySettings(img);
    g.applySettings(img);
}

std::string Cinematic::getFilterName() const {
return "Cinematic filter applied!";}

Summer::Summer():  DetailFilters(), ColorFilters(){}
Summer::~Summer(){}

///applies goldenhour and increases contrast
void Summer::applySettings(Image& img){
    GoldenHour gh;
    Contrast c(1.33);
    gh.applySettings(img);
    c.applySettings(img);
}

std::string Summer::getFilterName() const {
return "Summer-inspired filter applied!";}

Spring::Spring():  DetailFilters(), ColorFilters(){}
Spring::~Spring(){}

///applies pastels and adds a dreamy blur
void Spring::applySettings(Image& img){
    Pastels p;
    Blur b(0.58);
    p.applySettings(img);
    b.applySettings(img);
}

std::string Spring::getFilterName() const {
return "Spring-inspired filter applied!";}

Winter::Winter():  DetailFilters(), ColorFilters(){}
Winter::~Winter(){}

///applies moodyblues and lowers contrast for a foggy look
void Winter::applySettings(Image& img){
    MoodyBlues mb;
    Contrast c(0.56);
    mb.applySettings(img);
    c.applySettings(img);
}

std::string Winter::getFilterName() const {
return "Winter-inspired filter applied!";}

Autumn::Autumn():  DetailFilters(), ColorFilters(){}
Autumn::~Autumn(){}

///applies retro, adds grain and slightly increases contrast
void Autumn::applySettings(Image& img){
    Retro r;
    Grain g(0.21);
    Contrast c(1.19);
    r.applySettings(img);
    g.applySettings(img);
    c.applySettings(img);
    }

std::string Autumn::getFilterName() const {
return "Autumn-inspired filter applied!";}

#ifndef STATE_H
#define STATE_H
#include "Image.h"
#include<vector>
#include<memory>
#include<iostream>

/// STATE DESIGN PATTERN 
///allows an object to alter its behavior when its internal state changes
/// abstract class/ state interface
class State{
    public :
    virtual ~State()=default;
    virtual void mouseClick()=0;
    virtual void clearPreview(Image& img)=0;
};

///  standard state with no preview
class InitialState: public State{
    public:
    void mouseClick() override{}
    void clearPreview(Image& img) override{}
};

/// preview mode 
class Preview: public State{
    std::vector<unsigned char> backup;
    public:
    Preview(Image& img)
    {
        backup=img.getPixels();
    }
    void mouseClick() override{
        std::cout<<"Previewing image.\n";
    }
    void clearPreview(Image& img) override{
        img.getPixels()=backup;
    }

    /// exits the preview mode returning to previous data and applies the filter to show the preview
    /// template function because it applies to all of our filters 
    /// it would be redundant to write 20 functions (one for each filter)
    template<typename Filter>
    void showPreview(Image& img, Filter& filter)
    {
        img.getPixels()=backup;
        filter.applySettings(img);
    }
};
#endif
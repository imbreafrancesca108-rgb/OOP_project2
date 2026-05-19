#include "SizeFilters.h"
#include "Image.h"
#include "Exceptions.h"
#include<vector>
#include<iostream>


Crop::Crop(int nw, int nh): SizeFilters(), newWidth(nw), newHeight(nh){}
Crop::~Crop(){}

///crops image to specified dimensions
void Crop::applySettings(Image& img){
    int w=img.getWidth();
    int h=img.getHeight();
    int channels=img.getRGBChannels();
    std::vector<unsigned char>& originalPixels=img.getPixels();
    std::vector<unsigned char> modifiedPixels;

    if(newWidth>w||newHeight>h)
    {
        throw CropException();///throws exception if dimensions are larger than the original ones
    }

    ///copy the region between the new dimensions
    for(int y=0;y<newHeight;y++)
        for(int x=0;x<newWidth;x++)

    {///index of the original image
        int i=(y*w+x)*channels;

    ///copy all channels for this certain pixel
        for(int c=0;c<channels;c++)
            modifiedPixels.push_back(originalPixels[i+c]);
    }

    ///replace image with its cropped version
    img.getPixels().assign(modifiedPixels.begin(), modifiedPixels.end());
    img.setWidth(newWidth);
    img.setHeight(newHeight);
}

std::string Crop::getFilterName() const {
return "The image was cropped!";}

Resize::Resize(double p):  SizeFilters(), percentage(p){}
Resize::~Resize(){}

///resize image based on percentage(between 1-100)
void Resize::applySettings(Image& img){
    ///convert percentage to factor
    double p=this->percentage/100.0;

    int w=img.getWidth();
    int h=img.getHeight();
    int channels=img.getRGBChannels();
    std::vector<unsigned char>& originalPixels=img.getPixels();

    ///new dimensions
    int nw=w*p;
    int nh=h*p;
    if(nw<=0||nh<=0||nw>w||nh>h)
        throw ResizeException();///throws exception for invalid dimensions

    ///prea-allocate a result vector
    std::vector<unsigned char> modifiedPixels(nw*nh*channels);

    ///nearest-neighbor scaling by finding which original pixel to use
    for(int y=0;y<nh;y++)
        for(int x=0;x<nw;x++)
    {
        int originalX=int(x/p);
        int originalY=(int)(y/p);
        int original=(originalY*w+originalX)*channels;
        int i=(y*nw+x)*channels;
        for(int c=0;c<channels;c++)
            modifiedPixels[i+c]=originalPixels[original+c];
    }

    ///replace image with its resized version
    img.getPixels().assign(modifiedPixels.begin(), modifiedPixels.end());
    img.setWidth(nw);
    img.setHeight(nh);
    }

std::string Resize::getFilterName() const {
return "The image was resized!";}

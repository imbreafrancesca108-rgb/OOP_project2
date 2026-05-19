#include "Image.h"
#include "DetailFilters.h"
#include "Exceptions.h"
#include<vector>
#include<algorithm>

Contrast::Contrast(double a) : DetailFilters(), amountContrast(a){}
Contrast::~Contrast(){}

std::string Contrast::getFilterName() const {
    return "This filter changes the contrast!";}

///adjust contrast using standard formula new_pixel = amount * (pixel - 128) + 128
///amount>1 -> increase contrast ; amount<1 -> decrease contrast
void Contrast::applySettings(Image& img) {
    std::vector<unsigned char>& pixels=img.getPixels();
    int channels=img.getRGBChannels();

    if(amountContrast <= 0 || amountContrast > 20) {
        throw AmountException();
    }
    ///procces each pixel
    for(long i=0;i<(long)pixels.size();i+=channels)
        for(int c=0;c<channels;c++)
    {
        double pixel=(double)pixels[i+c];
        double new_pixel=amountContrast*(pixel-128.0f)+128.0f;

        ///stay within range
        if(new_pixel>255.0f)
            new_pixel=255.0f;
        else
            if(new_pixel<0.0f)
                new_pixel=0.0f;
        pixels[i+c]=(unsigned char)new_pixel;

    }
    }

Grain::Grain(double a) : DetailFilters(), amountGrain(a){}
Grain::~Grain(){}

std::string Grain::getFilterName() const {
    return "This filter adds grain!";}

///adds a film-like grain using deterministic noise based on pixel position
void Grain::applySettings(Image& img) {
    std::vector<unsigned char>& pixels=img.getPixels();
    int channels=img.getRGBChannels();

    if(amountGrain <= 0 || amountGrain > 20) {
        throw AmountException();
    }

    for(long i=0;i<(long)pixels.size();i+=channels){

        ///noise based on pixel index (between -0.5 and 0.5)
        double noise= ((double)(i % 101) / 100.0) -0.5;
        for(int c=0;c<channels;c++)
    {
        double pixel=(double)pixels[i+c];

        ///grain gets added proportionally with brightness
        ///dark areas get less and bright ones more
        double new_pixel=pixel+(noise*(255.0-pixel)*amountGrain);

        if(new_pixel>255.0f)
            new_pixel=255.0f;
        else
            if(new_pixel<0.0f)
                new_pixel=0.0f;
        pixels[i+c]=(unsigned char)new_pixel;

    }
    }
    }

Blur::Blur(double a) : DetailFilters(), amountBlur(a){}
Blur::~Blur(){}

std::string Blur::getFilterName() const {
    return "This filter adds a soft blur!";}

///adds a soft blur using the 5*5 kernel average
///amount between 0-20
void Blur::applySettings(Image& img) {
    ///copy for reading
    std::vector<unsigned char> originalPixels=img.getPixels();
    ///separate buffer for writing in order to not to corrupt the pixels
    std::vector<unsigned char> modifiedPixels=originalPixels;

    int channels=img.getRGBChannels();
    int w=img.getWidth();
    int h=img.getHeight();

    if(amountBlur <= 0 || amountBlur > 1) {
        throw AmountException();
    }

    ///we process all pixels except for the border(the 2 pixels from the edges) to prevent overflow
    for(int i=2;i<h-2;i++)
        for(int j=2;j<w-2;j++)
            for(int c=0;c<channels;c++)
            {
                double sum=0.0;

                ///average of 25 neighbor pixels
                for(int l=-2;l<=2;l++)
                    for(int m=-2;m<=2;m++)
                {
                    int p=((i+l)*w+(j+m))*channels+c;
                    sum+=originalPixels[p];
                }
                int modified=(i*w+j)*channels+c;
                ///blend original with blur
                modifiedPixels[modified]=(unsigned char)(originalPixels[modified]+amountBlur*(sum/25.0-originalPixels[modified]));
            }
    ///copy results
    img.getPixels()=modifiedPixels;
    }

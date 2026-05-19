#include "ColorFilters.h"
#include<vector>
#include<algorithm>
#include "Image.h"

BlackAndWhite::BlackAndWhite(): ColorFilters(){}
BlackAndWhite::~BlackAndWhite(){}

std::string BlackAndWhite::getFilterName() const {
return "This filter is Black&White/Grayscale!";}

///converts image to grayscale using the formula gray=0.299*R+0.587*G+0.114*B
void BlackAndWhite::applySettings(Image& img)
{
    std::vector<unsigned char>& pixels=img.getPixels();
    int channels=img.getRGBChannels();

    ///process each pixel
    for(long i=0;i<(long)pixels.size();i+=channels)
    {///extract rgb values
        float  r=(float)pixels[i];
        float  g=(float)pixels[i+1];
        float  b=(float)pixels[i+2];

        ///calculate luminance
        float  gray=(r*0.299f)+(g*0.587f)+(b*0.114f);

        ///assign gray to all channels
        pixels[i]=(unsigned char)std::min(255.0f,gray);
        pixels[i+1]=(unsigned char)std::min(255.0f,gray);
        pixels[i+2]=(unsigned char)std::min(255.0f,gray);
    }
}

///creates a warm, sunset-inspired look to induce golden hour
GoldenHour::GoldenHour(): ColorFilters(){}
GoldenHour::~GoldenHour(){}

std::string GoldenHour::getFilterName() const {
return "This filter is Golden Hour!";}

void GoldenHour::applySettings(Image& img)
{
    std::vector<unsigned char>& pixels=img.getPixels();
    int channels=img.getRGBChannels();

    for(long i=0;i<(long)pixels.size();i+=channels)
    {
        float  r=(float)pixels[i];
        float  g=(float)pixels[i+1];
        float  b=(float)pixels[i+2];

        ///we calculate the brightness to use it for highlights adjustment
        float  luminance=(r*0.299f)+(g*0.587f)+(b*0.114f);

        ///boost red, boost green just a bit and reduce blue
        float new_r=std::min(255.0f,r*1.4f);
        float new_g=std::min(255.0f,g*1.04f);
        float new_b=std::min(255.0f,b*0.6f);

        ///give an extra boost for the highlights
        if(luminance>150.0f)
            new_r=std::min(255.0f,new_r*1.1f);

        pixels[i]=(unsigned char)new_r;
        pixels[i+1]=(unsigned char)new_g;
        pixels[i+2]=(unsigned char)new_b;

    }
}

///creates a vintage look using the basic sepia transformation
Retro::Retro(): ColorFilters(){}
Retro::~Retro(){}

std::string Retro::getFilterName() const {
return "This filter is Retro/Sepia!";}

void Retro::applySettings(Image& img)
{
    std::vector<unsigned char>& pixels=img.getPixels();
    int channels=img.getRGBChannels();

    for(long i=0;i<(long)pixels.size();i+=channels)
    {
        float  r=(float)pixels[i];
        float  g=(float)pixels[i+1];
        float  b=(float)pixels[i+2];

        ///sepia transformation matrix
        float  new_r=(r*0.393f)+(g*0.769f)+(b*0.189f);
        float  new_g=(r*0.349f)+(g*0.686f)+(b*0.168f);
        float  new_b=(r*0.272f)+(g*0.534f)+(b*0.131f);

        pixels[i]=(unsigned char)std::min(255.0f,new_r);
        pixels[i+1]=(unsigned char)std::min(255.0f,new_g);
        pixels[i+2]=(unsigned char)std::min(255.0f,new_b);
    }
}

OrangeAndTeal::OrangeAndTeal(): ColorFilters(){}
OrangeAndTeal::~OrangeAndTeal(){}

std::string OrangeAndTeal::getFilterName() const {
return "This filter is Orange&Teal!";}

///induced a cinematic color-grading with cool-toned shadows and bright, warm highlights
void OrangeAndTeal::applySettings(Image& img)
{
    std::vector<unsigned char>& pixels=img.getPixels();
    int channels=img.getRGBChannels();

    for(long i=0;i<(long)pixels.size();i+=channels)
    {
        float  r=(float)pixels[i];
        float  g=(float)pixels[i+1];
        float  b=(float)pixels[i+2];

        float  luminance=(r*0.299f)+(g*0.587f)+(b*0.114f);
        float new_r=r,new_g=g,new_b=b;

        ///for shadows we increase the blue&green to enhance teal and reduce red
        if(luminance<128.0f)
        { new_r=std::min(255.0f,r*0.74f);
         new_g=std::min(255.0f,g*1.15f);
         new_b=std::min(255.0f,b*1.27f);}

         ///for highlights we increase red and green and reduce the blue
        if(luminance>=128.0f)
            {new_r=std::min(255.0f,r*1.27f);
        new_g=std::min(255.0f,g*1.03f);
        new_b=std::min(255.0f,b*0.72f);}

        pixels[i]=(unsigned char)new_r;
        pixels[i+1]=(unsigned char)new_g;
        pixels[i+2]=(unsigned char)new_b;

    }
}

Pastels::Pastels(): ColorFilters(){}
Pastels::~Pastels(){}

std::string Pastels::getFilterName() const {
return "This filter is Pastel/Soft!";}

///creates a soft, dreamy look desaturating colors
void Pastels::applySettings(Image& img)
{
    std::vector<unsigned char>& pixels=img.getPixels();
    int channels=img.getRGBChannels();

    for(long i=0;i<(long)pixels.size();i+=channels)
    {
        float  r=(float)pixels[i];
        float  g=(float)pixels[i+1];
        float  b=(float)pixels[i+2];

       ///desaturates to 70%
    float new_r =(r*0.7f)+55.0f;
    float new_g =(g*0.7f)+50.0f;
    float new_b =(b*0.7f)+40.0f;

    ///adds subtle pink/green shift based on original color
    if(r>g&&r>b) {
        /// red/pink areas get more pink
        new_r=std::min(255.0f,new_r+16.0f);
        new_b=std::min(255.0f,new_b+7.0f);
    }
    else if(g>r&&g>b) {
        ///green areas get more green
        new_g =std::min(255.0f,new_g+18.0f);
    }

        pixels[i]=(unsigned char)std::min(255.0f,new_r);
        pixels[i+1]=(unsigned char)std::min(255.0f,new_g);
        pixels[i+2]=(unsigned char)std::min(255.0f,new_b);
    }
}

MoodyBlues::MoodyBlues(): ColorFilters(){}
MoodyBlues::~MoodyBlues(){}

std::string MoodyBlues::getFilterName() const {
return "This filter Moody Blues!";}

///creates a dark, cool-toned aesthetic
void MoodyBlues::applySettings(Image& img)
{
    std::vector<unsigned char>& pixels=img.getPixels();
    int channels=img.getRGBChannels();

    for(long i=0;i<(long)pixels.size();i+=channels)
    {
        float  r=(float)pixels[i];
        float  g=(float)pixels[i+1];
        float  b=(float)pixels[i+2];

        ///drastically reduce red, slightly reduce green and increase blue
        float  new_r=r*0.62f;
        float  new_g=g*0.81f;
        float  new_b=std::min(255.0f,b*1.34f);

        ///apply some darkness
        pixels[i]=(unsigned char)(0.9f*new_r);
        pixels[i+1]=(unsigned char)(0.9f*new_g);
        pixels[i+2]=(unsigned char)(0.9f*new_b);
    }
}






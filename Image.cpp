#include "Image.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#if defined(__GNUC__) || defined(__clang__)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wmissing-field-initializers"
    #pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#if defined(__GNUC__) || defined(__clang__)
    #pragma GCC diagnostic pop
#endif
#include "Exceptions.h"
#include<stdexcept>
#include<iostream>
#include<vector>

///initialize static member to track images saved across all instances
int Image::totalEditedImages=0;

///default constructor-creates an empty image
Image::Image()
{width=0;
height=0;
rgb=0;
filename="";
originalWidth=0;
originalHeight=0;
}

///parametrized constructor- creates the image from raw pixel data with width, height and rgb channels
Image::Image(int w, int h, int c, unsigned char* p):
width(w), height(h), rgb(c), originalWidth(w), originalHeight(h){
    if(p!=nullptr)
      {for (long i=0;i<w*h*c;++i)
            pixels.push_back(p[i]);
    ///free the original memory allocated by stb_image
    stbi_image_free(p);
    filename="";
    originalPixels=pixels;
    }
}

///destructor- the vector automatically cleans up data
Image::~Image(){}

///copy constructor- creates a deep copy
Image::Image(const Image& other){
this->width=other.width;
this->height=other.height;
this->rgb=other.rgb;
this->filename=other.filename;
for(size_t i=0;i<other.pixels.size();++i)
    this->pixels.push_back(other.pixels[i]);
this->originalWidth=other.originalWidth;
this->originalHeight=other.originalHeight;
this->originalPixels=other.originalPixels;
}

///assigment operator
Image& Image::operator=(const Image& other){
 if(this!=&other)
 {///stops self-copy
     this->width=other.width;
     this->height=other.height;
     this->rgb=other.rgb;
     this->pixels=other.pixels;
     this->filename=other.filename;
     this->originalWidth=other.originalWidth;
     this->originalHeight=other.originalHeight;
     this->originalPixels=other.originalPixels;
 }
 return *this;
}

///getters
int Image::getWidth() const {return width;}
int Image::getHeight() const{return height;}
int Image::getRGBChannels () const{ return rgb;}
std::string Image::getFilename() const { return filename; }

///load image using stb_image library
bool Image::load(const std::string& path) {
    int w,h,c;
    unsigned char* dataPixels=stbi_load(path.c_str(),&w,&h,&c,0);

    if (dataPixels==nullptr) {
        throw OpenPath(path); ///throws error if the file is not found/doesn't exist
    }

    ///stores image properties
    this->width=w;
    this->height=h;
    this->rgb=c;
    this->pixels.clear();
    this->filename=path;
    for (long i=0;i<w*h*c;++i)
        this->pixels.push_back(dataPixels[i]);


    stbi_image_free(dataPixels);
    saveOriginal();
    return true;
}

/// saves original data for reset option
void Image::saveOriginal(){
    originalPixels=pixels;
    originalWidth=width;
    originalHeight=height;
}
///returns references to pixels in order to be modified
std::vector<unsigned char>& Image::getPixels(){
return pixels;}

///setters
void Image::setWidth(int w){
width=w;}
void Image::setHeight(int h){
height=h;}

///return the number of edited&saved images
int Image::getEditedImages(){
return totalEditedImages;}

///saves the image as a JPEG
void Image::saveImage(const std::string& path) const{
try{
    const unsigned char* rawData=&pixels[0];
    int result=stbi_write_jpg(path.c_str(),width,height,rgb,rawData,100);
    if (result)
    {
        std::cout<<"The image was edited saved at : "<<path<<std::endl;
        totalEditedImages++;
    }
    else
        throw SavePath(path);///throws error if we cannot find the saving path

    }
catch(const std::exception& e){
    std::cerr<<"Error: "<<e.what()<<"\n";
}
}





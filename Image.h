#ifndef IMAGE_H
#define IMAGE_H
#include<vector>
#include<string>
#include "Observer.h"
class Image: public Observable<Image>{
    int width;
    int height;
    int rgb;       ///rgb channels
    std::vector<unsigned char> pixels;       ///raw pixel data
    static int totalEditedImages;
    std::string filename;

    int originalWidth=0, originalHeight=0;
    std::vector<unsigned char> originalPixels;
public:
    Image();
    Image(int w, int h, int c, unsigned char* p);
    ~Image();
    Image(const Image& other);
    Image& operator=(const Image& other);
    int getWidth() const ;
    int getHeight() const ;
    int getRGBChannels() const ;
    std::vector<unsigned char>& getPixels();
    static int getEditedImages();
    void saveImage(const std::string& path) const;
    void setWidth(int w);
    void setHeight(int h);
    bool load(const std::string& path);
    std::string getFilename() const ;

    int getOriginalWidth() const {return originalWidth;}
    int getOriginalHeight() const {return originalHeight;}
    const std::vector<unsigned char>& getOriginalPixels() const {return originalPixels;}
    void saveOriginal();
};

#endif

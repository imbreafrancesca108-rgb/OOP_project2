#ifndef MENU_H
#define MENU_H

#include <SFML/Graphics.hpp> //include SFML library for graphic interface
#include <vector>
#include <string>
#include "Image.h"
#include "Command.h"
#include "Observer.h"

class Menu : public Observer<Image> {
    /// inheritance from Observer to notify on the modifications made
    static Menu* instance; /// one copy for the whole class
    std::vector<Image> gallery; /// all images uploaded
    CommandManager cmdManager; /// command history

    int selectedIndex=0; /// the image the user is currently working on
    Image previewImage; /// temporary copy for preview
    bool isPreviewActive=false;  /// preview or not
    std::string activeFilter="";
    double activeAmount=0;
    /// used for COPY
    std::string lastAppliedFilter="";
    double lastAppliedAmount=0.0;
    /// starting visual amount for sliders
    float contrastSlider=0;
    float grainSlider=0;
    float blurSlider=0;
    float resizeSlider=0;

    /// booleans to show if an accordion is opened or not
    bool showColor=false;
    bool showDetail=false;
    bool showPresets=false;
    bool showGallery=false;
    bool showSize=false;
    
    /// the state of the load/save pop-window
    bool isTypingLoad=false;
    bool isTypingSave=false;
    std::string typedPath="";
    
    /// the state of the crop pop-window
    bool isTypingCrop=false;
    std::string cropWidthStr="";
    std::string cropHeightStr="";
    int cropActiveField=0;

    std::vector<FilterSettings> clipboard; ///filter to be copied
    bool hasClipboard=false; ///prevents paste from not copying anything

    /// UI functions for graphic elements
    /// booleans that return true when hovering 
    bool drawButton(sf::RenderWindow&,sf::Font&,const std::string& label,
        float x,float y,sf::Color bg,float w,float h);
    /// for accordions 
    bool drawHeader(sf::RenderWindow&,sf::Font&,const std::string& label,
        float x,float y,bool open,float w);
    /// for sliders with minimum and maximum values
    float drawSlider(sf::RenderWindow&,sf::Font&,const std::string& label,
        float x,float y,float val,float trackW,double minA, double maxA);

    void drawModal (sf::RenderWindow&,sf::Font&);

    void drawCropModal(sf::RenderWindow&,sf::Font&);
    
    /// alters the "frame"(sprite) of our photo and the photo itself
    void refreshTexture(sf::Texture&,sf::Sprite&,Image&,sf::Vector2u canvas, 
        sf::Vector2f origin);

    Menu(); /// private constructor

public:
    static Menu* getInstance();
    void run();
    void update(Image& img) override; ///polymorphism to notify observers
};

#endif

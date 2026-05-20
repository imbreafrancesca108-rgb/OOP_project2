#include "Menu.h"
#include "FilterFactory.h"
#include <iostream>
#include <optional>
#include <algorithm>

Menu* Menu::instance = nullptr;
Menu* Menu::getInstance() 
{ if (!instance) 
    instance=new Menu(); 
    return instance; }
Menu::Menu() : selectedIndex(0), isPreviewActive(false) {}
void Menu::update(Image& img) 
{ std::cout<<"Image "<<img.getFilename()<<" was altered.";}

/// function to scale the photo to fit de canvas
void Menu::refreshTexture(sf::Texture& tex,sf::Sprite& spr,
    Image& img,sf::Vector2u canvas,sf::Vector2f origin)
 {
    int w=img.getWidth(); 
    int h=img.getHeight();
    int ch=img.getRGBChannels();
    auto& px=img.getPixels();
    if (w<=0||h<=0||px.empty()) 
        return;

   /// not every photo has 4 rgba channels so we add 
   /// aplha=255(maximum opacity) for SFML to work
    std::vector<std::uint8_t> rgba;
    rgba.reserve(static_cast<std::size_t>(w * h * 4));
    for (std::size_t i=0;i<px.size();i+=ch) {
        rgba.push_back(px[i]); ///R
        rgba.push_back(px[i+1]);  ///G
        rgba.push_back(px[i+2]); ///B
        rgba.push_back(ch==4 ? px[i+3]:255); /// existing A or 255
    }
     
    /// temporary image with our extracted data
    sf::Image sfImg({static_cast<unsigned>(w), static_cast<unsigned>(h)}, rgba.data());
    /// checking if we have data from the image using ans SFML method
    if (!tex.loadFromImage(sfImg))
    std::cerr<<"Loading pixel data from the image has failed. Try again!\n";
    
    /// we set the frame 
    tex.setSmooth(false);
    spr.setTexture(tex, true);

    /// we calculate the scale to match the canvas to not deform the photo
    float scale=std::min(static_cast<float>(canvas.x)/w,static_cast<float>(canvas.y)/h);
    if(activeFilter=="Resize")
    {scale=scale*(activeAmount/100.0f);
    }
    spr.setScale({scale, scale});
    ///center the photo
    spr.setPosition({origin.x + (static_cast<float>(canvas.x)-w*scale)/2.f,origin.y+(static_cast<float>(canvas.y)-h*scale)/2.f});
}

/// function to create a visual button
bool Menu::drawButton(sf::RenderWindow& win, sf::Font& font, const std::string& label,
    float x, float y, sf::Color bg, float w, float h) {
    /// curreent mouse postion
    sf::Vector2f m=sf::Vector2f(sf::Mouse::getPosition(win));
    /// checks if the mouse is hovering our button
    bool hover=sf::FloatRect({x, y}, {w, h}).contains(m);

    sf::RectangleShape shape({w, h});
    shape.setPosition({x, y});

    ///when hovering we lighten the color
    shape.setFillColor(hover? sf::Color(std::min(255,(int)bg.r+30), std::min(255,(int)bg.g+30), std::min(255,(int)bg.b+30)): bg);
    shape.setOutlineThickness(1.f);
    shape.setOutlineColor(sf::Color(105, 105, 105));
    win.draw(shape); /// drawing the rectangle in our window
    
    /// shaping the text on our button
    sf::Text txt(font, label, 15);
    txt.setFillColor(sf::Color::White);
    /// position text in the center
    txt.setOrigin(txt.getLocalBounds().getCenter());
    txt.setPosition({x+w/2.f, y+h/2.f});
    win.draw(txt);
    return hover;
}

/// function to create the bar/header of the accordion
bool Menu::drawHeader(sf::RenderWindow& win, sf::Font& font, const std::string& label,
    float x, float y, bool open, float w) {
    sf::Vector2f m=sf::Vector2f(sf::Mouse::getPosition(win));
    bool hover=sf::FloatRect({x, y}, {w, 38.f}).contains(m);

    sf::RectangleShape shape({w, 38.f});
    shape.setPosition({x, y});
    shape.setFillColor(hover ? sf::Color(255,219,231) : sf::Color(255,183,206));
    shape.setOutlineThickness(1.2f);
    shape.setOutlineColor(sf::Color(8,8,8));
    win.draw(shape);

    sf::Text txt(font, (open ? "v  " : ">  ")+label,16);
    txt.setFillColor(sf::Color(120,0,59));
    txt.setPosition({x+10.f,y+11.f});
    win.draw(txt);
    return hover;
}

/// function to draw a slider and return its new/original value
float Menu::drawSlider(sf::RenderWindow& win, sf::Font& font, const std::string& label,
    float x, float y, float val, float trackW, double minA, double maxA) {
    /// visually, the value is between 0 and 100
    /// however, we calculate it based on our min and max value (eg: 0.1-20.0 for Grain)
    double displayVal=minA+(val/100.f)*(maxA-minA);

    /// display of the value
    sf::Text lbl(font,label+": "+std::to_string(static_cast<int>(val)),16);
    lbl.setFillColor(sf::Color(255, 255, 255));
    lbl.setPosition({x, y});
    win.draw(lbl);
    /// slider track
    float ty=y+18.f;
    /// slider background
    sf::RectangleShape track({trackW, 4.f});
    track.setPosition({x, ty+6.f});
    track.setFillColor(sf::Color(85, 85, 85));
    win.draw(track);

    sf::RectangleShape fill({(val/100.f)*trackW, 4.f});
    fill.setPosition({x, ty+6.f});
    fill.setFillColor(sf::Color(255,215,55));
    win.draw(fill);

    sf::CircleShape knob(7.f);
    knob.setOrigin({7.f, 7.f});
    knob.setPosition({x+(val/100.f)*trackW,ty+8.f});
    knob.setFillColor(sf::Color(225, 225, 225));
    win.draw(knob);

    /// we check if the button is being dragged 
    if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
        sf::Vector2f m=sf::Vector2f(sf::Mouse::getPosition(win));
        if (sf::FloatRect({x-5.f,ty-4.f}, {trackW+10.f,22.f}).contains(m))
            /// we calculate the new position of our mouse
            /// we use std::clamp to make sure it stays between 0 and 100
            return std::clamp(((m.x-x)/trackW)*100.f, 0.f, 100.f);
    }
    return val;
}

/// pop-up window for the user to navigate between the options
void Menu::drawModal(sf::RenderWindow& win,sf::Font& font) {
    /// overlay
    sf::RectangleShape overlay({1200.f,760.f});
    overlay.setFillColor(sf::Color(0,0,0,165));
    win.draw(overlay);
    /// rectangle with options
    sf::RectangleShape box({800.f, 340.f});
    box.setPosition({240.f, 190.f});
    box.setFillColor(sf::Color(120,0,59));
    box.setOutlineThickness(2.f);
    box.setOutlineColor(sf::Color(255,255,255));
    win.draw(box);

    sf::Text title(font, isTypingLoad ? "Load Image - Enter file path:" : "Save Image - Enter file path:", 20);
    title.setFillColor(sf::Color(255,255,255));
    title.setPosition({270.f,220.f});
    win.draw(title);
    /// input field
    sf::RectangleShape field({700.f,50.f});
    field.setPosition({300.f,288.f});
    field.setFillColor(sf::Color(8,8,8));
    field.setOutlineThickness(1.f);
    field.setOutlineColor(sf::Color(255,255,255));
    win.draw(field);

    sf::Text inp(font,typedPath+"|",20);
    inp.setFillColor(sf::Color::White);
    inp.setPosition({303.f,290.f});
    win.draw(inp);

    sf::Text hint(font, "ENTER = continue    ESC = cancel", 17);
    hint.setFillColor(sf::Color(255,255,255));
    hint.setPosition({303.f, 350.f});
    win.draw(hint);
}

/// pop-up window for the user to introduce new dimensions for crop
void Menu::drawCropModal(sf::RenderWindow& win,sf::Font& font) {
    ///same as MODAL
    sf::RectangleShape overlay({1200.f,760.f});
    overlay.setFillColor(sf::Color(0,0,0,165));
    win.draw(overlay);

    sf::RectangleShape box({800.f, 340.f});
    box.setPosition({240.f, 190.f});
    box.setFillColor(sf::Color(120,0,59));
    box.setOutlineThickness(2.f);
    box.setOutlineColor(sf::Color(255,255,255));
    win.draw(box);

    sf::Text title(font, "Crop Image - Enter dimensions:",20);
    title.setFillColor(sf::Color(255,255,255));
    title.setPosition({270.f,220.f});
    win.draw(title);

    /// width label
    sf::Text lblW(font,"Width:",16);
    lblW.setFillColor(sf::Color(185,185,185));
    lblW.setPosition({300.f,250.f});
    win.draw(lblW);

    sf::RectangleShape fieldW({200.f,35.f});
    fieldW.setPosition({300.f,288.f});
    fieldW.setFillColor(sf::Color(22,22,32));
    fieldW.setOutlineThickness(1.f);
    fieldW.setOutlineColor(cropActiveField == 0 ? sf::Color(255,255,255) : sf::Color(70,70,70));
    win.draw(fieldW);
    /// we use "|" to show that the input filed is active
    sf::Text inpW(font,cropWidthStr + (cropActiveField == 0 ? "|" : ""), 13);
    inpW.setFillColor(sf::Color::White);
    inpW.setPosition({310.f,295.f});
    win.draw(inpW);

    /// height label
    sf::Text lblH(font,"Height:",16);
    lblH.setFillColor(sf::Color(185,185,185));
    lblH.setPosition({550.f,250.f});
    win.draw(lblH);

    sf::RectangleShape fieldH({200.f,35.f});
    fieldH.setPosition({550.f,288.f});
    fieldH.setFillColor(sf::Color(22,22,32));
    fieldH.setOutlineThickness(1.f);
    fieldH.setOutlineColor(cropActiveField == 1 ? sf::Color(255,255,255) : sf::Color(70,70,70));
    win.draw(fieldH);

    sf::Text inpH(font, cropHeightStr + (cropActiveField == 1 ? "|" : ""), 13);
    inpH.setFillColor(sf::Color::White);
    inpH.setPosition({560.f,295.f});
    win.draw(inpH);

    /// we show original dimenions so the user knows to introduce smaller ones
    if (!gallery.empty()) {
        std::string dimInfo = "Original: "+
            std::to_string(gallery[selectedIndex].getWidth())+" x "+
            std::to_string(gallery[selectedIndex].getHeight());
        sf::Text dims(font,dimInfo,16);
        dims.setFillColor(sf::Color(255,255,255));
        dims.setPosition({310.f,385.f});
        win.draw(dims);
    }

    sf::Text hint(font, "ENTER = continue    ESC = cancel", 17);
    hint.setFillColor(sf::Color(255,255,255));
    hint.setPosition({303.f, 350.f});
    win.draw(hint);
}

void Menu::run() {
    /// main window with 1200x760 resolution
    sf::RenderWindow window(sf::VideoMode({1200,760}), "Photo Editor");
    window.setFramerateLimit(60);
    /// loading font downloaded in the project folder
    sf::Font font;
    if (!font.openFromFile("arial.ttf")) {
        std::cerr<<"ERROR: arial.ttf was not found.\n";
        return;
    }

    sf::Texture mainTex;
    sf::Sprite mainSpr(mainTex);
    bool needsUpdate=true;

    const float TOP_H=55.f;
    const float PANEL_X=855.f;
    const float HDR_W=340.f;
    const float BTN_W=155.f;
    const float BTN_H=30.f;
    const float SLD_W=235.f;
    const sf::Vector2u CANVAS={855u, 705u};
    const sf::Vector2f CANVAS_ORIG={0.f, TOP_H};

    /// layout function to calculate the position of the accordion when 
    ///it is opened or closed
    auto layout = [&](float& colorHdrY,  float& detailHdrY,  float& sizeHdrY,
        float& presetsHdrY, float& dcY, float& dgY, float& dbY,
        float& rsY, float& cropBtnY, float cbY[6], float pbY[6]) {
        float ry=TOP_H+10.f;
        ///color filters section
        colorHdrY=ry; 
        ry +=40.f; /// header space 
        if (showColor) {ry+=4.f;
            for (int i=0;i<6;++i) { 
                cbY[i]=ry;
                ry+=34.f; } ///space for weach button
            ry+=4.f;
        }
        /// detail filters section
        detailHdrY=ry;
         ry+=40.f;
        if (showDetail) {ry+=4.f;
            dcY=ry; ry+=42.f;
            dgY=ry; ry+=42.f;
            dbY=ry; ry+=46.f;
        }
        /// size filters section
        sizeHdrY=ry;
         ry+=40.f;
        if (showSize) {ry+=4.f;
            rsY=ry; ry+=42.f;
            cropBtnY=ry; ry+=38.f;
            ry+=4.f;
        }
        /// presets section
        presetsHdrY=ry;
        ry+=40.f;
        if (showPresets){ry+=4.f;
            for (int i=0;i<6;++i)
            {pbY[i]=ry;
            ry+=34.f; }
        ry+=4.f;}
    };

    /// main loop
    while (window.isOpen()) {
        /// we listen to every action of the user
        while (const std::optional<sf::Event> ev=window.pollEvent()) {
            /// if we press X the window shuts
            if (ev->is<sf::Event::Closed>())
            { window.close(); 
            break; }

            if (isTypingCrop) {
                /// we check if the user pressed within our pop-up window
                if (const auto* mb2=ev->getIf<sf::Event::MouseButtonPressed>()) {
                    ///we check if the user pressed on width(left) or height(right)
                    sf::Vector2f m2{static_cast<float>(mb2->position.x),static_cast<float>(mb2->position.y)};
                   
                cropActiveField=sf::FloatRect({300.f,288.f},{200.f,35.f}).contains(m2) ? 0:
                                sf::FloatRect({550.f,288.f},{200.f,35.f}).contains(m2) ? 1:
                                    cropActiveField;}
                ///now we check if the user pressed escape, enter or backspace
                if (const auto* k=ev->getIf<sf::Event::KeyPressed>()) {
                    if (k->code==sf::Keyboard::Key::Escape) {
                        isTypingCrop=false; cropWidthStr=""; cropHeightStr="";
                    } 
                    else if (k->code==sf::Keyboard::Key::Backspace) {
                        if (cropActiveField==0&&!cropWidthStr.empty())  
                            cropWidthStr.pop_back();  /// we delete the wrong inputs
                        if (cropActiveField==1&&!cropHeightStr.empty())
                            cropHeightStr.pop_back();
                    } else if (k->code==sf::Keyboard::Key::Enter&&!cropWidthStr.empty()&&!cropHeightStr.empty()) {
                        try {/// we apply the filter
                            int cw=std::stoi(cropWidthStr);
                            int ch=std::stoi(cropHeightStr);
                            previewImage=gallery[selectedIndex];
                            FilterFactory::create("Crop",cw,ch)->applySettings(previewImage);
                            activeFilter="Crop"; activeAmount=0.0;
                            isPreviewActive=true; needsUpdate=true;
                        } catch (const std::exception& e) {
                            std::cerr<<"Crop: "<<e.what()<<"\n";
                        }
                        isTypingCrop=false; cropWidthStr=""; cropHeightStr="";
                    }
                }
                /// checking the input from the user
                if (const auto* te=ev->getIf<sf::Event::TextEntered>()) {
                    auto u=te->unicode;
                    if (u>='0'&&u<='9') {   /// only digits
                        if (cropActiveField==0) 
                        cropWidthStr+=static_cast<char>(u);
                        else                      
                        cropHeightStr+=static_cast<char>(u);
                    }
                }
                continue;
            }

            /// load/save modal
            if (isTypingLoad || isTypingSave) {
                /// we check what key the user pressed within the modal
                if (const auto* k=ev->getIf<sf::Event::KeyPressed>()) {
                    if (k->code==sf::Keyboard::Key::Escape) {
                        isTypingLoad=isTypingSave=false; typedPath="";
                    } else if (k->code==sf::Keyboard::Key::Backspace && !typedPath.empty()) {
                        typedPath.pop_back();
                    } else if (k->code==sf::Keyboard::Key::Enter) {
                        if (isTypingLoad) {
                            try {/// loads image and adds it to the gallery 
                                Image img; 
                                img.load(typedPath);
                                gallery.push_back(img);
                                selectedIndex=(int)gallery.size() - 1;
                                needsUpdate=true;
                            } catch (const std::exception& e) {
                                std::cerr<<"LOAD error: "<<e.what()<<"\n";
                            }
                        } else { /// saves image to the gallery
                            if (!gallery.empty())
                                try { gallery[selectedIndex].saveImage(typedPath); }
                                catch (const std::exception& e) 
                                { std::cerr<<"SAVE error: "<<e.what()<<"\n"; }
                        }
                        isTypingLoad=isTypingSave=false; typedPath="";
                    }
                }
                if (const auto* te=ev->getIf<sf::Event::TextEntered>()) {
                    auto u=te->unicode;
                    /// checks if the paths introduced contain backspace(blocks it)
                    if (u>=32&&u<127&&u!=8)  
                        typedPath +=static_cast<char>(u);
                }
                continue; /// blocks the rest of the events while the modal is opened
            }

            /// the interface should only consider a mouse click 
            /// if it s a left-click
            if (const auto* mb=ev->getIf<sf::Event::MouseButtonPressed>()) {
                if (mb->button!=sf::Mouse::Button::Left) continue;
                sf::Vector2f m{static_cast<float>(mb->position.x),static_cast<float>(mb->position.y)};

                /// topbar buttons design
                auto topHit=[&](float x, float w) {
                    return sf::FloatRect({x,12.f}, {w,32.f}).contains(m); 
                };
                /// load button
                if (topHit(10.f, 150.f))  
                { isTypingLoad=true; 
                    typedPath=""; }
                /// save button
                if (topHit(180.f,150.f))  
                { isTypingSave=true;
                typedPath=gallery.empty() ? "" : gallery[selectedIndex].getFilename(); }
                /// gallery button
                if (topHit(350.f,150.f)) 
                { showGallery=!showGallery; }
          
               /// apply/cancel area available only in preview mode
                if (isPreviewActive) {
                    /// apply button 
                    if (sf::FloatRect({700.f,12.f},{113.f,34.f}).contains(m)) {
                        try {
                            std::unique_ptr<ImageEditor> f;
                            /// each case from filter factory
                            if (activeFilter=="Contrast"||activeFilter=="Grain"||activeFilter=="Blur")
                                f=FilterFactory::create(activeFilter, activeAmount);
                            else if (activeFilter=="Resize")
                                f = FilterFactory::create("Resize", activeAmount);
                            else if (activeFilter=="Crop")
                                f = FilterFactory::create("Crop",previewImage.getWidth(), previewImage.getHeight());
                            else
                                f=FilterFactory::create(activeFilter);
                            /// we apply the change permanently
                            cmdManager.executeCommand(std::make_unique<ApplyFilterCommand>(gallery[selectedIndex],std::move(f)));
                            /// we save the changes for the copy setting
                            lastAppliedFilter=activeFilter;
                            lastAppliedAmount=activeAmount;
                        } catch (const std::exception& e) 
                        { std::cerr<<e.what()<<"\n"; }
                        isPreviewActive=false; needsUpdate=true;
                    }
                    /// cancel button
                    if (sf::FloatRect({825.f,12.f},{113.f,34.f}).contains(m))
                        { isPreviewActive=false;
                        needsUpdate=true; }
                }

               ///bottombar buttons 
               /// lambda function to identify which button was pressed
                auto panelHit=[&](float ox,float w) 
                {return sf::FloatRect({PANEL_X+ox,718.f}, {w,30.f}).contains(m); };
                /// copy button
                if (panelHit(5.f, 78.f)) {   
                    clipboard.clear();
                    if (!activeFilter.empty()) {
                        /// identify filter type
                        int t=(activeFilter=="Contrast"||activeFilter=="Grain"||activeFilter=="Blur"
                        ||activeFilter=="Resize") ? 2 : 1;
                        clipboard.push_back(FilterSettings(lastAppliedFilter,t,lastAppliedAmount));
                        hasClipboard=true;
                    }
                }
                /// paste button
                if (panelHit(90.f,78.f)&&hasClipboard&&!gallery.empty()) { // PASTE
                    try {
                        cmdManager.executeCommand(
                        std::make_unique<Paste>(gallery[selectedIndex],clipboard));
                        needsUpdate=true;
                    } catch (const std::exception& e) {std::cerr<<e.what()<<"\n"; }
                }
                /// undo & reset buttons
                if (panelHit(175.f, 78.f)) { cmdManager.undo(); isPreviewActive=false; needsUpdate=true; }
                if (panelHit(260.f, 75.f) && !gallery.empty()) {
                    cmdManager.executeCommand(std::make_unique<Reset>(gallery[selectedIndex]));
                    isPreviewActive=false; needsUpdate=true;
                }

               /// accordion
                float colorHdrY, detailHdrY, sizeHdrY ,presetsHdrY, dcY=0,dgY=0,dbY=0, rsY=0, cropBtnY=0;
                float cbY[6]={}, pbY[6]={};
                layout(colorHdrY,detailHdrY,sizeHdrY,presetsHdrY,dcY,dgY,dbY,rsY,cropBtnY,cbY,pbY);

                /// color section
                if (sf::FloatRect({PANEL_X,colorHdrY},{340.f,38.f}).contains(m))
                    showColor=!showColor;

                /// if the section is opened we check what filter is presssed
                if (showColor) {
                    const char* cn[]={"BlackAndWhite","GoldenHour","OrangeAndTeal",
                                        "Retro","Pastels","MoodyBlues"};
                    for (int i=0;i<6;++i)
                        if (sf::FloatRect({PANEL_X+5.f,cbY[i]},{155.f,30.f}).contains(m)&&!gallery.empty()) {
                            previewImage=gallery[selectedIndex];
                            try {
                                FilterFactory::create(cn[i])->applySettings(previewImage);
                                activeFilter=cn[i]; 
                                activeAmount=0.0;
                                isPreviewActive=true; 
                                needsUpdate=true;
                            } catch (...) {}
                        }
                }

                /// detail section
                if (sf::FloatRect({PANEL_X,detailHdrY},{340.f,38.f}).contains(m))
                    showDetail=!showDetail;

                /// detail preview buttons
                if (showDetail) {
                    
                    struct {const char* name; float& slider; double minA, maxA; float baseY; } rows[]={
                        {"Contrast",contrastSlider,0.1,20.0,dcY},
                        {"Grain",grainSlider,0.1,20.0,dgY},
                        {"Blur",blurSlider,0.1,20.0,dbY},
                    };
                    for (auto& r:rows)
                        if (sf::FloatRect({PANEL_X+250.f,r.baseY+1.f},{85.f,22.f}).contains(m)&&!gallery.empty()) {
                            double amt=r.minA+(r.slider/100.f)*(r.maxA-r.minA);
                            previewImage=gallery[selectedIndex];
                            try {
                                FilterFactory::create(r.name,amt)->applySettings(previewImage);
                                activeFilter=r.name; activeAmount=amt;
                                isPreviewActive=true; needsUpdate=true;
                            } catch (...) {}
                        }
                }

                /// size section
                if (sf::FloatRect({PANEL_X,sizeHdrY},{HDR_W,38.f}).contains(m)) 
                    showSize=!showSize;
                if (showSize) {
                    /// resize preview
                    if (sf::FloatRect({PANEL_X+250.f,rsY+1.f},{85.f,22.f}).contains(m)&&!gallery.empty()) {
                        double amt=std::max(1.0,(double)resizeSlider);
                        previewImage=gallery[selectedIndex];
                        try {
                            FilterFactory::create("Resize",amt)->applySettings(previewImage);
                            activeFilter="Resize"; 
                            activeAmount=amt;
                            isPreviewActive=true; 
                            needsUpdate=true;
                            
                        } catch (const std::exception& e)
                         { std::cerr<<e.what()<<"\n"; }
                    }
                    // crop button which opens modal
                    if (sf::FloatRect({PANEL_X+5.f,cropBtnY},{BTN_W,BTN_H}).contains(m)&&!gallery.empty()) {
                        isTypingCrop=true; 
                        cropWidthStr=""; 
                        cropHeightStr=""; 
                        cropActiveField=0;
                    }
                }


                /// presets section
                if (sf::FloatRect({PANEL_X,presetsHdrY},{340.f,38.f}).contains(m))
                    showPresets=!showPresets;

                /// Preset buttons preview
                if (showPresets) {
                    const char* pn[]={"Vintage","Cinematic","Summer","Spring","Winter","Autumn"};
                    for (int i=0;i<6;++i)
                        if (sf::FloatRect({PANEL_X+5.f,pbY[i]},{155.f,30.f}).contains(m)&&!gallery.empty()) {
                            previewImage=gallery[selectedIndex];
                            try {
                                FilterFactory::create(pn[i])->applySettings(previewImage);
                                activeFilter=pn[i]; activeAmount=0.0;
                                isPreviewActive=true; needsUpdate=true;
                            } catch (...) {}
                        }
                }

                /// gallery section ; small thumbnails 
                if (showGallery)
                /// maximum eight images for a good visual aspect
                    for (int i=0;i<(int)gallery.size()&&i<8;++i)
                        if (sf::FloatRect({10.f+i*100.f,664.f},{90.f,72.f}).contains(m))
                            /// small rectangle for each image
                            { selectedIndex=i;
                             isPreviewActive=false; 
                             needsUpdate=true; }
            }
        } 

       
        if (needsUpdate&&!gallery.empty()) {
            /// if we are in preview mode we send a copy
            Image& toShow=isPreviewActive ? previewImage : gallery[selectedIndex];
            /// otherwise we apply the modifications and adjust the dimensions
            refreshTexture(mainTex,mainSpr,toShow,CANVAS,CANVAS_ORIG);
            needsUpdate=false;
        }

        /// graphics
        window.clear(sf::Color(27,27,32));

       /// canvas for photo
        sf::RectangleShape canvasBg({static_cast<float>(CANVAS.x), static_cast<float>(CANVAS.y)});
        canvasBg.setPosition(CANVAS_ORIG);
        canvasBg.setFillColor(sf::Color(8,8,8));
        window.draw(canvasBg);
        /// if the gallery is not empty we display the current photo
        if (!gallery.empty()) {
            window.draw(mainSpr);
        } else {
            /// if the gallery is empty we display this text
            sf::Text hint(font,"Press LOAD to open an image",25);
            hint.setFillColor(sf::Color(255,183,206));
            hint.setOrigin(hint.getLocalBounds().getCenter());
            hint.setPosition({static_cast<float>(CANVAS.x)/2.f,TOP_H+static_cast<float>(CANVAS.y)/2.f});
            window.draw(hint);
        }

        /// right pannel with filters
        sf::RectangleShape panel({350.f,760.f});
        panel.setPosition({PANEL_X-5.f,0.f});
        panel.setFillColor(sf::Color(120,0,59));
        panel.setOutlineThickness(1.f);
        panel.setOutlineColor(sf::Color(52,52,62));
        window.draw(panel);

        /// accordion section
        float colorHdrY, detailHdrY, sizeHdrY, presetsHdrY, dcY=0,dgY=0,dbY=0, rsY=0, cropBtnY=0;
        float cbY[6]={}, pbY[6]={};
        layout(colorHdrY, detailHdrY, sizeHdrY, presetsHdrY, dcY, dgY, dbY, rsY, cropBtnY, cbY, pbY);

        /// color filters
        drawHeader(window, font,"COLOR FILTERS",PANEL_X,colorHdrY,showColor,HDR_W);
        if (showColor) {
            struct { const char* lbl; sf::Color col; } 
            cb[] = {{"Black & White",{255,189,0}},{"Golden Hour",{255,189,0}},{"Orange & Teal",{255,189,0}},
            {"Retro",{255,189,0}},{"Pastels",{255,189,0}},{"Moody Blues",{255,189,0}}};
            for (int i=0;i<6;++i)
                drawButton(window,font,cb[i].lbl,PANEL_X+5.f,cbY[i],cb[i].col,BTN_W,BTN_H);
        }

        /// detail filters
        drawHeader(window,font,"DETAIL FILTERS",PANEL_X,detailHdrY,showDetail,HDR_W);
        if (showDetail) {
            contrastSlider=drawSlider(window,font,"Contrast",PANEL_X+5.f,dcY,contrastSlider,SLD_W,0.0,100.0);
            drawButton(window,font,"Preview",PANEL_X+250.f,dcY+1.f,{255,189,0},85.f,22.f);

            grainSlider=drawSlider(window,font,"Grain", PANEL_X+5.f,dgY,grainSlider,SLD_W,0.0,100.0);
            drawButton(window,font,"Preview",PANEL_X+250.f,dgY+1.f,{255,189,0},85.f,22.f);

            blurSlider=drawSlider(window,font,"Blur",PANEL_X+5.f,dbY,blurSlider,SLD_W,0.0,100.0);
            drawButton(window,font,"Preview",PANEL_X+250.f,dbY+1.f,{255,189,0},85.f,22.f);
        }

       /// size filters
        drawHeader(window,font,"SIZE FILTERS",PANEL_X,sizeHdrY,showSize,HDR_W);
        if (showSize) {
            resizeSlider=drawSlider(window,font,"Resize %",PANEL_X+5.f,rsY,resizeSlider,SLD_W,0.0,100.0);
            drawButton(window,font,"Preview",PANEL_X+250.f,rsY+1.f,{255,189,0},85.f,22.f);
            drawButton(window,font,"Crop",PANEL_X+5.f,cropBtnY,{255,189,0},BTN_W,BTN_H);
        }

        /// presets
        drawHeader(window,font,"PRESETS",PANEL_X,presetsHdrY,showPresets,HDR_W);
        if (showPresets) {
            struct { const char* lbl; sf::Color col;} 
            pb[]={{"Vintage",{255,189,0}},{"Cinematic",{255,189,0}},{"Summer",{255,189,0}},{"Spring",{255,189,0}},
                {"Winter",{255,189,0}},{"Autumn",{255,189,0}}};
            for (int i=0;i<6;++i)
                drawButton(window,font,pb[i].lbl,PANEL_X+5.f,pbY[i],pb[i].col,BTN_W,BTN_H);
        }

        /// bottom bar
        sf::RectangleShape botBar({350.f,44.f});
        botBar.setPosition({PANEL_X-5.f,716.f});
        botBar.setFillColor(sf::Color(255,183,206));
        window.draw(botBar);
        drawButton(window,font,"COPY",PANEL_X+5.f,722.f,{8,8,8},78.f,30.f);
        drawButton(window,font,"PASTE",PANEL_X+90.f,722.f,{8,8,8},78.f,30.f);
        drawButton(window,font,"UNDO", PANEL_X+175.f,722.f,{8,8,8},78.f,30.f);
        drawButton(window,font,"RESET",PANEL_X+260.f,722.f,{8,8,8},75.f,30.f);

        /// top bar
        sf::RectangleShape topBar({1200.f,TOP_H});
        topBar.setFillColor(sf::Color(255,183,206));
        topBar.setPosition({0.f,0.f});
        window.draw(topBar);
        /// top bar buttons
        drawButton(window,font,"LOAD",10.f,12.f,{8,8,8},150.f,36.f);
        drawButton(window,font,"SAVE",180.f,12.f,{8,8,8},150.f,36.f);
        drawButton(window,font,"GALLERY",350.f,12.f,{8,8,8},150.f,36.f);
        /// app title
        sf::Text appName(font,"PhotoEditor",25);
        appName.setFillColor(sf::Color(120,0,59));
        appName.setPosition({1050.f,12.f});
        window.draw(appName);

        /// 
        if (isPreviewActive) {
            /// preview mode
            sf::RectangleShape badge({150.f,36.f});
            badge.setPosition({525.f,12.f});
            badge.setFillColor(sf::Color(255,189,0));
            badge.setOutlineThickness(1.3f);
            badge.setOutlineColor(sf::Color(255,255,255));
            window.draw(badge);
            sf::Text bdgTxt(font,"PREVIEW MODE",18);
            bdgTxt.setFillColor(sf::Color(255,255,255));
            bdgTxt.setOrigin(bdgTxt.getLocalBounds().getCenter());
            bdgTxt.setPosition({605.f,28.f});
            window.draw(bdgTxt);

            drawButton(window,font,"APPLY",700.f,12.f,{28,98,38},113.f,34.f);
            drawButton(window,font,"CANCEL",825.f,12.f,{108,28,28},113.f,34.f);
        }

        /// gallery
        if (showGallery&&!gallery.empty()) {
            sf::RectangleShape gallBg({850.f,82.f});
            gallBg.setPosition({0.f,660.f});
            gallBg.setFillColor(sf::Color(18,18,23,220));
            window.draw(gallBg);
            for (int i=0;i<(int)gallery.size()&&i<8;++i) {
                /// rectangle that represents the image
                bool sel=(i==selectedIndex);
                sf::RectangleShape thumb({90.f,70.f});
                thumb.setPosition({10.f+i*100.f,663.f});
                thumb.setFillColor(sf::Color(120,0,59));
                thumb.setOutlineThickness(sel?2.f:1.f);
                thumb.setOutlineColor(sel?sf::Color(120,0,59):sf::Color(255,255,255));
                window.draw(thumb);
                /// image name max 11 characters
                std::string fn=gallery[i].getFilename();
                auto p=fn.find_last_of("/\\");
                if (p!=std::string::npos) 
                    fn=fn.substr(p+1);
                if (fn.size()>11) 
                    fn=fn.substr(0,10) + "...";
                /// centers text
                sf::Text fnTxt(font,fn,11);
                fnTxt.setFillColor(sf::Color(255,255,255));
                fnTxt.setOrigin(fnTxt.getLocalBounds().getCenter());
                fnTxt.setPosition({10.f+i*100.f+45.f, 727.f});
                window.draw(fnTxt);
            }
        }

        // Modal text input
        if (isTypingLoad||isTypingSave) 
            drawModal(window, font);
        if (isTypingCrop)
            drawCropModal(window,font);

        window.display();

    } 
}

#ifndef COMMAND_H
#define COMMAND_H
#include <iostream>
#include "ImageEditor.h"
#include "Image.h"
#include "FilterFactory.h"
#include<memory>
#include<vector>
#include<string>

/// COMMAND DESIGN PATTERN
/// turns a request or action into an object
/// abstract class/ interface for commands
class Command{
    public:
    virtual ~Command()=default;
    /// command applied
    virtual void execute()=0;
    /// reverse actions
    virtual void undo()=0;
};

/// concrete command : applying filters
class ApplyFilterCommand:public Command{
    private:
    Image& img;
    std::unique_ptr<ImageEditor> filter;
    /// back-up for undo
    std::vector<unsigned char> oldPixels;
    int oldWidth;
    int oldHeight;
    public:
    ApplyFilterCommand(Image& image, std::unique_ptr<ImageEditor> f):
    img(image),filter(std::move(f)), oldWidth(0), oldHeight(0){}
    void execute() override{
        /// copies old state and applies filter
        oldPixels=img.getPixels();
        oldWidth=img.getWidth();
        oldHeight=img.getHeight();
        if(filter)
        filter->applySettings(img);
        }
    /// sets attributes from before
    void undo() override{
        img.getPixels()=oldPixels;
        img.setWidth(oldWidth);
        img.setHeight(oldHeight);
    }

};
/// concrete command : reset
class Reset: public Command{
    private:
    Image& img;
    std::vector<unsigned char> oldPixels;
    int oldWidth;
    int oldHeight;
    public:
    Reset(Image& image ):
    img(image), oldWidth(0), oldHeight(0){}
    void execute() override{
        oldPixels=img.getPixels();
        oldWidth=img.getWidth();
        oldHeight=img.getHeight();
        /// fetches original data from the photo
        img.getPixels()=img.getOriginalPixels();
        img.setWidth(img.getOriginalWidth());
        img.setHeight(img.getOriginalHeight());
        }

    void undo() override{
        img.getPixels()=oldPixels;
        img.setWidth(oldWidth);
        img.setHeight(oldHeight);
    }

};

/// container that saves filter data useful for copy function
class FilterSettings{
    private:
    std::string name;
    double amount;
    int width;
    int height;
    int type;
    
    public:
    FilterSettings(std::string n, double a=0.0, int w=0, int h=0, int t=1):
    name(n),amount(a),width(w),height(h),type(t){}
    std::string getName() const {return name;}
    double getAmount() const {return amount;}
    int getWidth() const {return width;}
    int getHeight() const {return height;}
    int getType() const {return type;}
};

/// concrete command : paste
class Paste: public Command{
    private:
    Image& img;
    std::vector<FilterSettings> settings;
    std::vector<unsigned char> oldPixels;
    int oldWidth;
    int oldHeight;
    public:
    Paste(Image& image, const std::vector<FilterSettings>& s):
    img(image), settings(s), oldWidth(0), oldHeight(0){}
    void execute() override{
        oldPixels=img.getPixels();
        oldWidth=img.getWidth();
        oldHeight=img.getHeight();
        for(auto& s: settings){
            /// iterates through the copied settins
            std::unique_ptr<ImageEditor> temp=nullptr;
            /// looks through filter types to apply them
            if(s.getType()==1)
            temp=FilterFactory::create(s.getName());
            if(s.getType()==2)
            temp=FilterFactory::create(s.getName(),s.getAmount());
            if(s.getType()==3)
            temp=FilterFactory::create(s.getName(),s.getWidth(),s.getHeight());
            if(temp)
            temp->applySettings(img);


        }
    
    }
     void undo() override{
        img.getPixels()=oldPixels;
        img.setWidth(oldWidth);
        img.setHeight(oldHeight);
    }
};

/// execution flow and command history
class CommandManager{
    std::vector<std::unique_ptr<Command>> history;
    public:
    /// polymorphically executes commands and saves them in history
    void executeCommand(std::unique_ptr<Command> cmd){
        cmd->execute();
        history.push_back(std::move(cmd));
    }
    void undo(){
        if(!history.empty()){
            history.back()->undo();
            history.pop_back();
        }
    }

};
#endif

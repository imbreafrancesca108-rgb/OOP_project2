# PhotoEditor - OOP_project_2

## 1.Project Description
This project is a mini-version of a photo editing application , written in C++ and following OOP coding principles. It allows the user to apply various effects and features to images using a visual, minimalist SFML interface. The options include color adjustments, size modifications and detail enhancements, as well as 6 presets combining these settings. 

## 2. Class Structure

**Core classes**

### **|. Image**
- defines the object
- stores pixel data, dimensions and rgb channels
- includes load/save operations
- includes the rule of 3(copy constructor, assignment operator, destructor)
- contains static member to track the total of edited images
- it also inherits from Observable in order to notify the observers when it is being modified

### **||. ImageEditor**

- defines the interface for editing
- includes pure virtual methods:
##### ->applySettings() : modify the image according to each filter
##### ->getFilterName() : returns the description of the filter

 **Derived classes**
 
### **I. ColorFilters (derived from ImageEditor)**
Includes 6 chromatic transformations :

- **BlackAndWhite**: grayscale conversion using luminance formula
- **GoldenHour**: warmth-focused look, enhancing red and reducing blue
- **Retro**: old-style look using sepia transformation formula
- **OrangeAndTeal**: cinema-inspired color grading focused on warm highlights and cool-toned shadows
- **Pastels**: soft/dreamy color pallete enhancing pinks and greens
- **Moody-blues**: dark and cool tones

### **II. DetailFilters (derived from ImageEditor)**

Includes 3 quality modifiers:

- **Contrast**: adjust contrast with formula new_pixel = amount * (pixel - 128) + 128
- **Grain**: adds a vintage/film-like grain using noise
- **Blur**: adds a soft blur using 5*5 kernel average

### **III.SizeFilters (derived from ImageEditor)**

Includes 2 sizing tools:

- **Crop**: rescales the image to new dimensions
- **Resize**: scales the image based on percentage

### **IV.Diamond Inheritance**

Includes 6 presets combining both color and detail features, using **virtual inheritance** to solve the diamond problem:

- **Vintage** : BlackAndWhite + Grain+ Blur
- **Cinematic**: OrangeAndTeal + Grain
- **Summer**: GoldenHour+ high Contrast
- **Spring**: Pastels + Blur
- **Winter** : MoodyBlues + low Contrast
- **Autumn**: Retro+ Grain + high Contrast

### **V.Exceptions classes**
Defines 6 custom exceptions derived from std::runtime_error:

- **AmountException**: invalid detail parameter values
- **OpenPath**: file could not be found
- **SavePath**: saving path could not be found
- **CropException**: invalid crop dimensions
- **ResizeException**: invalid resizing percentage
- **OptionException**: invalid/non-existing editing filters

**Design patterns**

### **I.Command Pattern**
Encapsulates all image processing actions as objects and provides a history tracking:

- **Command**: interface; defines the 2 methods each action has: execute and undo
- **ApplyFilterCommand**: applies a specific filter to the image preserving its previous state for reversals
- **Reset**: allows the user to go back to the original state of the picture, while also supporting the undo operation
- **FilterSettings**: stores information in order to support the copy function
- **Paste**: using FilterSettings, it transfers the coppied information to the selected picture
- **CommandManager**: acts as a history tracker, managing the execution and reversal of the commands

### **II.State Pattern**
Manages the behavioral flow of the application depending on its current state:

- **State**: interface; declares the 2 methods each behaviour has: interaction with the user and preview cleanup
- **InitialState**: default mode with no operations
- **Preview**: state that maintains a backup of the image data and allows the user to see or clear the preview for the selected settings

### **III.Factory Pattern**
Centralizes the creation logic for filters and provides static methods to generate a specific filter based on input parameters. There are 3 categories, split in : no-parameter filters (color & presets), one-parameter filters (detail & resize) and two-parameter filters ( crop ), while also taking into consideration the exceptions defined.

### **III.Observer Pattern**
Notifies multiple observers on any changes made to the observable object (Image). It ensures that whenever an image is altered , the user interface is automatically updated to reflect that modifications.

- **Observer**: template interface which provides an update method 
- **Observable** : template base class; manages the registration and cycle of the observers providing methods to add, remove and notify the registered observers


## **3.Technical Implementation**
All classes are split into .h and .cpp files.
#### **Diamond inheritance**: presets inherit form both 'ColorFilters' and 'DetailFilters' using **virtual public**.

#### SFML Graphical Interface & Rendering Logic

- the application used SFML 3.0 framework to provide a responsive and visual user interface
- the Menu class implements a loop that manages event polling, logic updates and frame rendering
- images are dynamic texture (sf::Texture) being modified through the refreshTexture function which transforms the pixels into a renderable sf::Image; refreshTexture(...) raw pixel data into an sf::Texture handling the conversion from arbitrary channel counts (RGB) to the RGBA format required by SFML by injecting a constant alpha channel (255)
- the pollEvent loop differents between interface states to provide the specific functions
- each button and feature captures mouseclicks in its preserved space
- the interface offers a chromatic harmony for the user
- drawButton(...) & drawHeader(...) encapsulate the creation of sf::RectangleShape and sf::Text and return a bool 
- drawSlider(...) calculates the mouse position relative to the slider track and uses std::clamp to map the pixel position back to a value between minA and maxA, providing a smooth "drag-and-drop" experience.
- drawModal(...) & drawCropModal(...) simulate an overlay system, creating a "blocking" UI effect; the input logic here separates the TextEntered event (for typing) from the general KeyPressed event (for Enter/Escape), ensuring that typing in a modal doesn't trigger unrelated keyboard shortcuts elsewhere

##### Memory observations
- unique pointers and used in FilterFactory and Command Manager in order to not manually delete a filter after its creation and we use std::move() to transfer the ownership from one scop to another since the ownership for unique pointers is exclusive
  
##### Static implementation & Singleton Pattern

- static int totalEditedImages in Image class which counts all saved images across the application
- static int getEditedImages() which returns the total count
- static Menu* instance in Menu class which implements the Singleton
- the Menu class is implemented as a Singleton to ensure a single instance being created; it consists of classes which implement the UI elements: buttons, accordion headers, sliders and modals , a function which alters the image , a run() method which ensures the core execution loop and the update method from the observer

#### Encapsulation and Resource Management
##### Access Modifiers (private, protected, public):

- **private** ->Image class members (width, height, pixels) - only accessible within Image class
- **protected** -> detail filter parameters (amountGrain, amountBlur,amountContrast)-to be accessible by derived classes in case there will be more made (HighContrast for example)
- **public** ->Interface methods (applySettings, getFilterName, load, save) - accessible to all

#### Rule of Three
Implemented in `Image` class:
- **Copy constructor** - Image(const Image& other) - deep copy of pixel data
- **Assignment operator** - Image& operator=(const Image& other) - prevents self-assignment, copies all members
- **Destructor** - ~Image()- cleans up resources

#### std::vector Usage

- std::vector<unsigned char> pixels - stores raw pixel data in Image class
- std::vector<Image> gallery - stores multiple images in Menu class

Vector manages memory automatically (no manual new/delete)

### Polymorphism (Virtual Methods & Abstract Classes)

#### Abstract Class
ImageEditor is an abstract class with pure virtual methods and many virtual functions throughout the programme which allow the system to treat different filter objects (BlackAndWhite, Contrast, Blur, etc.) uniformly through the same interface while they maintain their unique behaviors. 

## **4.Workflow**
When you run the application, the system opens the graphic interface created and waits for the user input. There is a text displayed which indicates that the user should upload an image. The top bar display 3 buttons: load, save and galerry. After loading an image, the user can use to apply any filter that is displayed in the right panne, in 4 accordion section: color, detail, size and presets. Every filter can be previewed and there will be two buttons appearing in the top bar which tell the user to either apply the modifications or cancel the action. The bottom bar consists of 4 buttons: copy, paste, undo and reset. You can also press the gallery button and select which photo you want to edit from the ones uploaded in the application. Crop and load/save options activate a small pop-up window for the user to input his/hers data. 


## **5.Additional mentions**
#### External Libraries
**- stb_image.h** - Image loading (supports JPG, PNG, BMP, TGA)

**- stb_image_write.h** - Image saving (JPG, PNG, BMP, TGA)

**- SFML 3.0** - UI elements

#### Compile & build instructions
This project uses SFML for graphics. To compile and run the application, you need to have the SFML development libraries installed.

Since this project was built using the MSYS2/MinGW environment, the easiest way to satisfy dependencies is via pacman:

**-Install SFML**: Open your MSYS2 terminal (typically UCRT64) and run:
pacman -S mingw-w64-ucrt-x86_64-sfml

**-Compile**: Use the following command to link the required SFML modules:
g++ *.cpp -o App -lsfml-graphics -lsfml-window -lsfml-system

**-Run**: Execute the generated binary:
./App.exe

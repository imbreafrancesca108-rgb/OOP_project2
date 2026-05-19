#include<iostream>
#include<vector>
#include<cstdlib>
#include "Image.h"
#include "Exceptions.h"
#include "Menu.h"
#include "Presets.h"

int main() {
    ///fixes the "frozen" bug on Ubuntu because it waits for std::cin
    if (std::getenv("GITHUB_ACTIONS")!=nullptr) {
        std::cout<<"Running on GitHub."<<std::endl;
        return 0;
    }
    Menu::getInstance()->run();

    return 0;
}

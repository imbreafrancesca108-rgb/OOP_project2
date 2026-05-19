#ifndef FILTERFACTORY_H
#define FILTERFACTORY_H
#include <iostream>
#include "ImageEditor.h"
#include "ColorFilters.h"
#include "DetailFilters.h"
#include "SizeFilters.h"
#include "Presets.h"
#include<memory>
#include<string>

/// FACTORY DESIGN PATTERN
class FilterFactory{
    public:
    /// simple filters with no parameters ( color & presets)
    static std::unique_ptr<ImageEditor> create(const std::string& filter);
    /// filters with sliders (resize & detail )
    static std::unique_ptr<ImageEditor> create(const std::string& filter, double amount);
    /// filters with 2 dimensions ( only crop )
    static std::unique_ptr<ImageEditor> create(const std::string& filter, int newWidth, int newHeight);
};
#endif
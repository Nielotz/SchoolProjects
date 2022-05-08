#include "headers/color.h"

color::RGB color::RGB::operator+(const RGB& other) const
{
    return RGB(this->red + other.red, this->green + other.green, this->blue + other.blue);
}

color::RGBA color::RGBA::operator+(const RGBA& other) const
{
    return RGBA{ this->red + other.red, this->green + other.green, this->blue + other.blue, this->alfa + other.alfa };
}
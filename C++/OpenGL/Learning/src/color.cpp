#include "headers/color.h"

color::RGB color::RGB::operator+(const RGB& other) const
{
    return RGB(this->red + other.red, this->green + other.green, this->blue + other.blue);
}

#pragma once


namespace color
{
    struct RGB
    {
        float red = 0.f;
        float green = 1.f;
        float blue = 0.f;

        RGB operator+(const RGB& other) const;
    };

    const RGB kRedRGB = { 1.f, 0.f, 0.f };
    const RGB kGreenRGB = { 0.f, 1.f, 0.f };
    const RGB kBlueRGB = { 0.f, 0.f, 1.f };
    const RGB kYellowRGB = kRedRGB + kGreenRGB;
    const RGB kPurpleRGB = kRedRGB + kBlueRGB;
}
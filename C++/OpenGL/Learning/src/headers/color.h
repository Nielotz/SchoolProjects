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

    struct RGBA
    {
        float red = 0.f;
        float green = 1.f;
        float blue = 0.f;
        float alfa = 0.f;

        RGBA operator+(const RGBA& other) const;
    };
     
    const RGBA kRedRGBA = { 1.f, 0.f, 0.f };
    const RGBA kGreenRGBA = { 0.f, 1.f, 0.f };
    const RGBA kBlueRGBA = { 0.f, 0.f, 1.f };
    const RGBA kYellowRGBA = kRedRGBA + kGreenRGBA;
    const RGBA kPurpleRGBA = kRedRGBA + kBlueRGBA;
}
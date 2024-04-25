#include "../public/TriangleColor.h"

TriangleColor::TriangleColor(float Hues[3])
{
    for (int i = 0; i < 3; i++)
    {
        ColorData_[i] = HSV2RGB(Hues[i]);
    }
}

DirectX::XMFLOAT4 TriangleColor::HSV2RGB(float Hue)
{
    float Saturation = 1.0;
    float Value = 1.0;
    float C = Value * Saturation;
    float X = C*(1-abs(fmod(Hue/60, 2) - 1));
    float R, G, B;
    
    if(Hue >= 0 && Hue < 60){
        R = C,G = X,B = 0;
    }
    else if(Hue >= 60 && Hue < 120){
        R = X,G = C,B = 0;
    }
    else if(Hue >= 120 && Hue < 180){
        R = 0,G = C,B = X;
    }
    else if(Hue >= 180 && Hue < 240){
        R = 0,G = X,B = C;
    }
    else if(Hue >= 240 && Hue < 300){
        R = X,G = 0,B = C;
    }
    else{
        R = C,G = 0,B = X;
    }
    
    return DirectX::XMFLOAT4(R, G, B, 1.0f);
}

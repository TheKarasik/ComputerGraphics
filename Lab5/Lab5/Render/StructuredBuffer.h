#pragma once
#include "AbstractBuffer.h"

template <typename T>
class StructuredBuffer : public  AbstractBuffer
{
public:
    StructuredBuffer(Renderer* renderer, unsigned short size);
    void UpdateBuffer(T* raw_data);
    ID3D11ShaderResourceView** SRV() {return &SRV_;}
private:
    ID3D11ShaderResourceView* SRV_;
};

#include "StructuredBuffer.hpp"

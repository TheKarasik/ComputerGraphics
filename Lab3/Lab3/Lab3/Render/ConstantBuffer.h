#pragma once
#include "AbstractBuffer.h"
#include "Structs.h"

class ConstantBuffer : public  AbstractBuffer
{
public:
    ConstantBuffer(Renderer* renderer, unsigned int sizeof_structure);
    void UpdateBuffer(void* raw_data);
private:
    D3D11_MAPPED_SUBRESOURCE res = {};
};

//#include "ConstantBuffer.hpp"
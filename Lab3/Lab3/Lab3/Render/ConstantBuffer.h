#pragma once
#include "AbstractBuffer.h"
#include "Structs.h"

class ConstantBuffer : public  AbstractBuffer
{
public:
    ConstantBuffer(Renderer* renderer, unsigned int sizeof_structure);
    void UpdateBuffer(ConstantDataVertexShader* raw_data);
private:
    
};

//#include "ConstantBuffer.hpp"
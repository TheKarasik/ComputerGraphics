#pragma once
#include "AbstractBuffer.h"
#include "Structs.h"
template <typename T>
class ConstantBuffer : public  AbstractBuffer
{
public:
    ConstantBuffer(Renderer* renderer);
    void UpdateBuffer(T* raw_data);
private:
    
};

#include "ConstantBuffer.hpp"

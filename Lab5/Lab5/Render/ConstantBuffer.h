#pragma once
#include "AbstractBuffer.h"
template <typename T>
class ConstantBuffer : public  AbstractBuffer
{
public:
    ConstantBuffer(Renderer* renderer);
    void UpdateBuffer(T* raw_data);
    //static ConstantBuffer<T> GetStagingBuffer();
private:
    
};

#include "ConstantBuffer.hpp"

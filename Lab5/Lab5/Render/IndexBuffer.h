#pragma once
#include "AbstractBuffer.h"

class IndexBuffer : public  AbstractBuffer
{
public:
    IndexBuffer(Renderer* renderer, void* data, unsigned int bytewidth);
};

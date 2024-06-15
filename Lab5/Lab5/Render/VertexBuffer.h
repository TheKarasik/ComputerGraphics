#pragma once
#include "AbstractBuffer.h"

class VertexBuffer : public  AbstractBuffer
{
public:
    VertexBuffer(Renderer* renderer, void* data, unsigned int bytewidth);
};

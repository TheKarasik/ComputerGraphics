#include "IndexBuffer.h"

IndexBuffer::IndexBuffer(Renderer* renderer, void* data, unsigned int bytewidth) : AbstractBuffer(renderer)
{
    usage_ = D3D11_USAGE_DEFAULT;
    buffer_type_ = D3D11_BIND_INDEX_BUFFER;
    raw_data_ = data;
    bytewidth_ = bytewidth;
    
    AbstractBuffer::CreateBuffer();
}

#include "VertexBuffer.h"
VertexBuffer::VertexBuffer(Renderer* renderer, void* data, unsigned int bytewidth) : AbstractBuffer(renderer)
{
    usage_ = D3D11_USAGE_DEFAULT;
    buffer_type_ = D3D11_BIND_VERTEX_BUFFER;
    raw_data_ = data;
    bytewidth_ = bytewidth;
    
    CreateBuffer();
}

#include "ConstantBuffer.h"

#include "Renderer.h"

/*template
ConstantBuffer<ConstantDataVertexShader>::ConstantBuffer(Renderer* renderer);

template
void ConstantBuffer<ConstantDataVertexShader>::UpdateBuffer(ConstantDataVertexShader* raw_data);*/

ConstantBuffer::ConstantBuffer(Renderer* renderer, unsigned int sizeof_structure) : AbstractBuffer(renderer)
{
    usage_ = D3D11_USAGE_DYNAMIC;
    buffer_type_ = D3D11_BIND_CONSTANT_BUFFER;
    CPU_acess_flag_ = D3D11_CPU_ACCESS_WRITE;
    raw_data_ = nullptr;
    bytewidth_ = sizeof_structure;
    
    CreateBuffer();
    renderer_->device()->CreateBuffer
        (&buffer_desc_, nullptr, &buffer_);
}

void ConstantBuffer::UpdateBuffer(void* raw_data)
{
    renderer_->Context()->Map(buffer_, 0, D3D11_MAP_WRITE_DISCARD, 0, &res);
    //auto dataPtr = static_cast<T*>(res.pData);
    memcpy(res.pData, &raw_data, bytewidth_);
    renderer_->Context()->Unmap(buffer_, 0);
}



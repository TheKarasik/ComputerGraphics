#include "Renderer.h"

template <typename T>
StructuredBuffer<T>::StructuredBuffer(Renderer* renderer, unsigned short size) : AbstractBuffer(renderer)
{
    if (!size) return ;
    
    usage_ = D3D11_USAGE_DYNAMIC;
    buffer_type_ = D3D11_BIND_SHADER_RESOURCE;
    CPU_acess_flag_ = D3D11_CPU_ACCESS_WRITE;
    misc_flag_ = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
    structured_byte_stride_ = sizeof T ;
    bytewidth_ = structured_byte_stride_ * size;
    raw_data_ = nullptr;
    
    CreateBuffer();

    D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc;
    srv_desc.Format = DXGI_FORMAT_UNKNOWN;
    srv_desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
    srv_desc.Buffer.FirstElement = 0;
    srv_desc.Buffer.NumElements = size;
    if(FAILED(renderer_->device()->CreateShaderResourceView(buffer_.Get(), &srv_desc, &SRV_)))
        return;
}

template <typename T>
void StructuredBuffer<T>::UpdateBuffer(T* raw_data)
{
    D3D11_MAPPED_SUBRESOURCE res;
    if (FAILED(renderer_->Context()->Map(buffer_.Get(),0, D3D11_MAP_WRITE_DISCARD, 0, &res))) return;
    size_t sizeInBytes = bytewidth_;
    memcpy_s(res.pData, sizeInBytes, raw_data, sizeInBytes);
    renderer_->Context()->Unmap(buffer_.Get(), 0);
    //renderer_->Context()->PSSetShaderResources(1, 1, &SRV);
}

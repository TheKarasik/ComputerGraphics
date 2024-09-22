#pragma once
#include "Renderer.h"

template <typename T>
UnorderedAccessView<T>::UnorderedAccessView(Renderer* renderer, unsigned int size) : AbstractBuffer(renderer)
{
    if (!size) return ;
    //buffer_ = static_cast<ID3D11Buffer*>(malloc(sizeof ID3D11Buffer));
    //buffer_ = new ID3D11Buffer{};
    
    D3D11_BUFFER_DESC stagingDesc;
    ZeroMemory(&stagingDesc, sizeof(stagingDesc));
    stagingDesc.Usage = D3D11_USAGE_STAGING;
    stagingDesc.ByteWidth = sizeof(UINT);
    stagingDesc.BindFlags = 0;
    stagingDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    renderer_->device()->CreateBuffer(&stagingDesc, nullptr, &StagingBuffer);

    //CB_count = new ConstantBuffer<Count>(renderer_);
    
    usage_ = D3D11_USAGE_DEFAULT;
    buffer_type_ = static_cast<D3D11_BIND_FLAG>(D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS);
    CPU_acess_flag_ = 0;
    misc_flag_ = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
    structured_byte_stride_ = sizeof T ;
    bytewidth_ = structured_byte_stride_ * size;
    raw_data_ = nullptr;

    //CreateBuffer();
}

template <typename T>
UnorderedAccessView<T>::UnorderedAccessView(Renderer* renderer, UnorderedAccessView* uav) : AbstractBuffer(renderer)
{
    if (!uav) return ;

    buffer_ = uav->buffer();
    structured_byte_stride_ = uav->structured_byte_stride_ ;
    bytewidth_ = uav->bytewidth_;
    
}

template <typename T>
void UnorderedAccessView<T>::UpdateBuffer(T* raw_data)
{
    renderer_->Context()->UpdateSubresource(buffer_.Get(), 0, nullptr, raw_data, 0, 0);
}

template <typename T>
void UnorderedAccessView<T>::CreateBuffer()
{
    AbstractBuffer::CreateBuffer();
    
    CreateUAV();
}

template <typename T>
void UnorderedAccessView<T>::CreateUAV()
{
    D3D11_UNORDERED_ACCESS_VIEW_DESC uav_desc_;
    uav_desc_.Format = UAV_format;
    uav_desc_.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
    uav_desc_.Buffer.FirstElement = 0;
    uav_desc_.Buffer.NumElements = bytewidth_ / structured_byte_stride_;
    uav_desc_.Buffer.Flags = UAV_flag;

    renderer_->device()->CreateUnorderedAccessView(buffer_.Get(), &uav_desc_, &UAV_);
}



template <typename T>
int UnorderedAccessView<T>::GetCount()
{
    renderer_->Context()->CopyStructureCount(StagingBuffer, 0, UAV_);
    D3D11_MAPPED_SUBRESOURCE mapped_subresource;
    renderer_->Context()->Map(StagingBuffer, 0, D3D11_MAP_READ, 0, &mapped_subresource);
    int count = *(int*)mapped_subresource.pData;
    renderer_->Context()->Unmap(StagingBuffer, 0);
    return count;
}

//#include "ConstantBuffer.h"
#pragma once
#include "Renderer.h"

/*template
ConstantBuffer<ConstantDataVertexShader>::ConstantBuffer(Renderer* renderer);

template
void ConstantBuffer<ConstantDataVertexShader>::UpdateBuffer(ConstantDataVertexShader* raw_data);*/
template <typename T>
ConstantBuffer<T>::ConstantBuffer(Renderer* renderer) : AbstractBuffer(renderer)
{
    /*usage_ = D3D11_USAGE_DYNAMIC;
    buffer_type_ = D3D11_BIND_CONSTANT_BUFFER;
    CPU_acess_flag_ = D3D11_CPU_ACCESS_WRITE;
    raw_data_ = nullptr;
    bytewidth_ = sizeof_structure;*/
    
    usage_ = D3D11_USAGE_DEFAULT;
    buffer_type_ = D3D11_BIND_CONSTANT_BUFFER;
    CPU_acess_flag_ = 0;
    raw_data_ = nullptr;
    bytewidth_ = sizeof T;
    
    CreateBuffer();
    //renderer_->device()->CreateBuffer(&buffer_desc_, nullptr, &buffer_);
    //enderer_->Context()->VSSetConstantBuffers(0, 1, &buffer_);
}
template <typename T>
void ConstantBuffer<T>::UpdateBuffer(T* raw_data)
{
    /*D3D11_MAPPED_SUBRESOURCE res = {};
    renderer_->Context()->Map(buffer_, 0, D3D11_MAP_WRITE_DISCARD, 0, &res);
    auto dataPtr = reinterpret_cast<TransformCBStruct*> (res.pData);
    //memcpy(dataPtr, &raw_data, bytewidth_);
    dataPtr->offset = raw_data->offset;
    dataPtr->size = raw_data->size;
    renderer_->Context()->Unmap(buffer_, 0);*/
    
    renderer_->Context()->UpdateSubresource(buffer_.Get(), 0, nullptr, raw_data, 0, 0);
}

/*template <typename T>
ConstantBuffer<T> ConstantBuffer<T>::GetStagingBuffer()
{
    D3D11_BUFFER_DESC stagingDesc;
    ZeroMemory(&stagingDesc, sizeof(stagingDesc));
    stagingDesc.Usage = D3D11_USAGE_STAGING;
    stagingDesc.ByteWidth = sizeof(UINT);
    stagingDesc.BindFlags = 0;
    stagingDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

}*/


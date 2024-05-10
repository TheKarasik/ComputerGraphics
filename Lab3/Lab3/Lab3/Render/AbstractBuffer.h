#pragma once
#include <d3d11.h>

#include "../../packages/directxtk_uwp.2024.2.22.1/include/SimpleMath.h"
class Renderer;


class AbstractBuffer
{
public:
    virtual ~AbstractBuffer() = default;
    ID3D11Buffer** buffer() {return &buffer_;}
protected:
    AbstractBuffer(Renderer* renderer);
    virtual void CreateBuffer() final;
    
    D3D11_BUFFER_DESC buffer_desc_{};
    D3D11_SUBRESOURCE_DATA data_{};
    ID3D11Buffer* buffer_ = nullptr;
    Renderer* renderer_;

    D3D11_USAGE usage_;
    D3D11_BIND_FLAG buffer_type_;
    UINT CPU_acess_flag_ = 0;
    UINT bytewidth_;
    void* raw_data_;
};

//#include "Buffer.hpp"

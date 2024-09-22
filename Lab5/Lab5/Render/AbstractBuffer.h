#pragma once
#include <d3d11.h>
#include <wrl/client.h>

#include "SimpleMath.h"
class Renderer;


class AbstractBuffer
{
public:
    virtual ~AbstractBuffer() = default;
    ID3D11Buffer** p_buffer() {return buffer_.GetAddressOf();}
    ID3D11Buffer* buffer() {return buffer_.Get();}
    //Microsoft::WRL::ComPtr<ID3D11Buffer> buffer() {return  buffer_;}
    virtual void CreateBuffer();
    
    D3D11_USAGE usage_;
    D3D11_BIND_FLAG buffer_type_;
    UINT structured_byte_stride_ = 0;
    UINT misc_flag_ = 0;
    UINT CPU_acess_flag_ = 0;
    UINT bytewidth_;
    void* raw_data_;
protected:
    AbstractBuffer(Renderer* renderer);
    Microsoft::WRL::ComPtr<ID3D11Buffer> buffer_;
    //ID3D11Buffer* buffer_ = nullptr;
    
    D3D11_BUFFER_DESC buffer_desc_{};
    D3D11_SUBRESOURCE_DATA data_{};
    Renderer* renderer_;

};

//#include "Buffer.hpp"

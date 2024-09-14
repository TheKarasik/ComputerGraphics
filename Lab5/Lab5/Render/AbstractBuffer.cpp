#pragma once
#include "AbstractBuffer.h"
#include "Structs.h"
#include "Renderer.h"

/*template 
AbstractBuffer<ConstantDataVertexShader>::AbstractBuffer(Renderer* renderer);

template
void AbstractBuffer<ConstantDataVertexShader>::CreateBuffer();*/


AbstractBuffer::AbstractBuffer(Renderer* renderer) : renderer_(renderer) {  }

void AbstractBuffer::CreateBuffer()
{
    buffer_desc_.Usage = usage_;
    buffer_desc_.BindFlags = buffer_type_;
    buffer_desc_.CPUAccessFlags = CPU_acess_flag_;
    buffer_desc_.MiscFlags = misc_flag_;
    buffer_desc_.StructureByteStride = structured_byte_stride_;
    buffer_desc_.ByteWidth = bytewidth_;
    if (!raw_data_)
    {
        //renderer_->device()->CreateBuffer(&buffer_desc_, &data_, &buffer_);
        renderer_->device()->CreateBuffer(&buffer_desc_, nullptr, &buffer_);
        return;
    }
    data_.pSysMem = raw_data_; 
    data_.SysMemPitch = 0;
    data_.SysMemSlicePitch = 0;
    
    renderer_->device()->CreateBuffer(&buffer_desc_, &data_, &buffer_);

    //UpdateBuffer(raw_data_);
}


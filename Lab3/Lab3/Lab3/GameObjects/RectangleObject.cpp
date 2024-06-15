#include "RectangleObject.h"
//#include "AbstractBuffer.h"
#include <random>

#include "ConstantBuffer.h"
#include "Renderer.h"
#include "Triangle.h"

RectangleObject::RectangleObject(Renderer* renderer, RectangleGeometry rect) :
    Drawable(renderer), rect_(rect),
    triangles_{new Triangle(renderer, TriangleGeometry{
    TriangleVertex{DirectX::XMFLOAT4(1.0, 1.0, 0.5, 1.0),
    DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)},
    TriangleVertex{DirectX::XMFLOAT4(1.0, -1.0, 0.5, 1.0),
    DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)},
    TriangleVertex{DirectX::XMFLOAT4(-1.0, -1.0, 0.5, 1.0),
    DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)}}),
    new Triangle(renderer, TriangleGeometry{
    TriangleVertex{DirectX::XMFLOAT4(-1.0, -1.0, 0.5, 1.0),
    DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)},
    TriangleVertex{DirectX::XMFLOAT4(1.0, 1.0, 0.5, 1.0),
    DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)},
    TriangleVertex{DirectX::XMFLOAT4(-1.0, 1.0, 0.5, 1.0),
    DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)}})}
{
    constant_buffer_ = new ConstantBuffer(renderer_, sizeof ConstantDataVertexShader);
    SetupBuffer();
}

RectangleObject::RectangleObject(Renderer* renderer) : Drawable(renderer)
{
    constant_buffer_ = new ConstantBuffer(renderer_, sizeof ConstantDataVertexShader);
    SetupBuffer();
}

RectangleObject::~RectangleObject()
{
    (*constant_buffer_->buffer())->Release();
    delete[] triangles_;
}

void RectangleObject::GenerateContextBufferData()
{
    data_.offset = DirectX::XMFLOAT4( rect_.pos.x, rect_.pos.y, 0, 0);
    data_.size = DirectX::XMFLOAT4( rect_.size.x, rect_.size.y, 0, 0);
}

void RectangleObject::update(float elapsed)
{
    SetupBuffer();
}

void RectangleObject::draw()
{
    //constant_buffer_->UpdateBuffer(&data_);
    renderer_->Context()->VSSetConstantBuffers(0, 1, constant_buffer_->buffer());
    for(auto triangle : triangles_)
    {
        //constant_buffer_->UpdateBuffer(&data_);
        triangle->draw();
    }
}

void RectangleObject::SetupBuffer()
{
    GenerateContextBufferData();
    constant_buffer_->UpdateBuffer(&data_);
}

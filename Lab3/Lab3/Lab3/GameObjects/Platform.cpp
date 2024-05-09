#include "Platform.h"
//#include "AbstractBuffer.h"
#include "ConstantBuffer.h"
#include "Renderer.h"



Platform::Platform(Renderer& renderer, float x_pos, float height) :
    GameComponent(renderer), x_pos_(x_pos), height_(height),
    triangles_{Triangle(renderer, TriangleGeometry{
    TriangleVertex{DirectX::XMFLOAT4(0.5, 0.5, 0.5, 1.0),
    DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)},
    TriangleVertex{DirectX::XMFLOAT4(0.5, -0.5, 0.5, 1.0),
    DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)},
    TriangleVertex{DirectX::XMFLOAT4(-0.5, -0.5, 0.5, 1.0),
    DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)}}),
    Triangle(renderer, TriangleGeometry{
    TriangleVertex{DirectX::XMFLOAT4(-0.5, -0.5, 0.5, 1.0),
    DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)},
    TriangleVertex{DirectX::XMFLOAT4(0.5, 0.5, 0.5, 1.0),
    DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)},
    TriangleVertex{DirectX::XMFLOAT4(-0.5, 0.5, 0.5, 1.0),
    DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)}})}
    
{
    constant_buffer_ = new ConstantBuffer(&renderer_, sizeof ConstantDataVertexShader);
    //GenerateRawData();
    update(&y_pos_);
}

void Platform::GenerateContextBufferData()
{
    data_.offset = DirectX::XMFLOAT4( x_pos_, y_pos_, 0, 0);
    data_.size = DirectX::XMFLOAT4( width_, height_, 1, 1);
    /*
    triangle_geometries_[0] = TriangleGeometry{
        TriangleVertex{DirectX::XMFLOAT4(x_pos_ - width_/2, y_pos_ + height_/2, 0.5, 1.0),
        DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)},
        TriangleVertex{DirectX::XMFLOAT4(x_pos_ + width_/2, y_pos_ + height_/2, 0.5, 1.0),
        DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)},
        TriangleVertex{DirectX::XMFLOAT4(x_pos_ - width_/2, y_pos_ - height_/2, 0.5, 1.0),
        DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)}};

    triangle_geometries_[1] = TriangleGeometry{
        TriangleVertex{DirectX::XMFLOAT4(x_pos_ + width_/2, y_pos_ - height_/2, 0.5, 1.0),
        DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)},
        TriangleVertex{DirectX::XMFLOAT4(x_pos_ + width_/2, y_pos_ + height_/2, 0.5, 1.0),
        DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)},
        TriangleVertex{DirectX::XMFLOAT4(x_pos_ - width_/2, y_pos_ - height_/2, 0.5, 1.0),
        DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)}};
        */
}

void Platform::update(float* y_pos)
{
    y_pos_ = *y_pos;
    GenerateContextBufferData();
    constant_buffer_->UpdateBuffer(&data_);
}

void Platform::draw()
{
    renderer_.Context()->VSSetConstantBuffers(0, 1, constant_buffer_->buffer());
    for(auto triangle : triangles_)
    {
        triangle.draw();
    }
}

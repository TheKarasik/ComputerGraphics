#include "Triangle.h"
#include "ConstantBuffer.h"
#include "IndexBuffer.h"
#include "VertexBuffer.h"
#include "Renderer.h"



Triangle::Triangle(Renderer& renderer) : GameComponent(renderer)
{
    triangle_ = TriangleGeometry{
        TriangleVertex{DirectX::XMFLOAT4(-0.5, -0.5, 0.5, 1.0),
        DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)},
        TriangleVertex{DirectX::XMFLOAT4(0.5, -0.5, 0.5, 1.0),
        DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)},
        TriangleVertex{DirectX::XMFLOAT4(0.0, 1.0, 0.5, 1.0),
        DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)}};
    
    Initialize();
}

Triangle::Triangle(Renderer& renderer, TriangleGeometry triangle) : GameComponent(renderer), triangle_(triangle)
{
    Initialize();
}

Triangle& Triangle::operator=(const Triangle& triangle)
{
    vertex_buffer_ = triangle.vertex_buffer_;
    index_buffer_ = triangle.index_buffer_;
    triangle_ = triangle.triangle_;
    strides = triangle.strides;
    offset = triangle.offset;
    return *this;
}

void Triangle::draw()
{
    //ConstantBuffer<ConstantDataVertexShader>* cb = renderer_.constant_buffer();
    ID3D11Buffer* const* vb = vertex_buffer_->buffer();
    renderer_.Context()->IASetIndexBuffer(*index_buffer_->buffer(),  DXGI_FORMAT_R32_UINT, 0);
    renderer_.Context()->IASetVertexBuffers(0, 1, vertex_buffer_->buffer(), &strides, &offset);
    
    renderer_.Context()->DrawIndexed(3, 0, 0);
}

void Triangle::update(TriangleGeometry* raw_data)
{
    //vertex_buffer_->UpdateBuffer(raw_data);
    //renderer_.constant_buffer()->UpdateBuffer(raw_data);
    
}

void Triangle::Initialize()
{
    int indecies[] = {0, 1, 2};
    vertex_buffer_ = new VertexBuffer(&renderer_, &triangle_, sizeof TriangleGeometry);
    index_buffer_ = new IndexBuffer(&renderer_, indecies, 3 * sizeof(int));
    strides = (sizeof TriangleVertex) * 3;
}

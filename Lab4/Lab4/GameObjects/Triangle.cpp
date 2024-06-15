#include "Triangle.h"
#include "ConstantBuffer.h"
#include "IndexBuffer.h"
#include "VertexBuffer.h"
#include "Renderer.h"



/*Triangle::Triangle(Renderer* renderer) : Drawable(renderer)
{
    auto A = TriangleVertex(DirectX::XMFLOAT4(-0.5, -0.5, 0, 1.0));
    auto B = TriangleVertex(DirectX::XMFLOAT4(0.5, -0.5, 0, 1.0));
    auto C = TriangleVertex(DirectX::XMFLOAT4(0.0, 1.0, 0, 1.0));
    triangle_ = TriangleGeometry{A, B, C};
    
    //Initialize();
}*/

Triangle::Triangle(Renderer* renderer, TriangleGeometry triangle) : Drawable(renderer), triangle_(triangle)
{
    //Initialize();
}

Triangle::~Triangle()
{
    (*vertex_buffer_->buffer())->Release();
    (*index_buffer_->buffer())->Release();
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
    //ID3D11Buffer* const* vb = vertex_buffer_->buffer();
    renderer_->Context()->IASetIndexBuffer(*index_buffer_->buffer(),  DXGI_FORMAT_R32_UINT, 0);
    renderer_->Context()->IASetVertexBuffers(0, 1, vertex_buffer_->buffer(), &strides, &offset);
    
    renderer_->Context()->DrawIndexed(3, 0, 0);
}

void Triangle::update(TriangleGeometry* raw_data)
{
    //vertex_buffer_->UpdateBuffer(raw_data);
    //renderer_.constant_buffer()->UpdateBuffer(raw_data);
    
}

void Triangle::change_color(DirectX::XMFLOAT4 color)
{
    for (int i = 0; i < 3; i++)
    {
        triangle_.vertecies[i].color = color;
    }
}

/*void Triangle::change_color(DirectX::XMFLOAT4 color)
{
    for (int i = 0; i < 3; i++)
    {
        triangle_.vertecies[i].color = color;
    }
    
}*/

void Triangle::Initialize()
{
    int indecies[] = {0, 1, 2};
    vertex_buffer_ = new VertexBuffer(renderer_, &triangle_, sizeof TriangleGeometry);
    index_buffer_ = new IndexBuffer(renderer_, indecies, 3 * sizeof(int));
    strides = sizeof TriangleVertex;
}

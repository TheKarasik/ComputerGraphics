#pragma once
#include "Drawable.h"
#include "Structs.h"
#include "AbstractBuffer.h"

class Triangle : public Drawable
{
public:
    //Triangle(Renderer* renderer);
    Triangle(Renderer* renderer, TriangleGeometry triangle);
    ~Triangle();
    Triangle& operator=(const Triangle& triangle);
    TriangleGeometry triangle() {return triangle_;}
    void draw() override;
    void update(TriangleGeometry* raw_data);
    void change_color(DirectX::XMFLOAT4 color);
    void Initialize();
    //void change_color(DirectX::XMFLOAT4 color);
private:
    
    AbstractBuffer* vertex_buffer_;
    AbstractBuffer* index_buffer_;
    //VertexBuffer<TriangleGeometry>* vertex_buffer_;
    //IndexBuffer<int>* index_buffer_;
    TriangleGeometry triangle_;
    unsigned int strides = 0;
    unsigned int offset = 0;
};

#pragma once
#include "ConstantBuffer.h"
#include "GameComponent.h"
#include "Structs.h"
#include "Triangle.h"

//
/*template <typename T>
class ConstantBuffer<T>;
template <typename T>
class AbstractBuffer<T>;*/
//class TriangleGeometry;
//class Triangle;



class Platform : public GameComponent
{
public:
    Platform(Renderer& renderer, float x_pos = -0.8, float height = 0.2);
    void GenerateContextBufferData();
    void update(float* y_pos);
    void draw() override;
    //AbstractBuffer<ConstantDataVertexShader>* constant_buffer() {return constant_buffer_;}
private:
    float x_pos_;
    float height_;
    float width_ = 0.05;
    float y_pos_ = 0;
    ConstantBuffer* constant_buffer_;
    ConstantDataVertexShader data_;
    Triangle triangles_[2];
};

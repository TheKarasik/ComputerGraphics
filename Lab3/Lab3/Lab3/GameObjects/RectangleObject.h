#pragma once
#include <DirectXMath.h>
#include "Structs.h"

#include "Updatable.h"
#include "Drawable.h"
//
/*template <typename T>
class ConstantBuffer<T>;
template <typename T>
class AbstractBuffer<T>;*/
//class TriangleGeometry;
//class Triangle;

class Triangle;
class ConstantBuffer;

class RectangleObject : public Drawable, public Updatable
{
public:
    RectangleObject(Renderer* renderer, RectangleGeometry rect_ =
    RectangleGeometry{DirectX::XMFLOAT2(0, 0), DirectX::XMFLOAT2(0.1, 0.1)});
    RectangleObject(Renderer* renderer);
    ~RectangleObject() override;
    void GenerateContextBufferData();
    void update(float elapsed) override;
    void draw() override;
    RectangleGeometry rect() {return rect_;}
    //AbstractBuffer<ConstantDataVertexShader>* constant_buffer() {return constant_buffer_;}
protected:
    void SetupBuffer();
    RectangleGeometry rect_ = RectangleGeometry{DirectX::XMFLOAT2(0, 0), DirectX::XMFLOAT2(0.1, 0.1)};
    ConstantBuffer* constant_buffer_;
    ConstantDataVertexShader data_;
    Triangle* triangles_[2];
    DirectX::SimpleMath::Rectangle rectangle_;
};

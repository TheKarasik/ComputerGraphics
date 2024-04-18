#include "../public/TriangleComponent.h"

TriangleComponent::TriangleComponent(Game* game, TriangleGeometry Locations, TriangleColor Colors, float HueStep) :
    GameComponent(game),
    Locations(Locations),
    Colors(Colors),
    HueStep(HueStep)
{
    GenerateVB();
    
    GenerateIB();
}

void TriangleComponent::Update(float h)
{
    float Hues[3] = {h, h+HueStep, h+HueStep};
    Colors = TriangleColor(Hues);
    
}

void TriangleComponent::Draw()
{
    GenerateVB();
    game_->Context()->IASetIndexBuffer(ib_, DXGI_FORMAT_R32_UINT, 0);
    game_->Context()->IASetVertexBuffers(0, 1, &vb_, strides, offsets);
}

void TriangleComponent::GenerateVB()
{
    DirectX::XMFLOAT4 vertieces[6] = {
        Locations.GeometryData()[0], Colors.ColorData()[0],
        Locations.GeometryData()[1], Colors.ColorData()[1],
        Locations.GeometryData()[2], Colors.ColorData()[2],
    };
    
    D3D11_BUFFER_DESC vertexBufDesc = {};
    vertexBufDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufDesc.CPUAccessFlags = 0;
    vertexBufDesc.MiscFlags = 0;
    vertexBufDesc.StructureByteStride = 0;
    vertexBufDesc.ByteWidth = sizeof(DirectX::XMFLOAT4) * std::size(vertieces);

    D3D11_SUBRESOURCE_DATA vertexData = {};
    vertexData.pSysMem = vertieces;
    vertexData.SysMemPitch = 0;
    vertexData.SysMemSlicePitch = 0;
    
    game_->device()->CreateBuffer(&vertexBufDesc, &vertexData, &vb_);
}

void TriangleComponent::GenerateIB()
{
    int indeces[3] = {0, 1, 2};    

    D3D11_BUFFER_DESC indexBufDesc = {};
    indexBufDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufDesc.CPUAccessFlags = 0;
    indexBufDesc.MiscFlags = 0;
    indexBufDesc.StructureByteStride = 0;
    indexBufDesc.ByteWidth = sizeof(int) * std::size(indeces);

    D3D11_SUBRESOURCE_DATA indexData = {};
    indexData.pSysMem = indeces;
    indexData.SysMemPitch = 0;
    indexData.SysMemSlicePitch = 0;
    
    game_->device()->CreateBuffer(&indexBufDesc, &indexData, &ib_);
}

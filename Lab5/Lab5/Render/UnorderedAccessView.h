#pragma once
#include "AbstractBuffer.h"

template <typename T>
class UnorderedAccessView : public AbstractBuffer
{
public:
    UnorderedAccessView(Renderer* renderer, unsigned int size);
    UnorderedAccessView(Renderer* renderer, UnorderedAccessView* uav);
    void UpdateBuffer(T* raw_data);
    int GetCount();
    ID3D11UnorderedAccessView* const* UAV() {return &UAV_;}

    void CreateBuffer() override;
    void CreateUAV();

    DXGI_FORMAT UAV_format = DXGI_FORMAT_UNKNOWN;
    D3D11_UAV_DIMENSION UAV_view_dimension = D3D11_UAV_DIMENSION_BUFFER;
    D3D11_BUFFER_UAV_FLAG UAV_flag = static_cast<D3D11_BUFFER_UAV_FLAG>(0);
    
private:
    ID3D11UnorderedAccessView* UAV_;
    ID3D11Buffer* StagingBuffer;
    //ConstantBuffer<Count>* CB_count;
};


#include "UnorderedAccessView.hpp"

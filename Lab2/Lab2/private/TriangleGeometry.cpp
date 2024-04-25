#include "../public/TriangleGeometry.h"

#include "../public/Delegates.h"

TriangleGeometry::TriangleGeometry(DirectX::XMFLOAT4 GeometryData[3])
{
    std::copy( GeometryData, GeometryData+3, GeometryData_);
}

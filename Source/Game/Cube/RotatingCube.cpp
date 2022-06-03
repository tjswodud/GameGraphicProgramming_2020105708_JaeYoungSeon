#include "Cube/RotatingCube.h"

RotatingCube::RotatingCube(const XMFLOAT4& outputColor)
    : BaseCube(outputColor)
{
}

void RotatingCube::Update(_In_ FLOAT deltaTime)
{
    // Rotate cube around the origin
    static FLOAT t = 0.0f;
    t += deltaTime;

    XMMATRIX rotate = XMMatrixRotationY(-2.0f * deltaTime);
    m_world = m_world * rotate;
}
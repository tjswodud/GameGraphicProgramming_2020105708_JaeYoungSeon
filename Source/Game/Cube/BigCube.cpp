#include "Cube/BigCube.h"

void BigCube::Update(_In_ FLOAT deltaTime)
{
    m_world = XMMatrixRotationY(deltaTime);
}
#include "Cube/CustomCube.h"

void CustomCube::Update(_In_ FLOAT deltaTime)
{
	XMMATRIX mSpin = XMMatrixRotationY(-deltaTime);
	XMMATRIX mOrbit = XMMatrixRotationZ(-deltaTime * 5.0f);
	XMMATRIX mTranslate = XMMatrixTranslation(4.0f, 0.0f, 0.0f);
	XMMATRIX mScale = XMMatrixScaling(0.5f, 0.5f, 0.3f);

	m_world = mScale * mSpin * mTranslate * mOrbit;
}
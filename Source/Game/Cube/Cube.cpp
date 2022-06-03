#include "Cube/Cube.h"

/*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
  Method:   Cube::Cube

  Summary:  Constructor

  Args:     const XMFLOAT4& outputColor
              Default color of the cube
M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
Cube::Cube(_In_ const XMFLOAT4& outputColor)
    : BaseCube(outputColor)
{
}

HRESULT Cube::Initialize(_In_ ID3D11Device* pDevice, _In_ ID3D11DeviceContext* pImmediateContext)
{
    BasicMeshEntry basicMeshEntry;
    basicMeshEntry.uNumIndices = NUM_INDICES;

    m_aMeshes.push_back(basicMeshEntry);

    if (HasTexture())
    {
        SetMaterialOfMesh(0, 0);
    }

    return initialize(pDevice, pImmediateContext);
}

/*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
  Method:   Cube::Update

  Summary:  Updates the cube every frame

  Args:     FLOAT deltaTime
              Elapsed time

  Modifies: [m_world].
M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
void Cube::Update(_In_ FLOAT deltaTime)
{
    // Does nothing
}
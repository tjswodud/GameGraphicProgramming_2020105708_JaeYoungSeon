#include "Cube/Cube.h"

/*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
  Method:   Cube::Cube

  Summary:  Constructor

  Args:     const std::filesystem::path& textureFilePath
              Path to the texture to use
M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
/*--------------------------------------------------------------------
  TODO: Cube::Cube definition (remove the comment)
--------------------------------------------------------------------*/
Cube::Cube(const std::filesystem::path& textureFilePath)
    : BaseCube(textureFilePath)
{ }

/*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
  Method:   Cube::Update

  Summary:  Updates the cube every frame

  Args:     FLOAT deltaTime
              Elapsed time

  Modifies: [m_world].
M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
void Cube::Update(_In_ FLOAT deltaTime)
{
    static FLOAT s_totalTime = 0.0f;
    s_totalTime += deltaTime;

    m_world = XMMatrixTranslation(0.0f, XMScalarSin(s_totalTime), 0.0f) * XMMatrixRotationY(s_totalTime);
}

/*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
  Method:   SwtubeCube::Cube

  Summary:  Constructor

  Args:     const std::filesystem::path& textureFilePath
              Path to the texture to use
M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
/*--------------------------------------------------------------------
  TODO: Cube::Cube definition (remove the comment)
--------------------------------------------------------------------*/
SwtubeCube::SwtubeCube(const std::filesystem::path& textureFilePath)
    : BaseCube(textureFilePath)
{ }

/*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
  Method:   Cube::Update

  Summary:  Updates the cube every frame

  Args:     FLOAT deltaTime
              Elapsed time

  Modifies: [m_world].
M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
void SwtubeCube::Update(_In_ FLOAT deltaTime)
{
    static FLOAT s_totalTime = 0.0f;
    s_totalTime += deltaTime;

    m_world = XMMatrixRotationY(s_totalTime) * XMMatrixTranslation(3.0f, XMScalarCos(s_totalTime), 0.0f);
}
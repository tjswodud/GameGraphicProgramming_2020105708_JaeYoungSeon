#include "Shader/SkyMapVertexShader.h"

namespace library
{
    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   SkyMapVertexShader::SkyMapVertexShader

      Summary:  Constructor

      Args:     PCWSTR pszFileName
                  Name of the file that contains the shader code
                PCSTR pszEntryPoint
                  Name of the shader entry point functino where shader
                  execution begins
                PCSTR pszShaderModel
                  Specifies the shader target or set of shader features
                  to compile against
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    SkyMapVertexShader::SkyMapVertexShader(_In_ PCWSTR pszFileName, _In_ PCSTR pszEntryPoint, _In_ PCSTR pszShaderModel)
        : VertexShader(pszFileName, pszEntryPoint, pszShaderModel)
    { }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   SkyMapVertexShader::Initialize

      Summary:  Initializes the vertex shader and the input layout

      Args:     ID3D11Device* pDevice
                  The Direct3D device to create the vertex shader

      Returns:  HRESULT
                  Status code
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    HRESULT SkyMapVertexShader::Initialize(_In_ ID3D11Device* pDevice)
    {
        HRESULT hr = S_OK;

        // Compile the vertex shader
        ComPtr<ID3DBlob> pVSBlob = nullptr;
        hr = compile(pVSBlob.GetAddressOf());

        if (FAILED(hr))
        {
            MessageBox(nullptr, L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
            return hr;
        }

        // Create the vertex shader
        hr = pDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, m_vertexShader.GetAddressOf());

        if (FAILED(hr))
        {
            return hr;
        }

        // Create the input layout
        D3D11_INPUT_ELEMENT_DESC aLayouts[] =
        {
            { "POSITION", 0u, DXGI_FORMAT_R32G32B32_FLOAT, 0u, 0u, D3D11_INPUT_PER_VERTEX_DATA, 0u }
        };
        UINT numElements = ARRAYSIZE(aLayouts);

        hr = pDevice->CreateInputLayout(aLayouts, numElements, pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), m_vertexLayout.GetAddressOf());

        if (FAILED(hr))
        {
            return hr;
        }

        return S_OK;
    }
}
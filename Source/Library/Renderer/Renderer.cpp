#include "Renderer/Renderer.h"

namespace library
{
    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderer::Renderer

      Summary:  Constructor

      Modifies: [m_driverType, m_featureLevel, m_d3dDevice, m_d3dDevice1,
                  m_immediateContext, m_immediateContext1, m_swapChain,
                  m_swapChain1, m_renderTargetView, m_vertexShader,
                  m_pixelShader, m_vertexLayout, m_vertexBuffer].
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: Renderer::Renderer definition (remove the comment)
    --------------------------------------------------------------------*/
    Renderer::Renderer()
        : m_driverType(D3D_DRIVER_TYPE_NULL)
        , m_featureLevel(D3D_FEATURE_LEVEL_11_0)
        , m_d3dDevice(nullptr)
        , m_d3dDevice1(nullptr)
        , m_immediateContext(nullptr)
        , m_immediateContext1(nullptr)
        , m_swapChain(nullptr)
        , m_swapChain1(nullptr)
        , m_renderTargetView(nullptr)
        , m_vertexShader(nullptr)
        , m_pixelShader(nullptr)
        , m_vertexLayout(nullptr)
        , m_vertexBuffer(nullptr)
    { }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderer::Initialize

      Summary:  Creates Direct3D device and swap chain

      Args:     HWND hWnd
                  Handle to the window

      Modifies: [m_d3dDevice, m_featureLevel, m_immediateContext,
                  m_d3dDevice1, m_immediateContext1, m_swapChain1,
                  m_swapChain, m_renderTargetView, m_vertexShader, 
                  m_vertexLayout, m_pixelShader, m_vertexBuffer].

      Returns:  HRESULT
                  Status code
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: Renderer::Initialize definition (remove the comment)
    --------------------------------------------------------------------*/
    HRESULT Renderer::Initialize(_In_ HWND hWnd)
    {
        HRESULT hr = S_OK;

        RECT rc;
        GetClientRect(hWnd, &rc);
        UINT width = rc.right - rc.left;
        UINT height = rc.bottom - rc.top;

        UINT createDeviceFlags = 0;
#ifdef _DEBUG
        createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

        D3D_DRIVER_TYPE driverTypes[] =
        {
            D3D_DRIVER_TYPE_HARDWARE,
            D3D_DRIVER_TYPE_WARP,
            D3D_DRIVER_TYPE_REFERENCE,
        };
        UINT numDriverTypes = ARRAYSIZE(driverTypes);

        D3D_FEATURE_LEVEL featureLevels[] =
        {
            D3D_FEATURE_LEVEL_11_1,
            D3D_FEATURE_LEVEL_11_0,
            D3D_FEATURE_LEVEL_10_1,
            D3D_FEATURE_LEVEL_10_0,
        };
        UINT numFeatureLevels = ARRAYSIZE(featureLevels);

        for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
        {
            m_driverType = driverTypes[driverTypeIndex];
            hr = D3D11CreateDevice(nullptr, m_driverType, nullptr, createDeviceFlags, featureLevels, numFeatureLevels,
                D3D11_SDK_VERSION, m_d3dDevice.GetAddressOf(), &m_featureLevel, m_immediateContext.GetAddressOf());

            if (hr == E_INVALIDARG)
            {
                // DirectX 11.0 platforms will not recognize D3D_FEATURE_LEVEL_11_1 so we need to retry without it
                hr = D3D11CreateDevice(nullptr, m_driverType, nullptr, createDeviceFlags, &featureLevels[1], numFeatureLevels - 1, D3D11_SDK_VERSION, m_d3dDevice.GetAddressOf(), &m_featureLevel, m_immediateContext.GetAddressOf());
            }

            if (SUCCEEDED(hr))
                break;
        }
        if (FAILED(hr))
            return hr;

        // Obtain DXGI factory from device (since we used nullptr for pAdapter above)
        Microsoft::WRL::ComPtr<IDXGIFactory1> dxgiFactory;
        {
            Microsoft::WRL::ComPtr<IDXGIDevice> dxgiDevice;
            hr = m_d3dDevice.As(&dxgiDevice);

            if (SUCCEEDED(hr))
            {
                Microsoft::WRL::ComPtr< IDXGIAdapter> adapter;
                hr = dxgiDevice->GetAdapter(adapter.GetAddressOf());

                if (SUCCEEDED(hr))
                {
                    hr = adapter->GetParent(__uuidof(IDXGIFactory1), reinterpret_cast<void**>(dxgiFactory.GetAddressOf()));
                    adapter.Reset();
                }
                dxgiDevice.Reset();
            }
        }
        if (FAILED(hr))
            return hr;

        // Create swap chain
        Microsoft::WRL::ComPtr<IDXGIFactory2> dxgiFactory2 = nullptr;
        hr = dxgiFactory.As(&dxgiFactory2);

        if (dxgiFactory2)
        {
            // DirectX 11.1 or later
            hr = m_d3dDevice.As(&m_d3dDevice1);

            if (SUCCEEDED(hr))
            {
                (void)m_immediateContext.As(&m_immediateContext1);
            }

            DXGI_SWAP_CHAIN_DESC1 sd = 
            {
                .Width = width,
                .Height = height,
                .Format = DXGI_FORMAT_R8G8B8A8_UNORM,
                .SampleDesc = { .Count = 1, .Quality = 0 },
                .BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
                .BufferCount = 1
            };

            hr = dxgiFactory2->CreateSwapChainForHwnd(m_d3dDevice.Get(), hWnd, &sd, nullptr, nullptr, m_swapChain1.GetAddressOf());
            if (SUCCEEDED(hr))
            {
                hr = m_swapChain1.As(&m_swapChain);
            }

            dxgiFactory2.Reset();
        }
        else
        {
            // DirectX 11.0 systems
            DXGI_SWAP_CHAIN_DESC sd =
            {
                .BufferDesc =
                {
                    .Width = width,
                    .Height = height,
                    .RefreshRate = { .Numerator = 60, .Denominator = 1 },
                    .Format = DXGI_FORMAT_R8G8B8A8_UNORM
                },
                .SampleDesc = { .Count = 1, .Quality = 0 },
                .BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
                .BufferCount = 1,
                .OutputWindow = hWnd,
                .Windowed = TRUE
            };

            hr = dxgiFactory->CreateSwapChain(m_d3dDevice.Get(), &sd, m_swapChain.GetAddressOf());
        }

        // Note this tutorial doesn't handle full-screen swapchains so we block the ALT+ENTER shortcut
        dxgiFactory->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER);

        dxgiFactory.Reset();

        if (FAILED(hr))
            return hr;

        // Create a render target view
        Microsoft::WRL::ComPtr< ID3D11Texture2D> pBackBuffer = nullptr;
        hr = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(pBackBuffer.GetAddressOf()));

        if (FAILED(hr))
        {
            return hr;
        }

        hr = m_d3dDevice->CreateRenderTargetView(pBackBuffer.Get(), nullptr, m_renderTargetView.GetAddressOf());
        pBackBuffer.Reset();

        if (FAILED(hr))
        {
            return hr;
        }

        m_immediateContext->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), nullptr);

        // Setup the viewport
        D3D11_VIEWPORT vp = 
        {
            .TopLeftX = 0,
            .TopLeftY = 0,
            .Width = (FLOAT)width,
            .Height = (FLOAT)height,
            .MinDepth = 0.0f,
            .MaxDepth = 1.0f
        };

        m_immediateContext->RSSetViewports(1, &vp);

        // Compile the vertex shader
        Microsoft::WRL::ComPtr<ID3DBlob> pVSBlob = nullptr;
        hr = compileShaderFromFile(L"../../Source/Library/Shaders/Lab03.fxh", "VS", "vs_4_0", pVSBlob.GetAddressOf());

        if (FAILED(hr))
        {
            MessageBox(nullptr, L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
            return hr;
        }

        // Create the vertex shader
        hr = m_d3dDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, m_vertexShader.GetAddressOf());
        if (FAILED(hr))
        {
            return hr;
        }

        // Define the input layout
        D3D11_INPUT_ELEMENT_DESC layout[] =
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        };
        UINT numElements = ARRAYSIZE(layout);

        // Create the input layout
        hr = m_d3dDevice->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), m_vertexLayout.GetAddressOf());

        if (FAILED(hr))
        {
            return hr;
        }

        // Set the input layout
        m_immediateContext->IASetInputLayout(m_vertexLayout.Get());

        // Compile the pixel shader
        Microsoft::WRL::ComPtr<ID3DBlob> pPSBlob = nullptr;
        hr = compileShaderFromFile(L"../../Source/Library/Shaders/Lab03.fxh", "PS", "ps_4_0", pPSBlob.GetAddressOf());

        if (FAILED(hr))
        {
            MessageBox(nullptr, L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
            return hr;
        }

        // Create the pixel shader
        hr = m_d3dDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, m_pixelShader.GetAddressOf());

        if (FAILED(hr))
        {
            return hr;
        }

        // Create vertex buffer
        SimpleVertex vertices[] =
        {
            XMFLOAT3(0.0f, 0.5f, 0.5f),
            XMFLOAT3(0.5f, -0.5f, 0.5f),
            XMFLOAT3(-0.5f, -0.5f, 0.5f),
        };

        D3D11_BUFFER_DESC bd =
        {
            .ByteWidth = sizeof(SimpleVertex) * 3,
            .Usage = D3D11_USAGE_DEFAULT,
            .BindFlags = D3D11_BIND_VERTEX_BUFFER,
            .CPUAccessFlags = 0,

        };

        D3D11_SUBRESOURCE_DATA InitData = { .pSysMem = vertices };
        hr = m_d3dDevice->CreateBuffer(&bd, &InitData, m_vertexBuffer.GetAddressOf());

        if (FAILED(hr))
        {
            return hr;
        }

        // Set vertex buffer
        UINT stride = sizeof(SimpleVertex);
        UINT offset = 0;
        m_immediateContext->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset);

        // Set primitive topology
        m_immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        return S_OK;
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderer::Render

      Summary:  Render the frame
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: Renderer::Render definition (remove the comment)
    --------------------------------------------------------------------*/
    void Renderer::Render()
    {
        // Clear the back buffer
        m_immediateContext->ClearRenderTargetView(m_renderTargetView.Get(), Colors::MidnightBlue);

        // Render a triangle
        m_immediateContext->VSSetShader(m_vertexShader.Get(), nullptr, 0);
        m_immediateContext->PSSetShader(m_pixelShader.Get(), nullptr, 0);
        m_immediateContext->Draw(3, 0);

        // Present the information rendered to the back buffer to the front buffer
        m_swapChain->Present(0, 0);
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderer::compileShaderFromFile

      Summary:  Helper for compiling shaders with D3DCompile

      Args:     PCWSTR pszFileName
                  A pointer to a constant null-terminated string that
                  contains the name of the file that contains the
                  shader code
                PCSTR pszEntryPoint
                  A pointer to a constant null-terminated string that
                  contains the name of the shader entry point function
                  where shader execution begins
                PCSTR pszShaderModel
                  A pointer to a constant null-terminated string that
                  specifies the shader target or set of shader
                  features to compile against
                ID3DBlob** ppBlobOut
                  A pointer to a variable that receives a pointer to
                  the ID3DBlob interface that you can use to access
                  the compiled code

      Returns:  HRESULT
                  Status code
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: Renderer::compileShaderFromFile definition (remove the comment)
    --------------------------------------------------------------------*/
    HRESULT Renderer::compileShaderFromFile(_In_ PCWSTR pszFileName, _In_ PCSTR pszEntryPoint, _In_ PCSTR szShaderModel, _Outptr_ ID3DBlob** ppBlobOut)
    {
        HRESULT hr = S_OK;

        DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
        dwShaderFlags |= D3DCOMPILE_DEBUG;
        dwShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

        Microsoft::WRL::ComPtr<ID3DBlob> pErrorBlob = nullptr;

        hr = D3DCompileFromFile(pszFileName, nullptr, nullptr, pszEntryPoint, szShaderModel, dwShaderFlags, 0, ppBlobOut, pErrorBlob.GetAddressOf());

        if (FAILED(hr))
        {
            if (pErrorBlob)
            {
                OutputDebugStringA(reinterpret_cast<const char*>(pErrorBlob->GetBufferPointer()));
            }
            return hr;
        }

        if (pErrorBlob)
        {
            pErrorBlob->Release();
        }

        return S_OK;
    }
}
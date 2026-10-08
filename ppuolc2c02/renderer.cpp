#include "pch.h"
#include "renderer.h"
#include <d2d1.h>
#include <Windows.h>
#include <dxgi1_2.h>
#include <dcommon.h>
#include <d2d1helper.h>
#include <d2d1_1helper.h>
#include <dxgiformat.h>
#include <dxgi.h>
#include <d2d1_1.h>
#include <wrl/client.h>
#include <dxgitype.h>
#include <d3d11.h>
#include <d3dcommon.h>
#include <string>

bool CD2DRenderer::InitializeDirectX(HWND hwnd)
{
    D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, d2dfactory.GetAddressOf());

    InitSwapChain(hwnd);
    CreateD2DContext();

    // Init main brush
    InitSolidBrush();

    return false;
}

void CD2DRenderer::RenderFrame()
{
    HRESULT hr;
    //double ms = clock.GetMilisecondsElapsed();

    d2dContext->BeginDraw();
    ClearScreen(0.0f, 0.0f, 0.0f);
    //game.Update(d2dContext.Get(), sBrush.Get());

    CountFps();
    DrawTray();

    hr = d2dContext->EndDraw();
    //COM_ERROR_IF_FAILED(hr, "Failed to run d2dcontext.EndDraw()");
    hr = swapChain->Present(1, 0);
    //COM_ERROR_IF_FAILED(hr, "Failed to run swapChain.Present()");
    //clock.Restart();
}

void CD2DRenderer::DrawTray()
{

}

void CD2DRenderer::CountFps()
{

}

bool CD2DRenderer::InitDevice(HWND hwnd)
{
    HRESULT hr;

    DXGI_SWAP_CHAIN_DESC scd;

    ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

    // INIT DISPLAY
    scd.BufferDesc.Height = this->windowHeight;
    scd.BufferDesc.Width = this->windowWidth;
    scd.BufferDesc.RefreshRate.Numerator = 60;
    scd.BufferDesc.RefreshRate.Denominator = 1;
    scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    scd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    scd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

    //important //
    scd.SampleDesc.Count = 1;
    scd.SampleDesc.Quality = 0;
    ///

    scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scd.BufferCount = 1;
    scd.OutputWindow = hwnd;
    scd.Windowed = TRUE;
    scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

    // INIT DEVICE AND SWAPCHAIN

    // This flag adds support for surfaces with a different color channel ordering than the API default.
    // You need it for compatibility with Direct2D.
    UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

    hr = D3D11CreateDevice(
        nullptr,                    // specify null to use the default adapter
        D3D_DRIVER_TYPE_HARDWARE,
        0,
        creationFlags,              // optionally set debug and Direct2D compatibility flags
        featureLevels,              // list of feature levels this app can support
        ARRAYSIZE(featureLevels),   // number of possible feature levels
        D3D11_SDK_VERSION,
        &device,                    // returns the Direct3D device created
        nullptr,            // returns feature level of device created
        d3d11DeviceContext.GetAddressOf()                    // returns the device immediate context
    );
    //COM_ERROR_IF_FAILED(hr, "Failed to create device and swapchain.");

    // Obtain the underlying DXGI device of the Direct3D11 device.
    hr = this->device.As(&dxgiDevice);
    //COM_ERROR_IF_FAILED(hr, "Failed to Obtain the underlying DXGI device of the Direct3D11 device.");

    return true;
}

bool CD2DRenderer::InitDepthStencilAndBlenderState()
{
    HRESULT hr;

    // INIT DEPTH STENCIL
    // Use CD3D11_TEXTURE2D_DESC instead of D3D11_TEXTURE2D_DESC for less config to write.
    //D3D11_TEXTURE2D_DESC depthStencilDesc; OLD
    CD3D11_TEXTURE2D_DESC depthStencilDesc(DXGI_FORMAT_D24_UNORM_S8_UINT, this->windowWidth, this->windowHeight);
    depthStencilDesc.MipLevels = 1;
    depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

    hr = this->device->CreateTexture2D(&depthStencilDesc, NULL, this->depthStencilBuffer.GetAddressOf());
    //COM_ERROR_IF_FAILED(hr, "Failed to create depth stencil buffer.");

    hr = this->device->CreateDepthStencilView(depthStencilBuffer.Get(), NULL, this->depthStencilView.GetAddressOf());
    //COM_ERROR_IF_FAILED(hr, "Failed to create depth stencil view.");

    // output merger.
    this->d3d11DeviceContext->OMSetRenderTargets(1, this->renderTargetView.GetAddressOf(), this->depthStencilView.Get());

    //Create depth stencil state.
    //D3D11_DEPTH_STENCIL_DESC depthStencilDesc;

    //Create depth stencil state
    CD3D11_DEPTH_STENCIL_DESC depthstencildesc(D3D11_DEFAULT);
    depthstencildesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;

    hr = this->device->CreateDepthStencilState(&depthstencildesc, this->depthStencilState.GetAddressOf());
    //COM_ERROR_IF_FAILED(hr, "Failed to create depth stencil state.");

    //Create and Set the Viewport
    CD3D11_VIEWPORT viewport(0.0f, 0.0f, static_cast<float>(this->windowWidth), static_cast<float>(this->windowHeight));

    // if you want split screen like in older games, you can add more viewports.
    this->d3d11DeviceContext->RSSetViewports(1, &viewport);

    //Create Rasterizer State
    CD3D11_RASTERIZER_DESC rasterizerDesc(D3D11_DEFAULT);
    hr = this->device->CreateRasterizerState(&rasterizerDesc, this->rasterizerState.GetAddressOf());
    //COM_ERROR_IF_FAILED(hr, "Failed to create rasterizer state.");

    //Create blend state
    D3D11_BLEND_DESC blendDesc = { 0 };
    D3D11_RENDER_TARGET_BLEND_DESC rtbd = { 0 };

    rtbd.BlendEnable = true;
    rtbd.SrcBlend = D3D11_BLEND::D3D11_BLEND_SRC_ALPHA;
    rtbd.DestBlend = D3D11_BLEND::D3D11_BLEND_INV_SRC_ALPHA;
    rtbd.BlendOp = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
    rtbd.BlendOpAlpha = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
    rtbd.SrcBlendAlpha = D3D11_BLEND::D3D11_BLEND_ONE;
    rtbd.DestBlendAlpha = D3D11_BLEND::D3D11_BLEND_ZERO;
    rtbd.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE::D3D11_COLOR_WRITE_ENABLE_ALL;

    blendDesc.RenderTarget[0] = rtbd;

    hr = this->device->CreateBlendState(&blendDesc, this->blenderState.GetAddressOf());
}

bool CD2DRenderer::InitSwapChain(HWND hwnd)
{
    HRESULT hr;

    // Allocate a descriptor.
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = { 0 };
    swapChainDesc.Width = 0;                           // use automatic sizing
    swapChainDesc.Height = 0;
    swapChainDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM; // this is the most common swapchain format
    swapChainDesc.Stereo = false;
    swapChainDesc.SampleDesc.Count = 1;                // don't use multi-sampling
    swapChainDesc.SampleDesc.Quality = 0;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.BufferCount = 2;                     // use double buffering to enable flip
    swapChainDesc.Scaling = DXGI_SCALING_NONE;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL; // all apps must use this SwapEffect
    swapChainDesc.Flags = 0;

    // Identify the physical adapter (GPU or card) this device is runs on.
    Microsoft::WRL::ComPtr<IDXGIAdapter> dxgiAdapter;
    dxgiDevice->GetAdapter(&dxgiAdapter);

    Microsoft::WRL::ComPtr<IDXGIFactory2> dxgiFactory;
    dxgiAdapter->GetParent(IID_PPV_ARGS(&dxgiFactory));

    hr = dxgiFactory->CreateSwapChainForHwnd(
        device.Get(),
        hwnd,
        &swapChainDesc,
        nullptr,
        nullptr,    // allow on all displays
        swapChain.GetAddressOf()
    );
    
    //COM_ERROR_IF_FAILED(hr, "Failed to CreateSwapChainForHwnd");

    // Ensure that DXGI doesn't queue more than one frame at a time.
    dxgiDevice->SetMaximumFrameLatency(1);

    // Get the backbuffer for this window which is be the final 3D render target.
    swapChain->GetBuffer(0, IID_PPV_ARGS(backBuffer.GetAddressOf()));

    return true;
}

bool CD2DRenderer::CreateD2DContext()
{
    HRESULT hr;

    hr = d2dfactory->CreateDevice(dxgiDevice.Get(), d2dDevice.GetAddressOf());
    //COM_ERROR_IF_FAILED(hr, "Failed to create D2D Device.");

    hr = d2dDevice->CreateDeviceContext(
        D2D1_DEVICE_CONTEXT_OPTIONS_NONE,
        d2dContext.GetAddressOf()
    );
    //COM_ERROR_IF_FAILED(hr, "Failed to create D2D Device Context.");

    // Direct2D needs the dxgi version of the backbuffer surface pointer.
    swapChain->GetBuffer(0, IID_PPV_ARGS(dxgiBackBuffer.GetAddressOf()));

    // Now we set up the Direct2D render target bitmap linked to the swapchain. 
    // Whenever we render to this bitmap, it is directly rendered to the 
    // swap chain associated with the window.
    D2D1_BITMAP_PROPERTIES1 bitmapProperties =
        D2D1::BitmapProperties1(
            D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
            D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE),
            96,
            96
        );

    // Get a D2D surface from the DXGI back buffer to use as the D2D render target.
    hr = d2dContext->CreateBitmapFromDxgiSurface(
        dxgiBackBuffer.Get(),
        &bitmapProperties,
        d2dTargetBitmap.GetAddressOf()
    );
    //COM_ERROR_IF_FAILED(hr, "Failed to CreateBitmapFromDxgiSurface");

    // Now we can set the Direct2D render target.
    d2dContext->SetTarget(d2dTargetBitmap.Get());

}

bool CD2DRenderer::Initialize(HWND hwnd, int width, int height)
{
    this->windowWidth = width;
    this->windowHeight = height;
    this->aspectRatio = static_cast<float>(this->windowWidth) / static_cast<float>(this->windowHeight);
    //this->fpsTimer.Start();
    //this->clock.Start();

    if (!InitDevice(hwnd))
        return false;

    if (!InitializeDirectX(hwnd))
        return false;

    if (!InitializeShaders())
        return false;

    //if (!InitializeScene())
    //	return false;

    InitGame();
    return true;
}

bool CD2DRenderer::InitializeScene()
{
    camera.SetPosition(0.0f, 0.0f, -2.0f);
    camera.SetProjectionValues(90.0f, this->aspectRatio, this->nearZ, this->farZ);

    return true;
}

void CD2DRenderer::ClearScreen(float r, float g, float b)
{
    this->d2dContext->Clear(D2D1::ColorF(r, g, b));
}

void CD2DRenderer::InitSolidBrush()
{
    HRESULT hr = d2dContext->CreateSolidColorBrush(D2D1::ColorF(255.0f, 255.0f, 255.0f, 1), &sBrush);
    //COM_ERROR_IF_FAILED(hr, "Failed to initialize brush");
}

void CD2DRenderer::BeginDraw()
{
    d2dContext->BeginDraw();
}

void CD2DRenderer::EndDraw()
{
    d2dContext->EndDraw();
}

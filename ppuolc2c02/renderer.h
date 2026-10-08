#pragma once
#include <windows.h>
#include <wrl/client.h>

#include <d2d1.h>
#include <d2d1_2.h>
#include <d2d1_1helper.h>
#include <d3d11.h>
#include <dxgi.h>
#include <dxgi1_2.h>
#include <d3dcommon.h>
#include <d2d1_1.h>
#include "camera.h"

class CD2DRenderer {
    CD2DRenderer(){}

    bool InitializeDirectX(HWND hwnd);
    bool InitializeShaders();
    bool InitializeScene();
    bool InitDevice(HWND hwnd);
    bool InitDepthStencilAndBlenderState();
    void InitSolidBrush();
    void CountFps();
    bool InitEffects();
    bool InitSwapChain(HWND hwnd);
    bool CreateD2DContext();

    bool Initialize(HWND hwnd, int width, int height);
    void RenderFrame();
    void ClearScreen(float r, float g, float b);
    void BeginDraw();
    void EndDraw();
    void InitGame();
    void DrawTray();
    
    int windowWidth = 0;
    int windowHeight = 0;
    float aspectRatio = 0.0f;
    float nearZ = 0.1f;
    float farZ = 1000.0f;
    Camera camera;

    //DXGI COM Pointers
    // D3D11 Device
    Microsoft::WRL::ComPtr<ID3D11Device> device;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> d3d11DeviceContext;
    Microsoft::WRL::ComPtr<ID2D1Device1> d2dDevice;
    Microsoft::WRL::ComPtr<IDXGIDevice1> dxgiDevice;

    // SwapChain
    Microsoft::WRL::ComPtr<IDXGISwapChain1> swapChain;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;
    Microsoft::WRL::ComPtr<IDXGIFactory2> dxgiFactory;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
    Microsoft::WRL::ComPtr<IDXGISurface> dxgiBackBuffer;

    //D2D COM Pointers
    Microsoft::WRL::ComPtr<ID2D1Factory2> d2dfactory;
    //Microsoft::WRL::ComPtr<ID2D1HwndRenderTarget> renderTarget;
    Microsoft::WRL::ComPtr<ID2D1DeviceContext1> d2dContext;

    // Brushes
    Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> sBrush;
    Microsoft::WRL::ComPtr<ID2D1Bitmap1> d2dTargetBitmap;

    // Rasterizer state
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState;
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState_CullFront;

    // Blender state
    Microsoft::WRL::ComPtr<ID3D11BlendState> blenderState;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState;
    Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencilBuffer;

    // This array defines the set of DirectX hardware feature levels this app  supports.
    // The ordering is important and you should  preserve it.
    // Don't forget to declare your app's minimum required feature level in its
    // description.
    D3D_FEATURE_LEVEL featureLevels[3] =
    {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
    };
};

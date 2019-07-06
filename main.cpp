#include <iostream>

#include <Windows.h>

#include "include/dx11/D3D11.h"
#include "include/dx11/D3DX11.h"
#include "include/dx10/D3DX10math.h"

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dx11.lib")
#pragma comment (lib, "d3dx10.lib")

IDXGISwapChain *swapChain;
ID3D11Device *dev;
ID3D11DeviceContext *devCon;
ID3D11RenderTargetView *backBuffer;

#pragma clang diagnostic push
#pragma ide diagnostic ignored "hicpp-multiway-paths-covered"
// this is the main message handler for the program
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){
    switch(message){
        case WM_DESTROY: {
            PostQuitMessage(0);
            return 0;
        }
    }

    return DefWindowProc (hWnd, message, wParam, lParam);
}
#pragma clang diagnostic pop

void InitD3D(HWND hwnd){
    DXGI_SWAP_CHAIN_DESC scd;

    ZeroMemory(&scd, sizeof(scd));

    scd.BufferCount = 1;
    scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scd.OutputWindow = hwnd;
    scd.SampleDesc.Count = 4;
    scd.Windowed = true;


    D3D11CreateDeviceAndSwapChain(
            nullptr,
            D3D_DRIVER_TYPE_HARDWARE,
            nullptr,
            NULL,
            nullptr,
            NULL,
            D3D11_SDK_VERSION,
            &scd,
            &swapChain,
            &dev,
            nullptr,
            &devCon
    );
    // get the address of the back buffer
    ID3D11Texture2D *pBackBuffer;
    swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

    // use the back buffer address to create the render target
    dev->CreateRenderTargetView(pBackBuffer, nullptr, &backBuffer);
    pBackBuffer->Release();

    // set the render target as the back buffer
    devCon->OMSetRenderTargets(1, &backBuffer, nullptr);

    // Set the viewport
    D3D11_VIEWPORT viewport;
    ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width = 800;
    viewport.Height = 600;

    devCon->RSSetViewports(1, &viewport);
}

// this is the function used to render a single frame
void RenderFrame()
{
    // clear the back buffer to a deep blue
    devCon->ClearRenderTargetView(backBuffer, D3DXCOLOR(0, 0.2f, 0.4f, 1.0f));

    // do 3D rendering on the back buffer here

    // switch the back buffer and the front buffer
    swapChain->Present(0, 0);
}

void CleanD3D(){
    swapChain->Release();
    backBuffer->Release();
    dev->Release();
    devCon->Release();
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "hicpp-signed-bitwise"
// the entry point for any Windows program
int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow) {

    HWND hWnd;
    WNDCLASSEX wc;

    ZeroMemory(&wc, sizeof(WNDCLASSEX));

    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
    wc.lpszClassName = "WindowClass";

    RegisterClassEx(&wc);

    RECT wr = {0, 0, 800, 600};
    AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);

    hWnd = CreateWindowEx(NULL,
                          "WindowClass",
                          "Our First Direct3D Program",
                          WS_OVERLAPPEDWINDOW,
                          300,
                          300,
                          wr.right - wr.left,
                          wr.bottom - wr.top,
                          nullptr,
                          nullptr,
                          hInstance,
                          nullptr);

    ShowWindow(hWnd, nCmdShow);

    // set up and initialize Direct3D

    InitD3D(hWnd);

    // enter the main loop:

    MSG msg;

    while(TRUE){
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            if (msg.message == WM_QUIT){
                break;
            }
            RenderFrame();
        } else {
            // Run game code here
            // ...
            // ...
        }
    }

    // clean up DirectX and COM
    CleanD3D();

    return msg.wParam;
}
#pragma clang diagnostic pop
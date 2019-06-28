#include <iostream>

#include <Windows.h>
#include <windowsx.h>

#include "include/dx11/D3D11.h"
#include "include/dx11/D3DX11.h"

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dx11.lib")
#pragma comment (lib, "d3dx10.lib")

IDXGISwapChain *swapChain;
ID3D11Device *dev;
ID3D11DeviceContext *devCon;

//void InitD3D(HWND hWnd);
//void CleanD3D();
//LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

// this is the main message handler for the program
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch(message)
    {
        case WM_DESTROY:
        {
            PostQuitMessage(0);
            return 0;
        } break;
    }

    return DefWindowProc (hWnd, message, wParam, lParam);
}

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
}

void CleanD3D(){
    swapChain->Release();
    dev->Release();
    devCon->Release();
}

int main(){
    CleanD3D();
    return 0;
}
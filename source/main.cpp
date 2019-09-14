#pragma clang diagnostic push
#include <iostream>

#include <Windows.h>

#include "../include/libs/dx11/D3D11.h"
#include "../include/libs/dx11/D3DX11.h"
#include "../include/libs/dx10/D3DX10math.h"
#include "../include/Platform.h"
#include <thread>
#include <chrono>

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dx11.lib")
#pragma comment (lib, "d3dx10.lib")

// define the screen resolution
#define SCREEN_WIDTH  800
#define SCREEN_HEIGHT 600

// global
IDXGISwapChain *swapChain;
ID3D11Device *dev;
ID3D11DeviceContext *devCon;
ID3D11RenderTargetView *backBuffer;

ID3D11VertexShader *pVs;
ID3D11PixelShader *pPs;

ID3D11Buffer *pVertexBuffer;

ID3D11InputLayout *pLayout;


struct VERTEX { // NOLINT(cppcoreguidelines-pro-type-member-init)
    FLOAT X, Y, Z;
    D3DXCOLOR Color;
};

struct FDrawUpData
{
    uint32 NumPrimitives{};
    uint32 NumVertices{};
    uint32 VertexDataStride{};
    void* OutVertexData;
    uint32 MinVertexIndex{};
    uint32 NumIndices{};
    uint32 IndexDataStride{};
    void* OutIndexData;

    FDrawUpData()
            : OutVertexData(nullptr)
            , OutIndexData(nullptr)
    {
    }
};

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

void InitPipeline(){
    // load and compile the two shaders
    ID3D10Blob *vs, *ps;
    D3DX11CompileFromFileA(
            R"(C:\Users\Kostia Yeriomin\CLionProjects\DirecXPrac\shaders\shader.shader)",
            nullptr,
            nullptr,
            "VShader",
            "vs_4_0",
            0,
            0,
            nullptr,
            &vs,
            nullptr,
            nullptr);
    D3DX11CompileFromFileA(
            R"(C:\Users\Kostia Yeriomin\CLionProjects\DirecXPrac\shaders\shader.shader)",
            nullptr,
            nullptr,
            "PShader",
            "ps_4_0",
            0,
            0,
            nullptr,
            &ps,
            nullptr,
            nullptr);

    // encapsulate both shaders into shader objects
    dev->CreateVertexShader(vs->GetBufferPointer(), vs->GetBufferSize(), nullptr, &pVs);
    dev->CreatePixelShader(ps->GetBufferPointer(), ps->GetBufferSize(), nullptr, &pPs);

    // set the shader objects
    devCon->VSSetShader(pVs, nullptr, 0);
    devCon->PSSetShader(pPs, nullptr, 0);

    // create the input layout object
    D3D11_INPUT_ELEMENT_DESC ied[] = {
            {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
    };

    dev->CreateInputLayout(ied, 2, vs->GetBufferPointer(), vs->GetBufferSize(), &pLayout);
    devCon->IASetInputLayout(pLayout);
}

void InitGraphics(){
    // create a triangle using the VERTEX struct
    VERTEX ourVertices[] = {
            {0.0f, 0.5f, 0.0f, D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f)},
            {0.45f, -0.5, 0.0f, D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f)},
            {-0.45f, -0.5f, 0.0f, D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f)}
    };

    // create the vertex buffer
    D3D11_BUFFER_DESC bd;
    ZeroMemory(&bd, sizeof(bd));

    bd.Usage = D3D11_USAGE_DYNAMIC; // write access access by CPU and GPU
    bd.ByteWidth = sizeof(VERTEX) * 3; // size is the VERTEX struct * 3
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER; // use as a vertex buffer
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; // allow CPU to write in buffer
    dev->CreateBuffer(&bd, nullptr, &pVertexBuffer); // create the buffer

    // copy the vertices into the buffer
    D3D11_MAPPED_SUBRESOURCE ms;
    devCon->Map(pVertexBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);
    memcpy(ms.pData, &ourVertices, sizeof(ourVertices));
    devCon->Unmap(pVertexBuffer, NULL);
}

void InitD3D(HWND hwnd){
    DXGI_SWAP_CHAIN_DESC scd;

    ZeroMemory(&scd, sizeof(scd));

    scd.BufferCount = 1;
    scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    scd.BufferDesc.Height = SCREEN_HEIGHT;
    scd.BufferDesc.Width = SCREEN_WIDTH;
    scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scd.OutputWindow = hwnd;
    scd.SampleDesc.Count = 4;
    scd.Windowed = true;
    scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;


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
    viewport.Width = SCREEN_WIDTH;
    viewport.Height = SCREEN_HEIGHT;

    devCon->RSSetViewports(1, &viewport);

    InitPipeline();
    InitGraphics();
}

// this is the function used to render a single frame
void RenderFrame()
{
    // clear the back buffer to a deep blue
    devCon->ClearRenderTargetView(backBuffer, D3DXCOLOR(0, 0.2f, 0.4f, 1.0f));

    // do 3D rendering on the back buffer here

    // select which vertex buffer to display
    UINT stride = sizeof(VERTEX);
    UINT offset = 0;
    devCon->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset);

    // select which primtive type we are using
    devCon->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // draw the vertex buffer to the back buffer
    devCon->Draw(3, 0);

    // switch the back buffer and the front buffer
    swapChain->Present(0, 0);
}

void CleanD3D(){
    swapChain->SetFullscreenState(FALSE, nullptr);

    // close and release all existing COM objects
    pVs->Release();
    pPs->Release();
    pVertexBuffer->Release();
    pLayout->Release();
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
//    wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
    wc.lpszClassName = "WindowClass";

    RegisterClassEx(&wc);

    RECT wr = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
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

        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }

    // clean up DirectX and COM
    CleanD3D();

    return msg.wParam;
}
#pragma clang diagnostic pop
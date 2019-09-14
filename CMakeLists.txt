cmake_minimum_required(VERSION 3.14)
project(DirecXPrac)

set(CMAKE_CXX_STANDARD 17)

add_executable(
        DirecXPrac
        WIN32

        source/main.cpp

        include/libs/dx10/D3D10.h
        include/libs/dx10/D3DX10.h
        include/libs/dx10/d3dx10async.h
        include/libs/dx10/D3DX10math.h
        include/libs/dx10/D3DX10tex.h
        include/libs/dx10/D3DX10core.h
        include/libs/dx10/D3DX10mesh.h
        include/libs/dx10/D3DX10math.inl

        include/libs/dx11/D3D11.h
        include/libs/dx11/D3DX11.h
        include/libs/dx11/D3DX11async.h
        include/libs/dx11/D3DX11core.h
        include/libs/dx11/D3DX11tex.h
        include/Platform.h include/render/D3D11DynamicRHI.h source/render/D3D11Commands.cpp)

target_link_libraries(DirecXPrac ${CMAKE_CURRENT_LIST_DIR}/libs/d3dx10.lib)
target_link_libraries(DirecXPrac ${CMAKE_CURRENT_LIST_DIR}/libs/d3d11.lib)
target_link_libraries(DirecXPrac ${CMAKE_CURRENT_LIST_DIR}/libs/d3dx11.lib)

#pragma once

#include "../Platform.h"

class D3D11DynamicRHI
{
public:

    virtual void RHIDrawPrimitive(uint32 BaseVertexIndex, uint32 NumPrimitives, uint32 NumInstances);

};

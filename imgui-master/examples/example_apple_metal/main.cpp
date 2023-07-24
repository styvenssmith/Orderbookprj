#include <iostream>
#include "imgui.h"
#include "imgui_impl_metal.h"

#if defined(__APPLE__)
#include <TargetConditionals.h>
#endif

#include <Metal/Metal.h>
#include <MetalKit/MetalKit.h>

int main()
{
    // Create Metal device and command queue
    id<MTLDevice> device = MTLCreateSystemDefaultDevice();
    id<MTLCommandQueue> commandQueue = [device newCommandQueue];
    
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    
}

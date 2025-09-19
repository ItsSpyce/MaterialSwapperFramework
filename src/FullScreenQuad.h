#pragma once
#include <d3d11.h>

// Helper to draw a full-screen triangle (no vertex buffer needed)
inline void DrawFullScreenQuad(ID3D11DeviceContext* context) {
    // Set primitive topology to triangle strip
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    // No vertex/index buffer needed for a full-screen quad with a simple vertex shader
    context->Draw(4, 0);
}

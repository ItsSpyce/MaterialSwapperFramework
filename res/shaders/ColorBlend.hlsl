Texture2D inputTexture : register(t0);
Texture2D maskTexture : register(t1);
SamplerState samplerState : register(s0);

cbuffer ColorBlendParams : register(b0)
{
    float3 blendColor;
    int blendMode; // 0 = Multiply, 1 = Add
}

struct PSInput
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD0;
};

float4 main(PSInput input) : SV_TARGET
{
    float4 src = inputTexture.Sample(samplerState, input.texcoord);
    float maskValue = maskTexture.Sample(samplerState, input.texcoord).r; // Grayscale mask

    float3 blended;
    if (blendMode == 0)
    {
        // Multiply blend
        blended = src.rgb * blendColor;
    }
    else if (blendMode == 1)
    {
        // Add blend
        blended = min(src.rgb + blendColor, float3(1.0, 1.0, 1.0));
    }
    else
    {
        blended = src.rgb;
    }

    // Interpolate between original and blended color using maskValue
    float3 finalColor = lerp(src.rgb, blended, maskValue);

    return float4(finalColor, src.a);
}
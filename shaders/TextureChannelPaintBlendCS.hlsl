Texture2D SourceTexture : register(t0);
Texture2D ChannelMaskTexture : register(t1);
RWTexture2D<float4> OutputTexture : register(u0);

SamplerState LinearClampSampler : register(s0);

cbuffer TextureChanelPaintBlendConstants : register(b0)
{
    float4 RedChannelColor;
    float4 GreenChannelColor;
    float4 BlueChannelColor;
    uint BlendMode;
    float3 Padding0;
};

static const uint BLEND_NORMAL = 0;
static const uint BLEND_ADD = 1;
static const uint BLEND_MULTIPLY = 2;
static const uint BLEND_SCREEN = 3;
static const uint BLEND_OVERLAY = 4;

float3 ApplyBlendMode(float3 baseColor, float3 paintColor, uint blendMode)
{
    switch (blendMode)
    {
        case BLEND_ADD:
            return saturate(baseColor + paintColor);

        case BLEND_MULTIPLY:
            return baseColor * paintColor;

        case BLEND_SCREEN:
            return 1.0 - (1.0 - baseColor) * (1.0 - paintColor);

        case BLEND_OVERLAY:
        {
            float3 low = 2.0 * baseColor * paintColor;
            float3 high = 1.0 - (2.0 * (1.0 - baseColor) * (1.0 - paintColor));
            return lerp(low, high, step(0.5, baseColor));
        }

        case BLEND_NORMAL:
        default:
            return paintColor;
    }
}

float4 ApplyChannelPaint(float4 baseColor, float channelMask, float4 channelColor, uint blendMode)
{
    const float channelStrength = saturate(channelMask * baseColor.a * channelColor.a);
    const float4 maskedColor = float4(channelColor.rgb, channelStrength);
    const float3 blendedColor = ApplyBlendMode(baseColor.rgb, maskedColor.rgb, blendMode);
    const float outputAlpha = saturate(baseColor.a * lerp(1.0, channelColor.a, channelMask));
    return float4(lerp(baseColor.rgb, blendedColor, maskedColor.a), outputAlpha);
}

[numthreads(8, 8, 1)]
void MainCS(uint3 dispatchThreadId : SV_DispatchThreadID)
{
    uint width;
    uint height;
    OutputTexture.GetDimensions(width, height);

    if (dispatchThreadId.x >= width || dispatchThreadId.y >= height)
    {
        return;
    }

    const uint2 pixel = dispatchThreadId.xy;
    const float2 uv = (float2(pixel) + 0.5) / float2(width, height);

    const float4 source = SourceTexture.SampleLevel(LinearClampSampler, uv, 0.0);
    const float3 channelMask = saturate(ChannelMaskTexture.SampleLevel(LinearClampSampler, uv, 0.0).rgb);

    float4 result = source;
    result = ApplyChannelPaint(result, channelMask.r, RedChannelColor, BlendMode);
    result = ApplyChannelPaint(result, channelMask.g, GreenChannelColor, BlendMode);
    result = ApplyChannelPaint(result, channelMask.b, BlueChannelColor, BlendMode);

    OutputTexture[pixel] = result;
}

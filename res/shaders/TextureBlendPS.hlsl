Texture2D inputTexture : register(t0);
Texture2D maskTexture : register(t1);
SamplerState samplerState : register(s0);

cbuffer ColorBlendParams : register(b0)
{
    float4 blendColor;
    uint blendMode; // 0 = none, 1 = add, 2 = multiply, 3 = screen, 4 = overlay, 5 = darken, 6 = lighten, 7 = color dodge, 8 = color burn, 9 = hard light, 10 = soft light
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
        // none
        blended = src.rgb;
    }
    else if (blendMode == 1)
    {
        // add
        blended = saturate(src.rgba + blendColor);
    }
    else if (blendMode == 2)
    {
        // multiply
        blended = src.rgba * blendColor;
    }
    else if (blendMode == 3)
    {
        // screen
        blended = 1.0 - (1.0 - src.rgba) * (1.0 - blendColor);
    }
    else if (blendMode == 4)
    {
        // overlay
        blended = lerp(2.0 * src.rgba * blendColor, 1.0 - 2.0 * (1.0 - src.rgba) * (1.0 - blendColor), step(0.5, src.rgba));
    }
    else if (blendMode == 5)
    {
        // darken
        blended = min(src.rgba, blendColor);
    }
    else if (blendMode == 6)
    {
        // lighten
        blended = max(src.rgba, blendColor);
    }
    else if (blendMode == 7)
    {
        // color dodge
        blended = src.rgba / (1.0 - blendColor);
        blended = saturate(blended);
    }
    else if (blendMode == 8)
    {
        // color burn
        blended = 1.0 - ((1.0 - src.rgba) / blendColor);
        blended = saturate(blended);
    }
    else if (blendMode == 9)
    {
        // hard light
        blended = lerp(2.0 * src.rgba * blendColor, 1.0 - 2.0 * (1.0 - src.rgba) * (1.0 - blendColor), step(0.5, blendColor));
    }
    else if (blendMode == 10)
    {
        // soft light
        blended = (1.0 - 2.0 * blendColor) * src.rgba * src.rgba + 2.0 * blendColor * src.rgba;
    }
    else
    {
        // fallback to none
        blended = src.rgba;
    }

    // Interpolate between original and blended color using maskValue
    float4 finalColor = lerp(src.rgba, blended, maskValue);

    return finalColor;
}
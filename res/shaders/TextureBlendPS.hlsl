Texture2D inputTexture : register(t0);
Texture2D maskTexture : register(t1);
SamplerState samplerState : register(s0);

cbuffer ColorBlendParams : register(b0)
{
    float3 blendColor;
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
        blended = saturate(src.rgb + blendColor);
    }
    else if (blendMode == 2)
    {
        // multiply
        blended = src.rgb * blendColor;
    }
    else if (blendMode == 3)
    {
        // screen
        blended = 1.0 - (1.0 - src.rgb) * (1.0 - blendColor);
    }
    else if (blendMode == 4)
    {
        // overlay
        blended = lerp(2.0 * src.rgb * blendColor, 1.0 - 2.0 * (1.0 - src.rgb) * (1.0 - blendColor), step(0.5, src.rgb));
    }
    else if (blendMode == 5)
    {
        // darken
        blended = min(src.rgb, blendColor);
    }
    else if (blendMode == 6)
    {
        // lighten
        blended = max(src.rgb, blendColor);
    }
    else if (blendMode == 7)
    {
        // color dodge
        blended = src.rgb / (1.0 - blendColor);
        blended = saturate(blended);
    }
    else if (blendMode == 8)
    {
        // color burn
        blended = 1.0 - ((1.0 - src.rgb) / blendColor);
        blended = saturate(blended);
    }
    else if (blendMode == 9)
    {
        // hard light
        blended = lerp(2.0 * src.rgb * blendColor, 1.0 - 2.0 * (1.0 - src.rgb) * (1.0 - blendColor), step(0.5, blendColor));
    }
    else if (blendMode == 10)
    {
        // soft light
        blended = (1.0 - 2.0 * blendColor) * src.rgb * src.rgb + 2.0 * blendColor * src.rgb;
    }
    else
    {
        // fallback to none
        blended = src.rgb;
    }

    // Interpolate between original and blended color using maskValue
    float3 finalColor = lerp(src.rgb, blended, maskValue);

    return float4(finalColor, src.a);
}
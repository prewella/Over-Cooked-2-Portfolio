#include "Engine_Shader_Defines.hlsli"

/* 전역변수 : 쉐이더 외부에 있는 데이터를 쉐이더 안으로 받아온다. */
matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

// For Slice
uint iNumCol, iNumRow, iIdx;

// For Extend
float g_fRatioX, g_fRatioY;

// For Gauge
float g_fPercent;

// For Falling
float g_fAccFalling;

// For Blind
float2 g_vBlindArea;

texture2D g_Texture;
texture2D g_DepthTexture;

struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

/* 정점 쉐이더 */
VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = In.vTexcoord;

    return Out;
}

struct VS_OUT_ALPHABLEND
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
};


VS_OUT_ALPHABLEND VS_MAIN_ALPHABLEND(VS_IN In)
{
    VS_OUT_ALPHABLEND Out = (VS_OUT_ALPHABLEND) 0;

    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = In.vTexcoord;
    Out.vProjPos = Out.vPosition;

    return Out;
}


struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct PS_OUT
{
    float4 vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
    if (Out.vColor.a <= 0.3f)
        discard;
	
    return Out;
}

PS_OUT PS_SLICE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float fColSize = 1 / (float) iNumCol;
    float fRowSize = 1 / (float) iNumRow;
    
    In.vTexcoord.x = (iIdx % iNumCol) * fColSize + (In.vTexcoord.x * fColSize);
    In.vTexcoord.y = (iIdx / iNumCol) * fRowSize + (In.vTexcoord.y * fRowSize);

    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
    if (Out.vColor.a <= 0.3f)
        discard;
	
    return Out;
}

PS_OUT PS_EXTEND(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    if (g_fRatioX < 1.f)
    {
        if (In.vTexcoord.x <= 0.45f * g_fRatioX)
        {
            In.vTexcoord.x = In.vTexcoord.x * 1 / g_fRatioX;
        }
        else if (In.vTexcoord.x >= 1 - (0.45f * g_fRatioX))
        {
            In.vTexcoord.x = 1 - ((1 - In.vTexcoord.x) * 1 / g_fRatioX);
        }
        else
        {
            float fCoord = (1 - (0.9f * g_fRatioX)) * 0.1f;
            In.vTexcoord.x = 0.45f + ((In.vTexcoord.x - (0.45f * g_fRatioX)) * fCoord);
        }
    }
    else
    {
        if (In.vTexcoord.x <= 0.45f / g_fRatioX)
        {
            In.vTexcoord.x = In.vTexcoord.x * g_fRatioX;
        }
        else if (In.vTexcoord.x >= 1 - (0.45f / g_fRatioX))
        {
            In.vTexcoord.x = 1 - ((1 - In.vTexcoord.x) * g_fRatioX);
        }
        else
        {
            float fCoord = (1 - (0.9f / g_fRatioX)) * 0.1f;
            In.vTexcoord.x = 0.45f + ((In.vTexcoord.x - (0.45f / g_fRatioX)) * fCoord);
        }
    }
    
    if (g_fRatioY < 1.f)
    {
        if (In.vTexcoord.y <= 0.45f * g_fRatioY)
        {
            In.vTexcoord.y = In.vTexcoord.y * 1 / g_fRatioY;
        }
        else if (In.vTexcoord.y >= 1 - (0.45f * g_fRatioY))
        {
            In.vTexcoord.y = 1 - ((1 - In.vTexcoord.y) * 1 / g_fRatioY);
        }
        else
        {
            float fCoord = (1 - (0.9f * g_fRatioY)) * 0.1f;
            In.vTexcoord.y = 0.45f + ((In.vTexcoord.y - (0.45f * g_fRatioY)) * fCoord);
        }
    }
    
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
    if (Out.vColor.a <= 0.3f)
        discard;
	
    return Out;
}

struct PS_IN_ALPHABLEND
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
};


PS_OUT PS_MAIN_ALPHABLEND(PS_IN_ALPHABLEND In)
{
    PS_OUT Out = (PS_OUT) 0;

    float2 vTexcoord = (float2) 0.f;

    vTexcoord.x = (In.vProjPos.x / In.vProjPos.w) * 0.5f + 0.5f;
    vTexcoord.y = (In.vProjPos.y / In.vProjPos.w) * -0.5f + 0.5f;

    //float4 vDepthDesc = g_DepthTexture.Sample(PointSampler, vTexcoord);
    //float fOldViewZ = vDepthDesc.y * 1000.f;
    
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
    Out.vColor.a = Out.vColor.a * saturate( /*fOldViewZ -*/In.vProjPos.w);

    return Out;
}

PS_OUT PS_SLICE_ALPHABLEND(PS_IN_ALPHABLEND In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float fColSize = 1 / (float) iNumCol;
    float fRowSize = 1 / (float) iNumRow;
    
    In.vTexcoord.x = (iIdx % iNumCol) * fColSize + (In.vTexcoord.x * fColSize);
    In.vTexcoord.y = (iIdx / iNumCol) * fRowSize + (In.vTexcoord.y * fRowSize);
    
    float2 vTexcoord = (float2) 0.f;

    //vTexcoord.x = (In.vProjPos.x / In.vProjPos.w) * 0.5f + 0.5f;
    //vTexcoord.y = (In.vProjPos.y / In.vProjPos.w) * -0.5f + 0.5f;

    //float4 vDepthDesc = g_DepthTexture.Sample(PointSampler, vTexcoord);
    //float fOldViewZ = vDepthDesc.y * 1000.f;
    
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
    Out.vColor.a = Out.vColor.a * saturate( /*fOldViewZ -*/In.vProjPos.w);

    return Out;
}

PS_OUT PS_EXTEND_ALPHABLEND(PS_IN_ALPHABLEND In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float2 vTexcoord = (float2) 0.f;
    
    if (g_fRatioX < 1.f)
    {
        if (In.vTexcoord.x <= 0.45f * g_fRatioX)
        {
            In.vTexcoord.x = In.vTexcoord.x * 1 / g_fRatioX;
        }
        else if (In.vTexcoord.x >= 1 - (0.45f * g_fRatioX))
        {
            In.vTexcoord.x = 1 - ((1 - In.vTexcoord.x) * 1 / g_fRatioX);
        }
        else
        {
            float fCoord = (1 - (0.9f * g_fRatioX)) * 0.1f;
            In.vTexcoord.x = 0.45f + ((In.vTexcoord.x - (0.45f * g_fRatioX)) * fCoord);
        }
    }
    else
    {
        if (In.vTexcoord.x <= 0.45f / g_fRatioX)
        {
            In.vTexcoord.x = In.vTexcoord.x * g_fRatioX;
        }
        else if (In.vTexcoord.x >= 1 - (0.45f / g_fRatioX))
        {
            In.vTexcoord.x = 1 - ((1 - In.vTexcoord.x) * g_fRatioX);
        }
        else
        {
            float fCoord = (1 - (0.9f / g_fRatioX)) * 0.1f;
            In.vTexcoord.x = 0.45f + ((In.vTexcoord.x - (0.45f / g_fRatioX)) * fCoord);
        }
    }
    
    if (g_fRatioY < 1.f)
    {
        if (In.vTexcoord.y <= 0.45f * g_fRatioY)
        {
            In.vTexcoord.y = In.vTexcoord.y * 1 / g_fRatioY;
        }
        else if (In.vTexcoord.y >= 1 - (0.45f * g_fRatioY))
        {
            In.vTexcoord.y = 1 - ((1 - In.vTexcoord.y) * 1 / g_fRatioY);
        }
        else
        {
            float fCoord = (1 - (0.9f * g_fRatioY)) * 0.1f;
            In.vTexcoord.y = 0.45f + ((In.vTexcoord.y - (0.45f * g_fRatioY)) * fCoord);
        }
    }
    

    //vTexcoord.x = (In.vProjPos.x / In.vProjPos.w) * 0.5f + 0.5f;
    //vTexcoord.y = (In.vProjPos.y / In.vProjPos.w) * -0.5f + 0.5f;

    //float4 vDepthDesc = g_DepthTexture.Sample(PointSampler, vTexcoord);
    //float fOldViewZ = vDepthDesc.y * 1000.f;
    
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
    Out.vColor.a = Out.vColor.a * saturate( /*fOldViewZ -*/In.vProjPos.w);

    return Out;
}

PS_OUT PS_GAUGE_ALPHABLEND(PS_IN_ALPHABLEND In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float2 vTexcoord = (float2) 0.f;

    //vTexcoord.x = (In.vProjPos.x / In.vProjPos.w) * 0.5f + 0.5f;
    //vTexcoord.y = (In.vProjPos.y / In.vProjPos.w) * -0.5f + 0.5f;

    //float4 vDepthDesc = g_DepthTexture.Sample(PointSampler, vTexcoord);
    //float fOldViewZ = vDepthDesc.y * 1000.f;
    
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
    
    if (In.vTexcoord.x >= g_fPercent)
        discard;
    
    Out.vColor.a = Out.vColor.a * saturate( /*fOldViewZ -*/In.vProjPos.w);

    return Out;
}

PS_OUT PS_GAUGE_COLOR_ALPHABLEND(PS_IN_ALPHABLEND In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float2 vTexcoord = (float2) 0.f;

    //vTexcoord.x = (In.vProjPos.x / In.vProjPos.w) * 0.5f + 0.5f;
    //vTexcoord.y = (In.vProjPos.y / In.vProjPos.w) * -0.5f + 0.5f;

    //float4 vDepthDesc = g_DepthTexture.Sample(PointSampler, vTexcoord);
    //float fOldViewZ = vDepthDesc.y * 1000.f;
    
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
    
    if (In.vTexcoord.x <= g_fPercent)
    {
        if (g_fPercent >= 0.5f)
            Out.vColor = float4((1 - g_fPercent) / g_fPercent, g_fPercent / g_fPercent, 0.f, Out.vColor.a);
        else
            Out.vColor = float4((1 - g_fPercent) / (1 - g_fPercent), g_fPercent / (1 - g_fPercent), 0.f, Out.vColor.a);
    }
    
    Out.vColor.a = Out.vColor.a * saturate( /*fOldViewZ -*/In.vProjPos.w);

    return Out;
}

PS_OUT PS_FALLING_ALPHABLEND(PS_IN_ALPHABLEND In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float2 vTexcoord = (float2) 0.f;

    //vTexcoord.x = (In.vProjPos.x / In.vProjPos.w) * 0.5f + 0.5f;
    //vTexcoord.y = (In.vProjPos.y / In.vProjPos.w) * -0.5f + 0.5f;

    //float4 vDepthDesc = g_DepthTexture.Sample(PointSampler, vTexcoord);
    //float fOldViewZ = vDepthDesc.y * 1000.f;
    
    In.vTexcoord.y += g_fAccFalling % 1.f;
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);

    
    Out.vColor.a = Out.vColor.a * saturate( /*fOldViewZ -*/In.vProjPos.w);

    return Out;
}

PS_OUT PS_BLIND_ALPHABLEND(PS_IN_ALPHABLEND In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float2 vTexcoord = (float2) 0.f;

    //vTexcoord.x = (In.vProjPos.x / In.vProjPos.w) * 0.5f + 0.5f;
    //vTexcoord.y = (In.vProjPos.y / In.vProjPos.w) * -0.5f + 0.5f;

    //float4 vDepthDesc = g_DepthTexture.Sample(PointSampler, vTexcoord);
    //float fOldViewZ = vDepthDesc.y * 1000.f;
    
    if (In.vTexcoord.x > g_vBlindArea.x * 0.5f && In.vTexcoord.x < 1 - g_vBlindArea.x * 0.5f)
    {
        if (In.vTexcoord.y > g_vBlindArea.y * 0.5f && In.vTexcoord.y < 1 - g_vBlindArea.y * 0.5f)
        {
            discard;
        }
    }
    
    Out.vColor = float4(0.f, 0.f, 0.f, 1.f);
    
    Out.vColor.a = Out.vColor.a * saturate( /*fOldViewZ -*/In.vProjPos.w);

    return Out;
}

PS_OUT PS_LIGHTBOX_ALPHABLEND(PS_IN_ALPHABLEND In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float2 vTexcoord = (float2) 0.f;

    //vTexcoord.x = (In.vProjPos.x / In.vProjPos.w) * 0.5f + 0.5f;
    //vTexcoord.y = (In.vProjPos.y / In.vProjPos.w) * -0.5f + 0.5f;

    //float4 vDepthDesc = g_DepthTexture.Sample(PointSampler, vTexcoord);
    //float fOldViewZ = vDepthDesc.y * 1000.f;
    
    Out.vColor = float4(0.f, 0.f, 0.f, 0.5f);
    
    Out.vColor.a = Out.vColor.a * saturate( /*fOldViewZ -*/In.vProjPos.w);

    return Out;
}

technique11 DefaultTechnique
{
    pass Default // 0
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = /*compile gs_5_0 GS_MAIN()*/NULL;
        HullShader = /*compile hs_5_0 HS_MAIN()*/NULL;
        DomainShader = /*compile ds_5_0 DS_MAIN()*/NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass Slice_UV // 1
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = /*compile gs_5_0 GS_MAIN()*/NULL;
        HullShader = /*compile hs_5_0 HS_MAIN()*/NULL;
        DomainShader = /*compile ds_5_0 DS_MAIN()*/NULL;
        PixelShader = compile ps_5_0 PS_SLICE();
    }

    pass Extend // 2
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = /*compile gs_5_0 GS_MAIN()*/NULL;
        HullShader = /*compile hs_5_0 HS_MAIN()*/NULL;
        DomainShader = /*compile ds_5_0 DS_MAIN()*/NULL;
        PixelShader = compile ps_5_0 PS_EXTEND();
    }

    pass Blend // 3
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN_ALPHABLEND();
        GeometryShader = /*compile gs_5_0 GS_MAIN()*/NULL;
        HullShader = /*compile hs_5_0 HS_MAIN()*/NULL;
        DomainShader = /*compile ds_5_0 DS_MAIN()*/NULL;
        PixelShader = compile ps_5_0 PS_MAIN_ALPHABLEND();
    }

    pass Blend_Slice // 4
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN_ALPHABLEND();
        GeometryShader = /*compile gs_5_0 GS_MAIN()*/NULL;
        HullShader = /*compile hs_5_0 HS_MAIN()*/NULL;
        DomainShader = /*compile ds_5_0 DS_MAIN()*/NULL;
        PixelShader = compile ps_5_0 PS_SLICE_ALPHABLEND();
    }

    pass Blend_Extend // 5
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN_ALPHABLEND();
        GeometryShader = /*compile gs_5_0 GS_MAIN()*/NULL;
        HullShader = /*compile hs_5_0 HS_MAIN()*/NULL;
        DomainShader = /*compile ds_5_0 DS_MAIN()*/NULL;
        PixelShader = compile ps_5_0 PS_EXTEND_ALPHABLEND();
    }

    pass Blend_Gauge // 6
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN_ALPHABLEND();
        GeometryShader = /*compile gs_5_0 GS_MAIN()*/NULL;
        HullShader = /*compile hs_5_0 HS_MAIN()*/NULL;
        DomainShader = /*compile ds_5_0 DS_MAIN()*/NULL;
        PixelShader = compile ps_5_0 PS_GAUGE_ALPHABLEND();
    }

    pass Blend_Gauge_Color // 7
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN_ALPHABLEND();
        GeometryShader = /*compile gs_5_0 GS_MAIN()*/NULL;
        HullShader = /*compile hs_5_0 HS_MAIN()*/NULL;
        DomainShader = /*compile ds_5_0 DS_MAIN()*/NULL;
        PixelShader = compile ps_5_0 PS_GAUGE_COLOR_ALPHABLEND();
    }

    pass Blend_Falling // 8
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN_ALPHABLEND();
        GeometryShader = /*compile gs_5_0 GS_MAIN()*/NULL;
        HullShader = /*compile hs_5_0 HS_MAIN()*/NULL;
        DomainShader = /*compile ds_5_0 DS_MAIN()*/NULL;
        PixelShader = compile ps_5_0 PS_FALLING_ALPHABLEND();
    }

    pass Blend_Blind // 9
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN_ALPHABLEND();
        GeometryShader = /*compile gs_5_0 GS_MAIN()*/NULL;
        HullShader = /*compile hs_5_0 HS_MAIN()*/NULL;
        DomainShader = /*compile ds_5_0 DS_MAIN()*/NULL;
        PixelShader = compile ps_5_0 PS_BLIND_ALPHABLEND();
    }

    pass Blend_LightBox // 10
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN_ALPHABLEND();
        GeometryShader = /*compile gs_5_0 GS_MAIN()*/NULL;
        HullShader = /*compile hs_5_0 HS_MAIN()*/NULL;
        DomainShader = /*compile ds_5_0 DS_MAIN()*/NULL;
        PixelShader = compile ps_5_0 PS_LIGHTBOX_ALPHABLEND();
    }
}
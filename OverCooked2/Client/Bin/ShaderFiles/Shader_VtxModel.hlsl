#include "Engine_Shader_Defines.hlsli"

/* 전역변수 : 쉐이더 외부에 있는 데이터를 쉐이더 안으로 받아온다. */
matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D g_DiffuseTexture;
//texture2D g_NormalTexture;

//For ColorSet
float4 vDiffuseColor;

// For Moving
float g_fAccFalling;

struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float3 vTangent : TANGENT;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float3 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;

    float3 vTangent : TANGENT;
    float3 vBinormal : BINORMAL;
};

/* 정점 쉐이더 */
VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
    Out.vNormal = normalize(mul(float4(In.vNormal, 0.f), g_WorldMatrix)).xyz;
    Out.vTangent = normalize(mul(float4(In.vTangent, 0.f), g_WorldMatrix)).xyz;
    Out.vBinormal = normalize(cross(Out.vNormal, Out.vTangent));
    Out.vTexcoord = In.vTexcoord;
    Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
    Out.vProjPos = Out.vPosition;

    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float3 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;

    float3 vTangent : TANGENT;
    float3 vBinormal : BINORMAL;
};

struct PS_OUT
{
    float4 vDiffuse : SV_TARGET0;
    float4 vNormal : SV_TARGET1;
    float4 vDepth : SV_TARGET2;
    float4 vFieldDepth : SV_TARGET3;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    //if (0.3f >= vMtrlDiffuse.a)
    //    discard;

    //vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);

    //float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;

    float3x3 WorldMatrix = float3x3(In.vTangent, In.vBinormal, In.vNormal);

    //float3 vWorldNormal = mul(vNormal, WorldMatrix);

    Out.vDiffuse = vMtrlDiffuse;
    //Out.vNormal = vector(vWorldNormal * 0.5f + 0.5f, 0.f);
    Out.vNormal = vector((In.vNormal * 0.5f + 0.5f).xyz, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.0f, 0.0f, 0.0f);
    Out.vFieldDepth = vector(In.vProjPos.z / In.vProjPos.w, 0.f, 0.0f, 0.0f);
    return Out;
}

PS_OUT PS_SETCOLOR(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    //if (0.3f >= vMtrlDiffuse.a)
    //    discard;

    //vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);

    //float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;

    float3x3 WorldMatrix = float3x3(In.vTangent, In.vBinormal, In.vNormal);

    //float3 vWorldNormal = mul(vNormal, WorldMatrix);
    
    
    Out.vDiffuse = vDiffuseColor; //float4(0.f,0.85f, 1.f, 0.75f);
    //Out.vNormal = vector(vWorldNormal * 0.5f + 0.5f, 0.f);
    Out.vNormal = vector((In.vNormal * 0.5f + 0.5f).xyz, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.0f, 0.0f, 0.0f);
    Out.vFieldDepth = vector(In.vProjPos.z / In.vProjPos.w, 0.f, 0.0f, 0.0f);
    return Out;
}

PS_OUT PS_GLASS(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    //if (0.3f >= vMtrlDiffuse.a)
    //    discard;

    //vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);

    //float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;

    float3x3 WorldMatrix = float3x3(In.vTangent, In.vBinormal, In.vNormal);

    //float3 vWorldNormal = mul(vNormal, WorldMatrix);

    Out.vDiffuse = float4(0.f,0.f, 0.f, 0.f);
    //Out.vNormal = vector(vWorldNormal * 0.5f + 0.5f, 0.f);
    Out.vNormal = vector((In.vNormal * 0.5f + 0.5f).xyz, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.0f, 0.0f, 0.0f);
    Out.vFieldDepth = vector(In.vProjPos.z / In.vProjPos.w, 0.f, 0.0f, 0.0f);
    
    if (Out.vDiffuse.w == 0.f)
        discard;
    
        return Out;
}

PS_OUT PS_CONTROL_MARK(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    if (0.3f >= vMtrlDiffuse.a)
        discard;

    //vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);

    //float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;

    float3x3 WorldMatrix = float3x3(In.vTangent, In.vBinormal, In.vNormal);

    //float3 vWorldNormal = mul(vNormal, WorldMatrix);

    Out.vDiffuse = vMtrlDiffuse;
    //Out.vNormal = vector(vWorldNormal * 0.5f + 0.5f, 0.f);
    Out.vNormal = vector((In.vNormal * 0.5f + 0.5f).xyz, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.0f, 0.0f, 0.0f);
    Out.vFieldDepth = vector(In.vProjPos.z / In.vProjPos.w, 0.f, 0.0f, 0.0f);
    return Out;
}


PS_OUT PS_HIGHLIGHT(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    //if (0.3f >= vMtrlDiffuse.a)
    //    discard;

    //vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);

    //float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;

    float3x3 WorldMatrix = float3x3(In.vTangent, In.vBinormal, In.vNormal);

    //float3 vWorldNormal = mul(vNormal, WorldMatrix);

    Out.vDiffuse = vMtrlDiffuse + float4(0.25f, 0.25f, 0.25f, 0.25f);
    //Out.vNormal = vector(vWorldNormal * 0.5f + 0.5f, 0.f);
    Out.vNormal = vector((In.vNormal * 0.5f + 0.5f).xyz, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.0f, 0.0f, 0.0f);
    Out.vFieldDepth = vector(In.vProjPos.z / In.vProjPos.w, 0.f, 0.0f, 0.0f);
    return Out;
}

PS_OUT PS_MOVING(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    In.vTexcoord = In.vTexcoord * 3.f;
    
    In.vTexcoord.x -= g_fAccFalling % 1.f;
    In.vTexcoord.y += 0.75f;
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord );
   
    float3x3 WorldMatrix = float3x3(In.vTangent, In.vBinormal, In.vNormal);
    

    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = vector((In.vNormal * 0.5f + 0.5f).xyz, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.0f, 0.0f, 0.0f);
    Out.vFieldDepth = vector(In.vProjPos.z / In.vProjPos.w, 0.f, 0.0f, 0.0f);
    return Out;
}


PS_OUT PS_ALPHATEST(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    if (0.f >= vMtrlDiffuse.a)
        discard;

    //vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);

    //float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;

    float3x3 WorldMatrix = float3x3(In.vTangent, In.vBinormal, In.vNormal);

    //float3 vWorldNormal = mul(vNormal, WorldMatrix);

    Out.vDiffuse = vMtrlDiffuse;
    //Out.vNormal = vector(vWorldNormal * 0.5f + 0.5f, 0.f);
    Out.vNormal = vector((In.vNormal * 0.5f + 0.5f).xyz, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.0f, 0.0f, 0.0f);
    Out.vFieldDepth = vector(In.vProjPos.z / In.vProjPos.w, 0.f, 0.0f, 0.0f);
    return Out;
}

PS_OUT PS_MOVING_ORIGIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    In.vTexcoord.x += g_fAccFalling % 1.f;
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
   
    float3x3 WorldMatrix = float3x3(In.vTangent, In.vBinormal, In.vNormal);
    

    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = vector((In.vNormal * 0.5f + 0.5f).xyz, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.0f, 0.0f, 0.0f);
    Out.vFieldDepth = vector(In.vProjPos.z / In.vProjPos.w, 0.f, 0.0f, 0.0f);
    return Out;
}

PS_OUT PS_MOVING_ORIGIN_HIGHLIGHT(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    In.vTexcoord.x += g_fAccFalling % 1.f;
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
   
    float3x3 WorldMatrix = float3x3(In.vTangent, In.vBinormal, In.vNormal);
    

    Out.vDiffuse = vMtrlDiffuse + float4(0.25f, 0.25f, 0.25f, 0.25f);
    Out.vNormal = vector((In.vNormal * 0.5f + 0.5f).xyz, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.0f, 0.0f, 0.0f);
    Out.vFieldDepth = vector(In.vProjPos.z / In.vProjPos.w, 0.f, 0.0f, 0.0f);
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

    pass SetColor // 1
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = /*compile gs_5_0 GS_MAIN()*/NULL;
        HullShader = /*compile hs_5_0 HS_MAIN()*/NULL;
        DomainShader = /*compile ds_5_0 DS_MAIN()*/NULL;
        PixelShader = compile ps_5_0 PS_SETCOLOR();
    }

    pass Glass // 2
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = /*compile gs_5_0 GS_MAIN()*/NULL;
        HullShader = /*compile hs_5_0 HS_MAIN()*/NULL;
        DomainShader = /*compile ds_5_0 DS_MAIN()*/NULL;
        PixelShader = compile ps_5_0 PS_GLASS();
    }

    pass Control_Mark // 3
    {
        SetRasterizerState(RS_NoneCull);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = /*compile gs_5_0 GS_MAIN()*/NULL;
        HullShader = /*compile hs_5_0 HS_MAIN()*/NULL;
        DomainShader = /*compile ds_5_0 DS_MAIN()*/NULL;
        PixelShader = compile ps_5_0 PS_CONTROL_MARK();
    }

    pass HighLight // 4
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = /*compile gs_5_0 GS_MAIN()*/NULL;
        HullShader = /*compile hs_5_0 HS_MAIN()*/NULL;
        DomainShader = /*compile ds_5_0 DS_MAIN()*/NULL;
        PixelShader = compile ps_5_0 PS_HIGHLIGHT();
    }

    pass Moving // 5
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = /*compile gs_5_0 GS_MAIN()*/NULL;
        HullShader = /*compile hs_5_0 HS_MAIN()*/NULL;
        DomainShader = /*compile ds_5_0 DS_MAIN()*/NULL;
        PixelShader = compile ps_5_0 PS_MOVING();
    }

    pass ALPHA_TEST // 6
    {
        SetRasterizerState(RS_NoneCull);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = /*compile gs_5_0 GS_MAIN()*/NULL;
        HullShader = /*compile hs_5_0 HS_MAIN()*/NULL;
        DomainShader = /*compile ds_5_0 DS_MAIN()*/NULL;
        PixelShader = compile ps_5_0 PS_ALPHATEST();
    }

    pass Moving_Origin // 7
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = /*compile gs_5_0 GS_MAIN()*/NULL;
        HullShader = /*compile hs_5_0 HS_MAIN()*/NULL;
        DomainShader = /*compile ds_5_0 DS_MAIN()*/NULL;
        PixelShader = compile ps_5_0 PS_MOVING_ORIGIN();
    }

    pass Moving_Origin_HightLight // 8
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = /*compile gs_5_0 GS_MAIN()*/NULL;
        HullShader = /*compile hs_5_0 HS_MAIN()*/NULL;
        DomainShader = /*compile ds_5_0 DS_MAIN()*/NULL;
        PixelShader = compile ps_5_0 PS_MOVING_ORIGIN_HIGHLIGHT();
    }
}
#include "common.slangh"

[[vk::push_constant]]
PushConstants pc;

struct VSInput {
    float3 inPosition;
    float3 inColor;
    float2 inTexCoord;
};

struct VSOutput {
    float4 pos  : SV_Position;
    float3 color ;
    float2 fragTexCoord;
};

// [[vk::binding(0)]] ConstantBuffer<UniformBuffer> ubo;

[[vk::binding(0,0)]] ConstantBuffer<FrameData> u_frame;
[[vk::binding(0,1)]] Sampler2D v_texture;



[shader("vertex")]
VSOutput vertMain(VSInput input) {
    VSOutput output;
    // output.pos = mul(u_frame.viewProj,mul(pc.modelMatrix, float4(input.inPosition,1.0f)));
    // output.pos = mul(pc.modelMatrix, float4(input.inPosition,1.0f));
    output.pos = float4(input.inPosition,1.0f);
    output.color = input.inColor;
    output.fragTexCoord = input.inTexCoord;
    return output;
}

[shader("fragment")]
float4 fragMain(VSOutput vertIn): SV_Target
{
    float3 result =  float3(0.1,0.1,0.7) * v_texture.Sample(vertIn.fragTexCoord).rgb;
    // result = float3( abs(u_frame.viewProj[3][2]),abs(u_frame.viewProj[0][0]),0.0f ); 
    return float4(result,1.0f);
}

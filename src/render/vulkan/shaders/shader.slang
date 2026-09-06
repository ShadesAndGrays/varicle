struct PushConstants {
    float4x4 mvpMatrix;
    // float4 colorTint;
    // uint textureIndex;
} 

[[vk::push_constant]]
PushConstants pc;

struct VSInput {
    float3 inPosition;
    float3 inColor;
    float2 inTexCoord;
};

struct UniformBuffer {
    float4x4 model;
    float4x4 view;
    float4x4 proj;
};


struct VSOutput {
    float4 pos  : SV_Position;
    float3 color ;
    float2 fragTexCoord;
};

[[vk::binding(0)]] ConstantBuffer<UniformBuffer> ubo;
[[vk::binding(1)]] Sampler2D texture;



[shader("vertex")]
VSOutput vertMain(VSInput input) {
    VSOutput output;
    // output.pos = 
    // mul(
    //     ubo.proj,
    //     mul (
    //         ubo.view,
    //         mul(
    //             ubo.model,
    //             float4(input.inPosition, 1.0)
    //         )
    //     )
    // );
    // output.pos = mul(pc.mvpMatrix, float4(input.inPosition,1.0f));
    output.pos = mul(pc.mvpMatrix, float4(input.inPosition,1.0f));
    output.color = input.inColor;
    output.fragTexCoord = input.inTexCoord;
    return output;
}

[shader("fragment")]
float4 fragMain(VSOutput vertIn): SV_Target
{
    float3 result =  vertIn.color * texture.Sample(vertIn.fragTexCoord).rgb;
    return float4(result,1.0);
}

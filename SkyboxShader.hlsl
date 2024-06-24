cbuffer MatrixBuffer{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

struct VertexData{
	float4 position : POSITION;
};

TextureCube Cubemap : register(t0);
SamplerState Sampler : register(s0);


struct Interpolators{
	float4 position : SV_POSITION;
	float3 wPos : POSITION;
};



Interpolators SkyboxVertexShader(VertexData input){
	Interpolators output;

	float4 wPos;
	float4 viewPos;

    input.position.w = 1.0f;
	wPos = mul(input.position, worldMatrix);
	output.wPos = wPos.xyz;

	viewPos = mul(float4(wPos.xyz, 0.0), viewMatrix);
	output.position = mul(viewPos, projectionMatrix);
	output.position.z = output.position.w; 

	return output;
}

float4 SkyboxPixelShader(Interpolators input) : SV_TARGET{
	return Cubemap.Sample(Sampler, input.wPos);
}
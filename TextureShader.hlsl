cbuffer MatrixBuffer{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

struct VertexData{
	float4 position : POSITION;
	float4 color : COLOR;
	float2 uv : TEXCOORD;
};

Texture2D shaderTexture : register(t0);
SamplerState SampleType : register(s0);


struct Interpolators{
	float4 position : SV_POSITION;
	float4 color : COLOR;
	float2 uv : TEXCOORD;
};



Interpolators TextureVertexShader(VertexData input){
	Interpolators output;

    input.position.w = 1.0f;
	output.position = mul( input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	output.color = input.color;
	output.uv = input.uv;

	return output;
}

float4 TexturePixelShader(Interpolators input) : SV_TARGET{
	float4 col;


	float4 texCol = shaderTexture.Sample(SampleType, input.uv);
	col = texCol;
	col = input.color * texCol + input.color/1.5f;		//blends fragment color with texture color, will mess up 2D rendering

	return col;
}
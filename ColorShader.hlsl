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


struct Interpolators{
	float4 position : SV_POSITION;
	float4 color : COLOR;
	float2 uv : TEXCOORD;
};



Interpolators ColorVertexShader(VertexData input){
	Interpolators output;

    input.position.w = 1.0f;
	output.position = mul( input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	output.color = input.color;
	output.uv = input.uv;

	return output;
}

float4 ColorPixelShader(Interpolators input) : SV_TARGET{
	return input.color;
}
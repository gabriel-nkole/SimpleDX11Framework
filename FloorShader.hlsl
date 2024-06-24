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
	float2 uv : TEXCOORD;
};



Interpolators FloorVertexShader(VertexData input){
	Interpolators output;

    input.position.w = 1.0f;
	output.position = mul( input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	output.uv = input.uv;

	return output;
}

float4 FloorPixelShader(Interpolators input) : SV_TARGET{
	float4 col;

	float thickness = 0.05f;
	float halfThickness = thickness/2.0f;

	float2 uv = input.uv * 50.0f;
	//if at boundary double the thickness
	if(uv.x < thickness || uv.x > 50.0f-thickness || uv.y < thickness || uv.y > 50.0f-thickness){
		halfThickness = thickness;
	}
	uv = frac(uv);
	

	float4 mainCol = float4(1.0f, 0.3f, 0.4f, 1.0f);
	mainCol.xyz *= 0.8f;
	float4 lineCol = 1.0f;
	//set output color to main color by default
	col = mainCol;

	//if close to edge, make output color the line color
	if(uv.x < halfThickness || uv.x > 1 - halfThickness || uv.y < halfThickness || uv.y > 1 - halfThickness){
		col = lineCol;
	}

	return col;
}
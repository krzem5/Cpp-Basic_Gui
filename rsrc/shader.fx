// Phong shading => https://docs.microsoft.com/en-us/windows/win32/direct3dgetstarted/work-with-shaders-and-shader-resources#review-the-pixel-shader
cbuffer ConstantBuffer:register(b0){
	matrix wm;
	matrix vm;
	matrix pm;
	float4 l_d[2];
	float4 l_c[2];
}
Texture2D tx:register(t0);
SamplerState ss:register(s0);



struct PixelShaderInput{
	float4 p:SV_POSITION;
	float3 n:NORMAL;
	float2 t:TEXCOORD0;
};



PixelShaderInput vertex_shader(float4 p:POSITION,float3 n:NORMAL,float2 t:TEXCOORD0){
	PixelShaderInput o={
		mul(mul(mul(p,wm),vm),pm),
		mul(float4(n,1),wm).xyz,
		t
	};
	return o;
}



float4 pixel_shader(float4 p:SV_POSITION,float3 n:NORMAL,float2 t:TEXCOORD0):SV_TARGET0{
	float4 o=0;
	for (int j=0;j<2;j++){
		o+=saturate(dot((float3)l_d[j],n)*l_c[j]);
	}
	return tx.Sample(ss,t)+o*0.3;
}

cbuffer ConstantBuffer:register(b0){
	matrix wm;
	matrix vm;
	matrix pm;
	float4 l_d[2];
	float4 l_c[2];
	float4 o_c;
}



struct PixelShaderInput{
	float4 Pos:SV_POSITION;
	float3 Norm:TEXCOORD0;
};



PixelShaderInput vertex_shader(float4 p:POSITION,float3 n:NORMAL){
	PixelShaderInput o=(PixelShaderInput)0;
	o.Pos=mul(mul(mul(p,wm),vm),pm);
	o.Norm=mul(float4(n,1),wm).xyz;
	return o;
}



float4 pixel_shader(float4 Pos:SV_POSITION,float3 Norm:TEXCOORD0):SV_Target{
	float4 o=0;
	for(int j=0;j<2;j++){
		o+=saturate(dot((float3)l_d[j],Norm)*l_c[j]);
	}
	o.a=1;
	return o;
}



float4 pixel_shader_solid(float4 Pos:SV_POSITION,float3 Norm:TEXCOORD0):SV_Target{
	return o_c;
}

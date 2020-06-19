#pragma pack_matrix(row_major)



cbuffer core:register(b0){
	matrix wm;
	matrix vm;
	matrix pm;
	uint lc;
	struct light{
		float4 p;
		float4 c;
	} ll[8];
};



cbuffer g_mat:register(b1){
	float4 Ka;
	float4 Kd;
	float4 Ks;
	float Ns;
	matrix t;
	matrix nt;
};



struct VS_OUT{
	float4 p:SV_POSITION;
	float3 wp:POSITION;
	float3 n:NORMAL;
	float2 tx:TEXCOORD;
};



VS_OUT vertex_shader(float4 p:POSITION,float2 tx:TEXCOORD,float3 n:NORMAL){
	VS_OUT o={mul(mul(mul(mul(p,t),wm),vm),pm),mul(mul(p,t),wm).xyz,normalize(mul(mul(n,(float3x3)nt),(float3x3)wm)),tx};
	return o;
}



float4 pixel_shader(float4 p:SV_POSITION,float3 wp:POSITION,float3 n:NORMAL,float2 tx:TEXCOORD):SV_Target{
	return Kd+saturate(dot((float3)ll[0].p,normalize(n))*ll[0].c)*0.3;
}



//float4 pixel_phong(float4 position:SV_POSITION,float3 outVec:POSITION0,float3 normal:NORMAL0,float3 light:POSITION1):SV_TARGET{
//	float3 L=normalize(light);
//	return saturate(Ka+(lightColor*Kd*max(dot(normal,L),0.0f)))+saturate(Ks*pow(max(dot(normalize(reflect(L,normal)),normalize(outVec)),0.0f),shininess.x-50.0f));
//}

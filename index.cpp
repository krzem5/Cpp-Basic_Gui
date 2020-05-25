#include "window.h"
#include <iostream>



struct VertexShaderInput{
	Matrix wm;
	Matrix vm;
	Matrix pm;
	Vector4 l_d[2];
	Vector4 l_c[2];
};



ulong vs;
ulong ps;
ulong ob;
ulong cb;
ulong ss;
ulong tx;



void i_cb(Window* w){
	w->renderer.enable_vsync=true;
	VertexShaderInputLayout vs_inp[]={
		{
			"POSITION",
			0,
			DXGI_FORMAT_R32G32B32_FLOAT,
			0,
			0,
			D3D11_INPUT_PER_VERTEX_DATA,
			0
		},
		{
			"NORMAL",
			0,
			DXGI_FORMAT_R32G32B32_FLOAT,
			0,
			12,
			D3D11_INPUT_PER_VERTEX_DATA,
			0
		},
		{
			"TEXCOORD",
			0,
			DXGI_FORMAT_R32G32_FLOAT,
			0,
			24,
			D3D11_INPUT_PER_VERTEX_DATA,
			0
		}
	};
	D3D11_SAMPLER_DESC sd={D3D11_FILTER_MIN_MAG_MIP_LINEAR,
		D3D11_TEXTURE_ADDRESS_CLAMP,
		D3D11_TEXTURE_ADDRESS_CLAMP,
		D3D11_TEXTURE_ADDRESS_CLAMP,
		0.0f,
		1,
		D3D11_COMPARISON_NEVER,
		{
			1.0f,
			1.0f,
			1.0f,
			1.0f
		},
		-FLT_MAX,
		FLT_MAX
	};
	ulong vs=w->renderer.load_vertex_shader(L"rsrc/shader.fx","vertex_shader","vs_4_0",vs_inp,3);
	ulong ps=w->renderer.load_pixel_shader(L"rsrc/shader.fx","pixel_shader","ps_4_0");
	ob=w->renderer.create_object_buffer(8);
	ObjectBuffer::ObjectBufferData* ob1=w->renderer.get_object_buffer(ob);
	ob1->add_vertexes({
		-1,-1,-1,0,-1,0,0,0,
		-1,-1,1,0,-1,0,0,1,
		1,-1,1,0,-1,0,1,1,
		1,-1,-1,0,-1,0,1,0,

		-1,-1,-1,-1,0,0,0,0,
		-1,-1,1,-1,0,0,0,1,
		-1,1,1,-1,0,0,1,1,
		-1,1,-1,-1,0,0,1,0,

		-1,-1,-1,0,0,-1,0,0,
		-1,1,-1,0,0,-1,0,1,
		1,1,-1,0,0,-1,1,1,
		1,-1,-1,0,0,-1,1,0,

		-1,1,-1,0,1,0,0,0,
		-1,1,1,0,1,0,0,1,
		1,1,1,0,1,0,1,1,
		1,1,-1,0,1,0,1,0,

		1,-1,-1,1,0,0,0,0,
		1,-1,1,1,0,0,0,1,
		1,1,1,1,0,0,1,1,
		1,1,-1,1,0,0,1,0,

		-1,-1,1,0,0,1,0,0,
		-1,1,1,0,0,1,0,1,
		1,1,1,0,0,1,1,1,
		1,-1,1,0,0,1,1,0
	});
	ob1->add_indicies({
		2,1,0,
		3,2,0,

		4,5,6,
		4,6,7,

		8,9,10,
		8,10,11,

		12,13,14,
		12,14,15,

		18,17,16,
		19,18,16,

		22,21,20,
		23,22,20
	});
	w->renderer.update_object_buffer(ob);
	// ob=RendererHelper::create_object_buffer_box(&w->renderer,Vector3(0,0,0),2);
	cb=w->renderer.create_constant_buffer(sizeof(VertexShaderInput));
	ss=w->renderer.create_sampler_state(sd);
	tx=w->renderer.read_texture_file(L"rsrc/box.png",true);
}



void u_cb(Window* w,double dt){
	static double t=0;
	t+=dt;
	w->renderer.clear();
	Matrix w_m=RendererHelper::create_y_rotation_matrix(t);
	Vector4 l_d[2]={
		Vector4(-0.577f,0.577f,-0.577f,1.0f),
		Vector4(0.0f,0.0f,-1.0f,1.0f),
	};
	Vector4 l_c[2]={
		Vector4(0.5f,0.5f,0.5f,1.0f),
		Vector4(0.5f,0.0f,0.0f,1.0f)
	};
	Matrix rm=RendererHelper::create_y_rotation_matrix(-2.0f*t);
	DirectX::XMStoreFloat4(&l_d[1],DirectX::XMVector3Transform(DirectX::XMLoadFloat4(&l_d[1]),rm));
	w->renderer.clear_color[2]=0.8f;
	VertexShaderInput cb1={};
	cb1.wm=RendererHelper::transpose_matrix(w_m);
	cb1.vm=RendererHelper::transpose_matrix(w->renderer.view_matrix);
	cb1.pm=RendererHelper::transpose_matrix(w->renderer.projection_matrix);
	cb1.l_d[0]=l_d[0];
	cb1.l_d[1]=l_d[1];
	cb1.l_c[0]=l_c[0];
	cb1.l_c[1]=l_c[1];
	w->renderer.update_constant_buffer(cb,&cb1);
	w->renderer.set_shader_data({
		{
			SHADER_DATA_TYPE_CONSTANT_BUFFER,
			cb,
			0
		},
		{
			SHADER_DATA_TYPE_TEXTURE,
			tx,
			0
		},
		{
			SHADER_DATA_TYPE_SAMPLER_STATE,
			ss,
			0
		}
	});
	w->renderer.use_vertex_shader(vs);
	w->renderer.use_pixel_shader(ps);
	w->renderer.render_object_buffer(ob);
	for (int i=0;i<2;i++){
		cb1.wm=RendererHelper::transpose_matrix(RendererHelper::create_scaling_matrix(0.2f,0.2f,0.2f)*RendererHelper::create_translation_matrix(5.0f*RendererHelper::create_vector(&l_d[i])));
		w->renderer.update_constant_buffer(cb,&cb1);
		w->renderer.render_object_buffer(ob);
	}
	w->renderer.show();
	if (w->pressed(27)==true){
		w->close();
	}
}



int main(int argc,char** argv){
	Window w(600,600,800,800,L"Window Name",&i_cb,&u_cb);
	std::cout<<"Window Created!"<<std::endl;
	w.merge_thread();
	std::cout<<"End!";
	return 0;
}

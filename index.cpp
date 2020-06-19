#include "window.h"
#include <iostream>



using namespace krzem;



struct VertexShaderInput{
	Matrix wm;
	Matrix vm;
	Matrix pm;
	uint lc;
	int* _=new int[3];
	struct light{
		Vector p;
		Vector c;
	} ll[8];
};



ulong vs;
ulong ps;
ulong ob;
ulong cb;
ulong ss;
ulong tx;
Camera c;
OBJFile obj;



void i_cb(Window* w){
	w->renderer.enable_vsync=true;
	VS_INPUT_LAYOUT vs_inp[]={
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
			"TEXCOORD",
			0,
			DXGI_FORMAT_R32G32_FLOAT,
			0,
			12,
			D3D11_INPUT_PER_VERTEX_DATA,
			0
		},
		{
			"NORMAL",
			0,
			DXGI_FORMAT_R32G32B32_FLOAT,
			0,
			20,
			D3D11_INPUT_PER_VERTEX_DATA,
			0
		}
	};
	ulong vs=w->renderer.load_vertex_shader(L"rsrc/phong.hlsl","vertex_shader","vs_4_0",vs_inp,3);
	ulong ps=w->renderer.load_pixel_shader(L"rsrc/phong.hlsl","pixel_shader","ps_4_0");
	cb=w->renderer.create_constant_buffer(sizeof(VertexShaderInput));
	c.set_window(w);
	c.lock=true;
	c.enabled=true;
	c.MOVE_SPEED=5;
	c.ROT_SPEED=5;
	c.reset();
	w->show_cursor(false);
	obj=OBJFile::load(L"rsrc\\model.obj",&w->renderer);
	w->renderer.set_shader_data({
		{
			SHADER_DATA_TYPE_CONSTANT_BUFFER,
			cb,
			0,
			SHADER_DATA_FLAG_VS|SHADER_DATA_FLAG_PS
		}
	});
	w->renderer.projection_matrix=Matrix::perspective_fov_matrix(XM_PIDIV4,w->w/(float)w->h,0.01f,1000.0f);
}



void u_cb(Window* w,double dt){
	static double t=0;
	t+=dt;
	c.update(dt);
	w->renderer.clear();
	VertexShaderInput cb1={};
	cb1.vm=c.matrix;
	cb1.pm=w->renderer.projection_matrix;
	cb1.lc=1;
	cb1.ll[0]={
		Vector(-1,1,-1,0)*Matrix::y_rotation_matrix(-1.5*t),
		Vector(0.5,0.5,0.5,1)
	};
	w->renderer.update_constant_buffer(cb,&cb1);
	w->renderer.use_vertex_shader(vs);
	w->renderer.use_pixel_shader(ps);
	obj.t=Matrix::translation_matrix(-0.5,0,-0.5)*Matrix::y_rotation_matrix(t/2);
	obj.nt=Matrix::y_rotation_matrix(t/2);
	obj.data[0].t=Matrix::x_rotation_matrix(-1.5*t);
	obj.data[0].nt=Matrix::x_rotation_matrix(-1.5*t);
	OBJFile::draw(obj,&w->renderer,1);
	w->renderer.show();
	if (w->pressed(0x1b)==true){
		w->close();
	}
}



int main(int argc,char** argv){
	Window w(600,600,800,600,L"Window Name",&i_cb,&u_cb);
	std::cout<<"Window Created!"<<std::endl;
	w.merge_thread();
	std::cout<<"End!";
	return 0;
}

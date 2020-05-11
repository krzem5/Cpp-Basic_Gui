#ifndef UNICODE
#define UNICODE
#endif
#include "window.cpp"
#include <iostream>
#include <thread>
#include <chrono>
#include <cmath>



struct ConstantBuffer{
	Matrix mWorld;
	Matrix mView;
	Matrix mProjection;
	Vector4 vLightDir[2];
	Vector4 vLightColor[2];
	Vector4 vOutputColor;
};



ulong vs;
ulong ps;
ulong pss;
ObjectBuffer ob;
ulong cb;



void i_cb(Window* w){
	D3D11_INPUT_ELEMENT_DESC vs_inp[]={
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
	};
	ulong vs=w->renderer.load_vertex_shader(L"rsrc/shader.fx","VS","vs_4_0",vs_inp,2);
	ulong ps=w->renderer.load_pixel_shader(L"rsrc/shader.fx","PS","ps_4_0");
	ulong pss=w->renderer.load_pixel_shader(L"rsrc/shader.fx","PSSolid","ps_4_0");
	ob.use_renderer(&w->renderer);
	cb=w->renderer.create_constant_buffer(sizeof(ConstantBuffer));
	ob.update();
}



void u_cb(Window* w,double dt){
	static double t=0;
	t+=dt;
	Matrix g_World = DirectX::XMMatrixRotationY( t );

	// Setup our lighting parameters
	Vector4 vLightDirs[2] =
	{
		Vector4( -0.577f, 0.577f, -0.577f, 1.0f ),
		Vector4( 0.0f, 0.0f, -1.0f, 1.0f ),
	};
	Vector4 vLightColors[2] =
	{
		Vector4( 0.5f, 0.5f, 0.5f, 1.0f ),
		Vector4( 0.5f, 0.0f, 0.0f, 1.0f )
	};
	Matrix mRotate = DirectX::XMMatrixRotationY( -2.0f * t );
	DirectX::XMVECTOR vLightDir = DirectX::XMLoadFloat4( &vLightDirs[1] );
	vLightDir = DirectX::XMVector3Transform( vLightDir, mRotate );
	DirectX::XMStoreFloat4( &vLightDirs[1], vLightDir );
	w->renderer.clear();
	w->renderer.clear_color[2]=0.8f;
	ConstantBuffer cb1={};
	cb1.mWorld = DirectX::XMMatrixTranspose( g_World );
	cb1.mView = DirectX::XMMatrixTranspose( w->renderer.view_matrix );
	cb1.mProjection = DirectX::XMMatrixTranspose( w->renderer.projection_matrix);
	cb1.vLightDir[0] = vLightDirs[0];
	cb1.vLightDir[1] = vLightDirs[1];
	cb1.vLightColor[0] = vLightColors[0];
	cb1.vLightColor[1] = vLightColors[1];
	cb1.vOutputColor = Vector4(0, 0, 0, 0);
	w->renderer.update_constant_buffer(cb,&cb1);
	w->renderer.use_vertex_shader(vs,cb);
	w->renderer.use_pixel_shader(ps,cb);
	ob.render();
	using namespace DirectX;
	for( int m = 0; m < 2; m++ )
	{
		Matrix mLight = DirectX::XMMatrixTranslationFromVector( 5.0f * DirectX::XMLoadFloat4( &vLightDirs[m] ) );
		Matrix mLightScale = DirectX::XMMatrixScaling( 0.2f, 0.2f, 0.2f );
		mLight = mLightScale * mLight;

		// Update the world variable to reflect the current light
		cb1.mWorld = DirectX::XMMatrixTranspose( mLight );
		cb1.vOutputColor = vLightColors[m];
		w->renderer.update_constant_buffer(cb,&cb1);
		w->renderer.use_pixel_shader(ps,cb);

		ob.render();
	}
	w->renderer.show();
}



int main(int argc,char** argv){
	Window w(600,600,800,600,L"Window Name",&i_cb,&u_cb);
	std::cout<<"Window Created!"<<std::endl;
	std::this_thread::sleep_for(std::chrono::milliseconds(5000));
	return 0;
}
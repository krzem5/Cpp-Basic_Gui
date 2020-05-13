// D:\K\Downloads\directx-sdk-samples\Direct3D11Tutorials\Tutorial06\Tutorial06.cppAAAAAAAAAAAAAAAAAAAAAAAAneffe0610@#AAAAAAAAAAAAAAAAAAAAAAAAAAAAA
#ifndef UNICODE
#define UNICODE
#endif
#include <windows.h>
#include <windowsx.h>
#include "window.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directXMath.h>
#include <map>
#pragma comment(lib,"user32")
#pragma comment(lib,"d3d11")
#pragma comment(lib,"d3dcompiler")



ObjectBuffer::ObjectBuffer(uint sz){
	this->sz=sz;
	this->data=new ObjectBufferData();
}



void ObjectBuffer::ObjectBufferData::add_vertexes(std::initializer_list<float> vl){
	for (std::initializer_list<float>::iterator i=vl.begin();i!=vl.end();i++){
		this->vertexes.push_back(*i);
	}
}



void ObjectBuffer::ObjectBufferData::add_indicies(std::initializer_list<ushort> il){
	for (std::initializer_list<ushort>::iterator i=il.begin();i!=il.end();i++){
		this->indicies.push_back(*i);
	}
}



Renderer::Renderer(){
	//
}



Renderer::~Renderer(){
	this->_vsl.clear();
	this->_psl.clear();
	this->_cbl.clear();
	this->_obl.clear();
	this->_d3_dc->ClearState();
	this->_d3_d->Release();
	this->_d3_d1->Release();
	this->_d3_dc->Release();
	this->_d3_dc1->Release();
	this->_d3_sc->Release();
	this->_d3_sc1->Release();
	this->_d3_rt->Release();
	this->_d3_ds->Release();
	this->_d3_sv->Release();
}



ulong Renderer::load_vertex_shader(const wchar_t* fp,const char* e,const char* v,D3D11_INPUT_ELEMENT_DESC* il,uint ill){
	this->_vsl[this->_n_vsl_id++]=nullptr;
	ID3DBlob* b=this->_compile_shader(fp,e,v);
	HRESULT hr=this->_d3_d->CreateVertexShader(b->GetBufferPointer(),b->GetBufferSize(),nullptr,&this->_vsl[this->_n_vsl_id-1]);
	if (FAILED(hr)){
		std::cout<<"ERR";
	}
	ID3D11InputLayout* vl=nullptr;
	hr=this->_d3_d->CreateInputLayout(il,ill,b->GetBufferPointer(),b->GetBufferSize(),&vl);
	if (FAILED(hr)){
		std::cout<<"ERR";
	}
	this->_d3_dc->IASetInputLayout(vl);
	b->Release();
	return this->_n_vsl_id-1;
}



ulong Renderer::load_pixel_shader(const wchar_t* fp,const char* e,const char* v){
	this->_psl[this->_n_psl_id++]=nullptr;
	ID3DBlob* b=this->_compile_shader(fp,e,v);
	this->_d3_d->CreatePixelShader(b->GetBufferPointer(),b->GetBufferSize(),nullptr,&this->_psl[this->_n_psl_id-1]);
	b->Release();
	return this->_n_psl_id-1;
}



ulong Renderer::create_constant_buffer(uint cbl){
	this->_cbl[this->_n_cbl_id++]=nullptr;
	D3D11_BUFFER_DESC bd={
		cbl,
		D3D11_USAGE_DEFAULT,
		D3D11_BIND_CONSTANT_BUFFER,
		0,
		0,
		0
	};
	HRESULT hr=this->_d3_d->CreateBuffer(&bd,nullptr,&this->_cbl[this->_n_cbl_id-1]);
	if (FAILED(hr)){
		std::cout<<"ERR";
	}
	return this->_n_cbl_id-1;
}



ulong Renderer::create_object_buffer(uint sz){
	this->_obl[this->_n_obl_id++]=new ObjectBuffer(sz*sizeof(float));
	return this->_n_obl_id-1;
}



void Renderer::update_constant_buffer(ulong cb_id,const void* dt){
	this->_d3_dc->UpdateSubresource(this->_cbl[cb_id],0,nullptr,dt,0,0);
}



ObjectBuffer::ObjectBufferData* Renderer::get_object_buffer(ulong ob_id){
	return this->_obl[ob_id]->data;
}



void Renderer::clear(){
	this->_d3_dc->ClearRenderTargetView(this->_d3_rt,this->clear_color);
	this->_d3_dc->ClearDepthStencilView(this->_d3_sv,D3D11_CLEAR_DEPTH,1.0f,0);
}



void Renderer::use_vertex_shader(ulong vs_id,ulong cb_id){
	this->_d3_dc->VSSetShader(this->_vsl[vs_id],nullptr,0);
	this->_d3_dc->VSSetConstantBuffers(0,1,&this->_cbl[cb_id]);
}



void Renderer::use_pixel_shader(ulong ps_id,ulong cb_id){
	this->_d3_dc->PSSetShader(this->_psl[ps_id],nullptr,0);
	this->_d3_dc->PSSetConstantBuffers(0,1,&this->_cbl[cb_id]);
}



void Renderer::update_object_buffer(ulong ob_id){
	this->_obl[ob_id]->ln=uint(this->_obl[ob_id]->data->vertexes.size());
	D3D11_BUFFER_DESC bd={
		uint(this->_obl[ob_id]->data->vertexes.size())*sizeof(float),
		D3D11_USAGE_DEFAULT,
		D3D11_BIND_VERTEX_BUFFER,
		0,
		0,
		0
	};
	D3D11_SUBRESOURCE_DATA dt={
		this->_obl[ob_id]->data->vertexes.data(),
		0,
		0
	};
	HRESULT hr=this->_d3_d->CreateBuffer(&bd,&dt,&this->_obl[ob_id]->vb);
	if (FAILED(hr)){
		std::cout<<"ERR"<<std::endl;
	}
	bd.Usage=D3D11_USAGE_DEFAULT;
	bd.ByteWidth=uint(this->_obl[ob_id]->data->indicies.size())*sizeof(ushort);
	bd.BindFlags=D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags=0;
	dt.pSysMem=this->_obl[ob_id]->data->indicies.data();
	hr=this->_d3_d->CreateBuffer(&bd,&dt,&this->_obl[ob_id]->ib);
	if (FAILED(hr)){
		std::cout<<"ERR"<<std::endl;
	}
}



void Renderer::render_object_buffer(ulong ob_id){
	uint off=0;
	this->_d3_dc->IASetVertexBuffers(0,1,&this->_obl[ob_id]->vb,&this->_obl[ob_id]->sz,&off);
	this->_d3_dc->IASetIndexBuffer(this->_obl[ob_id]->ib,DXGI_FORMAT_R16_UINT,0);
	this->_d3_dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	this->_d3_dc->DrawIndexed(this->_obl[ob_id]->ln,0,0);
}



void Renderer::show(){
	this->_d3_sc->Present(0,DXGI_PRESENT_DO_NOT_WAIT);
}



void Renderer::_i(HWND _hwnd){
	this->_hwnd=_hwnd;
	HRESULT hr;
	RECT rc;
	GetClientRect(this->_hwnd,&rc);
	uint width=rc.right-rc.left;
	uint height=rc.bottom-rc.top;
	D3D_FEATURE_LEVEL fl;
	D3D11CreateDevice(nullptr,D3D_DRIVER_TYPE_HARDWARE,0,D3D11_CREATE_DEVICE_DEBUG,nullptr,0,D3D11_SDK_VERSION,&this->_d3_d,&fl,&this->_d3_dc);
	IDXGIFactory1* dxgi_f=nullptr;
	IDXGIDevice* dxgi_d=nullptr;
	hr=this->_d3_d->QueryInterface(__uuidof(IDXGIDevice),reinterpret_cast<void**>(&dxgi_d));
	if (SUCCEEDED(hr)){
		IDXGIAdapter* dxgi_a=nullptr;
		hr=dxgi_d->GetAdapter(&dxgi_a);
		if (SUCCEEDED(hr)){
			hr=dxgi_a->GetParent(__uuidof(IDXGIFactory1),reinterpret_cast<void**>(&dxgi_f));
			dxgi_a->Release();
		}
		dxgi_d->Release();
	}
	IDXGIFactory2* dxgi_f2=nullptr;
	hr=dxgi_f->QueryInterface(__uuidof(IDXGIFactory2),reinterpret_cast<void**>(&dxgi_f2));
	if (dxgi_f2!=nullptr){
		hr=this->_d3_d->QueryInterface(__uuidof(ID3D11Device1),reinterpret_cast<void**>(&this->_d3_d1));
		if (SUCCEEDED(hr)){(void)this->_d3_dc->QueryInterface(__uuidof(ID3D11DeviceContext1),reinterpret_cast<void**>(&this->_d3_dc1));
		}
		DXGI_SWAP_CHAIN_DESC1 sc_d={
			width,
			height,
			DXGI_FORMAT_R8G8B8A8_UNORM,
			FALSE,
			{
				1,
				0
			},
			DXGI_USAGE_RENDER_TARGET_OUTPUT,
			1
		};
		hr=dxgi_f2->CreateSwapChainForHwnd(this->_d3_d,this->_hwnd,&sc_d,nullptr,nullptr,&this->_d3_sc1);
		if (SUCCEEDED(hr)){
			hr=this->_d3_sc1->QueryInterface(__uuidof(IDXGISwapChain),reinterpret_cast<void**>(&this->_d3_sc));
		}
		dxgi_f2->Release();
	}
	else{
		DXGI_SWAP_CHAIN_DESC sc_d={
			{
				width,
				height,
				{
					60,
					1
				},
				DXGI_FORMAT_UNKNOWN,
				DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED,
				DXGI_MODE_SCALING_UNSPECIFIED
			},
			{
				1,
				0
			},
			DXGI_USAGE_RENDER_TARGET_OUTPUT,
			1,
			this->_hwnd,
			TRUE,
			DXGI_SWAP_EFFECT_DISCARD,
			0
		};
		dxgi_f->CreateSwapChain(this->_d3_d,&sc_d,&this->_d3_sc);
	}
	dxgi_f->MakeWindowAssociation(this->_hwnd,DXGI_MWA_NO_ALT_ENTER);
	dxgi_f->Release();
	ID3D11Texture2D* bb=nullptr;
	hr=this->_d3_sc->GetBuffer(0,__uuidof(ID3D11Texture2D),(void**)&bb);
	if (FAILED(hr)){
		std::cout<<"ERR"<<std::endl;
		return;
	}
	hr=this->_d3_d->CreateRenderTargetView(bb,nullptr,&this->_d3_rt);
	if (FAILED(hr)){
		std::cout<<"ERR"<<std::endl;
		return;
	}
	bb->Release();
	D3D11_TEXTURE2D_DESC dd={
		width,
		height,
		1,
		1,
		DXGI_FORMAT_D24_UNORM_S8_UINT,
		{
			1,
			0
		},
		D3D11_USAGE_DEFAULT,
		D3D11_BIND_DEPTH_STENCIL,
		0,
		0
	};
	hr=this->_d3_d->CreateTexture2D(&dd,nullptr,&this->_d3_ds);
	if (FAILED(hr)){
		std::cout<<"ERR"<<std::endl;
		return;
	}
	D3D11_DEPTH_STENCIL_VIEW_DESC d_dsv={
		dd.Format,
		D3D11_DSV_DIMENSION_TEXTURE2D,
		0
	};
	d_dsv.Texture2D.MipSlice=0;
	hr=this->_d3_d->CreateDepthStencilView(this->_d3_ds,&d_dsv,&this->_d3_sv);
	if (FAILED(hr)){
		std::cout<<"ERR"<<std::endl;
		return;
	}
	this->_d3_dc->OMSetRenderTargets(1,&this->_d3_rt,this->_d3_sv);
	D3D11_VIEWPORT vp={
		0,
		0,
		(float)width,
		(float)height,
		0,
		1
	};
	this->_d3_dc->RSSetViewports(1,&vp);
	this->world_matrix=DirectX::XMMatrixIdentity();
	this->view_matrix=DirectX::XMMatrixLookAtLH(DirectX::XMVectorSet(0.0f,4.0f,-10.0f,0.0f),DirectX::XMVectorSet(0.0f,1.0f,0.0f,0.0f),DirectX::XMVectorSet(0.0f,1.0f,0.0f,0.0f));
	this->projection_matrix=DirectX::XMMatrixPerspectiveFovLH(DirectX::XM_PIDIV4,width/(float)height,0.01f,1000.0f);
}



ID3DBlob* Renderer::_compile_shader(const wchar_t* fp,const char* e,const char* v){
	ID3DBlob* o=nullptr;
	ID3DBlob* err=nullptr;
	HRESULT hr=D3DCompileFromFile(fp,nullptr,nullptr,e,v,D3DCOMPILE_ENABLE_STRICTNESS,0,&o,&err);
	if (FAILED(hr)){
		if (err!=nullptr){
			std::cout<<reinterpret_cast<const char*>(err->GetBufferPointer());
			err->Release();
		}
		return nullptr;
	}
	return o;
}



ulong RendererHelper::create_object_buffer_box(Renderer* r,Vector3 p,float sc){
	ulong ob=r->create_object_buffer(6);
	ObjectBuffer::ObjectBufferData* ob1=r->get_object_buffer(ob);
	ob1->add_vertexes({-sc+p.x,sc+p.y,-sc+p.z,0,1,0,sc+p.x,sc+p.y,-sc+p.z,0,1,0,sc+p.x,sc+p.y,sc+p.z,0,1,0,-sc+p.x,sc+p.y,sc+p.z,0,1,0,-sc+p.x,-sc+p.y,-sc+p.z,0,-1,0,sc+p.x,-sc+p.y,-sc+p.z,0,-1,0,sc+p.x,-sc+p.y,sc+p.z,0,-1,0,-sc+p.x,-sc+p.y,sc+p.z,0,-1,0,-sc+p.x,-sc+p.y,sc+p.z,-1,0,0,-sc+p.x,-sc+p.y,-sc+p.z,-1,0,0,-sc+p.x,sc+p.y,-sc+p.z,-1,0,0,-sc+p.x,sc+p.y,sc+p.z,-1,0,0,sc+p.x,-sc+p.y,sc+p.z,1,0,0,sc+p.x,-sc+p.y,-sc+p.z,1,0,0,sc+p.x,sc+p.y,-sc+p.z,1,0,0,sc+p.x,sc+p.y,sc+p.z,1,0,0,-sc+p.x,-sc+p.y,-sc+p.z,0,0,-1,sc+p.x,-sc+p.y,-sc+p.z,0,0,-1,sc+p.x,sc+p.y,-sc+p.z,0,0,-1,-sc+p.x,sc+p.y,-sc+p.z,0,0,-1,-sc+p.x,-sc+p.y,sc+p.z,0,0,1,sc+p.x,-sc+p.y,sc+p.z,0,0,1,sc+p.x,sc+p.y,sc+p.z,0,0,1,-sc+p.x,sc+p.y,sc+p.z,0,0,1});
	ob1->add_indicies({3,1,0,2,1,3,6,4,5,7,4,6,11,9,8,10,9,11,14,12,13,15,12,14,19,17,16,18,17,19,22,20,21,23,20,22});
	r->update_object_buffer(ob);
	return ob;
}



Window::Window(int x,int y,int w,int h,const wchar_t* nm,void (*i_cb)(Window*),void (*u_cb)(Window*,double)){
	this->_create_wr(x,y,w,h,nm,i_cb,u_cb);
}



Window::~Window(){
	ReleaseCapture();
	this->_thr.detach();
	DestroyWindow(this->_hwnd);
}



bool Window::pressed(int c){
	return GetKeyState(c)&0x8000;
}



int* Window::mouse(){
	return this->_m_p;
}



void Window::_create_wr(int x,int y,int w,int h,const wchar_t* nm,void (*i_cb)(Window*),void (*u_cb)(Window*,double)){
	this->_thr=std::thread(&Window::_create,this,x,y,w,h,nm,i_cb,u_cb);
	while(this->_cr==false){
		continue;
	}
}



void Window::_create(int x,int y,int w,int h,const wchar_t* nm,void (*i_cb)(Window*),void (*u_cb)(Window*,double)){
	this->name=const_cast<wchar_t*>(nm);
	HINSTANCE hInstance=GetModuleHandle(0);
	WNDCLASSEXW wc={
		sizeof(WNDCLASSEX),
		CS_DBLCLKS|CS_OWNDC|CS_HREDRAW|CS_VREDRAW,
		&_msg_cb,
		0,
		0,
		hInstance,
		0,// ICON
		0,// CURSOR,
		0,// BACKGROUND
		0,// RES-NAME
		nm,
		0 // SMALL ICON
	};
	RegisterClassExW(&wc);
	this->_hwnd=CreateWindowExW(wc.style,wc.lpszClassName,nm,WS_OVERLAPPEDWINDOW,x,y,w,h,nullptr,nullptr,hInstance,nullptr);
	if (this->_hwnd==nullptr){
		std::cout<<"ERROR"<<std::endl;
		return;
	}
	ShowWindow(this->_hwnd,1);
	MSG msg={0};
	while (msg.message!=WM_QUIT){
		if (PeekMessage(&msg,this->_hwnd,0,0,PM_REMOVE)==1){
			TranslateMessage(&msg);
			switch (msg.message){
				case WM_CREATE_RENDERER:
					SetCapture(this->_hwnd);
					this->renderer._i(this->_hwnd);
					(*i_cb)(this);
					this->_l_tm=std::chrono::high_resolution_clock::now();
					this->_cr=true;
					break;
				case WM_MOUSEMOVE:
					{
						MSG p;
						GetMessage(&p,this->_hwnd,0,0);
						this->_m_p[0]=GET_X_LPARAM(p.lParam);
						this->_m_p[1]=GET_Y_LPARAM(p.lParam);
					}
					break;
				default:
					DispatchMessage(&msg);
					break;
			}
		}
		if (this->_cr==true){
			// static double lt=0;
			// static ulonglong st=0;
			// ulonglong ct=GetTickCount64();
			// if (st==0){
			// 	st=ct;
			// }
			// double t=(ct-st)/1000.0f;
			// (*u_cb)(this,t-lt);
			// lt=t;
			std::chrono::high_resolution_clock::time_point tm=std::chrono::high_resolution_clock::now();
			(*u_cb)(this,(double)(std::chrono::duration_cast<std::chrono::nanoseconds>(tm-this->_l_tm).count())*1e-9);
			this->_l_tm=tm;
			// PAINTSTRUCT ps;
			// BeginPaint(this->_hwnd,&ps);
			// EndPaint(this->_hwnd,&ps);
		}
	}
}



long_ptr _msg_cb(HWND hwnd,uint msg,uint_ptr wp,long_ptr lp){
	switch (msg){
		case WM_CREATE:
			PostMessage(hwnd,WM_CREATE_RENDERER,0,0);
			return 0;
		case WM_PAINT:
			PAINTSTRUCT ps;
			BeginPaint(hwnd,&ps);
			EndPaint(hwnd,&ps);
			return 0;
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
	}
	return DefWindowProc(hwnd,msg,wp,lp);
}

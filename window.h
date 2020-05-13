#ifndef UNICODE
	#define UNICODE
#endif
#include <windows.h>
#include <thread>
#include <chrono>
#include <d3d11_1.h>
#include <directxmath.h>
#include <map>
#include <vector>



#define WM_CREATE_RENDERER (WM_USER+0x0001)



typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;
typedef unsigned long long ulonglong;
#if defined(_WIN64)
	typedef unsigned long long uint_ptr;
	typedef long long long_ptr;
#else
	typedef unsigned int uint_ptr;
	typedef long long_ptr;
#endif
typedef DirectX::XMFLOAT3 Vector3;
typedef DirectX::XMFLOAT4 Vector4;
typedef DirectX::XMMATRIX Matrix;



struct ObjectBuffer{
	uint sz;
	uint ln;
	ID3D11Buffer* vb;
	ID3D11Buffer* ib;
	struct ObjectBufferData{
		std::vector<float> vertexes;
		std::vector<ushort> indicies;
		void add_vertexes(std::initializer_list<float> vl);
		void add_indicies(std::initializer_list<ushort> il);
	} *data;
	ObjectBuffer(uint sz);
};



class Renderer{
	public:
		float clear_color[4]={0,0,0,255};
		Matrix world_matrix;
		Matrix view_matrix;
		Matrix projection_matrix;
		Renderer();
		~Renderer();
		ulong load_vertex_shader(const wchar_t* fp,const char* e,const char* v,D3D11_INPUT_ELEMENT_DESC* il,uint ill);
		ulong load_pixel_shader(const wchar_t* fp,const char* e,const char* v);
		ulong create_constant_buffer(uint cbl);
		ulong create_object_buffer(uint sz);
		void update_constant_buffer(ulong cb_id,const void* dt);
		ObjectBuffer::ObjectBufferData* get_object_buffer(ulong ob_id);
		void clear();
		void use_vertex_shader(ulong vs_id,ulong cb_id);
		void use_pixel_shader(ulong ps_id,ulong cb_id);
		void update_object_buffer(ulong ob_id);
		void render_object_buffer(ulong ob_id);
		void show();
		void _i(HWND _hwnd);
	private:
		HWND _hwnd=nullptr;
		std::map<ulong,ID3D11VertexShader*> _vsl;
		std::map<ulong,ID3D11PixelShader*> _psl;
		std::map<ulong,ID3D11Buffer*> _cbl;
		std::map<ulong,ObjectBuffer*> _obl;
		ulong _n_vsl_id=0L;
		ulong _n_psl_id=0L;
		ulong _n_cbl_id=0L;
		ulong _n_obl_id=0L;
		ID3D11Device* _d3_d=nullptr;
		ID3D11Device1* _d3_d1=nullptr;
		ID3D11DeviceContext* _d3_dc=nullptr;
		ID3D11DeviceContext1* _d3_dc1=nullptr;
		IDXGISwapChain* _d3_sc=nullptr;
		IDXGISwapChain1* _d3_sc1=nullptr;
		ID3D11RenderTargetView* _d3_rt=nullptr;
		ID3D11Texture2D* _d3_ds=nullptr;
		ID3D11DepthStencilView* _d3_sv=nullptr;
		ID3DBlob* _compile_shader(const wchar_t* fp,const char* e,const char* v);
};



class RendererHelper{
	public:
		static ulong create_object_buffer_box(Renderer* r,Vector3 p,float sc);
};



class Window{
	public:
		wchar_t* name=nullptr;
		Renderer renderer;
		Window(int x,int y,int w,int h,const wchar_t* nm,void (*i_cb)(Window*),void (*u_cb)(Window*,double));
		~Window();
		void fullscreen(bool m);
		bool pressed(int k);
		int* mouse();
	private:
		HWND _hwnd=nullptr;
		bool _cr=false;
		int _m_p[2];
		std::chrono::high_resolution_clock::time_point _l_tm;
		std::thread _thr;
		void _create_wr(int x,int y,int w,int h,const wchar_t* nm,void (*i_cb)(Window*),void (*u_cb)(Window*,double));
		void _create(int x,int y,int w,int h,const wchar_t* nm,void (*i_cb)(Window*),void (*u_cb)(Window*,double));
};



long_ptr _msg_cb(HWND hwnd,uint msg,uint_ptr wp,long_ptr lp);
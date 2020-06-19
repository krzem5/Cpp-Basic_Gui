#define UNICODE
#define NOMINMAX
#include <windows.h>
#include <wincodec.h>
#include <thread>
#include <chrono>
#include <d3d11_1.h>
#include <directxmath.h>
#include <map>
#include <vector>
#include <unordered_map>



#define WM_CREATE_RENDERER (WM_APP+0x0001)
#define WM_SHOW_CURSOR (WM_APP+0x0002)
#if (_WIN32_WINNT>=0x0602)&&!defined(DXGI_1_2_FORMATS)
	#define DXGI_1_2_FORMATS
#endif
#ifndef UNICODE
	#define UNICODE
#endif



namespace krzem{
	using namespace DirectX;



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
	typedef D3D11_INPUT_ELEMENT_DESC VS_INPUT_LAYOUT;
	typedef D3D11_SAMPLER_DESC SHADER_SAMPLER_DESCRIPTION;



	class Renderer;
	class Window;
	struct Matrix;



	enum SHADER_DATA_TYPE{
		SHADER_DATA_TYPE_CONSTANT_BUFFER,
		SHADER_DATA_TYPE_TEXTURE,
		SHADER_DATA_TYPE_SAMPLER_STATE
	};



	enum WINDOW_SIZE_TYPE{
		WINDOW_SIZE_TYPE_MAXIMISED,
		WINDOW_SIZE_TYPE_MINIMIZED,
		WINDOW_SIZE_TYPE_RESTORE
	};



	enum SHADER_DATA_FLAGS:uint{
		SHADER_DATA_FLAG_VS=1,
		SHADER_DATA_FLAG_PS=2
	};



	struct SHADER_DATA{
		SHADER_DATA_TYPE type;
		ulong id;
		uint r;
		uint fl;
	};



	struct WIC_CONVERT{
		GUID s;
		GUID t;
	};



	struct WIC_TO_DXGI{
		GUID wic_f;
		DXGI_FORMAT dxgi_f;
	};



	struct Vector{
		float x;
		float y;
		float z;
		float w;
		Vector();
		Vector(float x);
		Vector(float x,float y);
		Vector(float x,float y,float z);
		Vector(float x,float y,float z,float w);
		Vector clone();
		Vector operator=(Vector o);
		Vector operator+=(Vector o);
		Vector operator-=(Vector o);
		Vector operator*=(Vector o);
		Vector operator*=(Matrix o);
		Vector operator/=(Vector o);
		float dot(Vector o);
		Vector cross(Vector o);
		Vector norm();
	};



	struct Matrix{
		float _00;
		float _01;
		float _02;
		float _03;
		float _10;
		float _11;
		float _12;
		float _13;
		float _20;
		float _21;
		float _22;
		float _23;
		float _30;
		float _31;
		float _32;
		float _33;
		Matrix();
		Matrix(Vector v);
		Matrix(float _00,float _01,float _02,float _03,float _10,float _11,float _12,float _13,float _20,float _21,float _22,float _23,float _30,float _31,float _32,float _33);
		Matrix clone();
		Matrix operator=(Matrix o);
		Matrix operator+=(Matrix o);
		Matrix operator-=(Matrix o);
		Matrix operator*=(Matrix o);
		Matrix operator*=(Vector o);
		Matrix operator/=(Matrix o);
		static Matrix translation_matrix(float x,float y,float z);
		static Matrix x_rotation_matrix(float a);
		static Matrix y_rotation_matrix(float a);
		static Matrix z_rotation_matrix(float a);
		static Matrix perspective_fov_matrix(float fov,float a,float n,float f);
		static Matrix look_to_matrix(Vector e,Vector d,Vector u);
	};



	Vector operator+(Vector a,Vector b);
	Vector operator-(Vector a,Vector b);
	Vector operator*(Vector a,Vector b);
	Vector operator*(Vector a,Matrix b);
	Vector operator/(Vector a,Vector b);
	Matrix operator+(Matrix a,Matrix b);
	Matrix operator-(Matrix a,Matrix b);
	Matrix operator*(Matrix a,Matrix b);
	Matrix operator*(Matrix a,Vector b);
	Matrix operator/(Matrix a,Matrix b);



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



	struct OBJFile{
		wchar_t* nm;
		Matrix t;
		Matrix nt;
		uint data_sz;
		struct OBJFileGroup{
			char* nm;
			bool v;
			Matrix t;
			Matrix nt;
			uint dt_sz;
			struct OBJFileGroupData{
				struct OBJFileGroupDataMaterial{
					Vector Ka;
					Vector Kd;
					Vector Ks;
					float Ns;
				} mtl;
				ulong ob_id;
			} *dt;
		} *data;
		static OBJFile load(wchar_t* nm,Renderer* r);
		static void draw(OBJFile o,Renderer* r,uint rs);
	};



	class Camera{
		public:
			float MOVE_SPEED=1;
			float ROT_SPEED=1;
			float x,y,z,rx,ry,rz;
			bool lock=false;
			bool enabled=false;
			Matrix matrix;
			Camera();
			~Camera();
			void set_window(Window* w);
			void update(double dt);
			void reset();
		private:
			float _dx,_dy,_dz,_drx,_dry,_drz;
			Window* _w;
			float _ease(float v,float t);
	};



	class Renderer{
		public:
			float clear_color[4]={0,0,0,255};
			bool enable_vsync=false;
			Matrix world_matrix;
			Matrix view_matrix;
			Matrix projection_matrix;
			Renderer();
			~Renderer();
			ulong load_vertex_shader(const wchar_t* fp,const char* e,const char* v,D3D11_INPUT_ELEMENT_DESC* il,uint ill);
			ulong load_pixel_shader(const wchar_t* fp,const char* e,const char* v);
			ulong create_constant_buffer(uint cbl);
			ulong create_sampler_state(const D3D11_SAMPLER_DESC sd);
			ulong create_object_buffer(uint sz);
			ulong read_texture_file(const wchar_t* fp,bool sr);
			void update_constant_buffer(ulong cb_id,const void* dt);
			ObjectBuffer::ObjectBufferData* get_object_buffer(ulong ob_id);
			void clear();
			void set_shader_data(std::initializer_list<SHADER_DATA> dt);
			void use_vertex_shader(ulong vs_id);
			void use_pixel_shader(ulong ps_id);
			void update_object_buffer(ulong ob_id);
			void render_object_buffer(ulong ob_id);
			void show();
			void _i(HWND _hwnd);
			void _e(bool e);
			void _e_wic();
			void _r(bool s);
		private:
			HWND _hwnd=nullptr;
			bool _ea=false;
			uint _p=0;
			std::map<ulong,ID3D11VertexShader*> _vsl;
			std::map<ulong,ID3D11PixelShader*> _psl;
			std::map<ulong,ID3D11Buffer*> _cbl;
			std::map<ulong,ID3D11SamplerState*> _ssl;
			std::map<ulong,ObjectBuffer*> _obl;
			std::map<ulong,ID3D11Texture2D*> _txl;
			std::map<ulong,ID3D11ShaderResourceView*> _txsrl;
			ulong _n_vsl_id=0L;
			ulong _n_psl_id=0L;
			ulong _n_cbl_id=0L;
			ulong _n_ssl_id=0L;
			ulong _n_obl_id=0L;
			ulong _n_txl_id=0L;
			ID3D11Device* _d3_d=nullptr;
			ID3D11Device1* _d3_d1=nullptr;
			ID3D11DeviceContext* _d3_dc=nullptr;
			ID3D11DeviceContext1* _d3_dc1=nullptr;
			IDXGISwapChain* _d3_sc=nullptr;
			IDXGISwapChain1* _d3_sc1=nullptr;
			ID3D11RenderTargetView* _d3_rt=nullptr;
			ID3D11Texture2D* _d3_ds=nullptr;
			ID3D11DepthStencilView* _d3_sv=nullptr;
			IWICImagingFactory* _wic_f=nullptr;
			ID3DBlob* _compile_shader(const wchar_t* fp,const char* e,const char* v);
			void _err(const char* s,HRESULT hr);
			void _err(const char* s,const char* r);
	};



	class Window{
		public:
			static std::map<HWND,Window*> list;
			int x,y,w,h;
			int mx,my;
			wchar_t* name=nullptr;
			Renderer renderer;
			Window(int x,int y,int w,int h,const wchar_t* nm,void (*i_cb)(Window*),void (*u_cb)(Window*,double));
			~Window();
			HWND get_handle();
			void show_cursor(bool st);
			void merge_thread();
			void fullscreen(bool m);
			bool pressed(int k);
			void resize(uint w,uint h);
			void resize(WINDOW_SIZE_TYPE s);
			void close();
			void _r();
		private:
			HWND _hwnd=nullptr;
			uint _ss=0;
			bool _cr=false;
			bool _end=false;
			bool _m_r_thr=false;
			uint _rs=0;
			uint _rs_w=0;
			uint _rs_h=0;
			RECT _ss_r;
			long _ss_s;
			long _ss_exs;
			std::chrono::high_resolution_clock::time_point _l_tm;
			std::thread _w_thr;
			std::thread _r_thr;
			void _create_wr(int x,int y,int w,int h,const wchar_t* nm,void (*i_cb)(Window*),void (*u_cb)(Window*,double));
			void _create(int x,int y,int w,int h,const wchar_t* nm,void (*i_cb)(Window*));
			void _render_thr(void (*u_cb)(Window*,double));
			void _save_state();
			void _restore();
	};



	long_ptr _msg_cb(HWND hwnd,uint msg,uint_ptr wp,long_ptr lp);



	const static std::unordered_map<std::string,uint> OBJ_FILE_TYPE_MAP{
		{"",0},
		{"mtllib",1},
		{"usemtl",2},
		{"v",3},
		{"vn",4},
		{"vt",5},
		{"f",6},
		{"g",7},
		{"newmtl",8},
		{"Ka",9},
		{"Kd",10},
		{"Ks",11},
		{"Ns",12}
	};



	const static WIC_CONVERT WIC_CONVERT_D[]={
		{GUID_WICPixelFormatBlackWhite,GUID_WICPixelFormat8bppGray},
		{GUID_WICPixelFormat1bppIndexed,GUID_WICPixelFormat32bppRGBA},
		{GUID_WICPixelFormat2bppIndexed,GUID_WICPixelFormat32bppRGBA},
		{GUID_WICPixelFormat4bppIndexed,GUID_WICPixelFormat32bppRGBA},
		{GUID_WICPixelFormat8bppIndexed,GUID_WICPixelFormat32bppRGBA},
		{GUID_WICPixelFormat2bppGray,GUID_WICPixelFormat8bppGray},
		{GUID_WICPixelFormat4bppGray,GUID_WICPixelFormat8bppGray},
		{GUID_WICPixelFormat16bppGrayFixedPoint,GUID_WICPixelFormat16bppGrayHalf},
		{GUID_WICPixelFormat32bppGrayFixedPoint,GUID_WICPixelFormat32bppGrayFloat},
	#ifdef DXGI_1_2_FORMATS
		{GUID_WICPixelFormat16bppBGR555,GUID_WICPixelFormat16bppBGRA5551},
	#else
		{GUID_WICPixelFormat16bppBGR555,GUID_WICPixelFormat32bppRGBA},
		{GUID_WICPixelFormat16bppBGRA5551,GUID_WICPixelFormat32bppRGBA},
		{GUID_WICPixelFormat16bppBGR565,GUID_WICPixelFormat32bppRGBA},
	#endif
		{GUID_WICPixelFormat32bppBGR101010,GUID_WICPixelFormat32bppRGBA1010102},
		{GUID_WICPixelFormat24bppBGR,GUID_WICPixelFormat32bppRGBA},
		{GUID_WICPixelFormat24bppRGB,GUID_WICPixelFormat32bppRGBA},
		{GUID_WICPixelFormat32bppPBGRA,GUID_WICPixelFormat32bppRGBA},
		{GUID_WICPixelFormat32bppPRGBA,GUID_WICPixelFormat32bppRGBA},
		{GUID_WICPixelFormat48bppRGB,GUID_WICPixelFormat64bppRGBA},
		{GUID_WICPixelFormat48bppBGR,GUID_WICPixelFormat64bppRGBA},
		{GUID_WICPixelFormat64bppBGRA,GUID_WICPixelFormat64bppRGBA},
		{GUID_WICPixelFormat64bppPRGBA,GUID_WICPixelFormat64bppRGBA},
		{GUID_WICPixelFormat64bppPBGRA,GUID_WICPixelFormat64bppRGBA},
		{GUID_WICPixelFormat48bppRGBFixedPoint,GUID_WICPixelFormat64bppRGBAHalf},
		{GUID_WICPixelFormat48bppBGRFixedPoint,GUID_WICPixelFormat64bppRGBAHalf},
		{GUID_WICPixelFormat64bppRGBAFixedPoint,GUID_WICPixelFormat64bppRGBAHalf},
		{GUID_WICPixelFormat64bppBGRAFixedPoint,GUID_WICPixelFormat64bppRGBAHalf},
		{GUID_WICPixelFormat64bppRGBFixedPoint,GUID_WICPixelFormat64bppRGBAHalf},
		{GUID_WICPixelFormat64bppRGBHalf,GUID_WICPixelFormat64bppRGBAHalf},
		{GUID_WICPixelFormat48bppRGBHalf,GUID_WICPixelFormat64bppRGBAHalf},
		{GUID_WICPixelFormat96bppRGBFixedPoint,GUID_WICPixelFormat128bppRGBAFloat},
		{GUID_WICPixelFormat128bppPRGBAFloat,GUID_WICPixelFormat128bppRGBAFloat},
		{GUID_WICPixelFormat128bppRGBFloat,GUID_WICPixelFormat128bppRGBAFloat},
		{GUID_WICPixelFormat128bppRGBAFixedPoint,GUID_WICPixelFormat128bppRGBAFloat},
		{GUID_WICPixelFormat128bppRGBFixedPoint,GUID_WICPixelFormat128bppRGBAFloat},
		{GUID_WICPixelFormat32bppCMYK,GUID_WICPixelFormat32bppRGBA},
		{GUID_WICPixelFormat64bppCMYK,GUID_WICPixelFormat64bppRGBA},
		{GUID_WICPixelFormat40bppCMYKAlpha,GUID_WICPixelFormat64bppRGBA},
		{GUID_WICPixelFormat80bppCMYKAlpha,GUID_WICPixelFormat64bppRGBA},
	#ifdef DXGI_1_2_FORMATS
		{GUID_WICPixelFormat32bppRGB,GUID_WICPixelFormat32bppRGBA},
		{GUID_WICPixelFormat64bppRGB,GUID_WICPixelFormat64bppRGBA},
		{GUID_WICPixelFormat64bppPRGBAHalf,GUID_WICPixelFormat64bppRGBAHalf},
	#endif
	};



	const static WIC_TO_DXGI WIC_TO_DXGI_D[]={
		{GUID_WICPixelFormat128bppRGBAFloat,DXGI_FORMAT_R32G32B32A32_FLOAT},
		{GUID_WICPixelFormat64bppRGBAHalf,DXGI_FORMAT_R16G16B16A16_FLOAT},
		{GUID_WICPixelFormat64bppRGBA,DXGI_FORMAT_R16G16B16A16_UNORM},
		{GUID_WICPixelFormat32bppRGBA,DXGI_FORMAT_R8G8B8A8_UNORM},
		{GUID_WICPixelFormat32bppBGRA,DXGI_FORMAT_B8G8R8A8_UNORM},
		{GUID_WICPixelFormat32bppBGR,DXGI_FORMAT_B8G8R8X8_UNORM},
		{GUID_WICPixelFormat32bppRGBA1010102XR,DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM},
		{GUID_WICPixelFormat32bppRGBA1010102,DXGI_FORMAT_R10G10B10A2_UNORM},
		{GUID_WICPixelFormat32bppRGBE,DXGI_FORMAT_R9G9B9E5_SHAREDEXP},
	#ifdef DXGI_1_2_FORMATS
		{GUID_WICPixelFormat16bppBGRA5551,DXGI_FORMAT_B5G5R5A1_UNORM},
		{GUID_WICPixelFormat16bppBGR565,DXGI_FORMAT_B5G6R5_UNORM},
	#endif
		{GUID_WICPixelFormat32bppGrayFloat,DXGI_FORMAT_R32_FLOAT},
		{GUID_WICPixelFormat16bppGrayHalf,DXGI_FORMAT_R16_FLOAT},
		{GUID_WICPixelFormat16bppGray,DXGI_FORMAT_R16_UNORM},
		{GUID_WICPixelFormat8bppGray,DXGI_FORMAT_R8_UNORM},
		{GUID_WICPixelFormat8bppAlpha,DXGI_FORMAT_A8_UNORM},
	#ifdef DXGI_1_2_FORMATS
		{GUID_WICPixelFormat96bppRGBFloat,DXGI_FORMAT_R32G32B32_FLOAT},
	#endif
	};



}

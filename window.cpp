#include "window.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <memory>
#include <map>
#include <comdef.h>
#include <windows.h>
#include <windowsx.h>
#include <wincodec.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directXMath.h>
#include <dxgiformat.h>
#include <math.h>
#include <wchar.h>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <codecvt>
#pragma comment(lib,"user32")
#pragma comment(lib,"d3d11")
#pragma comment(lib,"d3dcompiler")
#pragma comment(lib,"Ole32")



using namespace krzem;



Vector::Vector(){
	//
}



Vector::Vector(float x){
	this->x=x;
}



Vector::Vector(float x,float y){
	this->x=x;
	this->y=y;
}



Vector::Vector(float x,float y,float z){
	this->x=x;
	this->y=y;
	this->z=z;
}



Vector::Vector(float x,float y,float z,float w){
	this->x=x;
	this->y=y;
	this->z=z;
	this->w=w;
}



Vector Vector::clone(){
	return {this->x,this->y,this->z,this->w};
}



Vector Vector::operator=(Vector o){
	this->x=o.x;
	this->y=o.y;
	this->z=o.z;
	this->w=o.w;
	return *this;
}



float Vector::dot(Vector o){
	return this->x*o.x+this->y*o.y+this->z*o.z+this->w*w;
}



Vector Vector::cross(Vector o){
	return {this->y*o.z-this->z*o.y,this->z*o.x-this->x*o.z,this->x*o.y-this->y*o.x};
}



Vector Vector::norm(){
	float m=sqrt(this->x*this->x+this->y*this->y+this->z*this->z+this->w*this->w);
	return {this->x/m,this->y/m,this->z/m,this->w/m};
}



Matrix::Matrix(){
	this->_00=1;
	this->_01=0;
	this->_02=0;
	this->_03=0;
	this->_10=0;
	this->_11=1;
	this->_12=0;
	this->_13=0;
	this->_20=0;
	this->_21=0;
	this->_22=1;
	this->_23=0;
	this->_30=0;
	this->_31=0;
	this->_32=0;
	this->_33=1;
}



Matrix::Matrix(float _00,float _01,float _02,float _03,float _10,float _11,float _12,float _13,float _20,float _21,float _22,float _23,float _30,float _31,float _32,float _33){
	this->_00=_00;
	this->_01=_01;
	this->_02=_02;
	this->_03=_03;
	this->_10=_10;
	this->_11=_11;
	this->_12=_12;
	this->_13=_13;
	this->_20=_20;
	this->_21=_21;
	this->_22=_22;
	this->_23=_23;
	this->_30=_30;
	this->_31=_31;
	this->_32=_32;
	this->_33=_33;
}



Matrix Matrix::operator=(Matrix o){
	this->_00=o._00;
	this->_01=o._01;
	this->_02=o._02;
	this->_03=o._03;
	this->_10=o._10;
	this->_11=o._11;
	this->_12=o._12;
	this->_13=o._13;
	this->_20=o._20;
	this->_21=o._21;
	this->_22=o._22;
	this->_23=o._23;
	this->_30=o._30;
	this->_31=o._31;
	this->_32=o._32;
	this->_33=o._33;
	return *this;
}



Matrix Matrix::translation_matrix(float x,float y,float z){
	return {1,0,0,0,0,1,0,0,0,0,1,0,x,y,z,1};
}



Matrix Matrix::x_rotation_matrix(float a){
	return {1,0,0,0,0,cos(a),-sin(a),0,0,sin(a),cos(a),0,0,0,0,1};
}



Matrix Matrix::y_rotation_matrix(float a){
	return {cos(a),0,-sin(a),0,0,1,0,0,sin(a),0,cos(a),0,0,0,0,1};
}



Matrix Matrix::perspective_fov_matrix(float fov,float a,float n,float f){
	return {cos(fov/2)/sin(fov/2)/a,0,0,0,0,cos(fov/2)/sin(fov/2),0,0,0,0,f/(f-n),1,0,0,-n*f/(f-n),0};
}



Matrix Matrix::look_to_matrix(Vector e,Vector d,Vector u){
	d=d.norm();
	d.y=-d.y;
	e.y=-e.y;
	Vector x=u.cross(d).norm();
	Vector y=x.cross(d);
	return {x.x,y.x,d.x,0,-x.y,-y.y,-d.y,0,x.z,y.z,d.z,0,-x.dot(e),-y.dot(e),-d.dot(e),1};
}



Vector krzem::operator*(Vector a,Matrix b){
	return {a.x*b._00+a.y*b._10+a.z*b._20+a.w*b._30,a.x*b._01+a.y*b._11+a.z*b._21+a.w*b._31,a.x*b._02+a.y*b._12+a.z*b._22+a.w*b._32,a.x*b._03+a.y*b._13+a.z*b._23+a.w*b._33};
}



Matrix krzem::operator*(Matrix a,Matrix b){
	return {a._00*b._00+a._01*b._10+a._02*b._20+a._03*b._30,a._00*b._01+a._01*b._11+a._02*b._21+a._03*b._31,a._00*b._02+a._01*b._12+a._02*b._22+a._03*b._32,a._00*b._03+a._01*b._13+a._02*b._23+a._03*b._33,a._10*b._00+a._11*b._10+a._12*b._20+a._13*b._30,a._10*b._01+a._11*b._11+a._12*b._21+a._13*b._31,a._10*b._02+a._11*b._12+a._12*b._22+a._13*b._32,a._10*b._03+a._11*b._13+a._12*b._23+a._13*b._33,a._20*b._00+a._21*b._10+a._22*b._20+a._23*b._30,a._20*b._01+a._21*b._11+a._22*b._21+a._23*b._31,a._20*b._02+a._21*b._12+a._22*b._22+a._23*b._32,a._20*b._03+a._21*b._13+a._22*b._23+a._23*b._33,a._30*b._00+a._31*b._10+a._32*b._20+a._33*b._30,a._30*b._01+a._31*b._11+a._32*b._21+a._33*b._31,a._30*b._02+a._31*b._12+a._32*b._22+a._33*b._32,a._30*b._03+a._31*b._13+a._32*b._23+a._33*b._33};
}



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



OBJFile OBJFile::load(wchar_t* fp,Renderer* r){
	static std::map<std::wstring,std::map<std::string,std::map<std::string,std::vector<float>>>> _m_cache;
	if (GetFileAttributesW(fp)==INVALID_FILE_ATTRIBUTES||GetFileAttributesW(fp)&FILE_ATTRIBUTE_DIRECTORY!=0){
		std::wcout<<L"File Not Found: "<<fp<<"\n";
		return {0};
	}
	struct dt_s_gm{
		std::vector<std::vector<float>> vxl;
		std::vector<std::vector<ushort>> fl;
	};
	struct dt_s{
		std::vector<float> vl;
		std::vector<float> vnl;
		std::vector<float> vtl;
		std::vector<std::wstring> ml;
		std::map<std::string,std::map<std::string,dt_s_gm>> f_m;
	} data;
	OBJFile o={fp};
	data.f_m["__main__"]={};
	std::string cg="__main__";
	std::string cm_id="__main_mat__";
	std::wstring fp_d(fp);
	fp_d=fp_d.substr(0,fp_d.find_last_of(L"/\\"))+(fp_d.find_last_of(L"/\\")==0?L"":L"\\");
	bool d_g=false;
	float* v_o=new float[4];
	std::ifstream fs(fp);
	std::string t,tt,l;
	while (std::getline(fs,l)){
		if (l.length()==0||l[0]=='#'){
			continue;
		}
		std::stringstream ss(l.substr(l.find_first_of(" ")+1));
		switch ((OBJ_FILE_TYPE_MAP.count(l.substr(0,l.find_first_of(" ")))==0?0:OBJ_FILE_TYPE_MAP.at(l.substr(0,l.find_first_of(" "))))){
			case 0:
				break;
			case 1:
				{
					std::wstring m_fp=fp_d+std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(l.substr(l.find_first_of(" ")+1));
					data.ml.push_back(m_fp);
					_m_cache[m_fp]={};
					std::ifstream m_fs(m_fp);
					std::string m_l,m_c_mtl,m_t;
					while (std::getline(m_fs,m_l)){
						if (m_l.length()==0||m_l[0]=='#'){
							continue;
						}
						std::stringstream m_ss(m_l.substr(m_l.find_first_of(" ")+1));
						std::string m_st_s="";
						switch ((OBJ_FILE_TYPE_MAP.count(m_l.substr(0,m_l.find_first_of(" ")))==0?0:OBJ_FILE_TYPE_MAP.at(m_l.substr(0,m_l.find_first_of(" "))))){
							case 0:
								break;
							case 8:
								m_c_mtl=m_l.substr(m_l.find_first_of(" ")+1);
								if (_m_cache[m_fp].count(m_c_mtl)>0){
									std::cout<<"ERROR: Duplicate Material Name: "<<m_c_mtl<<"\n";
									return {0};
								}
								_m_cache[m_fp][m_c_mtl]={};
								break;
							case 9:
								m_st_s="Ka";
							case 10:
								if (m_st_s.length()==0){
									m_st_s="Kd";
								}
							case 11:
								if (m_st_s.length()==0){
									m_st_s="Ks";
								}
								if (_m_cache[m_fp][m_c_mtl].count(m_st_s)>0){
									std::cout<<"ERROR: Duplicate Key "<<m_st_s<<" in Material "<<m_c_mtl<<"\n";
									return {0};
								}
								_m_cache[m_fp][m_c_mtl][m_st_s]={};
								for (uint i=0;i<3;i++){
									std::getline(m_ss,m_t,' ');
									_m_cache[m_fp][m_c_mtl][m_st_s].push_back(std::stof(m_t));
								}
								break;
							case 12:
								_m_cache[m_fp][m_c_mtl]["Ns"]={std::stof(m_l.substr(m_l.find_first_of(" ")+1))};
								break;
						}
					}
					m_fs.close();
				}
				break;
			case 2:
				cm_id=l.substr(l.find_first_of(" ")+1);
				break;
			case 3:
				for (uint i=0;i<3;i++){
					std::getline(ss,t,' ');
					data.vl.push_back((float)std::stod(t));
					if (data.vl.size()<=3){
						v_o[i]=data.vl[data.vl.size()-1];
					}
					else{
						v_o[i]=std::min(v_o[i],data.vl[data.vl.size()-1]);
						v_o[3]=std::max(v_o[6],data.vl[data.vl.size()-1]-v_o[i]);
					}
				}
				break;
			case 4:
				for (uint i=0;i<3;i++){
					std::getline(ss,t,' ');
					data.vnl.push_back((float)std::stod(t));
				}
				break;
			case 5:
				for (uint i=0;i<2;i++){
					std::getline(ss,t,' ');
					data.vtl.push_back((float)std::stod(t));
				}
				break;
			case 6:
				{
					if (cg=="__main__"){
						d_g=true;
					}
					if (data.f_m[cg].count(cm_id)==0){
						data.f_m[cg][cm_id]={};
					}
					std::vector<ushort> id_l;
					std::string tt;
					int bf_t=-1;
					for (uint i=0;i<3;i++){
						std::getline(ss,t,' ');
						if (bf_t==-1){
							bf_t=1;
							for (uint j=0;j<t.length()-1;j++){
								if (t[j]=='/'){
									bf_t++;
								}
							}
						}
						std::stringstream tss(t);
						std::vector<float> t_vx_l;
						for (uint j=0;j<bf_t;j++){
							std::getline(tss,tt,'/');
							if (tt.length()==0){
								continue;
							}
							uint v=std::stoi(tt)-1;
							if (j==0){
								for (uint k=v*3;k<v*3+3;k++){
									t_vx_l.push_back(data.vl[k]);
								}
							}
							else if (j==1){
								for (uint k=v*2;k<v*2+2;k++){
									t_vx_l.push_back(data.vtl[k]);
								}
							}
							else if (j==2){
								for (uint k=v*3;k<v*3+3;k++){
									t_vx_l.push_back(data.vnl[k]);
								}
							}
						}
						std::vector<std::vector<float>>::iterator vx_i=std::find(data.f_m[cg][cm_id].vxl.begin(),data.f_m[cg][cm_id].vxl.end(),t_vx_l);
						id_l.push_back((ushort)distance(data.f_m[cg][cm_id].vxl.begin(),vx_i));
						if (vx_i==data.f_m[cg][cm_id].vxl.end()){
							data.f_m[cg][cm_id].vxl.push_back(t_vx_l);
						}
					}
					data.f_m[cg][cm_id].fl.push_back(id_l);
				}
				break;
			case 7:
				cg=l.substr(l.find_first_of(" ")+1);
				if (data.f_m.count(cg)>0){
					break;
				}
				data.f_m[cg]={};
				break;
		}
	}
	fs.close();
	o.t={1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1};
	o.data_sz=data.f_m.size()-(d_g==false?1:0);
	o.data=new OBJFile::OBJFileGroup[data.f_m.size()-(d_g==false?1:0)];
	uint j=0;
	for (std::map<std::string,std::map<std::string,dt_s_gm>>::iterator i=data.f_m.begin();i!=data.f_m.end();i++){
		if (d_g==false&&i->first=="__main__"){
			continue;
		}
		o.data[j]={};
		o.data[j].nm=new char[i->first.length()+1];
		std::strcpy(o.data[j].nm,const_cast<char*>(i->first.c_str()));
		o.data[j].v=true;
		o.data[j].t={1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1};
		o.data[j].dt_sz=i->second.size();
		o.data[j].dt=new OBJFile::OBJFileGroup::OBJFileGroupData[i->second.size()];
		uint k=0;
		for (std::map<std::string,dt_s_gm>::iterator l=i->second.begin();l!=i->second.end();l++){
			o.data[j].dt[k]={};
			bool e=false;
			for (std::vector<std::wstring>::iterator m=data.ml.begin();m!=data.ml.end();m++){
				for (std::map<std::string,std::map<std::string,std::vector<float>>>::iterator n=_m_cache[*m].begin();n!=_m_cache[*m].end();n++){
					if (n->first==l->first){
						e=true;
						o.data[j].dt[k].mtl.Kd=Vector(n->second["Kd"][0],n->second["Kd"][1],n->second["Kd"][2]);///////////////////////////////////////////////////////////////////////////////////
					}
				}
				if (e==true){
					break;
				}
			}
			o.data[j].dt[k].ob_id=r->create_object_buffer(l->second.vxl[0].size());
			ObjectBuffer::ObjectBufferData* ob_b=r->get_object_buffer(o.data[j].dt[k].ob_id);
			for (std::vector<std::vector<float>>::iterator m=l->second.vxl.begin();m!=l->second.vxl.end();m++){
				uint ni=0;
				for (std::vector<float>::iterator n=(*m).begin();n!=(*m).end();n++){
					ob_b->vertexes.push_back((*n)/v_o[3]-v_o[ni]);
					ni++;
				}
			}
			for (std::vector<std::vector<ushort>>::iterator m=l->second.fl.begin();m!=l->second.fl.end();m++){
				for (std::vector<ushort>::iterator n=(*m).begin();n!=(*m).end();n++){
					ob_b->indicies.push_back(*n);
				}
			}
			r->update_object_buffer(o.data[j].dt[k].ob_id);
			k++;
		}
		j++;
	}
	return o;
}



void OBJFile::draw(OBJFile o,Renderer* r,uint rs){
	struct mat_cb{
		Vector Ka;
		Vector Kd;
		Vector Ks;
		float Ns;
		float _;
		float __;
		float ___;
		Matrix t;
		Matrix nt;
	};
	static ulong cb_id=r->create_constant_buffer(sizeof(mat_cb));
	static mat_cb cb={};
	r->set_shader_data({
		{
			SHADER_DATA_TYPE_CONSTANT_BUFFER,
			cb_id,
			rs,
			SHADER_DATA_FLAG_VS|SHADER_DATA_FLAG_PS
		}
	});
	for (uint i=0;i<o.data_sz;i++){
		if (o.data[i].v==false){
			continue;
		}
		cb.t=o.data[i].t*o.t;
		cb.nt=o.data[i].nt*o.nt;
		for (uint j=0;j<o.data[i].dt_sz;j++){
			cb.Kd=Vector(o.data[i].dt[j].mtl.Kd.x,o.data[i].dt[j].mtl.Kd.y,o.data[i].dt[j].mtl.Kd.z);
			r->update_constant_buffer(cb_id,&cb);
			r->render_object_buffer(o.data[i].dt[j].ob_id);
		}
	}
}



Camera::Camera(){
	//
}



Camera::~Camera(){
	//
}



void Camera::set_window(Window* w){
	this->_w=w;
}



void Camera::update(double dt){
	POINT mp;
	GetCursorPos(&mp);
	if (this->enabled==true){
		this->_drx+=(this->_w->y+this->_w->h/2.0-mp.y)*this->ROT_SPEED*dt;
		this->_dry+=(this->_w->x+this->_w->w/2.0-mp.x)*this->ROT_SPEED*dt;
		if (this->_drx>90){
			this->_drx=90;
		}
		if (this->_drx<-90){
			this->_drx=-90;
		}
		float dx=cos(this->_dry*4*atan(1)/180.0)*this->MOVE_SPEED*dt;
		float dz=sin(this->_dry*4*atan(1)/180.0)*this->MOVE_SPEED*dt;
		if (this->_w->pressed(0x57)){
			this->_dx-=dx;
			this->_dz-=dz;
		}
		if (this->_w->pressed(0x53)){
			this->_dx+=dx;
			this->_dz+=dz;
		}
		if (this->_w->pressed(0x41)){
			this->_dx+=dz;
			this->_dz-=dx;
		}
		if (this->_w->pressed(0x44)){
			this->_dx-=dz;
			this->_dz+=dx;
		}
		if (this->_w->pressed(0x20)){
			this->_dy+=this->MOVE_SPEED*dt;
		}
		if (this->_w->pressed(0x10)){
			this->_dy-=this->MOVE_SPEED*dt;
		}
		this->x=this->_ease(this->x,this->_dx);
		this->y=this->_ease(this->y,this->_dy);
		this->z=this->_ease(this->z,this->_dz);
		this->rx=this->_ease(this->rx,(this->_drx-90)*4*atan(1)/180.0);
		this->ry=this->_ease(this->ry,this->_dry*4*atan(1)/180.0);
	}
	if (this->lock==true){
		SetCursorPos(this->_w->x+this->_w->w/2,this->_w->y+this->_w->h/2);
	}
	this->matrix=Matrix::look_to_matrix(Vector(this->x,this->y,this->z),Vector(sin(this->rx)*cos(this->ry),cos(this->rx),sin(this->rx)*sin(this->ry)),Vector(0,1,0));
}



void Camera::reset(){
	SetCursorPos(this->_w->x+this->_w->w/2,this->_w->y+this->_w->h/2);
}



float Camera::_ease(float v,float t){
	if (abs(v-t)<1e-5){
		return t;
	}
	return v+0.45*(t-v);
}



Renderer::Renderer(){
	//
}



Renderer::~Renderer(){
	this->_e(true);
}



ulong Renderer::load_vertex_shader(const wchar_t* fp,const char* e,const char* v,D3D11_INPUT_ELEMENT_DESC* il,uint ill){
	this->_vsl[this->_n_vsl_id++]=nullptr;
	ID3DBlob* b=this->_compile_shader(fp,e,v);
	HRESULT hr=this->_d3_d->CreateVertexShader(b->GetBufferPointer(),b->GetBufferSize(),nullptr,&this->_vsl[this->_n_vsl_id-1]);
	if (FAILED(hr)){
		this->_err("Error creating VS",hr);
		return -1;
	}
	ID3D11InputLayout* vl=nullptr;
	hr=this->_d3_d->CreateInputLayout(il,ill,b->GetBufferPointer(),b->GetBufferSize(),&vl);
	if (FAILED(hr)){
		this->_err("Error creating VS Input Layout",hr);
		return -1;
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
		this->_err("Error Creating Constant Buffer",hr);
	}
	return this->_n_cbl_id-1;
}



ulong Renderer::create_sampler_state(const D3D11_SAMPLER_DESC sd){
	this->_ssl[this->_n_ssl_id++]=nullptr;
	HRESULT hr=this->_d3_d->CreateSamplerState(&sd,&this->_ssl[this->_n_ssl_id-1]);
	if (FAILED(hr)==true){
		this->_err("Error creating Sampler State",hr);
		return -1;
	}
	return this->_n_ssl_id-1;
}



ulong Renderer::create_object_buffer(uint sz){
	this->_obl[this->_n_obl_id++]=new ObjectBuffer(sz*sizeof(float));
	return this->_n_obl_id-1;
}



ulong Renderer::read_texture_file(const wchar_t* fp,bool sr){
	this->_txl[this->_n_txl_id++]=nullptr;
	IWICBitmapDecoder* dec;
	this->_wic_f->CreateDecoderFromFilename(fp,0,GENERIC_READ,WICDecodeMetadataCacheOnDemand,&dec);
	IWICBitmapFrameDecode* f;
	dec->GetFrame(0,&f);
	dec->Release();
	uint i_w;
	uint i_h;
	uint t_w;
	uint t_h;
	f->GetSize(&i_w,&i_h);
	if (i_w>D3D11_REQ_TEXTURE2D_U_OR_V_DIMENSION||i_h>D3D11_REQ_TEXTURE2D_U_OR_V_DIMENSION){
		float ar=static_cast<float>(i_h)/static_cast<float>(i_w);
		if (i_w>i_h){
			t_w=static_cast<uint>(D3D11_REQ_TEXTURE2D_U_OR_V_DIMENSION);
			t_h=static_cast<uint>(static_cast<float>(D3D11_REQ_TEXTURE2D_U_OR_V_DIMENSION)*ar);
		}
		else{
			t_w=static_cast<uint>(static_cast<float>(D3D11_REQ_TEXTURE2D_U_OR_V_DIMENSION)*ar);
			t_h=static_cast<uint>(D3D11_REQ_TEXTURE2D_U_OR_V_DIMENSION);
		}
	}
	else{
		t_w=i_w;
		t_h=i_h;
	}
	WICPixelFormatGUID px_f;
	f->GetPixelFormat(&px_f);
	WICPixelFormatGUID c_guid;
	memcpy(&c_guid,&px_f,sizeof(WICPixelFormatGUID));
	uint bpp=0;
	DXGI_FORMAT tf=DXGI_FORMAT_UNKNOWN;
	for (uint i=0;i<sizeof(WIC_TO_DXGI_D)/sizeof(WIC_TO_DXGI_D[0]);i++){
		if (memcmp(&WIC_TO_DXGI_D[i].wic_f,&px_f,sizeof(GUID))==0){
			tf=WIC_TO_DXGI_D[i].dxgi_f;
			break;
		}
	}
	if (tf==DXGI_FORMAT_UNKNOWN){
		for (size_t i=0;i<_countof(WIC_CONVERT_D);i++){
			if (memcmp(&WIC_CONVERT_D[i].s,&px_f,sizeof(WICPixelFormatGUID))==0){
				memcpy(&c_guid,&WIC_CONVERT_D[i].t,sizeof(WICPixelFormatGUID));
				tf=DXGI_FORMAT_UNKNOWN;
				for (uint i=0;i<sizeof(WIC_TO_DXGI_D)/sizeof(WIC_TO_DXGI_D[0]);i++){
					if (memcmp(&WIC_TO_DXGI_D[i].wic_f,&WIC_CONVERT_D[i].t,sizeof(GUID))==0){
						tf=WIC_TO_DXGI_D[i].dxgi_f;
						break;
					}
				}
				assert(tf!=DXGI_FORMAT_UNKNOWN);
				IWICComponentInfo* ci;
				this->_wic_f->CreateComponentInfo(c_guid,&ci);
				WICComponentType t;
				ci->GetComponentType(&t);
				if (t!=WICPixelFormat){
					bpp=0;
				}
				else{
					IWICPixelFormatInfo* pfi;
					ci->QueryInterface(__uuidof(IWICPixelFormatInfo),reinterpret_cast<void**>(&pfi));
					pfi->GetBitsPerPixel(&bpp);
					ci->Release();
					pfi->Release();
				}
				break;
			}
		}
	}
	else{
		IWICComponentInfo* ci;
		this->_wic_f->CreateComponentInfo(px_f,&ci);
		WICComponentType t;
		ci->GetComponentType(&t);
		if (t!=WICPixelFormat){
			bpp=0;
		}
		else{
			IWICPixelFormatInfo* pfi;
			ci->QueryInterface(__uuidof(IWICPixelFormatInfo),reinterpret_cast<void**>(&pfi));
			pfi->GetBitsPerPixel(&bpp);
			ci->Release();
			pfi->Release();
		}
	}
	uint ds=0;
	if (FAILED(this->_d3_d->CheckFormatSupport(tf,&ds))||!(ds&D3D11_FORMAT_SUPPORT_TEXTURE2D)){
		memcpy(&c_guid,&GUID_WICPixelFormat32bppRGBA,sizeof(WICPixelFormatGUID));
		tf=DXGI_FORMAT_R8G8B8A8_UNORM;
		bpp=32;
	}
	size_t rp=(t_w*bpp+7)/8;
	size_t i_sz=rp*t_h;
	std::unique_ptr<uint8_t[]> tmp(new uint8_t[ i_sz]);
	if (memcmp(&c_guid,&px_f,sizeof(GUID))==0&&t_w==i_w&&t_h==i_h){
		f->CopyPixels(0,static_cast<uint>(rp),static_cast<uint>(i_sz),tmp.get());
	}
	else if (t_w!=i_w||t_h!=i_h){
		IWICBitmapScaler* sc;
		this->_wic_f->CreateBitmapScaler(&sc);
		sc->Initialize(f,t_w,t_h,WICBitmapInterpolationModeFant);
		WICPixelFormatGUID pf_sc;
		sc->GetPixelFormat(&pf_sc);
		if (memcmp(&c_guid,&pf_sc,sizeof(GUID))==0){
			sc->CopyPixels(0,static_cast<uint>(rp),static_cast<uint>(i_sz),tmp.get());
		}
		else{
			IWICFormatConverter* fc;
			this->_wic_f->CreateFormatConverter(&fc);
			fc->Initialize(sc,c_guid,WICBitmapDitherTypeErrorDiffusion,0,0,WICBitmapPaletteTypeCustom);
			fc->CopyPixels(0,static_cast<uint>(rp),static_cast<uint>(i_sz),tmp.get());
			fc->Release();
		}
		sc->Release();
	}
	else{
		IWICFormatConverter* fc;
		this->_wic_f->CreateFormatConverter(&fc);
		fc->Initialize(f,c_guid,WICBitmapDitherTypeErrorDiffusion,0,0,WICBitmapPaletteTypeCustom);
		fc->CopyPixels(0,static_cast<uint>(rp), static_cast<uint>(i_sz),tmp.get());
		fc->Release();
	}
	bool sr_ag=false;
	if (sr==true){
		uint fmt_s=0;
		if (SUCCEEDED(this->_d3_d->CheckFormatSupport(tf,&fmt_s))&&(fmt_s&D3D11_FORMAT_SUPPORT_MIP_AUTOGEN)){
			sr_ag=true;
		}
	}
	D3D11_TEXTURE2D_DESC desc;
	desc.Width=t_w;
	desc.Height=t_h;
	desc.MipLevels=(sr_ag==true?0:1);
	desc.ArraySize=1;
	desc.Format=tf;
	desc.SampleDesc.Count=1;
	desc.SampleDesc.Quality=0;
	desc.Usage=D3D11_USAGE_DEFAULT;
	desc.BindFlags=(sr_ag==true?D3D11_BIND_SHADER_RESOURCE|D3D11_BIND_RENDER_TARGET:D3D11_BIND_SHADER_RESOURCE);
	desc.CPUAccessFlags=0;
	desc.MiscFlags=(sr_ag==true?D3D11_RESOURCE_MISC_GENERATE_MIPS:0);
	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem=tmp.get();
	initData.SysMemPitch=static_cast<uint>(rp);
	initData.SysMemSlicePitch=static_cast<uint>(i_sz);
	if (SUCCEEDED(this->_d3_d->CreateTexture2D(&desc,(sr_ag==true?nullptr:&initData),&this->_txl[this->_n_txl_id-1]))&&this->_txl[this->_n_txl_id-1]!=0){
		if (sr==true){
			this->_txsrl[this->_n_txl_id-1]=nullptr;
			D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
			memset(&SRVDesc,0,sizeof(SRVDesc));
			SRVDesc.Format=tf;
			SRVDesc.ViewDimension=D3D11_SRV_DIMENSION_TEXTURE2D;
			SRVDesc.Texture2D.MipLevels=(sr_ag==true?-1:1);
			this->_d3_d->CreateShaderResourceView(this->_txl[this->_n_txl_id-1],&SRVDesc,&this->_txsrl[this->_n_txl_id-1]);
			if (sr_ag==true){
				this->_d3_dc->UpdateSubresource(this->_txl[this->_n_txl_id-1],0,nullptr,tmp.get(),static_cast<uint>(rp),static_cast<uint>(i_sz));
				this->_d3_dc->GenerateMips(this->_txsrl[this->_n_txl_id-1]);
			}
		}
	}
	f->Release();
	return this->_n_txl_id-1;
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



void Renderer::set_shader_data(std::initializer_list<SHADER_DATA> dt){
	for (std::initializer_list<SHADER_DATA>::iterator i=dt.begin();i!=dt.end();i++){
		if ((*i).type==SHADER_DATA_TYPE_CONSTANT_BUFFER){
			if ((*i).fl&SHADER_DATA_FLAG_VS!=0){
				this->_d3_dc->VSSetConstantBuffers((*i).r,1,&this->_cbl[(*i).id]);
			}
			if ((*i).fl&SHADER_DATA_FLAG_PS!=0){
				this->_d3_dc->PSSetConstantBuffers((*i).r,1,&this->_cbl[(*i).id]);
			}
		}
		else if ((*i).type==SHADER_DATA_TYPE_TEXTURE){
			if ((*i).fl&SHADER_DATA_FLAG_VS!=0){
				this->_d3_dc->VSSetShaderResources((*i).r,1,&this->_txsrl[(*i).id]);
			}
			if ((*i).fl&SHADER_DATA_FLAG_PS!=0){
				this->_d3_dc->PSSetShaderResources((*i).r,1,&this->_txsrl[(*i).id]);
			}
		}
		else if ((*i).type==SHADER_DATA_TYPE_SAMPLER_STATE){
			if ((*i).fl&SHADER_DATA_FLAG_VS!=0){
				this->_d3_dc->VSSetSamplers((*i).r,1,&this->_ssl[(*i).id]);
			}
			if ((*i).fl&SHADER_DATA_FLAG_PS!=0){
				this->_d3_dc->PSSetSamplers((*i).r,1,&this->_ssl[(*i).id]);
			}
		}
		else{
			this->_err("Error applying Shader Data","Unknown Shader Data Type");
			return;
		}
	}
}



void Renderer::use_vertex_shader(ulong vs_id){
	this->_d3_dc->VSSetShader(this->_vsl[vs_id],nullptr,0);
}



void Renderer::use_pixel_shader(ulong ps_id){
	this->_d3_dc->PSSetShader(this->_psl[ps_id],nullptr,0);
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
		std::cout<<"ERR4"<<std::endl;
	}
	bd.Usage=D3D11_USAGE_DEFAULT;
	bd.ByteWidth=uint(this->_obl[ob_id]->data->indicies.size())*sizeof(ushort);
	bd.BindFlags=D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags=0;
	dt.pSysMem=this->_obl[ob_id]->data->indicies.data();
	hr=this->_d3_d->CreateBuffer(&bd,&dt,&this->_obl[ob_id]->ib);
	if (FAILED(hr)){
		this->_err("Error Updating the Object Buffer",hr);
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
	this->_d3_sc->Present((this->enable_vsync==true?1:0),DXGI_PRESENT_DO_NOT_WAIT);
	if (this->_p==1){
		this->_p=2;
		while (this->_p!=3){
			continue;
		}
	}
	this->_p=0;
}



void Renderer::_i(HWND _hwnd){
	this->_hwnd=_hwnd;
	D3D_FEATURE_LEVEL fl;
	D3D11CreateDevice(nullptr,D3D_DRIVER_TYPE_HARDWARE,0,D3D11_CREATE_DEVICE_DEBUG,nullptr,0,D3D11_SDK_VERSION,&this->_d3_d,&fl,&this->_d3_dc);
}



void Renderer::_e(bool e){
	if (e==true){
		this->_vsl.clear();
		this->_psl.clear();
		this->_cbl.clear();
		this->_ssl.clear();
		this->_obl.clear();
		this->_txl.clear();
		this->_txsrl.clear();
	}
	if (this->_d3_dc!=nullptr){
		this->_d3_dc->ClearState();
	}
	if (e==true&&this->_d3_d1!=nullptr){
		this->_d3_d1->Release();
	}
	if (e==true&&this->_d3_d!=nullptr){
		this->_d3_d->Release();
	}
	if (e==true&&this->_d3_dc1!=nullptr){
		this->_d3_dc1->Release();
	}
	if (e==true&&this->_d3_dc!=nullptr){
		this->_d3_dc->Release();
	}
	if (this->_d3_sc1!=nullptr){
		this->_d3_sc1->Release();
	}
	if (this->_d3_sc!=nullptr){
		this->_d3_sc->Release();
	}
	if (this->_d3_rt!=nullptr){
		this->_d3_rt->Release();
	}
	if (this->_d3_ds!=nullptr){
		this->_d3_ds->Release();
	}
	if (this->_d3_sv!=nullptr){
		this->_d3_sv->Release();
	}
}



void Renderer::_e_wic(){
	this->_wic_f->Release();
}



void Renderer::_r(bool s){
	if (this->_p!=0){
		return;
	}
	if (s==true){
		this->_p=1;
		while (this->_p!=2){
			continue;
		}
	}
	this->_e(false);
	RECT rc;
	GetClientRect(this->_hwnd,&rc);
	uint width=rc.right-rc.left;
	uint height=rc.bottom-rc.top;
	if (width<=0){
		width=1;
	}
	if (height<=0){
		height=1;
	}
	IDXGIFactory1* dxgi_f=nullptr;
	IDXGIDevice* dxgi_d=nullptr;
	HRESULT hr=this->_d3_d->QueryInterface(__uuidof(IDXGIDevice),reinterpret_cast<void**>(&dxgi_d));
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
		std::cout<<"ERR6"<<std::endl;
		return;
	}
	hr=this->_d3_d->CreateRenderTargetView(bb,nullptr,&this->_d3_rt);
	if (FAILED(hr)){
		std::cout<<"ERR7"<<std::endl;
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
		this->_err("Error creating Depth Stensil Texture",hr);
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
		std::cout<<"ERR9"<<std::endl;
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
	this->world_matrix={};
	this->view_matrix={};
	this->projection_matrix={};
	if (this->_wic_f==nullptr){
		CoCreateInstance(CLSID_WICImagingFactory,nullptr,CLSCTX_INPROC_SERVER,__uuidof(IWICImagingFactory),(void**)&this->_wic_f);
	}
	this->_p=3;
}



ID3DBlob* Renderer::_compile_shader(const wchar_t* fp,const char* e,const char* v){
	ID3DBlob* o=nullptr;
	ID3DBlob* err=nullptr;
	HRESULT hr=D3DCompileFromFile(fp,nullptr,nullptr,e,v,D3DCOMPILE_ENABLE_STRICTNESS,0,&o,&err);
	if (FAILED(hr)){
		if (err!=nullptr){
			this->_err("Error loading Shader",reinterpret_cast<const char*>(err->GetBufferPointer()));
			err->Release();
		}
		return nullptr;
	}
	return o;
}



void Renderer::_err(const char* s,HRESULT hr){
	_com_error e(hr);
	std::cout<<s<<": ";
	std::wcout<<e.ErrorMessage()<<L"\n";
}



void Renderer::_err(const char* s,const char* r){
	std::cout<<s<<": "<<r<<"\n";
}



// ulong RendererHelper::create_object_buffer_box(Renderer* r,Vector p,float sc){
// 	ulong ob=r->create_object_buffer(6);
// 	ObjectBuffer::ObjectBufferData* ob1=r->get_object_buffer(ob);
// 	ob1->add_vertexes({-sc+p.x,sc+p.y,-sc+p.z,0,1,0,sc+p.x,sc+p.y,-sc+p.z,0,1,0,sc+p.x,sc+p.y,sc+p.z,0,1,0,-sc+p.x,sc+p.y,sc+p.z,0,1,0,-sc+p.x,-sc+p.y,-sc+p.z,0,-1,0,sc+p.x,-sc+p.y,-sc+p.z,0,-1,0,sc+p.x,-sc+p.y,sc+p.z,0,-1,0,-sc+p.x,-sc+p.y,sc+p.z,0,-1,0,-sc+p.x,-sc+p.y,sc+p.z,-1,0,0,-sc+p.x,-sc+p.y,-sc+p.z,-1,0,0,-sc+p.x,sc+p.y,-sc+p.z,-1,0,0,-sc+p.x,sc+p.y,sc+p.z,-1,0,0,sc+p.x,-sc+p.y,sc+p.z,1,0,0,sc+p.x,-sc+p.y,-sc+p.z,1,0,0,sc+p.x,sc+p.y,-sc+p.z,1,0,0,sc+p.x,sc+p.y,sc+p.z,1,0,0,-sc+p.x,-sc+p.y,-sc+p.z,0,0,-1,sc+p.x,-sc+p.y,-sc+p.z,0,0,-1,sc+p.x,sc+p.y,-sc+p.z,0,0,-1,-sc+p.x,sc+p.y,-sc+p.z,0,0,-1,-sc+p.x,-sc+p.y,sc+p.z,0,0,1,sc+p.x,-sc+p.y,sc+p.z,0,0,1,sc+p.x,sc+p.y,sc+p.z,0,0,1,-sc+p.x,sc+p.y,sc+p.z,0,0,1});
// 	ob1->add_indicies({3,1,0,2,1,3,6,4,5,7,4,6,11,9,8,10,9,11,14,12,13,15,12,14,19,17,16,18,17,19,22,20,21,23,20,22});
// 	r->update_object_buffer(ob);
// 	return ob;
// }



Window::Window(int x,int y,int w,int h,const wchar_t* nm,void (*i_cb)(Window*),void (*u_cb)(Window*,double)){
	this->_create_wr(x,y,w,h,nm,i_cb,u_cb);
}



Window::~Window(){
	this->close();
}



HWND Window::get_handle(){
	return this->_hwnd;
}



void Window::show_cursor(bool st){
	PostMessage(this->_hwnd,WM_SHOW_CURSOR,st,0);
}



void Window::merge_thread(){
	this->_m_r_thr=true;
	this->_r_thr.join();
}



bool Window::pressed(int c){
	return GetKeyState(c)&0x8000;
}



void Window::resize(uint w,uint h){

}



void Window::resize(WINDOW_SIZE_TYPE s){
	if (s==WINDOW_SIZE_TYPE_MAXIMISED){
		if (this->_ss!=1){
			if (this->_ss==0){
				this->_save_state();
			}
			this->_ss=1;
			if (this->_cr==true){
				this->_rs=2;
			}
			else{
				ShowWindow(this->_hwnd,SW_MAXIMIZE);
				SendMessage(this->_hwnd,WM_SYSCOMMAND,SW_MAXIMIZE,0);
			}
		}
	}
	else if (s==WINDOW_SIZE_TYPE_MINIMIZED){
		if (this->_ss!=2){
			if (this->_ss==0){
				this->_save_state();
			}
			this->_ss=2;
			if (this->_cr==true){
				this->_rs=3;
			}
			else{
				ShowWindow(this->_hwnd,SW_MINIMIZE);
				SendMessage(this->_hwnd,WM_SYSCOMMAND,SW_MINIMIZE,0);
			}
		}
	}
	else if (s==WINDOW_SIZE_TYPE_RESTORE){
		if (this->_ss!=3){
			this->_ss=3;
			if (this->_cr==true){
				this->_rs=4;
			}
			else{
				this->_restore();
			}
		}
	}
	else{
		std::cout<<"ERR11";
	}
}



void Window::close(){
	if (this->_end==false){
		this->_end=true;
		ReleaseCapture();
		if (this->_m_r_thr==false){
			this->_r_thr.join();
		}
		this->_w_thr.join();
		DestroyWindow(this->_hwnd);
		this->renderer._e(true);
	}
}



void Window::_r(){
	if (this->_cr==true){
		this->renderer._r((this->_rs==0?true:false));
	}
}



void Window::_create_wr(int x,int y,int w,int h,const wchar_t* nm,void (*i_cb)(Window*),void (*u_cb)(Window*,double)){
	this->_w_thr=std::thread(&Window::_create,this,x,y,w,h,nm,i_cb);
	while (this->_cr==false){
		continue;
	}
	(*i_cb)(this);
	this->_l_tm=std::chrono::high_resolution_clock::now();
	this->_r_thr=std::thread(&Window::_render_thr,this,u_cb);
}



void Window::_create(int x,int y,int w,int h,const wchar_t* nm,void (*i_cb)(Window*)){
	this->name=const_cast<wchar_t*>(nm);
	WNDCLASSEXW wc={
		sizeof(WNDCLASSEX),
		CS_DBLCLKS|CS_OWNDC|CS_HREDRAW|CS_VREDRAW,
		&_msg_cb,
		0,
		0,
		GetModuleHandle(0),
		0,
		0,
		0,
		0,
		nm,
		0
	};
	RegisterClassExW(&wc);
	this->_hwnd=CreateWindowExW(wc.style,wc.lpszClassName,nm,WS_OVERLAPPEDWINDOW,x,y,w,h,nullptr,nullptr,GetModuleHandle(0),nullptr);
	this->x=x;
	this->y=y;
	this->w=w;
	this->h=h;
	SetWindowPos(this->_hwnd,HWND_NOTOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
	if (this->_hwnd==nullptr){
		std::cout<<"ERR10"<<std::endl;
		return;
	}
	Window::list[this->_hwnd]=this;
	ShowWindow(this->_hwnd,1);
	this->_save_state();
	MSG msg={0};
	while (msg.message!=WM_QUIT&&this->_end==false){
		if (PeekMessage(&msg,this->_hwnd,0,0,PM_REMOVE)==1){
			TranslateMessage(&msg);
			switch (msg.message){
				case WM_CREATE_RENDERER:
					SetCapture(this->_hwnd);
					this->renderer._i(this->_hwnd);
					this->_cr=true;
					this->renderer._r(false);
					break;
				case WM_SHOW_CURSOR:
					ShowCursor((msg.wParam==1?true:false));
					break;
				case WM_MOUSEMOVE:
					{
						this->mx=GET_X_LPARAM(msg.lParam);
						this->my=GET_Y_LPARAM(msg.lParam);
					}
					break;
				default:
					DispatchMessage(&msg);
					break;
			}
		}
	}
	this->_end=true;
	this->renderer._e_wic();
}



void Window::_render_thr(void (*u_cb)(Window*,double)){
	while (this->_end==false){
		if (this->_rs!=0){
			switch (this->_rs){
				case 1:
					// Normal size (_rs_w X _rs_h)
					break;
				case 2:
					ShowWindow(this->_hwnd,SW_MAXIMIZE);
					SendMessage(this->_hwnd,WM_SYSCOMMAND,SW_MAXIMIZE,0);
					break;
				case 3:
					ShowWindow(this->_hwnd,SW_MINIMIZE);
					SendMessage(this->_hwnd,WM_SYSCOMMAND,SW_MINIMIZE,0);
					break;
				case 4:
					this->_restore();
					break;
			}
			this->_rs=0;
		}
		std::chrono::high_resolution_clock::time_point tm=std::chrono::high_resolution_clock::now();
		(*u_cb)(this,(double)(std::chrono::duration_cast<std::chrono::nanoseconds>(tm-this->_l_tm).count())*1e-9);
		this->_l_tm=tm;
	}
}



void Window::_save_state(){
	GetWindowRect(this->_hwnd,&this->_ss_r);
	this->_ss_s=GetWindowLongPtrW(this->_hwnd,GWL_STYLE);
	this->_ss_exs=GetWindowLongPtrW(this->_hwnd,GWL_EXSTYLE);
}



void Window::_restore(){
	ShowWindow(this->_hwnd,SW_RESTORE);
	SetWindowPos(this->_hwnd,HWND_NOTOPMOST,this->_ss_r.left,this->_ss_r.top,this->_ss_r.right-this->_ss_r.left,this->_ss_r.bottom-this->_ss_r.top,0);
	SetWindowLongPtrW(this->_hwnd,GWL_STYLE,this->_ss_s);
	SetWindowLongPtrW(this->_hwnd,GWL_EXSTYLE,this->_ss_exs);
	this->renderer._r(true);
}



long_ptr krzem::_msg_cb(HWND hwnd,uint msg,uint_ptr wp,long_ptr lp){
	switch (msg){
		case WM_CREATE:
			PostMessage(hwnd,WM_CREATE_RENDERER,0,0);
			return 0;
		case WM_SIZE:
			Window::list[hwnd]->_r();
			return 0;
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
	}
	return DefWindowProc(hwnd,msg,wp,lp);
}



std::map<HWND,Window*> Window::list;

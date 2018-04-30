/*****************************************************************
Name :
Date : 2017/06/14
By   : CharlotteHonG
Final: 2017/06/14
*****************************************************************/
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <Timer.hpp>
#include "OpenBMP.hpp"

using namespace std;
using uch = unsigned char;
//----------------------------------------------------------------
// �g Bmp ��
void OpenBMP::bmpWrite( string name, const vector<uch>& raw,
	uint32_t width, uint32_t height, uint16_t bits)
{
	// �ɮ׸�T
	BmpFileHeader file_h(width, height, bits);
	// �Ϥ���T
	BmpInfoHeader info_h(width, height, bits);
	// �g��
	ofstream bmp(name, ios::binary);
	bmp.exceptions(ifstream::failbit|ifstream::badbit);
	bmp << file_h << info_h;
	// �g�զ�L
	if(bits == 8) {
		for(unsigned i = 0; i < 256; ++i) {
			bmp << uch(i) << uch(i) << uch(i) << uch(0);
		}
	}
	// �g�J�Ϥ���T
	size_t realW = info_h.biWidth * info_h.biBitCount/8.0;
	size_t alig = (realW*3) % 4;


	for(int j = height-1; j >= 0; --j) {
		for(unsigned i = 0; i < width; ++i) {
			if(bits==24) {
				bmp << raw[(j*width+i)*3 + 2];
				bmp << raw[(j*width+i)*3 + 1];
				bmp << raw[(j*width+i)*3 + 0];
			} else if(bits==8) {
				bmp << raw[(j*width+i)];
			}
		}
		// ���4byte
		for(unsigned i = 0; i < alig; ++i) {
			bmp << uch(0);
		}
	}
}
// Ū Bmp �ɮ�
void OpenBMP::bmpRead(vector<uch>& raw, string name,
	uint32_t* width, uint32_t* height, uint16_t* bits) {
	ifstream bmp(name.c_str(), ios::binary);
	bmp.exceptions(ifstream::failbit|ifstream::badbit);
	bmp.seekg(0, ios::beg);
	// Ū���Y
	BmpFileHeader file_h;
	bmp >> file_h;
	BmpInfoHeader info_h;
	bmp >> info_h;
	// �^�Ǹ�T
	if (width  != nullptr && 
		height != nullptr && 
		bits   != nullptr)
	{
		*width  = info_h.biWidth;
		*height = info_h.biHeight;
		*bits   = info_h.biBitCount;
	}
	// Ū Raw
	bmp.seekg(file_h.bfOffBits, ios::beg);
	raw.resize(info_h.biWidth * info_h.biHeight * (info_h.biBitCount/8));
	size_t realW = info_h.biWidth * info_h.biBitCount/8.0;
	size_t alig = (realW*3) % 4;
	char* p = reinterpret_cast<char*>(raw.data());
	for(int j = info_h.biHeight-1; j >= 0; --j) {
		for(unsigned i = 0; i < info_h.biWidth; ++i) {
			// �ӷ��O rgb
			if(info_h.biBitCount == 24) {
				bmp.read(p + j*info_h.biWidth*3+i*3 + 2, 1);
				bmp.read(p + j*info_h.biWidth*3+i*3 + 1, 1);
				bmp.read(p + j*info_h.biWidth*3+i*3 + 0, 1);
			}
			// �ӷ��O gray
			else if(info_h.biBitCount == 8) {
				bmp.read(p + j*info_h.biWidth+i, 1);
			}
		}
		bmp.seekg(alig, ios::cur); // ���} 4bite ������Ů�
	}
}
//----------------------------------------------------------------
// �g Raw ��
void OpenBMP::rawWrite(std::string name, const std::vector<uch>& raw) {
	std::ofstream raw_file(name.c_str(), std::ios::binary);
	raw_file.exceptions(std::ifstream::failbit|std::ifstream::badbit);
	raw_file.write(reinterpret_cast<const char*>(raw.data()), raw.size());
}
// Ū Raw ��
void OpenBMP::rawRead(std::vector<uch>& raw, std::string name) {
	std::ifstream raw_file(name.c_str(), 
		std::ios::binary | std::ios::ate);
	raw_file.exceptions(std::ifstream::failbit|std::ifstream::badbit);
	raw.resize(static_cast<size_t>(raw_file.tellg()));
	raw_file.seekg(0, std::ios::beg);
	raw_file.read(reinterpret_cast<char*>(raw.data()), raw.size());
	raw_file.close();
}

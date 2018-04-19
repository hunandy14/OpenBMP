/*****************************************************************
Name :
Date : 2017/06/12
By   : CharlotteHonG
Final: 2018/04/18

使用說明：
// 資料結構
vector<unsigned char> raw_img;
uint32_t width, height;
uint16_t bits;
// 讀寫 Bmp 
OpenBMP::bmpRead(raw_img, "ImgInput/kanna.bmp", &width, &height, &bits);
OpenBMP::bmpWrite("ImgOutput/out_kanna.bmp", raw_img, width, height, bits);
// 寫 raw 檔案
OpenBMP::rawWrite("ImgOutput/kanna.raw", raw_img);
// 轉灰階
OpenBMP::raw2gray(raw_img, raw_img);
OpenBMP::bmpWrite("ImgOutput/out_kanna_gray.bmp", raw_img, width, height, 8);
*****************************************************************/
#pragma once
#include <fstream>

//----------------------------------------------------------------
// 檔案檔頭 (BITMAPFILEHEADER)
#pragma pack(2) // 調整對齊
struct BmpFileHeader{
	uint16_t bfTybe=0x4d42;
	uint32_t bfSize;
	uint16_t bfReserved1=0;
	uint16_t bfReserved2=0;
	uint32_t bfOffBits=54;
	// constructor
	BmpFileHeader() = default;
	BmpFileHeader(uint32_t width, uint32_t height, uint16_t bits):
		bfSize(bfOffBits + width*height*bits/8)
	{
		if(bits==8) {bfSize += 1024, bfOffBits += 1024;}
	}
	// fstream
	friend std::ofstream& operator<<(
		std::ofstream& os, const BmpFileHeader& obj);
	friend std::ifstream& operator>>(
		std::ifstream& is, BmpFileHeader& obj);
	// ostream
	friend std::ostream& operator<<(
		std::ostream& os, const BmpFileHeader& obj);
};
#pragma pack() // 恢復對齊為預設
inline std::ofstream& operator<<(
	std::ofstream& os, const BmpFileHeader& obj){
	os.write((char*)&obj, sizeof(obj));
	return os;
}
inline std::ifstream& operator>>(
	std::ifstream& is, BmpFileHeader& obj){
	is.read((char*)&obj, sizeof(obj));
	return is;
}
inline std::ostream& operator<<(
	std::ostream& os, const BmpFileHeader& obj){
	using std::cout;
	using std::endl;
	cout << "# BmpFileHeader" << endl;
	cout << "    bfTybe      = " << obj.bfTybe << endl;
	cout << "    bfSize      = " << obj.bfSize << endl;
	cout << "    bfReserved1 = " << obj.bfReserved1 << endl;
	cout << "    bfReserved2 = " << obj.bfReserved2 << endl;
	cout << "    bfOffBits   = " << obj.bfOffBits;
	return os;
}


// 圖片資訊 (BITMAPINFOHEADER)
#pragma pack(2) // 調整對齊
struct BmpInfoHeader{
	uint32_t biSize=40;
	uint32_t biWidth;
	uint32_t biHeight;
	uint16_t biPlanes=1; // 1=defeaul, 0=custom
	uint16_t biBitCount;
	uint32_t biCompression=0;
	uint32_t biSizeImage;
	uint32_t biXPelsPerMeter=0; // 72dpi=2835, 96dpi=3780
	uint32_t biYPelsPerMeter=0; // 120dpi=4724, 300dpi=11811
	uint32_t biClrUsed=0;
	uint32_t biClrImportant=0;
	// constructor
	BmpInfoHeader() = default;
	BmpInfoHeader(uint32_t width, uint32_t height, uint16_t bits):
		biWidth(width), biHeight(height), biBitCount(bits),
		biSizeImage(width*height * bits/8)
	{
		if(bits==8) {biClrUsed=256;}
	}
	// fstream
	friend std::ofstream& operator<<(
		std::ofstream& os, const BmpInfoHeader& obj);
	friend std::ifstream& operator>>(
		std::ifstream& is, BmpInfoHeader& obj);
	// ostream
	friend std::ostream& operator<<(
		std::ostream& os, const BmpInfoHeader& obj);
};
#pragma pack() // 恢復對齊為預設
inline std::ofstream& operator<<(
	std::ofstream& os, const BmpInfoHeader& obj){
	os.write((char*)&obj, sizeof(obj));
	return os;
}
inline std::ifstream& operator>>(
	std::ifstream& is, BmpInfoHeader& obj){
	is.read((char*)&obj, sizeof(obj));
	return is;
}
inline std::ostream& operator<<(
	std::ostream& os, const BmpInfoHeader& obj){
	using std::cout;
	using std::endl;
	cout << "# BmpInfoHeader" << endl;
	cout << "    biSize          = " << obj.biSize << endl;
	cout << "    biWidth         = " << obj.biWidth << endl;
	cout << "    biHeight        = " << obj.biHeight << endl;
	cout << "    biPlanes        = " << obj.biPlanes << endl;
	cout << "    biBitCount      = " << obj.biBitCount << endl;
	cout << "    biCompression   = " << obj.biCompression << endl;
	cout << "    biSizeImage     = " << obj.biSizeImage << endl;
	cout << "    biXPelsPerMeter = " << obj.biXPelsPerMeter << endl;
	cout << "    biYPelsPerMeter = " << obj.biYPelsPerMeter << endl;
	cout << "    biClrUsed       = " << obj.biClrUsed << endl;
	cout << "    biClrImportant  = " << obj.biClrImportant;
	return os;
}


//----------------------------------------------------------------
class OpenBMP {
private:
	using uch = unsigned char;
	// RGB 轉灰階公式
	static uch rgb2gray(const uch* p) {
		return ((
			19595 * (*(p+0))+
			38469 * (*(p+1))+
			7472  * (*(p+2))) >> 16);
	}
public:
	// 轉灰階
	static void raw2gray(std::vector<uch>& dst, std::vector<uch>& src) {
		// 判定相等
		if(&dst!=&src)
			dst.resize(src.size()/3);
		// 轉換
		#pragma omp parallel for
		for (int i = 0; i < src.size()/3; ++i) {
			dst[i] = rgb2gray(&src[i*3]);
		} 
		dst.resize(src.size()/3);
	}
public:
	// 讀 Bmp 檔案
	static void bmpRead(std::vector<uch>& raw, std::string name,
		uint32_t* width=nullptr, uint32_t* height=nullptr, 
		uint16_t* bits=nullptr);
	// 寫 Bmp 檔
	static void bmpWrite(std::string name, std::vector<uch>& src,
		uint32_t width, uint32_t height, uint16_t bits=24);
	// 讀 Raw 檔
	static void rawRead(std::vector<uch>& src, std::string name);
	// 寫 Raw 檔
	static void rawWrite(std::string name, std::vector<uch>& src);
};


//----------------------------------------------------------------
struct basic_ImgData {
	std::vector<unsigned char> raw_img;
	uint32_t width;
	uint32_t height;
	uint16_t bits;
};


class ImgData: public basic_ImgData {
public:
	ImgData(std::string name) {
		OpenBMP::bmpRead(raw_img, name, &width, &height, &bits);
	}
	void write(std::string name) {
		OpenBMP::bmpWrite(name, raw_img, width, height, bits);
	}
public:
	std::vector<float> img;
};


class ImgData_nor: public basic_ImgData {
public:
	ImgData_nor(std::string name) {
		OpenBMP::bmpRead(raw_img, name, &width, &height, &bits);
		Normalization();
		raw_img.clear();
	}
	void write(std::string name) {
		reNormalization();
		OpenBMP::bmpWrite(name, raw_img, width, height, bits);
		nor_img.clear();
	}
public:
	void Normalization() {
		nor_img.resize(raw_img.size());
		#pragma omp parallel for
		for (int i = 0; i < nor_img.size(); i++) {
			nor_img[i] = raw_img[i] /255.0;
		}
	}
	void reNormalization() {
		raw_img.resize(nor_img.size());
		#pragma omp parallel for
		for (int i = 0; i < nor_img.size(); i++) {
			raw_img[i] = nor_img[i] *255.0;
		}
	}
public:
	std::vector<float> nor_img;
};
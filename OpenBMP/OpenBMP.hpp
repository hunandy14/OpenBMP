/*****************************************************************
Name :
Date : 2017/06/12
By   : CharlotteHonG
Final: 2018/04/18

使用說明：

// 宣告資料項目
string filename = "kanna.bmp";
vector<unsigned char> raw_img;
uint32_t weidth, heigh;
uint16_t bits;
// 讀 Bmp
Raw2Img::read_bmp(raw_img, filename, &weidth, &heigh, &bits);
// 寫 raw
Raw2Img::write_raw("out.raw", raw_img);
// 寫 Bmp
Raw2Img::raw2bmp("out.bmp", raw_img, weidth, heigh, bits);
// 轉灰階 
Raw2Img::raw2gray(raw_img);
Raw2Img::raw2bmp("out.bmp", raw_img, weidth, heigh, 8);
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
struct basic_ImgData {
	std::vector<unsigned char> raw_img;
	uint32_t width;
	uint32_t height;
	uint16_t bits;
};


//----------------------------------------------------------------
class OpenBMP {
private:
	using uch = unsigned char;
	// RGB 轉灰階公式
	static uch rgb2gray(uch* p) {
		return ((
			19595 * (*(p+0))+
			38469 * (*(p+1))+
			7472  * (*(p+2))) >> 16);
	}
	// 創建檔頭
	static BmpFileHeader makeFH(
		uint32_t width, uint32_t height, uint16_t bits)
	{
		BmpFileHeader file_h;
		file_h.bfSize = file_h.bfOffBits + width*height * bits/8;
		if(bits==8) {file_h.bfSize += 1024, file_h.bfOffBits += 1024;}
		return file_h;
	}
	static BmpInfoHeader makeIH(
		uint32_t width, uint32_t height, uint16_t bits)
	{
		BmpInfoHeader info_h;
		info_h.biWidth = width;
		info_h.biHeight = height;
		info_h.biBitCount = bits;
		info_h.biSizeImage = width*height * bits/8;
		if(bits==8) {info_h.biClrUsed=256;}
		return info_h;
	}
public:
	// 轉灰階
	static void raw2gray(std::vector<uch>& dst, std::vector<uch>& src) {
		// 判定相等
		if(&dst!=&src)
			dst.resize(src.size()/3);
		// 轉換
		#pragma omp parallel for
		for (unsigned i = 0; i < src.size()/3; ++i) {
			dst[i] = rgb2gray(&src[i*3]);
		} dst.resize(src.size()/3);
	}
public:
	// 讀 Bmp 檔案
	static void read_bmp(std::vector<uch>& raw, std::string name,
		uint32_t* width=nullptr, uint32_t* height=nullptr, 
		uint16_t* bits=nullptr);
	// 寫 Bmp 檔
	static void raw2bmp(std::string name, std::vector<uch>& src,
		uint32_t width, uint32_t height, uint16_t bits=24);
	// 讀 Raw 檔
	static void read_raw(std::vector<uch>& src, std::string name);
	// 寫 Raw 檔
	static void write_raw(std::string name, std::vector<uch>& src);
};


//----------------------------------------------------------------

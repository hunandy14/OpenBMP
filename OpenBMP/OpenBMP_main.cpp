/*****************************************************************
Name : OpenBMP Version 1.0.1
Date : 2017/06/11
By   : CharlotteHonG
Final: 2018/06/01
*****************************************************************/
#include <iostream>
#include "OpenBMP/OpenBMP.hpp"
#include "Timer.hpp"
using namespace std;

void WarpScale_rgb(const basic_ImgData &src, basic_ImgData &dst, double ratio);
void bilinear(const ImgData & src, ImgData & dst, double ratio);

//================================================================
int main(int argc, char const *argv[]) {
	Timer t0;
	ImgData imgTest1, imgOut;


	// 讀圖
	const ImgData img("ImgInput/test.bmp");
	// 圖片基本資訊
	img.info_print();
	// 輸出
	img.bmp("ImgOutput/test_orig.bmp");



	// 改變半張圖範例
	/*imgOut = img.toConvertGray();
	for (size_t i = 0; i < imgOut.size()/2; i++) {
		imgOut[i]=255;
	} imgOut.bmp("ImgOutput/out_test.bmp");*/



	// 轉灰階1 (會改變自己)
	/*imgOut = img;
	imgOut.convertGray();
	imgOut.bmp("ImgOutput/out_test-gray1.bmp");
	// 轉灰階2 (不會改變自己)
	imgOut = img.toConvertGray();
	imgOut.bmp("ImgOutput/out_test-gray2.bmp");*/



	// at2d() 測試
	/*imgOut.resize(100, 100, 24);
	for (size_t j = 0; j < imgOut.height; j++) {
		for (size_t i = 0; i < imgOut.width; i++) {
			auto p = imgOut.at2d(j, i);
			p[0]=128; p[1]=128; p[2]=128;
		}
	} imgOut.bmp("ImgOutput/out_test-at2d.bmp");*/



	// 擷取測試
	/*imgOut = img.toSnip(500, 500, 00, 100);
	imgOut.bmp("ImgOutput/out_test-toSnip.bmp");*/


	// 插補測試
	/*t0.start();
	WarpScale_rgb(img, imgOut, 5);
	//bilinear(img, imgOut, 5);
	t0.print("BASIC bilinear time");
	imgOut.bmp("ImgOutput/out_test1.bmp");*/


	// 連續線性插補測試
	/*int testCount=3;
	t0.start();
	bilinear(img, imgTest1, 0.5);
	for (size_t i = 0; i < testCount; i++) {
		ImgData temp;
		bilinear(imgTest1, temp, 2.0);
		bilinear(temp, imgTest1, 0.5);
	}
	bilinear(imgTest1, imgOut, 2.0);
	t0.print("BASIC bilinear time");
	imgOut.bmp("ImgOutput/out_test1.bmp");
	// 快速線性插補測試
	t0.start();
	WarpScale_rgb(img, imgTest1, 0.5);
	for (size_t i = 0; i < testCount; i++) {
		ImgData temp;
		WarpScale_rgb(imgTest1, temp, 2.0);
		WarpScale_rgb(temp, imgTest1, 0.5);
	}
	WarpScale_rgb(imgTest1, imgOut, 2.0);
	t0.print("FAST  bilinear time");
	imgOut.bmp("ImgOutput/out_test2.bmp");*/



	// 大小相符
	/*imgOut.clear();
	cout << "imgTest == img? " << (imgOut==img) << endl;
	imgOut.resize(img);
	cout << "imgTest == img? " << (imgOut==img) << endl;*/

	
	
	// 測試畫線函式
	/*imgOut = ImgData("ImgInput/lena.bmp");
	imgOut.convertRGB();
	for (size_t i = 0; i < 36; i++) {
		Draw::draw_arrowRGB(imgOut, 200, 200, 100.f*sqrt(2), i*10);
	} imgOut.bmp("arrow.bmp");*/



	// 基礎方法使用範例
	// 資料結構
	/*vector<unsigned char> raw_img;
	uint32_t width, height;
	uint16_t bits;
	// 讀寫 Bmp 
	OpenBMP::bmpRead(raw_img, "ImgInput/test.bmp", &width, &height, &bits);
	OpenBMP::bmpWrite("ImgOutput/out_test.bmp", raw_img, width, height, bits);
	// 寫 raw 檔案
	OpenBMP::rawWrite("ImgOutput/test.raw", raw_img);
	// 轉灰階
	OpenBMP::raw2gray(raw_img, raw_img);
	OpenBMP::bmpWrite("ImgOutput/out_test_gray.bmp", raw_img, width, height, 8);*/
	return 0;
}
//================================================================

// 快速線性插值_核心
static void fast_Bilinear_rgb(unsigned char* p, 
	const basic_ImgData& src, double y, double x)
{
	// 起點
	int _x = (int)x;
	int _y = (int)y;
	// 左邊比值
	double l_x = x - (double)_x;
	double r_x = 1.f - l_x;
	double t_y = y - (double)_y;
	double b_y = 1.f - t_y;
	int srcW = src.width;

	// 計算RGB
	double R , G, B;
	int x2 = (_x+1) > (int)srcW-1? (int)srcW-1: _x+1;
	int y2 = (_y+1) > (int)srcW-1? (int)srcW-1: _y+1;
	R  = (double)src.raw_img[(_y * srcW + _x) *3 + 0] * (r_x * b_y);
	G  = (double)src.raw_img[(_y * srcW + _x) *3 + 1] * (r_x * b_y);
	B  = (double)src.raw_img[(_y * srcW + _x) *3 + 2] * (r_x * b_y);
	R += (double)src.raw_img[(_y * srcW + x2) *3 + 0] * (l_x * b_y);
	G += (double)src.raw_img[(_y * srcW + x2) *3 + 1] * (l_x * b_y);
	B += (double)src.raw_img[(_y * srcW + x2) *3 + 2] * (l_x * b_y);
	R += (double)src.raw_img[(y2 * srcW + _x) *3 + 0] * (r_x * t_y);
	G += (double)src.raw_img[(y2 * srcW + _x) *3 + 1] * (r_x * t_y);
	B += (double)src.raw_img[(y2 * srcW + _x) *3 + 2] * (r_x * t_y);
	R += (double)src.raw_img[(y2 * srcW + x2) *3 + 0] * (l_x * t_y);
	G += (double)src.raw_img[(y2 * srcW + x2) *3 + 1] * (l_x * t_y);
	B += (double)src.raw_img[(y2 * srcW + x2) *3 + 2] * (l_x * t_y);

	p[0] = (unsigned char) R;
	p[1] = (unsigned char) G;
	p[2] = (unsigned char) B;
}
// 快速線性插值
void WarpScale_rgb(const basic_ImgData &src, basic_ImgData &dst, double ratio){
	// 防呆
	if (src.bits != 24) runtime_error("IMG is not 24bit.");
	// 初始化 dst
	dst.width  = (int)((src.width  * ratio) +0.5);
	dst.height = (int)((src.height * ratio) +0.5);
	dst.bits   = src.bits;
	dst.raw_img.resize(dst.width * dst.height * dst.bits>>3);

	// 縮小的倍率
	double r1W = ((double)src.width )/(dst.width );
	double r1H = ((double)src.height)/(dst.height);
	// 放大的倍率
	double r2W = (src.width -1.0)/(dst.width -1.0);
	double r2H = (src.height-1.0)/(dst.height-1.0);
	// 縮小時候的誤差
	double deviW = ((src.width-1.0)  - (dst.width -1.0)*(r1W)) /dst.width;
	double deviH = ((src.height-1.0) - (dst.height-1.0)*(r1H)) /dst.height;

	// 跑新圖座標
#pragma omp parallel for
	for (int j = 0; j < (int)dst.height; ++j) {
		for (int i = 0; i < (int)dst.width; ++i) {
			// 調整對齊
			double srcY=0, srcX=0;
			if (ratio < 1.0) {
				//srcY = ((j+0.5f)/Ratio) - 0.5;
				//srcX = ((i+0.5f)/Ratio) - 0.5;
				srcX = i*(r1W+deviW);
				srcY = j*(r1H+deviH);
			} else if (ratio >= 1.0) {
				srcX = i*r2W;
				srcY = j*r2H;
			}

			// 獲取插補值
			unsigned char* p = &dst.raw_img[(j*dst.width + i) *3];
			if (ratio>1) {
				fast_Bilinear_rgb(p, src, srcY, srcX);
			} else {
				fast_Bilinear_rgb(p, src, srcY, srcX);
			}
		}
	}
}
// 一般線性插補
void bilinear(const ImgData& src, ImgData& dst, double ratio) {
	// 重設置目標圖像大小
	dst.resize((uint16_t)(src.width*ratio), (uint16_t)(src.height*ratio), src.bits);
	// 縮小的倍率
	double r1W = ((double)src.width )/(dst.width );
	double r1H = ((double)src.height)/(dst.height);
	// 放大的倍率
	double r2W = (src.width -1.0)/(dst.width -1.0);
	double r2H = (src.height-1.0)/(dst.height-1.0);
	// 縮小時候的誤差
	double deviW = ((src.width-1.0)  - (dst.width -1.0)*(r1W)) /dst.width;
	double deviH = ((src.height-1.0) - (dst.height-1.0)*(r1H)) /dst.height;

#pragma omp parallel for
	for (int j = 0; j < (int)dst.height; j++) {
		for (int i = 0; i < (int)dst.width; i++) {
			double srcY=0, srcX=0;
			// 調整對齊
			if (ratio < 1.0) {
				srcX = i*(r1W+deviW);
				srcY = j*(r1H+deviH);
			} else if (ratio >= 1.0) {
				srcX = i*r2W;
				srcY = j*r2H;
			}
			auto dstImg = dst.at2d(j, i);
			auto srcImg = src.at2d_linear(srcY, srcX);
			for (uint16_t rgb = 0; rgb < src.bits>>3; rgb++) {
				dstImg[rgb] = static_cast<unsigned char>(srcImg[rgb]);
			}
		}
	}
}

/*****************************************************************
Name : OpenBMP
Date : 2017/06/11
By   : CharlotteHonG
Final: 2018/04/18
*****************************************************************/
#include <iostream>
#include <vector>
#include <Timer.hpp>
#include "OpenBMP\OpenBMP.hpp"
using namespace std;


// �ֳt �u�ʴ���
inline static void fast_Bilinear_rgb(unsigned char* p, 
	const basic_ImgData& src, double y, double x)
{
	// �_�I
	int _x = (int)x;
	int _y = (int)y;
	// ������
	double l_x = x - (double)_x;
	double r_x = 1.f - l_x;
	double t_y = y - (double)_y;
	double b_y = 1.f - t_y;
	int srcW = src.width;
	int srcH = src.height;

	// �p��RGB
	double R , G, B;
	int x2 = (_x+1) > src.width -1? src.width -1: _x+1;
	int y2 = (_y+1) > src.height-1? src.height-1: _y+1;
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

	*(p+0) = (unsigned char) R;
	*(p+1) = (unsigned char) G;
	*(p+2) = (unsigned char) B;
}
void WarpScale(const basic_ImgData &src, basic_ImgData &dst, double Ratio){
	int newH = (int)((src.height * Ratio) +0.5);
	int newW = (int)((src.width  * Ratio) +0.5);
	// ��l�� dst
	dst.raw_img.resize(newW * newH * src.bits/8.0);
	dst.width  = newW;
	dst.height = newH;
	dst.bits   = src.bits;
	// �Y�p�����v
	double r1W = ((double)src.width )/(dst.width );
	double r1H = ((double)src.height)/(dst.height);
	// ��j�����v
	double r2W = (src.width -1.0)/(dst.width -1.0);
	double r2H = (src.height-1.0)/(dst.height-1.0);
	// �Y�p�ɭԪ��~�t
	double deviW = ((src.width-1.0)  - (dst.width -1.0)*(r1W)) /dst.width;
	double deviH = ((src.height-1.0) - (dst.height-1.0)*(r1H)) /dst.height;
	// �]�s�Ϯy��
#pragma omp parallel for
	for (int j = 0; j < newH; ++j) {
		for (int i = 0; i < newW; ++i) {
			// �վ���
			double srcY, srcX;
			if (Ratio < 1.0) {
				/*srcX = (i+0.5) * ((double)src.width /dst.width ) - 0.5;
				srcY = (j+0.5) * ((double)src.height/dst.height) - 0.5;*/
				srcX = i*(r1W+deviW);
				srcY = j*(r1H+deviH);
			} else if (Ratio >= 1.0) {
				srcX = i*r2W;
				srcY = j*r2H;
			}
			// ������ɭ�
			unsigned char* p = &dst.raw_img[(j*newW + i) *3];
			if (Ratio>1) {
				fast_Bilinear_rgb(p, src, srcY, srcX);
			}
		}
	}
}

void bilinear(const ImgData& src, ImgData& dst, double ratio) {
	//Timer t0;
	//t0.start();

	dst.resize(src.width*ratio, src.height*ratio, src.bits);

	// �Y�p�����v
	double r1W = ((double)src.width )/(dst.width );
	double r1H = ((double)src.height)/(dst.height);
	// ��j�����v
	double r2W = (src.width -1.0)/(dst.width -1.0);
	double r2H = (src.height-1.0)/(dst.height-1.0);
	// �Y�p�ɭԪ��~�t
	double deviW = ((src.width-1.0)  - (dst.width -1.0)*(r1W)) /dst.width;
	double deviH = ((src.height-1.0) - (dst.height-1.0)*(r1H)) /dst.height;

#pragma omp parallel for
	for (int j = 0; j < dst.height; j++) {
		for (int i = 0; i < dst.width; i++) {
			double ratio = (double)dst.width/src.width;
			double srcY=0, srcX=0;
			if (ratio < 1.0) {
				/*srcX = (i+0.5) * ((double)src.width /dst.width ) - 0.5;
				srcY = (j+0.5) * ((double)src.height/dst.height) - 0.5;*/
				srcX = i*(r1W+deviW);
				srcY = j*(r1H+deviH);
			} else if (ratio >= 1.0) {
				srcX = i*r2W;
				srcY = j*r2H;
			}
			auto dstImg = dst.at2d(j, i);
			auto srcImg = src.at2d_linear(srcY, srcX);

			// �����Y�񦳵L���`��
			if (j<=0 and i<=2) {
				cout << srcX << ", " << srcY << endl;
			} else if (j == dst.height-1 and i == dst.width-1) {
				cout << srcX << ", " << srcY << endl;
			}

			for (size_t rgb = 0; rgb < src.bits>>3; rgb++) {
				dstImg[rgb] = srcImg[rgb];
			}
		}
	}
	//t0.print("bilinear");
}
//================================================================
int main(int argc, char const *argv[]) {
	ImgData img("ImgInput/test.bmp");
	//img.convertGray();
	//img.bmp("ImgOutput/out_test.bmp");

	// �U�д���
	/*ImgData imgTest=img;
	for (size_t i = 0; i < imgTest.size(); i++) {
		imgTest[i]=128;
	}
	imgTest.bmp("ImgOutput/out_test.bmp");*/

	// at2d() ����
	/*ImgData imgTest;
	imgTest.resize(100, 100, 24);
	for (size_t j = 0; j < imgTest.height; j++) {
		for (size_t i = 0; i < imgTest.width; i++) {
			auto p = imgTest.at2d(j, i);
			p[0]=128; p[1]=128; p[2]=128;
		}
	}
	imgTest.bmp("ImgOutput/out_test.bmp");*/

	// �^������
	/*ImgData snip=img.toSnip(500, 500, 00, 100);
	snip.bmp("ImgOutput/out_test.bmp");*/

	// �u�ʴ��ɴ���
	ImgData imgTest1,imgTest2, temp1, temp2, temp;
	double ratio=2;
	bilinear(img, imgTest1, 4);
	/*bilinear(imgTest1, imgTest2, 2.0);
	bilinear(imgTest2, imgTest1, 0.5);
	bilinear(imgTest1, imgTest2, 2.0);
	bilinear(imgTest2, imgTest1, 0.5);
	bilinear(imgTest1, imgTest2, 2.0);*/
	//imgTest1.bmp("ImgOutput/out_test.bmp");

	Timer t0;
	t0.start();
	bilinear(img, temp, 0.5);
	bilinear(temp, imgTest1, 2);
	t0.print("t1");
	imgTest1.bmp("ImgOutput/out_test1.bmp");

	t0.start();
	WarpScale(temp, imgTest2, 2);
	WarpScale(img, temp, 0.5);
	t0.print("t2");
	temp.bmp("ImgOutput/out_test2.bmp");
	imgTest2.bmp("ImgOutput/out_test22.bmp");

	cout << endl;
	// �j�p�۲�
	/*ImgData imgTest;
	cout << "imgTest==img " << (imgTest==img) << endl;
	imgTest.resize(img);
	cout << "imgTest==img " << (imgTest==img) << endl;*/

	/*// ��Ƶ��c
	vector<unsigned char> raw_img;
	uint32_t width, height;
	uint16_t bits;
	// Ū�g Bmp 
	OpenBMP::bmpRead(raw_img, "ImgInput/test.bmp", &width, &height, &bits);
	OpenBMP::bmpWrite("ImgOutput/out_test.bmp", raw_img, width, height, bits);
	// �g raw �ɮ�
	OpenBMP::rawWrite("ImgOutput/test.raw", raw_img);
	// ��Ƕ�
	OpenBMP::raw2gray(raw_img, raw_img);
	OpenBMP::bmpWrite("ImgOutput/out_test_gray.bmp", raw_img, width, height, 8);*/

	return 0;
}
//================================================================

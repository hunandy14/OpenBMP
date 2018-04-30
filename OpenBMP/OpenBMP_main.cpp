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

	// �u�ʴ���
	//ImgData imgTest(960, 540, 8);
	float ratio;
	ImgData imgTest;
	Timer t0;

	ratio=0.5;
	imgTest.resize(img.width*ratio, img.height*ratio, img.bits);
	t0.start();

	double toleX = 1.0/imgTest.width, toleY = 1.0/imgTest.height;

#pragma omp parallel for
	for (int j = 0; j < imgTest.height; j++) {
		for (int i = 0; i < imgTest.width; i++) {
			double ratio = (double)imgTest.width/img.width;
			float srcY=0, srcX=0;
			if (ratio >= 1) {
				srcX = (i / ((double)(imgTest.width -1.0)/(img.width -1.0)));
				srcY = (j / ((double)(imgTest.height-1.0)/(img.height-1.0)));
			} else if (ratio < 1) {
				//srcX = (i+0.5) * ((double)img.width /imgTest.width ) - 0.5;
				//srcY = (j+0.5) * ((double)img.height/imgTest.height) - 0.5;
				srcX = i * ((img.width +1.0)/imgTest.width );
				srcY = j * ((img.height+1.0)/imgTest.height);
			}
			auto dstImg = imgTest.at2d(j, i);
			auto srcImg = img.at2d_linear(srcY, srcX);

			if (j==0 and i==0) {
				cout << srcX << ", " << srcY << endl;
			} else if (j == imgTest.height-1 and i == imgTest.width-1) {
				cout << srcX << ", " << srcY << endl;
			}

			for (size_t rgb = 0; rgb < img.bits>>3; rgb++) {
				dstImg[rgb] = srcImg[rgb];
			}
		}
	}
	t0.print("bilinear");

	img=imgTest;

	ratio=2;
	imgTest.resize(img.width*ratio, img.height*ratio, img.bits);

	cout << "��j" << endl;
	t0.start();
#pragma omp parallel for
	for (int j = 0; j < imgTest.height; j++) {
		for (int i = 0; i < imgTest.width; i++) {
			double ratio = (double)imgTest.width/img.width;
			float srcY=0, srcX=0;
			if (ratio >= 1) {
				srcX = (i / ((double)(imgTest.width -1.0)/(img.width -1.0)));
				srcY = (j / ((double)(imgTest.height-1.0)/(img.height-1.0)));
			} else if (ratio < 1) {
				//srcX = (i+0.5) * ((double)img.width /imgTest.width ) - 0.5;
				//srcY = (j+0.5) * ((double)img.height/imgTest.height) - 0.5;
				srcX = i * ((img.width +1.0)/imgTest.width );
				srcY = j * ((img.height+1.0)/imgTest.height);
			}
			auto dstImg = imgTest.at2d(j, i);
			auto srcImg = img.at2d_linear(srcY, srcX);

			if (j==0 and i==0) {
				cout << srcX << ", " << srcY << endl;
			} else if (j == imgTest.height-1 and i == imgTest.width-1) {
				cout << srcX << ", " << srcY << endl;
			}

			for (size_t rgb = 0; rgb < img.bits>>3; rgb++) {
				dstImg[rgb] = srcImg[rgb];
			}
		}
	}
	t0.print("bilinear");




	imgTest.bmp("ImgOutput/out_test.bmp");

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

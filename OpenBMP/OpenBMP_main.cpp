/*****************************************************************
Name : OpenBMP
Date : 2017/06/11
By   : CharlotteHonG
Final: 2018/04/18
*****************************************************************/
#include <iostream>
#include <vector>
#include <Timer.hpp>
#include "OpenBMP.hpp"
using namespace std;
//================================================================
int main(int argc, char const *argv[]) {
	Timer t1;
	vector<unsigned char> raw_img;
	uint32_t weidth, heigh;
	uint16_t bits;
	/* Åª¼g Bmp */
	OpenBMP::read_bmp(raw_img, "ImgInput/kanna.bmp", &weidth, &heigh, &bits);

	OpenBMP::write_raw("ImgOutput/kanna.raw", raw_img);
	OpenBMP::raw2bmp("ImgOutput/out_kanna.bmp", raw_img, weidth, heigh, bits);
	/* Âà¦Ç¶¥ */
	t1.start();
	OpenBMP::raw2gray(raw_img, raw_img);
	t1.print(" raw2gray");
	OpenBMP::raw2bmp("ImgOutput/out_kanna_gray.bmp", raw_img, weidth, heigh, 8);

	return 0;
}
//================================================================

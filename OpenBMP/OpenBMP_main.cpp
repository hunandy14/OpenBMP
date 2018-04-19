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
	ImgData img("ImgInput/kanna.bmp");
	Timer t1;
	img.write("ImgOutput/out_kanna.bmp");
	t1.print("img");

	ImgData_nor img_nor("ImgInput/kanna.bmp");
	img_nor.write("ImgOutput/out_kanna_nor.bmp");
	return 0;
}
//================================================================

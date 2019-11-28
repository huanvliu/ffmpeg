#include "VideoWriter.h"
#include <string>
#include <opencv2/opencv.hpp>
#include <time.h>
#include <iostream>
using namespace std;


int main() {
	cv::Mat img = cv::imread("./t1.jpg");
	_VWirter w("./test.h264", 25, img.cols, img.rows);
	
	clock_t start = clock();
	for(int i = 0; i < 250; i++) {
		w.write(img.data, img.step);
	}
	double duration = clock() - start;
	cout << 1000 * duration / CLOCKS_PER_SEC << endl;
	w.close();
}
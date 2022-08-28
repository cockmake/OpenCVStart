#pragma once
#pragma execution_character_set("utf-8")
#include<opencv2/opencv.hpp>
#include<openvino/openvino.hpp>
using namespace std;
using namespace cv;
using namespace ov;

class SuperResolution
{
private:
	InferRequest inferRequest;
	Shape inputShape, inputShape2, outputShape;
	Tensor inputTensor, inputTensor2, outputTensor;
	size_t c, h, w, h2, w2;
	float* output, * input_data, * input_data2;
public:
	SuperResolution();
	Mat processThis(Mat& src);
};


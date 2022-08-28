#pragma once
#pragma execution_character_set("utf-8")
#include<opencv2/opencv.hpp>
#include<openvino/openvino.hpp>
using namespace std;
using namespace cv;
using namespace ov;
class PersonSeg{
private:
	InferRequest inferRequest;
	Shape inputShape, outputShape, outputShape2, outputShape3;
	Tensor inputTensor, outputTensor, outputTensor2, outputTensor3;
	double confidence = 0.5;
	size_t c, h, w, mask_h, mask_w;
	int64* labels;
	float* output2, * raw_mask;
public:
	PersonSeg();
	Mat processThis(Mat& src, Mat& background);
	void setConfidence(double cf);
};


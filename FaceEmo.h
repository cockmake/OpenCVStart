#pragma once
#pragma execution_character_set("utf-8")
#include<opencv2/opencv.hpp>
#include<openvino/openvino.hpp>

using namespace std;
using namespace cv;
using namespace ov;
class FaceEmo{
private:
	InferRequest inferRequest, inferRequest2;
	Shape inputShape, inputShape2;
	Tensor inputTensor, inputTensor2, outputTensor, outputTensor2;
	double confidence = 0.5;
	size_t c, h, w, h2, w2;
	float *input_data, *input_data2;
	float *output, *output2;
	vector<Mat> process_one(const Mat& src);
	vector<vector<QString>> process_two(const vector<Mat>& mats);
public:
	vector<QString> emos = { "面无表情", "开心", "伤心", "惊讶", "生气" };
	FaceEmo();
	vector<vector<QString>> processThis(Mat& src);
	void setConfidence(double cf);
};


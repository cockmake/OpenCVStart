#pragma once
#pragma execution_character_set("utf-8")
#include<opencv2/opencv.hpp>
#include<openvino/openvino.hpp>
using namespace std;
using namespace cv;
using namespace ov;
class PersonDetection : public QObject{
private:
	InferRequest inferRequest;
	Shape inputShape;
	Tensor inputTensor, outputTensor;
	double confidence = 0.5;
	size_t c, h, w;
	float* output;
public:
	PersonDetection();
	vector<vector<QString>> processThis(Mat& src);
	void setConfidence(double cf);
};


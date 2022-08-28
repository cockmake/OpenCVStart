#include "stdafx.h"
#include "PersonDetection.h"
PersonDetection::PersonDetection() {
	string model_bin = QDir::currentPath().toStdString() + "\\models\\person-detection-0303.bin",
		model_xml = QDir::currentPath().toStdString() + "\\models\\person-detection-0303.xml";
	//初始化推理请求
	Core ie;
	shared_ptr<Model> p_model = ie.read_model(model_xml, model_bin);
	CompiledModel compiledModel = ie.compile_model(p_model, "CPU");
	inferRequest = compiledModel.create_infer_request();
	//获取输入输输出的Shape
	inputTensor = inferRequest.get_input_tensor();
	inputShape = inputTensor.get_shape();
	c = inputShape[1];
	h = inputShape[2];
	w = inputShape[3];
}
vector<vector<QString>> PersonDetection::processThis(Mat& src) {
	//获取输入数据
	Mat blobImg, ret;
	resize(src, blobImg, Size(w, h));

	float width_ratio = src.cols / static_cast<float>(w), height_ratio = src.rows / static_cast<float>(h);
	vector<vector<QString>> ret_container;
	blobImg.convertTo(blobImg, CV_32FC3);

	float* input_data = inputTensor.data<float>();
	for (size_t i = 0; i < c; i++)
	{
		for (size_t j = 0; j < h; j++)
		{
			for (size_t k = 0; k < w; k++)
			{
				input_data[i * h * w + j * w + k] = blobImg.at<Vec3f>(j, k)[i];
			}
		}
	}
	inferRequest.infer();
	outputTensor = inferRequest.get_output_tensor(0);
	output = outputTensor.data<float>();
	for (int i = 0; i < 100; i++) {
		if (output[i * 5 + 4] >= this->confidence) {
			vector<QString> one_example;
			one_example.push_back("人");

			Rect box = Rect(Point2i(output[i * 5] * width_ratio, output[i * 5 + 1] * height_ratio),
				Point2i(output[i * 5 + 2] * width_ratio, output[i * 5 + 3] * height_ratio));
			rectangle(src, box, Scalar(0, 0, 255), 2);
			putText(src, to_string(i + 1), box.tl(), 1, 2, Scalar(173, 202, 25), 2);

			one_example.push_back(QString::number((box.tl().x + box.br().x) / 2));
			one_example.push_back(QString::number((box.tl().y + box.br().y) / 2));
			one_example.push_back(QString::number(int(output[i * 5 + 4] * 10000) / 100.0) + "%");
			ret_container.emplace_back(one_example);
		}
		else break;
	}
	return ret_container;
}
void PersonDetection::setConfidence(double cf) {
	this->confidence = cf;
}
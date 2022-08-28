#include "stdafx.h"
#include "FaceEmo.h"
FaceEmo::FaceEmo() {
	string model_xml, model_bin, model_xml2, model_bin2;
	model_xml = QDir::currentPath().toStdString() + string("\\models\\face-detection-0205.xml");
	model_bin = QDir::currentPath().toStdString() + string("\\models\\face-detection-0205.bin");
	model_xml2 = QDir::currentPath().toStdString() + string("\\models\\emotions-recognition-retail-0003.xml");
	model_bin2 = QDir::currentPath().toStdString() + string("\\models\\emotions-recognition-retail-0003.bin");
	Core ie;
	shared_ptr<Model> p_model = ie.read_model(model_xml, model_bin), p_model2 = ie.read_model(model_xml2, model_bin2);
	CompiledModel compiledModel = ie.compile_model(p_model, "CPU"), compiledModel2 = ie.compile_model(p_model2, "CPU");
	inferRequest = compiledModel.create_infer_request();
	inferRequest2 = compiledModel2.create_infer_request();
	//获取输入输输出的Shape
	inputTensor = inferRequest.get_input_tensor();
	inputShape = inputTensor.get_shape();
	c = inputShape[1];
	h = inputShape[2];
	w = inputShape[3];

	inputTensor2 = inferRequest2.get_input_tensor();
	inputShape2 = inputTensor2.get_shape();
	h2 = inputShape2[2];
	w2 = inputShape2[3];
}
vector<Mat> FaceEmo::process_one(const Mat& src) {
	//获取输入数据
	Mat blobImg;
	resize(src, blobImg, Size(w, h));

	float width_ratio = src.cols / static_cast<float>(w), height_ratio = src.rows / static_cast<float>(h);

	blobImg.convertTo(blobImg, CV_32FC3);

	input_data = inputTensor.data<float>();

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
	vector<Mat> ret;
	for (int i = 0; i < 200; i++) {
		if (output[i * 5 + 4] >= confidence) {
			Rect box = Rect(Point2i(output[i * 5] * width_ratio, output[i * 5 + 1] * height_ratio - 20),
				Point2i(output[i * 5 + 2] * width_ratio, output[i * 5 + 3] * height_ratio));
			rectangle(src, box, Scalar(0, 0, 255), 2);
			ret.emplace_back(src(box));
			putText(src, to_string(i + 1), box.tl(), 1, 2, Scalar(173, 202, 25), 2);
		}
		else break;
	}
	return ret;
}
vector<vector<QString>> FaceEmo::process_two(const vector<Mat>& mats) {
	vector<vector<QString>> ret;
	Mat blobImg;
	for (Mat mat : mats) {
		vector<QString> one_example;
		cv::resize(mat, blobImg, Size(w2, h2));
		blobImg.convertTo(blobImg, CV_32FC3);

		input_data2 = inputTensor2.data<float>();
		for (size_t i = 0; i < c; i++)
		{
			for (size_t j = 0; j < h2; j++)
			{
				for (size_t k = 0; k < w2; k++)
				{
					input_data2[i * h2 * w2 + j * w2 + k] = blobImg.at<Vec3f>(j, k)[i];
				}
			}
		}
		inferRequest2.infer();
		outputTensor2 = inferRequest2.get_output_tensor(0);
		output2 = outputTensor2.data<float>();
		int idx = 0;
		float max_value = output2[0];
		for (int i = 1; i < 5; i++) {
			if (output2[i] > max_value) {
				idx = i;
				max_value = output2[i];
			}
		}
		one_example.push_back("面部");
		one_example.push_back(emos[idx]);
		one_example.push_back(QString::number(int(max_value * 10000) / 100.0) + "%");
		ret.emplace_back(one_example);
	}
	return ret;
}
vector<vector<QString>> FaceEmo::processThis(Mat& src) {
	vector<Mat> mats = process_one(src);
	vector<vector<QString>> ret = process_two(mats);
	return ret;
}
void FaceEmo::setConfidence(double cf) {
	this->confidence = cf;
}
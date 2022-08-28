#include "stdafx.h"
#include "SuperResolution.h"
SuperResolution::SuperResolution() {
	string model_bin = QDir::currentPath().toStdString() + "\\models\\single-image-super-resolution-1033.bin",
		model_xml = QDir::currentPath().toStdString() + "\\models\\single-image-super-resolution-1033.xml";

	Core ie;
	shared_ptr<Model> p_model = ie.read_model(model_xml, model_bin);
	CompiledModel compiledModel = ie.compile_model(p_model, "CPU");
	inferRequest = compiledModel.create_infer_request();

	inputTensor = inferRequest.get_input_tensor(0);
	inputTensor2 = inferRequest.get_input_tensor(1);
	inputShape = inputTensor.get_shape();
	inputShape2 = inputTensor2.get_shape();
	c = inputShape[1];
	h = inputShape[2], h2 = inputShape2[2];
	w = inputShape[3], w2 = inputShape2[3];
	
	outputShape = inferRequest.get_output_tensor(0).get_shape();
}
Mat SuperResolution::processThis(Mat& src) {
	//获取输入数据
	Mat blobImg, blobImg2;

	resize(src, blobImg, Size(w, h));
	resize(src, blobImg2, Size(w2, h2));

	blobImg.convertTo(blobImg, CV_32F);
	blobImg2.convertTo(blobImg2, CV_32F);

	input_data = inputTensor.data<float>();
	input_data2 = inputTensor2.data<float>();

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
	for (size_t i = 0; i < c; i++)
	{
		for (size_t j = 0; j < h2; j++)
		{
			for (size_t k = 0; k < w2; k++)
			{
				input_data2[i * h2 * w2 + j * w2 + k] = blobImg2.at<Vec3f>(j, k)[i];
			}
		}
	}

	inferRequest.infer();

	outputTensor = inferRequest.get_output_tensor(0);

	output = outputTensor.data<float>();

	Mat ret = Mat::zeros(Size(w2, h2), CV_32FC3);
	for (size_t i = 0; i < c; i++)
	{
		for (size_t j = 0; j < h2; j++)
		{
			for (size_t k = 0; k < w2; k++)
			{
				ret.at<Vec3f>(j, k)[i] = output[i * h2 * w2 + j * w2 + k];
			}
		}
	}
	return ret;
}
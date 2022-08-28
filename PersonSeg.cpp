#include "stdafx.h"
#include "PersonSeg.h"

PersonSeg::PersonSeg() {
	string xml_path = QDir::currentPath().toStdString() + "\\models\\instance-segmentation-person-0007.xml";
	string bin_path = QDir::currentPath().toStdString() + "\\models\\instance-segmentation-person-0007.bin";

	Core ie;
	shared_ptr<Model> p_model = ie.read_model(xml_path, bin_path);
	CompiledModel compiledModel = ie.compile_model(p_model, "CPU");
	inferRequest = compiledModel.create_infer_request();
	inferRequest.infer();

	inputTensor = inferRequest.get_input_tensor();
	inputShape = inputTensor.get_shape();
	c = inputShape[1];
	h = inputShape[2];
	w = inputShape[3];
	outputShape = inferRequest.get_output_tensor(1).get_shape();
	outputShape2 = inferRequest.get_output_tensor(0).get_shape();
	outputShape3 = inferRequest.get_output_tensor(2).get_shape();
	mask_h = outputShape3[1], mask_w = outputShape3[2];
}
Mat PersonSeg::processThis(Mat& src, Mat& background) {
	resize(background, background, src.size());
	Mat blobImg, ret;
	resize(src, blobImg, Size(w, h));

	float width_ratio = src.cols / static_cast<float>(w), height_ratio = src.rows / static_cast<float>(h);

	blobImg.convertTo(blobImg, CV_32F);

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
	outputTensor = inferRequest.get_output_tensor(1);
	labels = outputTensor.data<int64>();
	outputTensor2 = inferRequest.get_output_tensor(0);
	output2 = outputTensor2.data<float>();
	outputTensor3 = inferRequest.get_output_tensor(2);
	raw_mask = outputTensor3.data<float>();

	for (int i = 0; i < 10; i++) {
		if (output2[i * 5 + 4] >= confidence) {
			Rect box = Rect(Point2i(output2[i * 5] * width_ratio, output2[i * 5 + 1] * height_ratio),
				Point2i(output2[i * 5 + 2] * width_ratio, output2[i * 5 + 3] * height_ratio));
			rectangle(src, box, Scalar(96, 144, 190), 2);
			int label = labels[i];
			float* mask_arr = raw_mask + mask_h * mask_w * i + mask_h * mask_w * label;
			Mat mask_mat(mask_h, mask_w, CV_32FC1, mask_arr);
			cv::resize(mask_mat, mask_mat, box.size());

			Mat roi_img = src(box), roi_img2 = background(box);

			roi_img.copyTo(roi_img2, mask_mat >= confidence);
		}
		else break;
	}
	return background;
}
void PersonSeg::setConfidence(double cf) {
	this->confidence = cf;
}
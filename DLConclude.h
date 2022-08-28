#pragma once
#pragma execution_character_set("utf-8")

#include <QWidget>
#include "ui_DLConclude.h"
#include "PersonDetection.h"
#include <opencv2/opencv.hpp>
#include "Tools.h"
#include "SuperResolution.h"
#include "FaceEmo.h"
#include "PersonSeg.h"

using namespace cv;
using namespace std;

class DLConclude : public QWidget
{
	Q_OBJECT

public:
	DLConclude(QWidget *parent = nullptr);
	~DLConclude();
	void closeEvent(QCloseEvent* e);
	void upload_and_infer();
	void labelShowMat(Mat& img);
	void scalePixmap(QPixmap& pixmap);
	void re_infer();
	void img_save();
	void camera_infer();
	void updateTable(vector<vector<QString>>& ret);
	void super_infer();
	void super_open();
	void widthChanged();
	void heightChanged();
	void seg_camera();
	void add_background();
	

private:
	Ui::DLConcludeClass ui;
	PersonDetection personDetection;
	SuperResolution superResolution;
	FaceEmo faceEmo;
	PersonSeg personSeg;
	Mat raw_img, cur_img, background;
	VideoCapture cap;
	bool width_change = false, height_change = false;

signals:
	void hasClosed();
};

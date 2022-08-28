#include "stdafx.h"
#include "DLConclude.h"

DLConclude::DLConclude(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	ui.info_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
	ui.info_table->setSelectionBehavior(QAbstractItemView::SelectRows);
	ui.info_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

	ui.info_table_4->setEditTriggers(QAbstractItemView::NoEditTriggers);
	ui.info_table_4->setSelectionBehavior(QAbstractItemView::SelectRows);
	ui.info_table_4->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

	connect(ui.confidence,&QDoubleSpinBox::textChanged, [&]() {
		personDetection.setConfidence(ui.confidence->value());
		if (!raw_img.empty()) re_infer();
		});
	//槽类需要继承QObject才可以connect连接信号和槽

	connect(ui.upload_img, &QPushButton::clicked, [&]() {
		upload_and_infer();
		});

	connect(ui.save_img, &QPushButton::clicked, [&]() {
		img_save();
		});

	connect(ui.open_camera, &QPushButton::clicked, [&]() {
		camera_infer();
		});

	connect(ui.upload_img_2, &QPushButton::clicked, [&]() {
		super_open();
		});
	connect(ui.resize_op, &QPushButton::clicked, [&]() {
		super_infer();
		});
	connect(ui.width, &QSpinBox::textChanged, [&]() {
		widthChanged();
		});
	connect(ui.height, &QSpinBox::textChanged, [&]() {
		heightChanged();
		});

	connect(ui.keep_radio, &QCheckBox::stateChanged, [&]() {
		widthChanged();
	});

	connect(ui.save_img_2, &QPushButton::clicked, [&]() {
		img_save();
	});

	connect(ui.upload_img_3, &QPushButton::clicked, [&]() {
		upload_and_infer();
	});

	connect(ui.tabWidget, &QTabWidget::currentChanged, [&]() {
		if (cap.isOpened()) {
			cap.release();
			ui.open_camera->setText("打开摄像头");
			ui.open_camera_2->setText("打开摄像头"); 
			ui.open_camera_3->setText("打开摄像头");
		} 
	});

	connect(ui.save_img_3, &QPushButton::clicked, [&]() {
		img_save();
		});

	connect(ui.open_camera_2, &QPushButton::clicked, [&]() {
		camera_infer();
		});

	connect(ui.confidence_4, &QDoubleSpinBox::textChanged, [&]() {
		faceEmo.setConfidence(ui.confidence_4->value());
		if (!raw_img.empty()) re_infer();
	});

	connect(ui.open_camera_3, &QPushButton::clicked, [&]() {
		seg_camera();
		});

	connect(ui.add_background, &QPushButton::clicked, [&]() {
		add_background();
		});
	connect(ui.clear_background, &QPushButton::clicked, [&]() {
		if (!background.empty()) {
			background.release();
			background = Mat::zeros(Size(50, 50), CV_8UC3);
		} 
		});

}
void DLConclude::upload_and_infer() {
	QString img_path = QFileDialog::getOpenFileName(this, "选择图片", imgs_path, "*.jpg *.png *.jpeg");
	if (img_path.endsWith("png") || img_path.endsWith("jpg") || img_path.endsWith("jpeg")) {
		if (cap.isOpened()) {
			cap.release();
			if (ui.tabWidget->currentIndex() == 0) {
				ui.open_camera->setText("打开摄像头");
			}
			else if(ui.tabWidget->currentIndex() == 2){
				ui.open_camera_2->setText("打开摄像头");
			}	
		}
		QPixmap pixmap = QPixmap(img_path);
		Mat img = imread(img_path.toStdString());
		raw_img = img.clone();
		cur_img = img;
		
		vector<vector<QString>> ret;
		if (ui.tabWidget->currentIndex() == 0) {
			ret = personDetection.processThis(img);
		}
		else if (ui.tabWidget->currentIndex() == 2) {
			ret = faceEmo.processThis(img);
		}
		
		labelShowMat(img);
		updateTable(ret);
		cur_img = img;
	}
}
void DLConclude::re_infer() {
	Mat img = raw_img.clone();
	vector<vector<QString>> ret;
	if (ui.tabWidget->currentIndex() == 0) {
		ret = personDetection.processThis(img);
	}
	else if (ui.tabWidget->currentIndex() == 2) {
		ret = faceEmo.processThis(img);
	}
	labelShowMat(img);
	updateTable(ret);
	cur_img = img;
}
void DLConclude::camera_infer() {
	QPushButton* btn;
	if (ui.tabWidget->currentIndex() == 0) {
		btn = ui.open_camera;
	}
	else if (ui.tabWidget->currentIndex() == 2) {
		btn = ui.open_camera_2;
	}

	if (cap.isOpened()) {
		cap.release();
		btn->setText("打开摄像头");
	}
	else {
		cap = VideoCapture(0);
		if (!cap.isOpened()) {
			QMessageBox::warning(this, "提示", "未检测出您的摄像头！");
			return;
		}
		else if(btn->text() == "打开摄像头") {
			btn->setText("关闭摄像头");
			while (cap.read(raw_img)) {
				flip(raw_img, raw_img, 1);
				vector<vector<QString>> ret;
				if (ui.tabWidget->currentIndex() == 0) {
					ret = personDetection.processThis(raw_img);
				}
				else if (ui.tabWidget->currentIndex() == 2) {
					ret = faceEmo.processThis(raw_img);
				}
				labelShowMat(raw_img);
				updateTable(ret);
				cur_img = raw_img;
				waitKey(1);
			}
		}
	}
}
void DLConclude::img_save() {
	if (cur_img.empty()) {
		QMessageBox::warning(this, "提示", "当前图像为空！");
		return;
	}
	QString save_path = QFileDialog::getSaveFileName(this, "保存图片", imgs_path, "*.jpg *.png *.jpeg");
	if (save_path.isEmpty()) return;
	imwrite(save_path.toStdString(), cur_img);
	QMessageBox::information(this, "提示", "保存成功！");
}
void DLConclude::super_open() {
	QString img_path = QFileDialog::getOpenFileName(this, "选择图片", imgs_path, "*.jpg *.png *.jpeg");
	if (img_path.endsWith("png") || img_path.endsWith("jpg") || img_path.endsWith("jpeg")) {
		if (cap.isOpened()) {
			cap.release();
			ui.open_camera->setText("打开摄像头");
		}
		QPixmap pixmap = QPixmap(img_path);
		Mat img = imread(img_path.toStdString());
		raw_img = img.clone();
		labelShowMat(img);
		cur_img = img;
		if (ui.keep_radio->isChecked()) {
			int width = img.cols, height = img.rows;
			double a = min(1920.0 / width, 1080.0 / height);
			width *= a, height *= a;
			ui.width->setValue(width);
			ui.height->setValue(height);
		}
	}
}
void DLConclude::super_infer() {
	if (raw_img.empty()) {
		QMessageBox::warning(this, "提示", "当前图像为空！");
		return;
	}
	Mat img = raw_img.clone();
	img = superResolution.processThis(img);
	//先获取参数
	int width = ui.width->value(), height = ui.height->value();
	cv::resize(img, img, Size(width, height));
	img.convertTo(img, CV_8UC3, 255);
	labelShowMat(img);
	cur_img = img;

}
void DLConclude::widthChanged() {
	if (height_change || !ui.keep_radio->isChecked() || raw_img.empty()) return;
	width_change = true;
	int ret = ui.width->value() * raw_img.rows / raw_img.cols;
	ui.height->setValue(ret);
	width_change = false;
}
void DLConclude::heightChanged() {
	if (width_change || !ui.keep_radio->isChecked() || raw_img.empty()) return;
	height_change = true;
	int ret = ui.height->value() * raw_img.cols / raw_img.rows;
	ui.width->setValue(ret);
	height_change = false;
}
void DLConclude::add_background() {
	QString img_path = QFileDialog::getOpenFileName(this, "选择图片", imgs_path, "*.jpg *.png *.jpeg");
	if (img_path.endsWith("png") || img_path.endsWith("jpg") || img_path.endsWith("jpeg")) {
		Mat img = imread(img_path.toStdString());
		background = img;
	}
}
void DLConclude::seg_camera() {
	if (ui.open_camera_3->text() == "关闭摄像头") {
		if (cap.isOpened()) cap.release();
		ui.open_camera_3->setText("打开摄像头");
		return;
	}
	if (cap.isOpened()) {
		cap.release();
	}
	cap = VideoCapture(0);
	if (!cap.isOpened()) {
		QMessageBox::warning(this, "提示", "未检测出您的摄像头！");
		return;
	}
	ui.open_camera_3->setText("关闭摄像头");
	Mat background_copy;
	if (background.empty()) {
		background = Mat::zeros(Size(100, 100), CV_8UC3);
	}
	background_copy = background.clone();
	while (cap.read(raw_img)) {
		flip(raw_img, raw_img, 1);
		Mat ret = personSeg.processThis(raw_img, background_copy);
		labelShowMat(ret);
		cur_img = ret;
		background_copy = background.clone();
		waitKey(1);
	}
}
void DLConclude::updateTable(vector<vector<QString>>& ret) {
	QTableWidget* table;
	if (ui.tabWidget->currentIndex() == 0) {
		table = ui.info_table;
	}
	else {
		table = ui.info_table_4;
	}
	table->setRowCount(ret.size());
	for (int i = 0; i < ret.size(); i++) {
		vector<QString>& items = ret[i];
		for (int j = 0; j < items.size(); j++) {
			QTableWidgetItem* item = new QTableWidgetItem(items[j]);
			table->setItem(i, j, item);
		}
		/*QTableWidgetItem* item = new QTableWidgetItem(ret[i][0]);
		ui.info_table->setItem(i, 0, item);
		item = new QTableWidgetItem(QString::number(stoi(ret[i][1].toStdString()) * stoi(ret[i][1].toStdString()) / static_cast<float>(ui.label->width())));
		ui.info_table->setItem(i, 1, item);
		item = new QTableWidgetItem(QString::number(stoi(ret[i][2].toStdString()) * stoi(ret[i][2].toStdString()) / static_cast<float>(ui.label->height())));
		ui.info_table->setItem(i, 2, item);*/	
	}
}
void DLConclude::labelShowMat(Mat& img) {
	QPixmap pixmap = matToQpixmap(img);
	scalePixmap(pixmap);
	ui.show_img->setPixmap(pixmap);
}
void DLConclude::scalePixmap(QPixmap& pixmap) {
	QSize size = QSize(ui.show_img->width(), ui.show_img->height());
	pixmap = pixmap.scaled(size, Qt::KeepAspectRatio);
}
void DLConclude::closeEvent(QCloseEvent* e) {
	if (cap.isOpened()) {
		cap.release();
		ui.open_camera->setText("打开摄像头");
		ui.open_camera_2->setText("打开摄像头");
		ui.open_camera_3->setText("打开摄像头");
	}
	emit hasClosed();
	
}
DLConclude::~DLConclude()
{}

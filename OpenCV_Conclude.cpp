#include "stdafx.h"
#include "OpenCV_Conclude.h"

OpenCV_Conclude::OpenCV_Conclude(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    connect(ui.pushButton, &QPushButton::clicked, this, &OpenCV_Conclude::open_img);
    ui.pushButton->setToolTip("imread");
    connect(ui.save_img, &QPushButton::clicked, this, &OpenCV_Conclude::save_img);
    ui.save_img->setToolTip("imwrite");
    connect(ui.toGray, &QPushButton::clicked, this, &OpenCV_Conclude::toGray);
    ui.toGray->setToolTip("cvtColor");
    ui.modifiedTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui.modifiedTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    connect(ui.modifiedTable, &QTableWidget::cellClicked, this, &OpenCV_Conclude::showCurImage);
    connect(ui.otsu_binary, &QPushButton::clicked, this, &OpenCV_Conclude::otsuBinary);
    ui.otsu_binary->setToolTip("threshold");
    connect(ui.adaptive_binary, &QPushButton::clicked, this, &OpenCV_Conclude::adaptiveBinary);
    ui.adaptive_binary->setToolTip("adaptiveThreshold");
    connect(ui.thresh_binary, &QPushButton::clicked, this, &OpenCV_Conclude::threshBinary);
    ui.thresh_binary->setToolTip("threshold");
    connect(ui.mean_btn, &QPushButton::clicked, this, &OpenCV_Conclude::blurOperate);
    ui.mean_btn->setToolTip("blur");
    connect(ui.median_btn, &QPushButton::clicked, this, &OpenCV_Conclude::medianOperate);
    ui.median_btn->setToolTip("median");
    connect(ui.gaussian_btn, &QPushButton::clicked, this, &OpenCV_Conclude::gaussianOperate);
    ui.gaussian_btn->setToolTip("Gaussian");

    connect(ui.dilate_btn, &QPushButton::clicked, [&]() {
        this->morphOperate("膨胀");
    });
    ui.dilate_btn->setToolTip("DILATE");

    connect(ui.erode_btn, &QPushButton::clicked, [&]() {
        this->morphOperate("腐蚀");
        });
    ui.erode_btn->setToolTip("ERODE");

    connect(ui.open_btn, &QPushButton::clicked, [&]() {
        this->morphOperate("开操作");
        });
    ui.open_btn->setToolTip("OPEN");

    connect(ui.close_btn, &QPushButton::clicked, [&]() {
        this->morphOperate("闭操作");
        });
    ui.close_btn->setToolTip("CLOSE");

    connect(ui.binary_inv, &QPushButton::clicked, [&]() {
        this->binaryInv();
        });
    ui.binary_inv->setToolTip("bitwise_not");
    connect(ui.tophat_btn, &QPushButton::clicked, [&]() {
        this->morphOperate("顶帽");
        });
    ui.tophat_btn->setToolTip("TOPHAT");

    connect(ui.blackhat_btn, &QPushButton::clicked, [&]() {
        this->morphOperate("黑帽");
        });
    ui.blackhat_btn->setToolTip("BLACKHAT");

    connect(ui.gradient_btn, &QPushButton::clicked, [&]() {
        this->morphOperate("梯度");
        });

    ui.gradient_btn->setToolTip("GRATIENT");
    connect(ui.sobel_btn, &QPushButton::clicked, [&]() {
        this->sobel_scharr("sobel", "x_y");
        });
    ui.sobel_btn->setToolTip("Sobel");

    connect(ui.sobel_x_btn, &QPushButton::clicked, [&]() {
        this->sobel_scharr("sobel", "x");
        });
    ui.sobel_x_btn->setToolTip("Sobel");

    connect(ui.sobel_y_btn, &QPushButton::clicked, [&]() {
        this->sobel_scharr("sobel", "y");
        });
    ui.sobel_y_btn->setToolTip("Sobel");

    connect(ui.laplacian_btn, &QPushButton::clicked, [&]() {
        this->laplacian();
        });
    ui.laplacian_btn->setToolTip("Laplacian");

    connect(ui.scharr_btn, &QPushButton::clicked, [&]() {
        this->sobel_scharr("scharr", "x_y");
        });
    ui.scharr_btn->setToolTip("Scharr");

    connect(ui.scharr_x_btn, &QPushButton::clicked, [&]() {
        this->sobel_scharr("scharr", "x");
        });
    ui.scharr_x_btn->setToolTip("Scharr");

    connect(ui.scharr_y_btn, &QPushButton::clicked, [&]() {
        this->sobel_scharr("scharr", "y");
        });
    ui.scharr_y_btn->setToolTip("Scharr");

    connect(ui.canny_btn, &QPushButton::clicked, [&]() {
        this->canny();
        });
    ui.canny_btn->setToolTip("Canny");
    
}
void OpenCV_Conclude::open_img() {
    QString img_path = QFileDialog::getOpenFileName(this, "选择图片", imgs_path, "*.jpg *.png *.jpeg");
    if (img_path.endsWith("png") || img_path.endsWith("jpg") || img_path.endsWith("jpeg")) {
        QPixmap pixmap = QPixmap(img_path);

        imgs_container.clear();
        history.clear();
        history.push_back(QString("选择图片"));
        updateTable();
        Mat img = imread(img_path.toStdString());
        labelShowMat(img);
        cur_img = img;
        updateHistory();
    }
}
void OpenCV_Conclude::save_img() {
    if (cur_img.empty()) {
        QMessageBox::warning(this, "提示", "当前图像为空！");
        return;
    }
    QString save_path = QFileDialog::getSaveFileName(this, "保存图片", imgs_path, "*.jpg *.png *.jpeg");
    if (save_path.isEmpty()) return;
    imwrite(save_path.toStdString(), cur_img);
    QMessageBox::information(this, "提示", "保存成功！");
}
void OpenCV_Conclude::toGray() {
    if (imgs_container.size() == 0) return;
    Mat img = cur_img.clone();
    if(img.channels() != 1) cvtColor(img, img, COLOR_BGR2GRAY);
    isFinalImage();
    history.push_back(QString("灰度处理"));
    updateTable();
    labelShowMat(img);
    cur_img = img;
    updateHistory();
}
void OpenCV_Conclude::otsuBinary() {
    if (imgs_container.size() == 0) return;
    Mat img = cur_img.clone();
    if (img.channels() != 1) {
        QMessageBox::warning(this, "提示", "当前图像通道必须为1！");
        return;
    }
    isFinalImage();
    int thresh = threshold(img, img, 0, 255, THRESH_BINARY | THRESH_OTSU);
    history.push_back(QString("全局自适应二值化(阈值: ") + QString::number(thresh) + QString(")"));
    updateTable();
    labelShowMat(img);
    cur_img = img;
    updateHistory();
}
void OpenCV_Conclude::adaptiveBinary() {
    if (imgs_container.size() == 0) return;
    Mat img = cur_img.clone();
    if (img.channels() != 1) {
        QMessageBox::warning(this, "提示", "当前图像通道必须为1！");
        return;
    }
    //获取参数
    auto method = ui.comboBox->currentIndex() == 0 ? ADAPTIVE_THRESH_MEAN_C : ADAPTIVE_THRESH_GAUSSIAN_C;
    QString desc = ui.comboBox->currentIndex() == 0 ? "均值" : "高斯";
    if ((ui.spinBox->value() & 1) == 0) {
        QMessageBox::warning(this, "提示", "只接受奇数大小的Kernel_Size！");
        return;
    }
    adaptiveThreshold(img, img, 255, method, THRESH_BINARY, ui.spinBox->value(), 0);
    isFinalImage();
    history.push_back(QString("局部自适应二值化(") + desc + QString(",") + QString::number(ui.spinBox->value()) + QString(")"));
    updateTable();
    labelShowMat(img);
    cur_img = img;
    updateHistory();
}
void OpenCV_Conclude::threshBinary() {
    if (imgs_container.size() == 0) return;
    Mat img = cur_img.clone();
    if (img.channels() != 1) {
        QMessageBox::warning(this, "提示", "当前图像通道必须为1！");
        return;
    }
    //获取参数
    QString thresh = ui.thresh->text();
    if (thresh == "") {
        QMessageBox::warning(this, "提示", "阈值不能为空！");
        return;
    }
    int th = stoi(thresh.toStdString());
    threshold(img, img, th, 255, THRESH_BINARY);
    isFinalImage();
    history.push_back(QString("阈值二值化(阈值:") + thresh + QString(")"));
    updateTable();
    labelShowMat(img);
    cur_img = img;
    updateHistory();
}
void OpenCV_Conclude::binaryInv() {
    if (imgs_container.size() == 0) return;
    Mat img = cur_img.clone();
    if (img.channels() != 1) {
        QMessageBox::warning(this, "提示", "当前图像通道必须为1！");
        return;
    }
    isFinalImage();
    threshold(img, img, 0, 255, THRESH_BINARY | THRESH_OTSU);
    bitwise_not(img, img);
    history.push_back(QString("黑白背景变化"));
    updateTable();
    labelShowMat(img);
    cur_img = img;
    updateHistory();
    

}
void OpenCV_Conclude::blurOperate() {
    if (imgs_container.size() == 0) return;
    Mat img = cur_img.clone();
    isFinalImage();
    Size size = Size(ui.mean_x->value(), ui.mean_y->value());
    blur(img, img, size);
    history.push_back(QString("均值模糊(") + QString::number(ui.mean_x->value()) + "X" + QString::number(ui.mean_y->value()) + ")");

    cur_img = img;
    updateTable();
    labelShowMat(img);
    updateHistory();
    
}
void OpenCV_Conclude::medianOperate() {
    if (imgs_container.size() == 0) return;
    Mat img = cur_img.clone();
    isFinalImage();
    int ksize = ui.median_kernel_size->value();
    if (ksize & 1 == 0) {
        QMessageBox::warning(this, "提示", "中值滤波的核大小必须为奇数！");
        return;
    }
    medianBlur(img, img, ksize);
    history.push_back(QString("中值模糊(") + QString::number(ui.median_kernel_size->value()) + "X" + QString::number(ui.median_kernel_size->value()) + ")");
    cur_img = img;
    updateTable();
    labelShowMat(img);
    updateHistory();
}
void OpenCV_Conclude::gaussianOperate() {
    if (imgs_container.size() == 0) return;
    Mat img = cur_img.clone();
    isFinalImage();
    Size size = Size(ui.gaussian_x->value(), ui.gaussian_y->value());
    if ((size.width & 1 == 0) || (size.height & 1 == 0)) {
        QMessageBox::warning(this, "提示", "高斯卷积核必须为奇数！");
        return;
    }
    GaussianBlur(img, img, size, 0, 0);
    history.push_back(QString("高斯模糊(") + QString::number(ui.gaussian_x->value()) + "X" + QString::number(ui.gaussian_y->value()) + ")");

    cur_img = img;
    updateTable();
    labelShowMat(img);
    updateHistory();
}
void OpenCV_Conclude::morphOperate(string op) {
    if (imgs_container.size() == 0) return;
    Mat img = cur_img.clone();
    isFinalImage();
    //获取参数
    int kernel_size_x = ui.morph_kernel_x->value(), kernel_size_y = ui.morph_kernel_y->value();
    Mat kernel = getStructuringElement(MORPH_RECT, Size(kernel_size_x, kernel_size_y));
    //图像操作
    int morph_op = -1;
    if (op == "腐蚀") morph_op = MORPH_ERODE;
    else if (op == "膨胀") morph_op = MORPH_DILATE;
    else if (op == "开操作") morph_op = MORPH_OPEN;
    else if (op == "闭操作") morph_op = MORPH_CLOSE;
    else if (op == "顶帽") morph_op = MORPH_TOPHAT;
    else if (op == "黑帽") morph_op = MORPH_BLACKHAT;
    else if (op == "梯度") morph_op = MORPH_GRADIENT;
    morphologyEx(img, img, morph_op, kernel);
    //记录历史
    history.push_back(QString(op.c_str()) + QString("(") + QString::number(kernel_size_x) + "," + QString::number(kernel_size_y) + QString(")"));
    updateTable();
    labelShowMat(img);
    cur_img = img;
    updateHistory();
}
void OpenCV_Conclude::sobel_scharr(string op, string direction) {
    if (imgs_container.size() == 0) return;
    Mat img = cur_img.clone();
    if (img.channels() != 1) cvtColor(img, img, COLOR_BGR2GRAY);
    isFinalImage();
    int x, y;
    if (direction == "x") x = 1, y = 0;
    else if (direction == "y") x = 0, y = 1;
    else x = 1, y = 1;
    int ksize = ui.radioButton->isChecked() ? 3 : 5;
    if (op == "sobel") {
        if (x && y) {
            Mat tmp, tmp2;
            Sobel(img, tmp, -1, 0, 1);
            Sobel(img, tmp2, -1, 1, 0);
            bitwise_or(tmp, tmp2, img);
        }
        else {
            Sobel(img, img, -1, x, y, ksize);
        }
    } 
    else {
        if (x && y) {
            Mat tmp, tmp2;
            Scharr(img, tmp, -1, 0, 1);
            Scharr(img, tmp2, -1, 1, 0);
            bitwise_or(tmp, tmp2, img);
        }
        else {
            Scharr(img, img, -1, x, y);
        }
    }
    
    //记录历史
    history.push_back(op.c_str() + QString(" : ") + QString(direction.c_str()));

    updateTable();
    labelShowMat(img);
    cur_img = img;
    updateHistory();
}
void OpenCV_Conclude::laplacian() {
    if (imgs_container.size() == 0) return;
    Mat img = cur_img.clone();
    if (img.channels() != 1) cvtColor(img, img, COLOR_BGR2GRAY);
    isFinalImage();
    int ksize = ui.radioButton->isChecked() ? 3 : 5;
    cv::Laplacian(img, img, -1, ksize);
    //记录历史
    history.push_back("拉普拉斯");
    updateTable();
    labelShowMat(img);
    cur_img = img;
    updateHistory();
}
void OpenCV_Conclude::canny() {
    if (imgs_container.size() == 0) return;
    Mat img = cur_img.clone();
    if (img.channels() != 1) cvtColor(img, img, COLOR_BGR2GRAY);
    isFinalImage();
    //获取参数
    int ksize = ui.radioButton->isChecked() ? 3 : 5;
    double min_val = 100, max_val = 150;
    bool flag = false;
    if (ui.comboBox_2->currentIndex() == 0) {
        min_val = 150, max_val = 200;
    }
    else if (ui.comboBox_2->currentIndex() == 2) {
        min_val = 25, max_val = 75;
        flag = true;
    }
    Canny(img, img, min_val, max_val, ksize, flag);
    //记录历史
    history.push_back("canny");
    updateTable();
    labelShowMat(img);
    cur_img = img;
    updateHistory();
}
void OpenCV_Conclude::updateHistory() {
    imgs_container.emplace_back(cur_img);
    cur_idx = imgs_container.size() - 1;
}
void OpenCV_Conclude::showCurImage(int row, int col) {
    if (cur_idx == row) return;
    cur_img = imgs_container[row];
    cur_idx = row;
    labelShowMat(cur_img);
}
void OpenCV_Conclude::labelShowMat(Mat& img) {
    QPixmap pixmap = matToQpixmap(img);
    scalePixmap(pixmap);
    ui.label->setPixmap(pixmap);
}
void OpenCV_Conclude::isFinalImage() {
    if (cur_idx + 1 != imgs_container.size()) {
        ui.label->setText("1");
        imgs_container.erase(imgs_container.begin() + cur_idx + 1, imgs_container.end());
        history.erase(history.begin() + cur_idx + 1, history.end());
    }
}
void OpenCV_Conclude::scalePixmap(QPixmap& pixmap) {
    QSize size = QSize(ui.label->width(), ui.label->height());
    pixmap = pixmap.scaled(size, Qt::KeepAspectRatio);
}
void OpenCV_Conclude::updateTable() {
    ui.modifiedTable->setRowCount(history.size());
    if (history.size() == 0) return;
    QTableWidgetItem* item = new QTableWidgetItem(history.back());
    ui.modifiedTable->setItem(history.size() - 1, 0, item);
    ui.modifiedTable->selectRow(history.size() - 1);
    
}
void OpenCV_Conclude::closeEvent(QCloseEvent* e) {
    if (imgs_container.size() > 1) {
        imgs_container.erase(imgs_container.begin() + 1, imgs_container.end());
        history.erase(history.begin() + 1, history.end());
        updateTable();
        updateHistory();
    }
    emit hasClosed();
}
OpenCV_Conclude::~OpenCV_Conclude()
{

}
#pragma once
#pragma execution_character_set("utf-8")  

#include <QtWidgets/QMainWindow>
#include "ui_OpenCV_Conclude.h"
#include <opencv2/opencv.hpp>
#include "Tools.h"

using namespace cv;
using namespace std;

class OpenCV_Conclude : public QMainWindow
{
    Q_OBJECT
public slots:
    void open_img();
    void save_img();
    void toGray();
    void showCurImage(int row, int col);
    void otsuBinary();
    void adaptiveBinary();
    void threshBinary();
    void blurOperate();
    void medianOperate();
    void gaussianOperate();
    void morphOperate(string op);
    void binaryInv();
    void sobel_scharr(string op, string direction);
    void laplacian();
    void canny();

public:
    OpenCV_Conclude(QWidget *parent = nullptr);
    ~OpenCV_Conclude();
    void scalePixmap(QPixmap& pixmap);
    void updateTable();
    void labelShowMat(Mat& img);
    void updateHistory();
    void closeEvent(QCloseEvent* e);

private:
    void isFinalImage();
    Ui::OpenCV_ConcludeClass ui;
    size_t cur_idx = -1;
    Mat cur_img;
    vector<Mat> imgs_container;
    QVector<QString> history;
signals:
    void hasClosed();
};

#pragma once
#include<opencv2/opencv.hpp>
using namespace cv;
static QString imgs_path = "D:\\";
inline Mat bitmapToMat(QPixmap& pixmap) {
    QImage q_img = pixmap.toImage();
    Mat img;
    switch (q_img.format())
    {
    case QImage::Format_ARGB32:
    case QImage::Format_RGB32:
    case QImage::Format_ARGB32_Premultiplied:
        img = cv::Mat(q_img.height(), q_img.width(), CV_8UC4, (void*)q_img.constBits(), q_img.bytesPerLine());
        break;
    case QImage::Format_RGB888:
        img = cv::Mat(q_img.height(), q_img.width(), CV_8UC3, (void*)q_img.constBits(), q_img.bytesPerLine());
        cv::cvtColor(img, img, COLOR_RGB2BGR);
        break;
    case QImage::Format_Indexed8:
        img = cv::Mat(q_img.height(), q_img.width(), CV_8UC1, (void*)q_img.constBits(), q_img.bytesPerLine());
        break;
    }
    return img;
}
inline QPixmap matToQpixmap(const Mat& mat) {
    switch (mat.type())
    {
    case CV_8UC4:
    {
        QImage image(mat.data, mat.cols, mat.rows, mat.step, QImage::Format_RGB32);
        return QPixmap::fromImage(image);
    }
    case CV_8UC3:
    {
        QImage image(mat.data, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
        return QPixmap::fromImage(image.rgbSwapped());
    }

    case CV_8UC1:
    {
        static QVector<QRgb>  sColorTable;
        if (sColorTable.isEmpty())
        {
            for (int i = 0; i < 256; ++i)
                sColorTable.push_back(qRgb(i, i, i));
        }
        QImage image(mat.data, mat.cols, mat.rows, mat.step, QImage::Format_Indexed8);
        image.setColorTable(sColorTable);
        return QPixmap::fromImage(image);
    }

    default:
        qDebug("Image format is not supported: depth=%d and %d channels\n", mat.depth(), mat.channels());
        break;
    }
    return QPixmap::fromImage(QImage());
}
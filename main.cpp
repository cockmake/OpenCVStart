#include "stdafx.h"
#include "OpenCV_Conclude.h"
#include <QtWidgets/QApplication>
#include"Home.h"
#include"DLConclude.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QIcon icon = QIcon(":/imgs/icon.png");
    a.setWindowIcon(icon);

    Home h;
    h.show();

    OpenCV_Conclude oc;
    QObject::connect(&h, &Home::open_opencv_conclude, [&]() {
        oc.show();
        h.hide();
        });
    QObject::connect(&oc, &OpenCV_Conclude::hasClosed, [&]() {
        h.show();
        });

    DLConclude dc;
    QObject::connect(&h, &Home::open_dl_conclude, [&]() {
        dc.show();
        h.hide();
        });
    QObject::connect(&dc, &DLConclude::hasClosed, [&]() {
        h.show();
        });

    return a.exec();
}

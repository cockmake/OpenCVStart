#include "stdafx.h"
#include "Home.h"

Home::Home(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	QIcon ic(":/imgs/oc.png"), ic2(":/imgs/dc.png"), ic3(":/imgs/expect.png");
	connect(ui.pushButton, &QPushButton::clicked, [&]() {
		emit open_opencv_conclude();
		});
	ui.pushButton->setIcon(ic);
	connect(ui.pushButton_2, &QPushButton::clicked, [&]() {
		emit open_dl_conclude();
		});
	ui.pushButton_2->setIcon(ic2);
	connect(ui.pushButton_3, &QPushButton::clicked, [&]() {
		QMessageBox::warning(this, "提示", "嘿嘿嘿(^v^)，能力有限，待我修炼数日！");
		});
	ui.pushButton_3->setIcon(ic3);
	//hh
}

Home::~Home()
{}

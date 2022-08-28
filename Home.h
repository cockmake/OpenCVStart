#pragma once
#pragma execution_character_set("utf-8")
#include <QWidget>
#include "ui_Home.h"

class Home : public QWidget
{
	Q_OBJECT

public:
	Home(QWidget *parent = nullptr);
	~Home();

signals:
	void open_opencv_conclude();
signals:
	void open_dl_conclude();

private:
	Ui::HomeClass ui;
};

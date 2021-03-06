﻿#pragma once
#include <QWidget>
#include <QImage>
#include <QPainter>
#include <QWheelEvent>
#include <QMenu>
#include <QCursor>
#include <QFileDialog>
#include <QSplitter>
#include <QVariant>
#include <QMessageBox>
#include "selectrect.h"

enum MouseDown{Left,Mid,Right,No};
class ImageWidget :
	public QWidget
{
	Q_OBJECT
public:
	ImageWidget(QWidget *parent);
	~ImageWidget();

    void setImage(QImage* img)
	{
        mp_img = img;
		isImageLoad = true;
		update();
	}

	void clear()
	{
		if(isImageLoad)
		{
			isImageLoad = false;
			//delete mp_img;
            mp_img = NULL;
            update();
		}

	}
	
	void iniActions();
	
	void wheelEvent(QWheelEvent *e);
	void mouseMoveEvent(QMouseEvent * e);
	void mousePressEvent(QMouseEvent * e);
    void mouseReleaseEvent(QMouseEvent *e);
    void paintEvent(QPaintEvent *e);
    void contextMenuEvent(QContextMenuEvent *e);
//	void mouseDoubleClickEvent(QMouseEvent *e);


public slots:
	void resetPos();
	void zoomout();
	void zoomin();
	void translate(int x,int y);
	void save();
    void select();
	void con_dia();
    void receiver_rect(QVariant rect_data);
signals:
	void sig_condional_dia(int x,int y);   
    void is_select_press();

private slots:


private:
    QImage *mp_img = new QImage;

	float scalex;
	float scaley;
	int xtranslate;
	int ytranslate;
    int last_x_pos = 0;
    int last_y_pos = 0;

	int mousePosX;
	int mousePosY;
	bool isImageLoad;

	MouseDown mouse;
	
	QAction* mActionResetPos;
	QAction* mActionSave;
    QAction* mActionSelect;

	QAction* conditionDialation;
	QMenu* mMenu;
	bool maxmum;

	//QPoint mospos;
public:
	int widgetid;
};

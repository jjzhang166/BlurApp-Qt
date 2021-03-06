﻿#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QWidget>
#include <QPaintEvent>
#include <QMessageBox>
#include <QString>
#include <QTextCodec>
#include <QFileDialog>
#include <QDebug>
#include <iostream>
#include <vector>
#include <algorithm>
#include <math.h>
#include <assert.h>
#include <opencv/cv.h>
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "ImageWidget.h"
#include "selectrect.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->slider_th1->setMinimum(1);
    ui->slider_th1->setMaximum(800);
    ui->slider_th1->setValue(1);
    ui->slider_th1->setSingleStep(1);

    ui->slider_th2->setMinimum(1);
    ui->slider_th2->setMaximum(800);
    ui->slider_th2->setValue(1);
    ui->slider_th2->setSingleStep(1);

    ui->slider_size->setMinimum(1);
    ui->slider_size->setMaximum(799);
    ui->slider_size->setValue(9);
    ui->slider_size->setSingleStep(2);

    ui->spinBox_th1->setMinimum(1);
    ui->spinBox_th1->setMaximum(800);
    ui->spinBox_th1->setValue(1);
    ui->spinBox_th1->setSingleStep(1);

    ui->spinBox_th2->setMinimum(1);
    ui->spinBox_th2->setMaximum(800);
    ui->spinBox_th2->setValue(1);
    ui->spinBox_th2->setSingleStep(1);

    ui->spinBox_size->setMinimum(1);
    ui->spinBox_size->setMaximum(799);
    ui->spinBox_size->setValue(9);
    ui->spinBox_size->setSingleStep(2);

    size = ui->slider_size->value();
    sigmaX = ui->slider_th1->value();
    sigmaY = ui->slider_th2->value();


    connect(ui->slider_th1,SIGNAL(valueChanged(int)),ui->spinBox_th1,SLOT(setValue(int)));
    connect(ui->slider_th2,SIGNAL(valueChanged(int)),ui->spinBox_th2,SLOT(setValue(int)));
    connect(ui->slider_size,SIGNAL(valueChanged(int)),ui->spinBox_size,SLOT(setValue(int)));

    connect(ui->spinBox_th1,SIGNAL(valueChanged(int)),ui->slider_th1,SLOT(setValue(int)));
    connect(ui->spinBox_th2,SIGNAL(valueChanged(int)),ui->slider_th2,SLOT(setValue(int)));
    connect(ui->spinBox_size,SIGNAL(valueChanged(int)),ui->slider_size,SLOT(setValue(int)));

    connect(ui->slider_th1,SIGNAL(valueChanged(int)),this,SLOT(get_blur_img_value()));
    connect(ui->slider_th2,SIGNAL(valueChanged(int)),this,SLOT(get_blur_img_value()));
    connect(ui->slider_size,SIGNAL(valueChanged(int)),this,SLOT(get_blur_img_value()));

    connect(ui->slider_th1,SIGNAL(sliderReleased()),this,SLOT(blur_img_process()));
    connect(ui->slider_th2,SIGNAL(sliderReleased()),this,SLOT(blur_img_process()));
    connect(ui->slider_size,SIGNAL(sliderReleased()),this,SLOT(blur_img_process()));

    connect(ui->spinBox_th1,SIGNAL(editingFinished()),this,SLOT(blur_img_process()));
    connect(ui->spinBox_th2,SIGNAL(editingFinished()),this,SLOT(blur_img_process()));
    connect(ui->spinBox_size,SIGNAL(editingFinished()),this,SLOT(blur_img_process()));

//    ui->m->hide();
//    ui->m->setAttribute(Qt::WA_DeleteOnClose);
    connect(ui->show_img,SIGNAL(is_select_press()),this,SLOT(rec_is_select_press()));

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_open_file_clicked()
{
//    QString filedir = QFileDialog::getOpenFileName(this, tr("Open File"),
//        "C:/Users/Aaron/Pictures/standord test/",
//        tr("Images (*.png *.xpm *.jpg *.tiff *.bmp)"));
//    ori_img = new QImage;
//    ori_img->load(filedir);
    QString filename = QFileDialog::getOpenFileName(this,tr("Open Image"),"",tr("Image File(*.bmp *.jpg *.jpeg *.png)"));
    QTextCodec *code = QTextCodec::codecForName("gb18030");
    std::string name = code->fromUnicode(filename).data();
    input_file = cv::imread(name);
//    if (filedir.isNull()||filedir == "")
    if(!input_file.data)
    {
        img_data = false;
        QMessageBox msgBox(QMessageBox::Critical,tr("错误"),tr("未载入图像！"));
        msgBox.exec();
    }
    else
    {

        img_data = true;

        blur_img_process();
    }
}



QImage MainWindow::Mat2QImage(cv::Mat &mat)
{
    //单通道
    if(mat.type() == CV_8UC1)
    {
        QImage image(mat.cols, mat.rows, QImage::Format_Indexed8);

        image.setColorCount(256);
        for(int i = 0; i < 256; i++)
        {
            image.setColor(i, qRgb(i, i, i));
        }
        uchar *pSrc = mat.data;
        for(int row = 0; row < mat.rows; row ++)
        {
            uchar *pDest = image.scanLine(row);
            memcpy(pDest, pSrc, mat.cols);
            pSrc += mat.step;
        }
        return image;
    }
    //3通道
    else if(mat.type() == CV_8UC3)
    {
        const uchar *pSrc = (const uchar*)mat.data;
        QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
        return image.rgbSwapped();
    }
    //4通道
    else if(mat.type() == CV_8UC4)
    {
        const uchar *pSrc = (const uchar*)mat.data;
        QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_ARGB32);
        return image.copy();
    }
    else
    {
        return QImage();
    }
}

cv::Mat MainWindow::QImage2Mat(QImage image)
{
    cv::Mat mat;
//    qDebug() << image.format();
    switch(image.format())
    {
    case QImage::Format_ARGB32:
    case QImage::Format_RGB32:
    case QImage::Format_ARGB32_Premultiplied:
        mat = cv::Mat(image.height(), image.width(), CV_8UC4, (void*)image.constBits(), image.bytesPerLine());
        break;
    case QImage::Format_RGB888:
        mat = cv::Mat(image.height(), image.width(), CV_8UC3, (void*)image.constBits(), image.bytesPerLine());
        cv::cvtColor(mat, mat, CV_BGR2RGB);
        break;
    case QImage::Format_Indexed8:
        mat = cv::Mat(image.height(), image.width(), CV_8UC1, (void*)image.constBits(), image.bytesPerLine());
        break;
    }
    return mat;
}

void MainWindow::auto_size(cv::Mat &src)
{
    int w;
    int h;
    if(src.cols > 640)
    {
        h = round(double(src.rows)/double(src.cols)*double(640.0));
        cv::resize(src,src,cv::Size(640,h),3);
    }
    if(src.rows > 480)
    {
        w = round(double(src.cols)/double(src.rows)*double(480.0));
        cv::resize(src,src,cv::Size(w,480),3);
    }
    if(src.cols < 640 || src.rows < 480)
    {
        if(src.cols < src.rows)
        {
            w = round(double(src.cols)/double(src.rows)*double(480.0));
            cv::resize(src,src,cv::Size(w,480),3);
        }
        else
        {
            h = round(double(src.rows)/double(src.cols)*double(640.0));
            cv::resize(src,src,cv::Size(640,h),3);
        }
    }
}

void MainWindow::show_open_img(cv::Mat &input_img)
{
    *send_img = Mat2QImage(input_img);
//    qDebug() << image.format();
//    image = image.convertToFormat(QImage::Format_RGB32);
//    qDebug() << image.format();
//    int p_image = int(&image);
    ui->show_img->setImage(send_img);
}

void MainWindow::get_blur_img_value()
{
    size = ui->slider_size->value();
    sigmaX = ui->slider_th1->value();
    sigmaY = ui->slider_th2->value();
//    if(sigmaX % 2 == 0)
//    {
//        sigmaX -= 1;
//        ui->slider_th1->setValue(sigmaX);
//    }
//    if (sigmaY % 2 == 0)
//     {
//        sigmaY -= 1;
//        ui->slider_th2->setValue(sigmaY);
//    }
    if (size % 2 == 0)
    {
        size -= 1;
        ui->slider_size->setValue(size);
    }
}

void MainWindow::blur_img_process()
{
    if(img_data)
    {
        cv::GaussianBlur(input_file,blur_img,cv::Size(size,size),sigmaX,sigmaY);
        show_open_img(blur_img);
    }
}


void MainWindow::rec_is_select_press()
{
    SelectRect* m = new SelectRect(ui->show_img);
////    qDebug() << ui->show_img->geometry();
    m->setGeometry(0,0,ui->show_img->geometry().width(),ui->show_img->geometry().height());
    m->show();
    connect(m,SIGNAL(send_data(QVariant)),ui->show_img,SLOT(receiver_rect(QVariant)));
}

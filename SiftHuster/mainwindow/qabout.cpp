﻿#include "qabout.h"

#include <QFrame>
#include <QFont>
#include <QPalette>
#include <QDebug>

#define SAFE_FREE(p) {if(p != NULL) {delete p; p = NULL;}}

QAbout::QAbout(QWidget *parent) : QWidget(parent)
{
    /* init ui */
    initUiComponent();
}

QAbout::~QAbout()
{
    SAFE_FREE(titleLabel);
    SAFE_FREE(authorLabel);
    SAFE_FREE(infoLabel);
    SAFE_FREE(infoTextEdit);
    SAFE_FREE(exitBtn);
}

void QAbout::exitBtnClicked(void)
{
    this->close();
}

void QAbout::initUiComponent(void)
{
    int label_w = 300, label_h = 20;
    int text_w = 300, text_h = 120;
    int btn_w = 80, btn_h = 30;
    int btn_x = QABOUT_WIDGET_WIDTH - btn_w;
    int btn_y = QABOUT_WIDGET_HEIGHT - btn_h;

    titleLabel = new QLabel(this);
    titleLabel->setText(tr("SiftGUI for Windows(Version v0.0.1)"));
    titleLabel->setGeometry(20, 10, label_w, label_h);

    QFont titleFont("Microsoft YaHei", 10, QFont::Bold);
    titleLabel->setFont(titleFont);

    authorLabel = new QLabel(this);
    authorLabel->setText(tr("Author: gitorup"));
    authorLabel->setGeometry(100, 30, label_w, label_h);

//    infoLabel = new QLabel(this);
//    infoLabel->setOpenExternalLinks(true);
//    infoLabel->setText(tr("<a href=\"https://github.com/gitorup/QImageViewer\">"
//                          "[https://github.com/gitorup/QImageViewer]"));
//    infoLabel->setGeometry(40, 50, label_w, label_h);

    QString info;
    info.append("                                     Statement\n");
    info.append("SiftGUI is homework for Computer Vision lessonc in HUST. ");
    info.append("Authors: SenQiao M201972716 & Haodong Tian m201972598."
                "If you have any question, please contact us."
                "Email: 653999083@qq.com"
                );


    infoTextEdit = new QTextEdit(this);
    infoTextEdit->setText(info);
    infoTextEdit->setReadOnly(1);
    infoTextEdit->setGeometry(15, 80, text_w, text_h);

    QPalette palette;
    palette.setColor(QPalette::Text, Qt::red);
    infoTextEdit->setPalette(palette);
    QFont infoFont("Microsoft YaHei", 8, QFont::Cursive);
    infoTextEdit->setFont(infoFont);

    exitBtn = new QPushButton(this);
    exitBtn->setText(tr("OK"));
    exitBtn->setGeometry(btn_x - 10, btn_y - 5, btn_w, btn_h);
    connect(exitBtn, SIGNAL(clicked(bool)), this, SLOT(exitBtnClicked()));
}

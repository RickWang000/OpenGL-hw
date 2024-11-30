/********************************************************************************
** Form generated from reading UI file 'QtOpenGLDemo.ui'
**
** Created by: Qt User Interface Compiler version 6.7.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_QTOPENGLDEMO_H
#define UI_QTOPENGLDEMO_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_QtOpenGLDemoClass
{
public:
    QGroupBox *groupBox;
    QRadioButton *perspectiveButton;
    QRadioButton *orthoButton;
    QGroupBox *groupBox_2;
    QTextBrowser *textBrowser;

    void setupUi(QWidget *QtOpenGLDemoClass)
    {
        if (QtOpenGLDemoClass->objectName().isEmpty())
            QtOpenGLDemoClass->setObjectName("QtOpenGLDemoClass");
        QtOpenGLDemoClass->resize(900, 700);
        groupBox = new QGroupBox(QtOpenGLDemoClass);
        groupBox->setObjectName("groupBox");
        groupBox->setGeometry(QRect(720, 30, 161, 101));
        perspectiveButton = new QRadioButton(groupBox);
        perspectiveButton->setObjectName("perspectiveButton");
        perspectiveButton->setGeometry(QRect(10, 30, 132, 22));
        perspectiveButton->setChecked(true);
        orthoButton = new QRadioButton(groupBox);
        orthoButton->setObjectName("orthoButton");
        orthoButton->setGeometry(QRect(10, 60, 132, 22));
        groupBox_2 = new QGroupBox(QtOpenGLDemoClass);
        groupBox_2->setObjectName("groupBox_2");
        groupBox_2->setGeometry(QRect(720, 180, 161, 481));
        textBrowser = new QTextBrowser(groupBox_2);
        textBrowser->setObjectName("textBrowser");
        textBrowser->setGeometry(QRect(10, 20, 141, 451));

        retranslateUi(QtOpenGLDemoClass);

        QMetaObject::connectSlotsByName(QtOpenGLDemoClass);
    } // setupUi

    void retranslateUi(QWidget *QtOpenGLDemoClass)
    {
        QtOpenGLDemoClass->setWindowTitle(QCoreApplication::translate("QtOpenGLDemoClass", "QtOpenGLDemo", nullptr));
        groupBox->setTitle(QCoreApplication::translate("QtOpenGLDemoClass", "Projection", nullptr));
        perspectiveButton->setText(QCoreApplication::translate("QtOpenGLDemoClass", "Perspective", nullptr));
        orthoButton->setText(QCoreApplication::translate("QtOpenGLDemoClass", "Ortho", nullptr));
        groupBox_2->setTitle(QCoreApplication::translate("QtOpenGLDemoClass", "Collision Detect", nullptr));
    } // retranslateUi

};

namespace Ui {
    class QtOpenGLDemoClass: public Ui_QtOpenGLDemoClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QTOPENGLDEMO_H

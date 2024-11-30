#ifndef QTOPENGLDEMO_H
#define QTOPENGLDEMO_H

#include <QWidget>
#include "OpenGLWidget.h"

QT_BEGIN_NAMESPACE
namespace Ui { class QtOpenGLDemoClass; }
QT_END_NAMESPACE

class QtOpenGLDemo : public QWidget
{
    Q_OBJECT

public:
    QtOpenGLDemo(QWidget *parent = nullptr);
    ~QtOpenGLDemo();

public slots:
    void set_ortho();
    void set_persective();
    void set_projection_button();
    void updateCollisionInfo(const QString& message);

private:
    Ui::QtOpenGLDemoClass *ui;
    CoreFunctionWidget *core_widget;
};
#endif // QTOPENGLDEMO_H

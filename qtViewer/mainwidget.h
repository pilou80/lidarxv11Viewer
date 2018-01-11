#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include "lidarxv11.h"

namespace Ui {
class mainWidget;
}

class mainWidget : public QWidget
{
    Q_OBJECT

public:
    explicit mainWidget(QWidget *parent = 0);
    ~mainWidget();

private slots:
    void on_groupBoxMode_clicked();

    void on_horizontalSliderSpeed_valueChanged(int value);

    void on_spinBoxSpeed_valueChanged(int value);

private:
    Ui::mainWidget *ui;
    lidarXV11 *m_lidar;
};

#endif // MAINWIDGET_H

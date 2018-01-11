#ifndef FORMPID_H
#define FORMPID_H

#include <QWidget>

namespace Ui {
class formPID;
}

class formPID : public QWidget
{
    Q_OBJECT

public:
    explicit formPID(QWidget *parent = 0);
    ~formPID();

    void setPID(float Kp, float Ki, float Kd);
    void setPeriod(uint periodms);

private:
    Ui::formPID *ui;
};

#endif // FORMPID_H

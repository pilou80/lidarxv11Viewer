#ifndef DRAW360_H
#define DRAW360_H

#include <QWidget>
#include <lidarxv11.h>

// ATESTER http://www.qcustomplot.com/
class draw360 : public QWidget
{
    Q_OBJECT

public:
    explicit draw360(QWidget *parent = 0);


    QList<value360_t*> *m_values;
signals:

public slots:
    void setValue(value360_t value);

private slots:
    void paintEvent(QPaintEvent *);

private:

    int m_maximum_dist;

};

#endif // DRAW360_H

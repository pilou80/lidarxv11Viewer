#include "draw360.h"
#include <QPainter>
 #include <QDebug>


draw360::draw360( QWidget *parent) : QWidget(parent)
{
    m_values = new QList<value360_t*>;

    value360_t *val;

    for(int i=0;i<360;i++)
    {
        val = new value360_t;
        val->dist = 1000+i*10;
        val->angle = i;
        val->strenght = 1.0;
        val->valid = false;
        val->strenghtWarn = false;
        m_values->append(val);
    }

    m_maximum_dist = 6000;
    update();
}

void draw360::setValue(value360_t value)
{
    foreach(value360_t *val, *m_values)
    {
        if(val->angle == value.angle)
            *val = value;
    }
}

void draw360::paintEvent(QPaintEvent *)
{
    QPainter paint(this);

    int correctedX, correctedY;
    int correctedWidth, correctedHeight;

    correctedX=0;
    correctedY=0;
    correctedWidth = this->width()-1;
    correctedHeight = this->height()-1;

    if(this->width() > this->height())
    {
        correctedX = (correctedWidth-correctedHeight) / 2;
        correctedWidth = correctedHeight;
    }
    else if(this->width() < this->height())
    {
        correctedY = (correctedHeight-correctedWidth) / 2;
        correctedHeight = correctedWidth;
    }

    //qDebug() << "new Repaint =======";
    QPen pen;
    pen.setColor(Qt::red);

    paint.setPen(pen);

    paint.drawEllipse(correctedX,
                      correctedY,
                      correctedHeight,
                      correctedWidth);

    pen.setColor(Qt::black);
    paint.setPen(pen);
    foreach(value360_t *val, *m_values)
    {
        float factor;
        factor = (float)val->dist/(float)m_maximum_dist;
        int realWidthHeight = (int)((float)correctedWidth*factor);
        int realX = correctedX + (correctedWidth - realWidthHeight) /2;
        int realY = correctedY + (correctedWidth - realWidthHeight) /2;
        //qDebug()<< realX << realY << realWidthHeight;

        paint.drawArc(realX,
                      realY,
                      realWidthHeight,
                      realWidthHeight,
                      val->angle * 16,
                      16
                      );
    }
    paint.end();
}

#include "grwidget.h"
#include "ui_grwidget.h"

#include <QPainter>
#include <QRgb>
#include <QPaintEvent>

GrWidget::GrWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GrWidget)
{
    ui->setupUi(this);
}

GrWidget::~GrWidget()
{
    delete ui;
}

void GrWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter;
    painter.begin(this);
    painter.fillRect(event->rect(), QWidget::palette().color(QWidget::backgroundRole()));

    painter.drawImage(0, 0, *m_image);

    QColor color = Qt::green;
    color.setAlpha(64);

    painter.fillRect(QRect(0, 0, m_left->value() * *m_zoom / 100, m_image->height()), color);
    painter.fillRect(QRect(m_image->width(), 0, -m_right->value() * *m_zoom / 100, m_image->height()), color);

    painter.fillRect(QRect(0, 0, m_image->width(), m_top->value() * *m_zoom / 100), color);
    painter.fillRect(QRect(0, m_image->height(), m_image->width(), -m_bottom->value() * *m_zoom / 100), color);

    QWidget::paintEvent(event);
    painter.end();
}

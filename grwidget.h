#ifndef GRWIDGET_H
#define GRWIDGET_H

#include <QWidget>
#include <QSpinBox>

namespace Ui {
class GrWidget;
}

class GrWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GrWidget(QWidget *parent = nullptr);
    ~GrWidget();

    void paintEvent(QPaintEvent *event);

    void setParams(QImage* image, int* zoom, QSpinBox* left, QSpinBox* right, QSpinBox* bottom, QSpinBox* top)
    {
        m_image = image;
        m_zoom = zoom;
        m_left = left;
        m_right = right;
        m_bottom = bottom;
        m_top = top;
    }

private:
    Ui::GrWidget *ui;

    QImage* m_image;

    int* m_zoom;

    QSpinBox* m_left;
    QSpinBox* m_right;
    QSpinBox* m_bottom;
    QSpinBox* m_top;
};

#endif // GRWIDGET_H

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QImage>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_action_Open_triggered();
    void on_action_Save_As_triggered();
    void on_actionE_xit_triggered();
    void on_actionA_bout_triggered();
    void on_spinBoxLeft_valueChanged(int arg1);
    void on_spinBoxBottom_valueChanged(int arg1);
    void on_spinBoxRight_valueChanged(int arg1);
    void on_spinBoxTop_valueChanged(int arg1);
    void on_spinBoxHOffset_valueChanged(int arg1);
    void on_comboBoxZoom_currentTextChanged(const QString &arg1);

private:
    void openImage(QString newImageFilename);

    void updateZoomedImage();

    Ui::MainWindow *ui;

    QImage m_image;
    QImage m_zoomed;
    QString m_filename;

    int m_zoom = 100;
};
#endif // MAINWINDOW_H

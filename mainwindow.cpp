#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>
#include <QFileDialog>

#include <cstdio>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->widget->setParams(&m_zoomed,
                          &m_zoom,
                          ui->spinBoxLeft,
                          ui->spinBoxRight,
                          ui->spinBoxBottom,
                          ui->spinBoxTop
                          );
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::openImage(QString newImageFilename)
{
    m_filename = newImageFilename;
    ui->statusbar->showMessage("Opening...");
    QApplication::processEvents();
    if (m_image.load(m_filename))
    {
        ui->statusbar->showMessage("Image '" + m_filename + "' opened, " + QString::number(m_image.width()) + "x" + QString::number(m_image.height()));
        updateZoomedImage();
        ui->widget->update();
    }
    else
    {
        ui->statusbar->showMessage("Error loading " + m_filename);
    }
}

void MainWindow::updateZoomedImage()
{
    QString z = ui->comboBoxZoom->currentText();

    z.replace('%', ' ');
    m_zoom = std::clamp(z.trimmed().toInt(), 1, 100);

    int wz = m_image.width() * m_zoom / 100;
    int hz = m_image.height() * m_zoom / 100;

    QImage tmp(wz, hz, QImage::Format_ARGB32);

    for (int y = 0; y != hz; ++y)
    {
        for (int x = 0; x != wz; ++x)
        {
            tmp.setPixel(x, y, m_image.pixel(x * m_image.width() / wz, y * m_image.height() / hz));
        }
    }

    m_zoomed.swap(tmp);
}

void MainWindow::on_action_Open_triggered()
{
    QFileDialog dialog(this);
    dialog.setNameFilter("Images (*.png);;All files (*)");
    if (dialog.exec())
    {
        if (dialog.selectedFiles().size() == 1)
        {
            openImage(dialog.selectedFiles()[0]);
        }
    }
}

void MainWindow::on_action_Save_As_triggered()
{
    int newWidth = m_image.width() - ui->spinBoxLeft->value() - ui->spinBoxRight->value();
    int newHeight = m_image.height() - ui->spinBoxTop->value() - ui->spinBoxBottom->value();

    if (newWidth > 0 && newHeight > 0)
    {

        ui->statusbar->showMessage("Saving...");
        QApplication::processEvents();

        QImage im(newWidth, newHeight, QImage::Format_RGB888);
        for (int y = 0; y != newHeight; ++y)
        {
            for (int x = 0; x != newWidth; ++x)
            {
                im.setPixel(x, y, m_image.pixel(x+ui->spinBoxLeft->value(), y+ui->spinBoxTop->value()));
            }
        }
        QString cropFilename = m_filename + "_crop.jpg";
        im.save(cropFilename, "jpg", ui->spinBoxQuality->value());

        QString crop360Filename = m_filename + "_crop360.jpg";

        QString statusSaveMessage = "Saved '" + cropFilename + "'";

        FILE* fin = fopen(cropFilename.toStdString().c_str(), "rb");
        if (fin)
        {
            FILE* fout = fopen(crop360Filename.toStdString().c_str(), "wb");
            if (fout)
            {
                unsigned char buf2[2];
                if (fread(buf2, 1, 2, fin) == 2)
                {
                    fwrite(buf2, 1, 2, fout);
                }
                const char* str1 = "http://ns.adobe.com/xap/1.0/";
                // 0
                const char* str2 = "<?xpacket begin='";
                // ef bb bf
                const char* str3fmt =
                    "' id='W5M0MpCehiHzreSzNTczkc9d'?>\n<x:xmpmeta xmlns:x=\"adobe:ns:meta/\"> <rdf:RDF xmlns:rdf=\"http://www.w3.org/1999/02/22-rdf-syntax-ns#\"> "
                    "<rdf:Description rdf:about=\"\" xmlns:GPano=\"http://ns.google.com/photos/1.0/panorama/\" GPano:ProjectionType=\"equirectangular\" GPano:UsePanoramaViewer=\"True\" GPano:CroppedAreaImageWidthPixels=\"%d\" GPano:CroppedAreaImageHeightPixels=\"%d\" GPano:FullPanoWidthPixels=\"%d\" GPano:FullPanoHeightPixels=\"%d\" GPano:CroppedAreaLeftPixels=\"%d\" GPano:CroppedAreaTopPixels=\"%d\" GPano:StitchingSoftware=\"Crop360 (github.com/Franticware/crop360)\" />"
                    "</rdf:RDF></x:xmpmeta><?xpacket end='w'?>";

                int cropW = newWidth;
                int cropH = newHeight;
                int fullW = m_image.width();
                int fullH = m_image.height();
                int cropL = ui->spinBoxLeft->value();
                int cropT = ui->spinBoxTop->value();

                char str3buf[1024] = {0};
                snprintf(str3buf, 1023, str3fmt, cropW, cropH, fullW, fullH, cropL, cropT);

                size_t len = 2 /* length bytes */ + strlen(str1) + 1 /* zero byte */ + strlen(str2) + 3 + strlen(str3buf);

                fputc(0xff, fout); // 2 marker bytes are not included in the length
                fputc(0xe1, fout);
                fputc((unsigned char)(len >> 8), fout); // 2 length bytes ARE included in the length
                fputc((unsigned char)(len), fout);
                fputs(str1, fout);
                fputc(0, fout);
                fputs(str2, fout);
                fputc(0xef, fout);
                fputc(0xbb, fout);
                fputc(0xbf, fout);
                fputs(str3buf, fout);
                unsigned char buf1024[1024];
                size_t readBytes = 0;
                while ((readBytes = fread(buf1024, 1, 1024, fin)))
                {
                    fwrite(buf1024, 1, readBytes, fout);
                }
                fclose(fout);
                statusSaveMessage += " and '" + crop360Filename + "'";
            }
            fclose(fin);
        }
        ui->statusbar->showMessage(statusSaveMessage);
    }
    else
    {
        // error message
        QMessageBox::critical(this,
                                 "Error saving image"
                                 ,
                                 "No image content"
                                 ,
                                 QMessageBox::Ok
                                 );
    }
}

void MainWindow::on_actionE_xit_triggered()
{
    close();
}

void MainWindow::on_actionA_bout_triggered()
{
    QMessageBox::information(this,
                             "About Crop360"
                             ,
                             "© 2023 Franticware\n"
                             "github.com/Franticware/Crop360\n\n"
                             "MIT License"
                             ,
                             QMessageBox::Ok
                             );
}

void MainWindow::on_spinBoxLeft_valueChanged(int)
{
    ui->widget->update();
}


void MainWindow::on_spinBoxBottom_valueChanged(int)
{
    ui->widget->update();
}


void MainWindow::on_spinBoxRight_valueChanged(int)
{
    ui->widget->update();
}


void MainWindow::on_spinBoxTop_valueChanged(int)
{
    ui->widget->update();
}


void MainWindow::on_spinBoxHOffset_valueChanged(int)
{
    ui->widget->update();
}

void MainWindow::on_comboBoxZoom_currentTextChanged(const QString &)
{
    updateZoomedImage();
    ui->widget->update();
}


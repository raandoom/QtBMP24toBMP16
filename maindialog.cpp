#include <QBoxLayout>
#include <QPushButton>
#include <QComboBox>
#include <QFileDialog>
#include <QProcess>
#include "maindialog.h"

unsigned short checkBMPbpp(QString path);
void convert24to16(QString srcPath, QString destPath, int orient);

MainDialog::MainDialog(QWidget *parent) :
    QDialog(parent)
{
    imagePath = 0;
    setWindowTitle("Convert *.bmp from 24-bit to 16-bit");
    QVBoxLayout *mainL = new QVBoxLayout;
    setLayout(mainL);;
    preview24 = new QLabel("<Empty>", this);
    preview24->setMinimumSize(200,150);
    preview24->setAlignment(Qt::AlignCenter);
    preview24->setAutoFillBackground(true);
    preview24->setPalette(QPalette(QColor(255,255,255)));
    mainL->addWidget(preview24);
    QPushButton *open24 = new QPushButton("Open", this);
    QLabel *orientLabel = new QLabel("Save with orientation:", this);
    QComboBox *bmpOrient = new QComboBox(this);
    bmpOrient->addItem("as source image");
    bmpOrient->addItem("from top to bottom");
    bmpOrient->addItem("from bottom to top");
    QPushButton *save16 = new QPushButton("Save", this);
    mainL->addWidget(open24);
    mainL->addWidget(orientLabel);
    mainL->addWidget(bmpOrient);
    mainL->addWidget(save16);

    connect(open24, SIGNAL(clicked()), this, SLOT(openClicked()));
    connect(bmpOrient, SIGNAL(currentIndexChanged(int)),
            this, SLOT(setBmpOrient(int)));
    connect(save16, SIGNAL(clicked()), this, SLOT(saveClicked()));
    orient = bmpOrient->currentIndex();
}
//==============================================================================
void MainDialog::openClicked()
{
    QStringList *environment = new QStringList(QProcess::systemEnvironment());
    QString home = environment->filter("HOME=").at(0);
    home.remove("HOME=");
    QString tempPath = QFileDialog::getOpenFileName(this,
                                                    "Open 24-bit *.bmp image:",
                                                    home,
                                                    "*.bmp");
    if (tempPath.isEmpty() == true)
    {
        return;
    }
    if (checkBMPbpp(tempPath) != 24)
    {
        QDialog *warning = new QDialog(this);
        warning->setAttribute(Qt::WA_DeleteOnClose);
        warning->setModal(true);
        warning->setWindowTitle("Warning!");
        QVBoxLayout *warningL = new QVBoxLayout(warning);
        warning->setLayout(warningL);
        QLabel *warningT = new QLabel("Selected image is not a 24-bit *.bmp!");
        warningL->addWidget(warningT);
        QPushButton *warningBtn = new QPushButton("Close", warning);
        warningL->addWidget(warningBtn);
        connect(warningBtn, SIGNAL(clicked()), warning, SLOT(close()));
        warning->show();
        return;
    }
    imagePath = new QString(tempPath);
    preview24->setPixmap(
                QPixmap::fromImage(
                    QImage(tempPath).
                    scaled(preview24->size(), Qt::KeepAspectRatio)));
}
//==============================================================================
void MainDialog::saveClicked()
{
    if (imagePath == 0) return;
    QStringList *environment = new QStringList(QProcess::systemEnvironment());
    QString home = environment->filter("HOME=").at(0);
    home.remove("HOME=");
    QString tempPath = QFileDialog::getSaveFileName(this,
                                                    "Save 16-bit *.bmp image:",
                                                    home,
                                                    "*.bmp");
    if (tempPath.isEmpty() == true)
    {
        return;
    }
    if (tempPath.right(4).compare(".bmp", Qt::CaseInsensitive) != 0)
        tempPath.append(".bmp");
    convert24to16(*imagePath, tempPath, orient);
}
//==============================================================================
void MainDialog::resizeEvent(QResizeEvent *)
{
    if (imagePath != 0)
    {
        preview24->setPixmap(
                    QPixmap::fromImage(
                        QImage(*imagePath).
                        scaled(preview24->size(), Qt::KeepAspectRatio)));
    }
}

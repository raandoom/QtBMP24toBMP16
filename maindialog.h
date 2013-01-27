#ifndef MAINDIALOG_H
#define MAINDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QComboBox>

class MainDialog : public QDialog
{
    Q_OBJECT
public:
    explicit MainDialog(QWidget *parent = 0);
    
signals:

    
public slots:
    void setBmpOrient(int i) {orient = i;}
    void openClicked();
    void saveClicked();

private:
    QString *imagePath;
    QLabel *preview24;
    QComboBox *bmpOrient;
    QLabel *turnOrient;
    int orient;
    ushort bpp;
    void resizeEvent(QResizeEvent *);
};

#endif // MAINDIALOG_H

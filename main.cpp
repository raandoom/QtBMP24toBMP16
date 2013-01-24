#include <QApplication>
#include "maindialog.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainDialog mDlg;
    mDlg.show();
    return app.exec();
}

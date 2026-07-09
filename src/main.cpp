#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    QApplication::setApplicationName(QStringLiteral("NewStep 测控软件"));
    QApplication::setOrganizationName(QStringLiteral("NewStep"));
    MainWindow window;
    window.show();
    return app.exec();
}

#include <QApplication>
#include <QStyleFactory>
#include <QDebug>

#include "mainwindow.h"

int main(int argc, char *argv[])
{
    qSetMessagePattern(QStringLiteral("%{time dd.MM.yyyy HH:mm:ss.zzz} "
                                      "["
                                      "%{if-debug}D%{endif}"
                                      "%{if-info}I%{endif}"
                                      "%{if-warning}W%{endif}"
                                      "%{if-critical}C%{endif}"
                                      "%{if-fatal}F%{endif}"
                                      "] "
                                      "%{function}(): "
                                      "%{message}"));

    Q_INIT_RESOURCE(resources);

    QApplication app(argc, argv);

    QApplication::setStyle(QStyleFactory::create("Windows"));

    MainWindow mainWindow;
    mainWindow.show();

    return app.exec();
}

#include <QApplication>
#include <QStyleFactory>
#include <QCommandLineParser>

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

    QCoreApplication::setOrganizationDomain("brunner.ninja");
    QCoreApplication::setOrganizationName("brunner.ninja");
    QCoreApplication::setApplicationName("QtGameMaker");
    QCoreApplication::setApplicationVersion("1.0");

    QCommandLineParser parser;
    parser.setApplicationDescription(QCoreApplication::applicationName());
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("project", "The project to open.");
    parser.process(app);

    QApplication::setStyle(QStyleFactory::create("Windows"));

    MainWindow mainWindow{parser.positionalArguments().value(0)};
    mainWindow.show();

    return app.exec();
}

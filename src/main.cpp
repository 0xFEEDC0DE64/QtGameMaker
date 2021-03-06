#include <QApplication>
#include <QCommandLineParser>
#include <QLoggingCategory>

#include "mainwindow.h"

Q_LOGGING_CATEGORY(lcVk, "qt.vulkan")

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

    QLoggingCategory::setFilterRules(QStringLiteral("qt.vulkan=true"));

    Q_INIT_RESOURCE(resources_editor);
    Q_INIT_RESOURCE(resources_engine);

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

    MainWindow mainWindow{parser.positionalArguments().value(0)};
    mainWindow.show();

    return app.exec();
}

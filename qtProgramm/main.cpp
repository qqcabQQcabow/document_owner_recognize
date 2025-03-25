#include "mainwindow.h"

#include <QApplication>
#include <QTextStream>
#include <QStyleFactory>

int main(int argc, char *argv[])
{
    QFont font_custom("JetBrainsMono Nerd Font");
    //font_custom.setStyleHint(QFont::Monospace);
    font_custom.setPointSize(12);

    QApplication app(argc, argv);
    app.setFont(font_custom);
    app.setStyle(QStyleFactory::create("Fusion"));
    QPalette p;
    p = app.palette();
    p.setColor(QPalette::Window, QColor(0, 43,54));
    p.setColor(QPalette::Base, QColor(0, 43,54));
    p.setColor(QPalette::Button, QColor(7, 43,54));
    p.setColor(QPalette::Highlight, QColor(38,139,210));
    p.setColor(QPalette::ButtonText, QColor(215,215,215));
    p.setColor(QPalette::HighlightedText, QColor(215,215,215));
    p.setColor(QPalette::Text, QColor(215,215,215));

    app.setPalette(p);


    MainWindow window(nullptr);
    window.show();
    return app.exec();
}

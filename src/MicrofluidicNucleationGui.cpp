//
// Created by nicholas on 15.03.25.
//
#include <MicrofluidicNucleationGui/MicrofluidicNucleationGui.h>
#include <MicrofluidicNucleationGui/MainWindow.h>
#include <QApplication>


int mfngui::MicrofluidicNucleationGui::main(int argc, char **argv)
{
    QApplication app(argc, argv);
    MainWindow w;
    w.show();
    return app.exec();
}

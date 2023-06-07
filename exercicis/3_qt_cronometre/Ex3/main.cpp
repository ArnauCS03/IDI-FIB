#include <QApplication>
#include "Cronòmetre.h"

int main(int argc, char** argv) {
    QApplication app(argc, argv);
    Cronometre cronometre;
    cronometre.show();
    return app.exec();
}
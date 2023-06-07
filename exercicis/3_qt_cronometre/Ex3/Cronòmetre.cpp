#include "Cronòmetre.h"

Cronometre::Cronometre(QWidget* parent): QWidget(parent) 
{
    ui.setupUi(this);

    ui.lcdNumber->setDigitCount(12);
    ui.lcdNumber->display("00:00:00:000");

    time = QTime(0, 0, 0, 0);

    timer = new QTimer(this);
    timer->setInterval(1); 

    connect(timer, &QTimer::timeout, this, [&]() {
        time = time.addMSecs(1); // incrementem temps 1 microsegon
        QString timeString = time.toString("hh:mm:ss:zzz"); 
        ui.lcdNumber->display(timeString); 
    });
}

void Cronometre::start_stop() {

    if (not started) {
        timer->start();
        started = true;
        if (record) {
            ui.recordLabel->show();
            if (time.toString("hh:mm:ss:zzz") < recordTime) {
                QPalette palette = ui.lcdNumber->palette(); 
                palette.setColor(QPalette::Normal, QPalette::WindowText, Qt::green); 
                ui.lcdNumber->setPalette(palette);
            } 
        }
    }
    else {
        timer->stop();
        started = false;

        if (inici) {
            recordTime = ui.lcdNumber->value(); // guardem el valor quan para
            recordTime = time.toString("hh:mm:ss:zzz");
            inici = false;
        }
        else {

            if (time.toString("hh:mm:ss:zzz") < recordTime) {
                QPalette palette = ui.lcdNumber->palette(); 
                palette.setColor(QPalette::Normal, QPalette::WindowText, Qt::green); 
                ui.lcdNumber->setPalette(palette);
            } 
            else if (time.toString("hh:mm:ss:zzz") == recordTime){ 
                QPalette palette = ui.lcdNumber->palette(); 
                palette.setColor(QPalette::Normal, QPalette::WindowText, Qt::black); 
                ui.lcdNumber->setPalette(palette);
            }
            else {
                QPalette palette = ui.lcdNumber->palette(); 
                palette.setColor(QPalette::Normal, QPalette::WindowText, Qt::red); 
                ui.lcdNumber->setPalette(palette);
            }
            
            QString nouTemps = time.toString("hh:mm:ss:zzz");
            if (nouTemps < recordTime) {
                record = true;
                recordTime = nouTemps;
            }
        }
        ui.recordLabel->setText(recordTime); // actualitzem el label amb el record
    }
}

void Cronometre::reset() {
    
    started = false;
    timer->stop();
    time = QTime(0,0,0,0);
    ui.lcdNumber->display("00:00:00:000");

    QPalette palette = ui.lcdNumber->palette();
    palette.setColor(QPalette::Normal, QPalette::WindowText, Qt::black);
    ui.lcdNumber->setPalette(palette); 
}
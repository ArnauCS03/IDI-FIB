#include <QTimer>
#include <QTime>
#include <QLCDNumber>
#include <QString>
#include <QPalette>
#include "ui_Cronòmetre.h"


class Cronometre : public QWidget 
{
    Q_OBJECT

    public:
      Cronometre(QWidget *parent = 0);

    public slots:
      void start_stop();
      void reset();

    private:
      Ui::Cronometre ui;
      QTimer *timer;
      QTime time;
      QString recordTime;

      bool started = false;
      bool record = false;
      bool inici = true;
};
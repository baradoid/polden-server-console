#include "CommandController.h"

#include <QDebug>
//#include <QTextCodec>
#include <QEventLoop>

CommandController::CommandController(QObject *parent) : QObject(parent)
{

}


void CommandController::processMessage(const QString &msg)
{
    //QTextCodec *codec1 = QTextCodec::codecForName( "CP866" );


    if((msg == "but1\r\n") || (msg == "but1")){
        qInfo() << "CommandController> button1";
        //emit button1();
    }
    else if((msg == "but2\r\n")||(msg == "but2")){
        qInfo() << "CommandController> button2";
        //emit button2();
    }
    else if((msg == "cancel\r\n")||(msg == "cancel")){
        qInfo() << "CommandController> cancel";
        //emit buttonCancel();
    }
    else if((msg == "light_turned_off\r\n")||(msg == "light_turned_off")){
        qInfo() << "CommandController> light off detected";
        //emit lightTurnedOff();
    }
    else if((msg == "light_turned_on\r\n")||(msg == "light_turned_on")){
        qInfo() << "CommandController> light on detected";
        //emit lightTurnedOn();
    }
    else{
        //qDebug() << "CommandController> rcv: " << msg ;
    }
}

void CommandController::waitForButton1()
{
    QEventLoop loop;
    loop.connect(this, SIGNAL(button1()), SLOT(quit()));
    loop.exec();
}

void CommandController::waitForButton2()
{
    QEventLoop loop;
    loop.connect(this, SIGNAL(button2()), SLOT(quit()));
    loop.exec();
}

void CommandController::waitForButton2OrCancel()
{
    QEventLoop loop;
    loop.connect(this, SIGNAL(button2()), SLOT(quit()));
    loop.connect(this, SIGNAL(cancel()), SLOT(quit()));
    loop.exec();
}

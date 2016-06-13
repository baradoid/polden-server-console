#include "ConsoleInputProcessor.h"
#include <QTextStream>

ConsoleInputProcessor::ConsoleInputProcessor(QObject *parent) : QObject(parent)
{

}

void ConsoleInputProcessor::process()
{
    QTextStream cin(stdin);
    forever{
        QString str = cin.readLine();
        //qDebug() << "detected line:" << str;
        emit msgRecvd(str);
        //QThread::msleep(200);

    }
}

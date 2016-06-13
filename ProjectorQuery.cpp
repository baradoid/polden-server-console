#include "ProjectorQuery.h"
#include <QThread>
#include <QTcpSocket>
#include <QEventLoop>

ProjectorQuery::ProjectorQuery(QString _ip, QString _name) :
    QObject(0),
    lastState(unknownState),
    ip(_ip),
    name(_name)
{

}

void ProjectorQuery::waitForPowerOn()
{
    if(getState() == onState)
        return;
    QEventLoop loop;
    loop.connect(this, SIGNAL(powerOnState()), SLOT(quit()));
    loop.exec();
}

void ProjectorQuery::waitForPowerOff()
{
    if(getState() == offState)
        return;

    QEventLoop loop;
    loop.connect(this, SIGNAL(powerOffState()), SLOT(quit()));
    loop.exec();
}

void ProjectorQuery::process()
{
    QTcpSocket projectorClient(this);

    projectorClient.connectToHost(ip, 4352);
    if(projectorClient.waitForReadyRead(-1) == true){
        QString repl(projectorClient.readAll());
        //qDebug() << "proj reply" << repl;
    }
    else{
        return;
    }

    int i=0;
    //bool errTimerEna = false;
    int timerCnt = 0;
    forever{

        projectorClient.write(GET_STATE_QUERY);


        if(projectorClient.waitForReadyRead(2000) == true){
            QString repl(projectorClient.readAll());
            TProjState curState = unknownState;

            if(repl == "%1POWR=0\r"){
                //qDebug() << "emit powerOffState";
                emit powerOffState();
                curState = offState;
            }
            else if(repl == "%1POWR=1\r"){
                //qDebug() << "emit powerOnState";
                emit powerOnState();
                curState = onState;
            }
            else if(repl == "%1POWR=2\r"){
                curState = coolingState;
            }
            else if(repl == "%1POWR=3\r"){
                curState = warmUpState;
            }
            //else
            //    qDebug() << "unknown repl:" << repl;

            if(sendState.length() > 0){
                QString lowLevelCmd;
                switch(sendState.takeFirst()){
                    case onState:
                    lowLevelCmd = POWER_ON_CMD;
                    break;
                    case offState:
                    lowLevelCmd = POWER_OFF_CMD;
                    break;
                    default:
                    break;
                }
                projectorClient.write(lowLevelCmd.toLatin1());
            }
            if(curState != lastState){
                lastState = curState;
                qDebug() << "ProjectorQuery> proj" << name <<"new state:" << repl;
                //emit newState(repl);
            }
        }
        QThread::msleep(500);


        i++;
    }
}

void ProjectorQuery::on()
{
    lock.lockForWrite();   
    sendState << onState;
    lock.unlock();
}

void ProjectorQuery::off()
{
    lock.lockForWrite();   
    sendState << offState;
    lock.unlock();
}


TProjState ProjectorQuery::getState()
{
    lock.lockForRead();
    TProjState state = lastState;
    lock.unlock();
    return state;
}


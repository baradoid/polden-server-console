#include "ProjectorQuery.h"
#include <QThread>
#include <QTcpSocket>
#include <QEventLoop>
#include <QTimer>

void powerOnProjectors()
{
    extern QList<ProjectorQuery*> pqList;

    foreach(ProjectorQuery* pq, pqList){
        pq->on();
    }
}

ProjectorQuery::ProjectorQuery(QString _ip/*, QString _name*/, int timeOutSecs) :
    QObject(0),
    lastState(unknownState),
    ip(_ip)/*,
    name(_name)*/
{

    /*turnOffTimer.connect(
                [=](){off()}, SIGNAL(timeout()), );*/
    connect(&turnOffTimer, &QTimer::timeout,
            [this](){
            qInfo() << "ProjectorQuery" << ip <<"> off time out";
            off();
        });

    turnOffTimer.setInterval(timeOutSecs*1000);
    turnOffTimer.start();
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
        qDebug() << "!!!! ProjectorQuery exited !!!!" ;
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
                curState = offState;
            }
            else if(repl == "%1POWR=1\r"){
                //qDebug() << "emit powerOnState";
                curState = onState;
            }
            else if(repl == "%1POWR=2\r"){
                curState = coolingState;
            }
            else if(repl == "%1POWR=3\r"){
                curState = warmUpState;
            }

            if(curState != lastState){
                lastState = curState;                
                qInfo() << "ProjectorQuery" << ip <<">new state:" << repl;
                //emit newState(repl);
            }
            if(curState == onState)
                emit powerOnState();
            else if(curState == offState)
                emit powerOffState();
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

        }
        QThread::msleep(500);


        i++;
    }
}

void ProjectorQuery::sendCmd(QString cmd)
{
    lock.lockForWrite();
    QTcpSocket projectorClient(this);

    projectorClient.connectToHost(ip, 4352);
    if(projectorClient.waitForReadyRead(-1) == true){
        QString repl(projectorClient.readAll());
        //qDebug() << "proj reply" << repl;
    }
    else{
        qDebug() << "!!!! ProjectorQuery error !!!!" ;
    }
    projectorClient.write(cmd.toLatin1());
    projectorClient.flush();
    projectorClient.close();
    lock.unlock();
}

void ProjectorQuery::on()
{
    sendCmd(POWER_ON_CMD);
    turnOffTimer.start();
}

void ProjectorQuery::off()
{
    sendCmd(POWER_OFF_CMD);
}


TProjState ProjectorQuery::getState()
{
    lock.lockForRead();
    TProjState state = unknownState;
    QTcpSocket projectorClient(this);

    projectorClient.connectToHost(ip, 4352);
    if(projectorClient.waitForReadyRead(-1) == true){
        QString repl(projectorClient.readAll());
        //qDebug() << "proj reply" << repl;
    }
    else{
        qDebug() << "!!!! ProjectorQuery error !!!!" ;
        return unknownState;
    }
    projectorClient.write(GET_STATE_QUERY);

    if(projectorClient.waitForReadyRead(2000) == true){
        QString repl(projectorClient.readAll());

        if(repl == "%1POWR=0\r"){
            //qDebug() << "emit powerOffState";
            state = offState;
        }
        else if(repl == "%1POWR=1\r"){
            //qDebug() << "emit powerOnState";
            state = onState;
        }
        else if(repl == "%1POWR=2\r"){
            state = coolingState;
        }
        else if(repl == "%1POWR=3\r"){
            state = warmUpState;
        }
        else{
            qDebug() << "unknown repl:" << repl;

        }
        if(lastState != state){
            //qInfo() << "PQ" << ip <<"> new state:" << repl;
            lastState = state;
        }
    }
    lock.unlock();
    return state;
}

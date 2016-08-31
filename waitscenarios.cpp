#include <QTcpServer>
#include "CommandController.h"
#include "ProjectorQuery.h"


#include <QEventLoop>
#include <QTimer>
#include <QProcess>
#include <QThread>
#include <QMap>


TCmdButton waitForTimeoutOrCancelCmd(int secTimeout)
{
    extern CommandController cmdCtl;

    QEventLoop el;

    QMetaObject::Connection m_conn1, m_conn2;
    m_conn1 = QObject::connect(&cmdCtl, &CommandController::buttonCancel, [&el]() { el.exit(cmdButtonCancel); });

    QTimer timer;
    timer.setSingleShot(true);
    timer.setInterval(secTimeout * 1000);
    m_conn2 = QObject::connect(&timer, &QTimer::timeout, [&el]() { el.exit(cmdTimeout); });
    timer.start();

    TCmdButton ret = (TCmdButton)el.exec();

    QObject::disconnect(m_conn1);
    QObject::disconnect(m_conn2);

    return ret;
}

//0 - exit on finish, 1 - exit on cancel
TCmdButton waitForFinishPlayOrCancel(QProcess *videoPlayer)
{
    //extern QProcess videoPlayer;
    extern CommandController cmdCtl;

    QEventLoop el;

    QMetaObject::Connection m_conn1, m_conn2;

    m_conn1 = QObject::connect(&cmdCtl, &CommandController::buttonCancel, [&el]() { el.exit(cmdButtonCancel); });
    //QObject::connect((QObject*)&videoPlayer, &QProcess::finished, [&el]() { el.exit(0); });
    m_conn2 = QObject::connect(videoPlayer, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),
          [&el](int exitCode, QProcess::ExitStatus exitStatus){ qDebug() <<"video finished signal exit code " << exitCode; el.exit(cmdFinished); });

    TCmdButton ret = (TCmdButton)el.exec();

    QObject::disconnect(m_conn1);
    QObject::disconnect(m_conn2);

    return ret;
}

TCmdButton waitForBut1CmdOrBut2Cmd()
{
    extern CommandController cmdCtl;

    QEventLoop el;

    QMetaObject::Connection m_conn1, m_conn2;
    m_conn1 = QObject::connect(&cmdCtl, &CommandController::button1, [&el]() { el.exit(cmdButton1); });
    m_conn2 = QObject::connect(&cmdCtl, &CommandController::button2, [&el]() { el.exit(cmdButton2); });

    TCmdButton ret = (TCmdButton)el.exec();

    QObject::disconnect(m_conn1);
    QObject::disconnect(m_conn2);

    return ret;
}


TCmdButton waitForProjectorsStateOrCancel(QList<ProjectorQuery*> &pqList, TProjState stateWaitFor)
{
    QMap<int, QString> projStateName;
    projStateName[onState] = "On";
    projStateName[offState] = "Off";
    projStateName[coolingState] = "Cooling";
    projStateName[warmUpState] = "WarmUp";

    extern CommandController cmdCtl;
    QEventLoop el;

    QMetaObject::Connection m_conn1, m_conn2;
    m_conn1 = QObject::connect(&cmdCtl, &CommandController::buttonCancel, [&el]() { el.exit(cmdButtonCancel); });

    TCmdButton ret;
    foreach (ProjectorQuery* pq, pqList) {
        TProjState ps = pq->getState();
        if(ps == stateWaitFor){
            ret = cmdFinished;
            qInfo() << "proj" << pq->ip << "in state"<< projStateName[ps] <<". Nothing to do.";
        }
        else{
            qInfo() << "proj" << pq->ip << "in state"<< projStateName[ps] <<". Changing state.";
            if(stateWaitFor == offState)
                pq->off();
            else if(stateWaitFor == onState)
                pq->on();
        }
    }

    foreach (ProjectorQuery* pq, pqList) {
        qInfo() << "wait for proj" << pq->ip ;
        if(stateWaitFor == offState){
            TProjState state = pq->getState();
            while( ((state == offState) || (state == coolingState)) == false){
                state = pq->getState();
                QThread::msleep(250);
            }
        }
        else{
            while(pq->getState() != stateWaitFor){
                if(stateWaitFor == onState)
                    pq->on();
                QThread::msleep(250);
            }
        }

        ret = cmdFinished;
    }

    QObject::disconnect(m_conn1);

    return ret;
}

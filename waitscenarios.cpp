#include <QTcpServer>
#include "CommandController.h"
#include "ProjectorQuery.h"


#include <QEventLoop>
#include <QTimer>
#include <QProcess>


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
TCmdButton waitForFinishPlayOrCancel()
{
    extern QProcess videoPlayer;
    extern CommandController cmdCtl;

    QEventLoop el;

    QMetaObject::Connection m_conn1, m_conn2;

    m_conn1 = QObject::connect(&cmdCtl, &CommandController::buttonCancel, [&el]() { el.exit(cmdButtonCancel); });
    //QObject::connect((QObject*)&videoPlayer, &QProcess::finished, [&el]() { el.exit(0); });
    m_conn2 = QObject::connect(&videoPlayer, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),
          [&el](int exitCode, QProcess::ExitStatus exitStatus){  el.exit(cmdFinished); });

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
TCmdButton waitForProjectorsOnOrCancel()
{
    extern CommandController cmdCtl;
    extern QList<ProjectorQuery*> pqList;
    QEventLoop el;

    QMetaObject::Connection m_conn1, m_conn2;
    m_conn1 = QObject::connect(&cmdCtl, &CommandController::buttonCancel, [&el]() { el.exit(cmdButtonCancel); });

    TCmdButton ret;
    foreach (ProjectorQuery* pq, pqList) {
        TProjState ps = pq->getState();
        if(ps == onState){
            ret = cmdFinished;
            qInfo() << "proj " << pq->ip << " in state "<< ps <<". Nothing to do.";
        }
        else{
            qInfo() << "proj " << pq->ip << " in state "<< ps <<". Send PowerOn command.";
            pq->on();
        }
    }

    foreach (ProjectorQuery* pq, pqList) {
        if(pq->getState() == onState){
            ret = cmdFinished;
        }
        else{
            m_conn2 = QObject::connect(pq, &ProjectorQuery::powerOnState, [&el]() { el.exit(cmdFinished); });

            qInfo() << "main> wait for power on proj " << pq->ip;
            ret = (TCmdButton)el.exec();
            QObject::disconnect(m_conn2);
            if(ret == cmdButtonCancel){
                break;
            }
        }
    }

    QObject::disconnect(m_conn1);

    return ret;
}

TCmdButton waitForProjectorsOffOrCancel()
{
    extern CommandController cmdCtl;
    extern QList<ProjectorQuery*> pqList;
    QEventLoop el;

    QMetaObject::Connection m_conn1, m_conn2;
    m_conn1 = QObject::connect(&cmdCtl, &CommandController::buttonCancel, [&el]() { el.exit(cmdButtonCancel); });

    TCmdButton ret;
    foreach (ProjectorQuery* pq, pqList) {
        TProjState ps = pq->getState();
        if((ps == offState) || (ps == coolingState)){
            ret = cmdFinished;
            qInfo() << "proj " << pq->ip << " in state "<< ps <<". Nothing to do.";
        }
        else{
            qInfo() << "proj " << pq->ip << " in state "<< ps <<". Send PowerOff command.";
            pq->off();
        }
    }

    foreach (ProjectorQuery* pq, pqList) {
        if(pq->getState() == offState){
            ret = cmdFinished;
        }
        else{
            m_conn2 = QObject::connect(pq, &ProjectorQuery::powerOffState, [&el]() { el.exit(cmdFinished); });

            qInfo() << "main> wait for power off proj " << pq->ip;
            ret = (TCmdButton)el.exec();
            QObject::disconnect(m_conn2);
            if(ret == cmdButtonCancel){
                break;
            }
        }
    }

    QObject::disconnect(m_conn1);

    return ret;
}

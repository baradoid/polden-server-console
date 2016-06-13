#include <QCoreApplication>
#include <QThread>
#include <QTcpServer>
#include <QTcpSocket>

#include "telnettcpserver.h"
#include "ProjectorQuery.h"
#include "CommandController.h"
#include "ConsoleInputProcessor.h"
#include "waitScenarios.h"

#include <qtconcurrentrun.h>
#include <QEventLoop>

#include <QSound>
#include <QTimer>
#include <QProcess>

//class Logger : public QObject {
//   // Q_OBJECT
//    //Q_SIGNAL
//public:
//    Logger(QObject *parent = 0) : QObject(parent) {}

//    void notify(const QString text) {
//        emit notification(text);
//        qInfo(qPrintable(text));
//    }

//signals:
//   Q_SIGNAL void notification(const QString);


//};


CommandController cmdCtl;
QProcess videoPlayer;
QList<ProjectorQuery*> pqList;
TelnetTcpServer *tcpServ = NULL;


void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg);

void turnOnLight()
{
    qInfo() << "light_on" ;
}

void turnOffLight()
{
    qInfo() << "light_off" ;
}

int main(int argc, char *argv[])
{   
    qInstallMessageHandler(myMessageOutput);
    QCoreApplication a(argc, argv);

    //Logger log(&a);

    QList<QThread*> threads;

    QThread *ipThread = new QThread(&a); // thread owned by the application object
    ConsoleInputProcessor *ip = new ConsoleInputProcessor();
    ip->moveToThread(ipThread);
    ip->connect(ipThread, SIGNAL(started()), SLOT(process()));
    cmdCtl.connect(ip, SIGNAL(msgRecvd(QString)), SLOT(processMessage(const QString)));
    //tcpServ.connect(pq, SIGNAL(messageReady(QString)), SLOT(printfSlot(const QString)));

    ipThread->start();
    threads << ipThread;


    TelnetTcpServer tcpServLoc(23);
    tcpServ = &tcpServLoc;
    cmdCtl.connect(tcpServ, SIGNAL(msgRecvd(QString)), SLOT(processMessage(const QString)));

    //QThread * thread = new QThread(&a); // thread owned by the application object
    //Notified * notified = new Notified; // can't have an owner before it's moved to another thread
    //tcpServ.moveToThread(thread);
    //cmdCtl.connect(&tcpServ, SIGNAL(msgRecvd(QString)), SLOT(processMessage(const QString)));
    //thread->start();
    //threads << thread;
    //tcpServ.connect((QObject*)&w1, SIGNAL(resultReady(const QString)), SLOT(printfSlot(const QString)));

    //connect(&w1, SIGNAL(resultReady(QString)), &tcpServ, SLOT(printf(const char*)));
    //tcpServ.start();

//    w1.start();

    pqList << new ProjectorQuery("192.168.1.10", "192.168.1.10");
    /*pqList << new ProjectorQuery("192.168.0.55", "192.168.0.55");
    pqList << new ProjectorQuery("192.168.0.14", "192.168.0.14");
    pqList << new ProjectorQuery("192.168.0.8", "192.168.0.8");
    pqList << new ProjectorQuery("192.168.0.86", "192.168.0.86");*/
    //pqList << new ProjectorQuery();
    //pqList << new ProjectorQuery();
    //pqList << new ProjectorQuery();

    foreach (ProjectorQuery* pq, pqList) {
        QThread *pqThread = new QThread(&a); // thread owned by the application object
        pq->moveToThread(pqThread);
        pq->connect(pqThread, SIGNAL(started()), SLOT(process()));
        //tcpServ.connect(pq, SIGNAL(messageReady(QString)), SLOT(printfSlot(const QString)));
//        QObject::connect(pq, &ProjectorQuery::newState,
//                         [pq](const QString &s) {
//            qInfo() << " projector "<< pq->name <<" in " << s <<"state";
//        });
        pqThread->start();
        threads << pqThread;
    }


    QSound warningSound("Content\\Success.wav");

    QStringList args1 ;
    args1.append("Content\\Video.avi");
    args1.append("/play");
    //args1.append("/fullscreen");
    args1.append("/close");

    QStringList args2 ;
    args2.append("Content\\Video.avi");
    args2.append("/open");
    //args2.append("/fullscreen");


    videoPlayer.setProgram("MPC-HC64\\mpc-hc64");
    videoPlayer.setArguments(args1);

    QHash<TCmdButton, QString> resultMap;
    resultMap.insert(cmdButton1, "But1");
    resultMap.insert(cmdButton2, "But2");
    resultMap.insert(cmdButtonCancel, "ButCancel");
    resultMap.insert(cmdTimeout, "timeout");
    resultMap.insert(cmdFinished, "finished");

     forever{
        qInfo() << "main> =====  ";
        qInfo() << "main> video player start black";        
        videoPlayer.close();
        videoPlayer.setArguments(args2);
        videoPlayer.start();
        qInfo() << "main> turn ON light";
        turnOnLight();
        qInfo() << "main> wait for light turn ON";
        //add wait for light turn ON
        qInfo() <<"main> power off projectors";
        TCmdButton ret = waitForProjectorsOffOrCancel();
        qInfo() <<"main> waitForProjectorsOff end with " << resultMap[ret];

        qDebug("main> wait for command");
        TCmdButton mainCmd = waitForBut1CmdOrBut2Cmd();
        qInfo()<<"main> recvd " << resultMap[mainCmd] << " cmd";
        warningSound.play();
        qInfo() <<"main> power on projectors";
        ret = waitForProjectorsOnOrCancel();
        qInfo() <<"main> waitForProjectorsOn end with " << resultMap[ret];
        if(ret == cmdButtonCancel)
            continue;
        if(mainCmd == cmdButton1){
            qInfo() <<"main> wait for timeout";
            ret = waitForTimeoutOrCancelCmd(10);
            qInfo() <<"main> waiting timeout end with " << resultMap[ret] << " cmd";
            if(ret == cmdButtonCancel)
                continue;
        }
        else if(mainCmd = cmdButton2){
            qInfo() <<"main> wait for 2nd push but2 or cancel";
            //ret = waitForTimeoutOrCancelCmd(10);
        }

        warningSound.stop();

        qInfo() << "main> turn OFF light";
        turnOffLight();
        qInfo() << "main> wait for light turn OFF";
        //add wait for light turn OFF
        videoPlayer.close();
        videoPlayer.setArguments(args1);
        qInfo() <<"main> video player start";
        videoPlayer.start();
        ret = waitForFinishPlayOrCancel();
        qInfo() <<"main> video player end with " << resultMap[ret];

        continue;
    }

    return a.exec();
}


QMutex lock;
void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    lock.lock();
    QByteArray localMsg = msg.toLocal8Bit();
//    switch (type) {
//    case QtDebugMsg:
//        fprintf(stderr, "Debug: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
//        break;
//    case QtInfoMsg:
//        fprintf(stderr, "Info: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
//        break;
//    case QtWarningMsg:
//        fprintf(stderr, "Warning: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
//        break;
//    case QtCriticalMsg:
//        fprintf(stderr, "Critical: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
//        break;
//    case QtFatalMsg:
//        fprintf(stderr, "Fatal: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
//        abort();
//    }
    printf("%s\n", localMsg.constData());

    if(tcpServ != NULL){
        tcpServ->printf(msg+'\n');
    }
    lock.unlock();
}

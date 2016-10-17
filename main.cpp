#include <QCoreApplication>
#include <QThread>
#include <QTcpServer>
#include <QTcpSocket>

#include "telnettcpserver.h"
#include "ProjectorQuery.h"
#include "CommandController.h"
#include "ConsoleInputProcessor.h"
#include "waitScenarios.h"
#include "lightserver.h"

#include <qtconcurrentrun.h>
#include <QEventLoop>

#include <QSound>
#include <QTimer>
#include <QProcess>
#include <QSettings>
#include <QFileInfo>

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

QList<ProjectorQuery*> pqList;
TelnetTcpServer *tcpServ = NULL;


void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg);

void turnOnLight()
{
    qInfo() << "light_on" ;
    tcpServ->printfLC("light ON\r\n");
}

void turnOnLightFast()
{
    qInfo() << "light_on" ;
    tcpServ->printfLC("light ON fast\r\n");
}


void turnOffLight()
{
    qInfo() << "light_off" ;
    tcpServ->printfLC("light OFF\r\n");
}

void setFastBlink()
{
    tcpServ->printfLC("blink fast\r\n");
}

void setNormalBlink()
{
    tcpServ->printfLC("blink normal\r\n");
}

void setGreenLed()
{
    tcpServ->printfLC("green\r\n");
}



int main(int argc, char *argv[])
{   
    qInstallMessageHandler(myMessageOutput);
    QCoreApplication a(argc, argv);

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


    //LightServer lightServ;
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

    if(QFileInfo("projSet.ini").exists() == true)
        qInfo("projSet.ini exist");
    else
        qInfo("projSet.ini not found");

    QSettings projSet("projSet.ini", QSettings::IniFormat, &a);

    //QString lcIP = projSet.value("")
    int turnOffTimeOut = projSet.value("projTurnOffTimeOut", 300).toInt();
    int projCount = projSet.beginReadArray("projectors");

    qInfo(" projectors in projSet.ini: %d ", projCount);
    qInfo(" pojectors turn off timeOut: %d", turnOffTimeOut);
    for(int i=0; i<projCount; i++){
        projSet.setArrayIndex(i);
        QString projIp = projSet.value("ip").toString();
        //qDebug("projector %d: %s", i, projIp.toLatin1());
        qDebug() << "projector" << i << ":" << projIp;
        pqList << new ProjectorQuery(projIp, turnOffTimeOut);
    }
    projSet.endArray();

//    QString lcIp = projSet.value("lightControllerIp").toString();
//    if(lcIp.size() == 0){
//        qDebug() << "lightControllerIp NULL";
//    }

    QString acceptPath = projSet.value("acceptSoundPath").toString();
    QString successPath = projSet.value("successSoundPath").toString();
    QString videoPath = projSet.value("videoPath").toString();
    QString videoPlayerPath = projSet.value("videoPlayerPath").toString();

    qInfo(qPrintable(QString("accept sound path: ")+ QString(acceptPath)));
    qInfo(qPrintable(QString("warning sound path: ")+ QString(successPath)));
    qInfo(qPrintable(QString("video path: ")+ QString(videoPath)));
    qInfo(qPrintable(QString("video player path: ")+ QString(videoPlayerPath)));

    TCmdButton ret;

    QSound warningSound(successPath);
    QSound acceptSound(acceptPath);

    QStringList args1 ;
    args1.append(videoPath);
    args1.append("/play");
    args1.append("/fullscreen");
    args1.append("/close");

    QStringList args2 ;
    args2.append(videoPath);
    args2.append("/open");
    args2.append("/fullscreen");
    QProcess videoPlayer;
    videoPlayer.setWorkingDirectory("C:\\Content\\MPC-HC64\\");
    videoPlayer.setProgram("mpc-hc64.exe");
    videoPlayer.setArguments(args1);

    //pqList << new ProjectorQuery("192.168.1.10", "192.168.1.10");
    /*pqList << new ProjectorQuery("192.168.0.55", "192.168.0.55");
    pqList << new ProjectorQuery("192.168.0.14", "192.168.0.14");
    pqList << new ProjectorQuery("192.168.0.8", "192.168.0.8");
    pqList << new ProjectorQuery("192.168.0.86", "192.168.0.86");*/
    //pqList << new ProjectorQuery();
    //pqList << new ProjectorQuery();
    //pqList << new ProjectorQuery();

//    foreach (ProjectorQuery* pq, pqList) {
//        QThread *pqThread = new QThread(&a); // thread owned by the application object
//        pq->moveToThread(pqThread);
//        pq->connect(pqThread, SIGNAL(started()), SLOT(process()));
//        //tcpServ.connect(pq, SIGNAL(messageReady(QString)), SLOT(printfSlot(const QString)));
////        QObject::connect(pq, &ProjectorQuery::newState,
////                         [pq](const QString &s) {
////            qInfo() << " projector "<< pq->name <<" in " << s <<"state";
////        });
//        pqThread->start();
//        threads << pqThread;
//    }


    QHash<TCmdButton, QString> resultMap;
    resultMap.insert(cmdButton1, "But1");
    resultMap.insert(cmdButton2, "But2");
    resultMap.insert(cmdButtonCancel, "ButCancel");
    resultMap.insert(cmdTimeout, "timeout");
    resultMap.insert(cmdFinished, "finished");
    acceptSound.play();

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
        //qInfo() <<"main> power off projectors";
        // = waitForProjectorsStateOrCancel(pqList, offState);
        //qInfo() <<"main> waitForProjectorsOff end with " << resultMap[ret];

        qInfo() << "main> set normal blink";
        setNormalBlink();
        setGreenLed();

        acceptSound.play();
        qDebug("main> wait for command");
        TCmdButton mainCmd = waitForBut1CmdOrBut2Cmd();
        qInfo() << "main> set fast blink";
        setFastBlink();
        qInfo()<<"main> recvd " << resultMap[mainCmd] << " cmd";
        warningSound.play();
        qInfo() <<"main> power on projectors";
        powerOnProjectors();
        ret = waitForProjectorsStateOrCancel(pqList, onState);
        qInfo() <<"main> waitForProjectorsOn end with " << resultMap[ret];
        if(ret == cmdButtonCancel)
            continue;
        if(mainCmd == cmdButton1){
            int pauseSec = projSet.value("but1PauseSeconds", 3*60).toInt();
            qInfo() <<"main> wait for timeout" << pauseSec << " seconds";

            ret = waitForTimeoutOrCancelCmd(pauseSec);
            qInfo() <<"main> waiting timeout end with " << resultMap[ret] << " cmd";
            if(ret == cmdButtonCancel)
                continue;
        }
        else if(mainCmd = cmdButton2){
            qInfo() <<"main> wait for 2nd push but2 or cancel";
            waitForBut1CmdOrBut2Cmd();
            if(ret == cmdButtonCancel){
                qInfo() <<"main> cancel but - fast light on";
                turnOnLightFast();
                continue;
            }
        }

        warningSound.stop();

        qInfo() << "main> turn OFF light";
        turnOffLight();
        qInfo() << "main> wait for light turn OFF";
        //add wait for light turn OFF
        QThread::sleep(3);
        videoPlayer.close();
        videoPlayer.setArguments(args1);
        qInfo() <<"main> video player start";
        //ret = waitForFinishPlayOrCancel();
        videoPlayer.start();
        QThread::sleep(3);
        ret = waitForFinishPlayOrCancel(&videoPlayer);
        qInfo() <<"main> video player end with " << resultMap[ret];
        if(ret == cmdButtonCancel){
            qInfo() <<"main> cancel but - fast light on";
            turnOnLightFast();
        }

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

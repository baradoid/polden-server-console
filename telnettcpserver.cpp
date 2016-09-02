#include "telnettcpserver.h"
#include <QThread>
//#include <QTextCodec>

TelnetTcpServer::TelnetTcpServer(int port, QObject *parent) : QObject(parent),
    clientsInd(0)
{
    //moveToThread(this);    
    mTcpServer = new QTcpServer(this);
    connect(mTcpServer, SIGNAL(newConnection()),
            this, SLOT(slotNewConnection()));

    //connect(this, SIGNAL(printSignal(const char*)), this, SLOT(printfSlot(const char*)));

    if(mTcpServer->listen(QHostAddress::Any, port) == true){
        qInfo() << "TelnetTcpServer> server is started";
    } else {
        qInfo() << "TelnetTcpServer> server is not started";

    }

    sockSignalMapperReadyRead = new QSignalMapper(this);
    sockSignalMapperDisconnected = new QSignalMapper(this);

    connect(sockSignalMapperReadyRead, SIGNAL(mapped(int)), //SIGNAL(mapped(QString)),
            this, SLOT(slotServerRead(int)));
    connect(sockSignalMapperDisconnected, SIGNAL(mapped(int)), //SIGNAL(mapped(QString)),
            this, SLOT(slotClientDisconnected(int)));

    connect(this, SIGNAL(printSignal(const QString &)),
            this, SLOT(printfSlot(const QString &)));

}


void TelnetTcpServer::slotNewConnection()
{
    QTcpSocket *mTcpSocket;

    mTcpSocket = mTcpServer->nextPendingConnection();

    //sockList << mTcpSocket;
    sockHash[clientsInd] = mTcpSocket;
    sockAttr[clientsInd] = new TSocketAttr;

    sockAttr[clientsInd]->bWo = false;

    //mTcpSocket->write(qPrintable(QString("terminal %1 connected\n").arg(clientsInd)));

    //connect(mTcpSocket, &QTcpSocket::readyRead, this, &TelnetTcpServer::slotServerRead);
    //connect(mTcpSocket, &QTcpSocket::disconnected, this, &TelnetTcpServer::slotClientDisconnected);

    connect(mTcpSocket, SIGNAL(readyRead()), sockSignalMapperReadyRead, SLOT(map()));
    connect(mTcpSocket, SIGNAL(disconnected()), sockSignalMapperDisconnected, SLOT(map()));

    //connect(this, SIGNAL(msgRecvd(QString)), mTcpSocket, SLOT()
    sockSignalMapperReadyRead->setMapping(mTcpSocket, clientsInd);
    sockSignalMapperDisconnected->setMapping(mTcpSocket, clientsInd);    
    qDebug("TelnetTcpServer>terminal %d connected", clientsInd);
    clientsInd++;
}

void TelnetTcpServer::slotServerRead(int sockInd)
{
    //qDebug("slotServerRead %d", sockInd);
    QTcpSocket *sock = sockHash[sockInd];
    TSocketAttr *sAttr = sockAttr[sockInd];
    //Codec

    //QTextCodec *codec1 = QTextCodec::codecForName( "CP1251" );

    while(sock->bytesAvailable()>0)
    {
        QByteArray msg = sock->readAll();
        //sock->write(msg);

        //qDebug() << codec1->fromUnicode(msg);
        if( (msg == "wo\r\n") || (msg == "wo\r") || (msg == "wo\n") ){            
            qInfo("TelnetTcpServer>wo msg recvd! set but to green");
            sAttr->bWo = true;

            sock->write(QString("green\r\n").toLatin1());
        }
        else if(msg == "ping\r\n"){
            qDebug("TelnetTcpServer>ping request recvd!");
            sock->write(QString("pong\r\n").toLatin1());
        }
        else{
            QString qstrMsg = QString::fromUtf8(msg);
            emit msgRecvd(qstrMsg);
        }
    }
}

void TelnetTcpServer::slotClientDisconnected(int sockInd)
{
    qDebug("TelnetTcpServer>terminal %d disconnected", sockInd);
    QTcpSocket *sock = sockHash[sockInd];
    sock->close();
}

void TelnetTcpServer::printfSlot(const QString &s)
{
//    if(mTcpSocket != NULL){
//        if(mTcpSocket->isWritable() == true){
//            mTcpSocket->write(qPrintable(s));
//            mTcpSocket->write(qPrintable('\n'));
//        }
//    }

    QList<int> sockHashKeys = sockHash.keys();

    foreach (int key, sockHashKeys) {
        QTcpSocket *sock = sockHash[key];
        TSocketAttr *sAttr = sockAttr[key];

        if((sock->isWritable() == true) && (sAttr->bWo == false)){
            sock->write(qPrintable(s));
            sock->flush();
        }
    }

//    foreach (QTcpSocket *sock, sockList) {
//        if(sock->isWritable() == true){
//            sock->write(qPrintable(s));
//            sock->write(qPrintable('\n'));
//        }
//    }

    //qInfo("qTelnetServ %s", qPrintable(s));
}

//void TelnetTcpServer::printfWoSlot(const QString &s)
//{
////    if(mTcpSocket != NULL){
////        if(mTcpSocket->isWritable() == true){
////            mTcpSocket->write(qPrintable(s));
////            mTcpSocket->write(qPrintable('\n'));
////        }
////    }

//    QList<int> sockHashKeys = sockHash.keys();

//    foreach (int key, sockHashKeys) {
//        QTcpSocket *sock = sockHash[key];
//        TSocketAttr *sAttr = sockAttr[key];

//        if((sock->isWritable() == true) ){
//            sock->write(qPrintable(s));
//            sock->flush();
//        }
//    }

////    foreach (QTcpSocket *sock, sockList) {
////        if(sock->isWritable() == true){
////            sock->write(qPrintable(s));
////            sock->write(qPrintable('\n'));
////        }
////    }

//    //qInfo("qTelnetServ %s", qPrintable(s));
//}


void TelnetTcpServer::printf(const QString &s)
{
    emit printSignal(s);
}

void TelnetTcpServer::printfLC(const QString &s)
{
    QList<int> sockHashKeys = sockHash.keys();

    foreach (int key, sockHashKeys) {
        QTcpSocket *sock = sockHash[key];
        TSocketAttr *sAttr = sockAttr[key];

        if(sock->isWritable() == true){
            sock->write(qPrintable(s));
            sock->flush();
        }
    }
    QThread::msleep(500);
}



//void TelnetTcpServerThread::run()
//{
//    mTcpServer = new QTcpServer(this);
//    if(!mTcpServer->listen(QHostAddress::Any, 23)){
//        qDebug() << "server is not started";
//    } else {
//        qDebug() << "server is started";
//    }

//    forever{
//        mTcpServer->waitForNewConnection();
//        mTcpSocket = mTcpServer->nextPendingConnection();
//        forever{
//            mTcpSocket->write("lala");
//        }

//    }



//}

#ifndef TELNETTCPSERVER_H
#define TELNETTCPSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QSignalMapper>

typedef struct{
    bool bWo;
} TSocketAttr;

class TelnetTcpServer : public QObject
{
    Q_OBJECT
public:
    explicit TelnetTcpServer(int port, QObject *parent = 0);


signals:

public slots:
    void slotNewConnection();
    void slotServerRead(int);
    void slotClientDisconnected(int);

    void printfSlot(const QString &s);

    //void printf(const char *str);
private:
    QTcpServer * mTcpServer;
    //QTcpSocket * mTcpSocket;

    //QList<QTcpSocket*> sockList;

    QHash<int, QTcpSocket*> sockHash;
    QHash<int, TSocketAttr*> sockAttr;

    QSignalMapper *sockSignalMapperReadyRead;
    QSignalMapper *sockSignalMapperDisconnected;
    int clientsInd;

signals:
    void printSignal(const char *str);
    void msgRecvd(const QString &msg);
};

//class TelnetTcpServerThread : public QThread
//{
//public:
//    void printf(const char *str);

//private:
//    QTcpServer * mTcpServer;
//    QTcpSocket * mTcpSocket;
//};

#endif // TELNETTCPSERVER_H

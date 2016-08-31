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

public slots:
    void slotNewConnection();
    void slotServerRead(int);
    void slotClientDisconnected(int);



    void printf(const QString &msg);
    //void printfLc(const QString &msg);
    void printfLC(const QString &s);
private slots:
    void printfSlot(const QString &s);

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
    void printSignal(const QString &msg);
    void msgRecvd(const QString &msg);
    //void printfWoSlot(const QString &s);
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

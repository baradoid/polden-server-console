#ifndef LIGHTSERVER_H
#define LIGHTSERVER_H

#include <QObject>

#include <QTcpServer>
#include <QTcpSocket>

class LightServer : public QObject
{
    Q_OBJECT
public:
    explicit LightServer(QObject *parent = 0);

signals:

public slots:
private:
    QTcpServer * mTcpServer;
};

#endif // LIGHTSERVER_H

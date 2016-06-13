#pragma once

#include <QObject>
#include <QThread>

class ConsoleInputProcessor : public QObject
{
    Q_OBJECT
public:
    explicit ConsoleInputProcessor(QObject *parent = 0);

public slots:
    void process();

signals:
    void msgRecvd(const QString &msg);
};

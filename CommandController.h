#pragma once

#include <QObject>

typedef enum{
    cmdButton1,
    cmdButton2,
    cmdButtonCancel,
    cmdTimeout,
    cmdFinished
} TCmdButton;
class CommandController : public QObject
{
    Q_OBJECT
public:
    explicit CommandController(QObject *parent = 0);

    void waitForButton1();
    void waitForButton2();
    void waitForButton2OrCancel();
    void waitForCancel();
signals:
    void button1();
    void button2();
    void buttonCancel();

    void lightTurnedOff();
    void lightTurnedOn();


public slots:
    void processMessage(const QString &msg);
};

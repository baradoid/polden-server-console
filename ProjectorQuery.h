#ifndef PROJECTORQUERY_H
#define PROJECTORQUERY_H

#include <QObject>
#include <QReadWriteLock>

#define POWER_ON_QUERY "%1POWR=1\r"
#define POWER_OFF_QUERY "%1POWR=0\r"
#define GET_STATE_QUERY "%1POWR ?\r"

#define POWER_ON_CMD "%1POWR 1\r"
#define POWER_OFF_CMD "%1POWR 0\r"

typedef enum{
        unknownState = -1,
        onState = 1,
        warmUpState = 3,
        coolingState = 2,
        offState = 0
} TProjState;

class ProjectorQuery : public QObject
{
    Q_OBJECT
public:
    explicit ProjectorQuery(QString _ip/*, QString _name*/);
    void on();
    void off();


    void waitForPowerOn();
    void waitForPowerOff();

    TProjState getState();

    QString ip/*, name*/;

signals:       
    void finished();
    void error(QString err);
    void messageReady(const QString &s);

    void powerOnState();
    void powerOffState();

    void newState(const QString &s);

public slots:
    void process();



private:
    void sendCmd(QString cmd);

    QReadWriteLock lock;
    TProjState lastState;
    QList<TProjState> sendState;

};

#endif // PROJECTORQUERY_H

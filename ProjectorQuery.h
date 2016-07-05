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
        unknownState,
        onState,
        warmUpState,
        coolingState,
        offState
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


signals:       
    void finished();
    void error(QString err);
    void messageReady(const QString &s);

    void powerOnState();
    void powerOffState();

    void newState(const QString &s);

public slots:
    void process();


public:
    QString ip/*, name*/;
private:
    QReadWriteLock lock;
    TProjState lastState;
    QList<TProjState> sendState;

};

#endif // PROJECTORQUERY_H

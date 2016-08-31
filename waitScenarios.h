#pragma once
#include <QList>
#include <QProcess>

TCmdButton waitForTimeoutOrCancelCmd(int secTimeout);
TCmdButton waitForFinishPlayOrCancel(QProcess *videoPlayer);
TCmdButton waitForBut1CmdOrBut2Cmd();
TCmdButton waitForProjectorsStateOrCancel(QList<ProjectorQuery*> &pqList, TProjState stateWaitFor);
TCmdButton waitForLightOffOrCancel();
TCmdButton waitForLightOnOrCancel();

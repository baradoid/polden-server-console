#pragma once
#include <QList>

TCmdButton waitForTimeoutOrCancelCmd(int secTimeout);
TCmdButton waitForFinishPlayOrCancel();
TCmdButton waitForBut1CmdOrBut2Cmd();
TCmdButton waitForProjectorsStateOrCancel(QList<ProjectorQuery*> &pqList, TProjState stateWaitFor);
TCmdButton waitForLightOffOrCancel();
TCmdButton waitForLightOnOrCancel();

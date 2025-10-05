#!/bin/bash

Exec=getData

daqPid=`ps -aef | awk '{print $2 " " $8}' | grep $Exec | awk '{print $1}'`
sleep 1

if [[ $daqPid != "" ]]; then
  echo "'$Exec' is running: PID is $daqPid"
  sleep 1
  echo "killing process $daqPid"
  sleep 1
  kill -9 $daqPid
  echo "process $daqPid killed"
  sleep 1

  DATE=`date +%Y.%m.%d`
  HOUR=`date +%H:%M`

  echo "'$Exec' killed at " $DATE " " $HOUR
  sleep 1

else
  DATE=`date +%Y.%m.%d`
  HOUR=`date +%H:%M`

  echo "'$Exec' is not running at " $DATE " " $HOUR
  sleep 1
fi

echo "last data file is" `ls -rt ~/TB2025data/RawData-2025*.txt | tail -1`

daqPid=`ps -aef | awk '{print $2 " " $8}' | grep $Exec | awk '{print $1}'`
if [[ $daqPid != "" ]]; then
  echo "Stopping was unsuccessful: '$Exec' is still running: PID is $daqPid"
  echo "Please  was unsuccessful: '$Exec' is still running: PID is $daqPid"
  sleep 1
fi

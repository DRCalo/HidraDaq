#!/bin/bash

DATE=`date +%Y.%m.%d`
HOUR=`date +%H:%M`

Exec=getData
StopScript=stopBobDaq.sh

cd /home/hidra/working/

daqPid=`ps -aef | awk '{print $2 " " $8}' | grep $Exec | awk '{print $1}'`
sleep 1

if [[ $daqPid != "" ]]; then
  echo "'$Exec' is running: PID is $daqPid"
  sleep 1
  echo "please stop it before starting a new run"
  sleep 1
  echo "you can use the 'stopHidra' alias command to stop it gracefully"
  sleep 1

else
  echo "'$Exec $1 $2 $3' starting at " $DATE " " $HOUR

  $Exec $1 $2 &>> logfile.all.txt &
  sleep 1

  echo "'$Exec $1 $2 $3' started"
  sleep 1

  echo "you can use the 'stopHidra' alias command to stop it gracefully"

  daqPid=`ps -aef | awk '{print $2 " " $8}' | grep $Exec | awk '{print $1}'`
  sleep 1

  if [[ $daqPid != "" ]]; then
    echo "'$Exec' is running: PID is $daqPid"
    sleep 1
    echo -n " ... 3"
    sleep 1
    echo -n " ... 2"
    sleep 1
    echo -n " ... 1"
    sleep 1
    echo

    top -p $daqPid
  else
    echo " "
    echo "             *** '$Exec' is NOT running ***"
    sleep 1
    echo " "
    echo "     *** please take care: something went WRONG ***"
    echo " "
    sleep 1
  fi

fi

echo "last data file is" `ls -rt ~/TB2025data/RawData-2025*.txt | tail -1`


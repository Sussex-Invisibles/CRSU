#!/bin/bash
# Author: Martti Nirkko, 25/04/2018
# Usage: Use hostname as input argument (e.g. www.snolab.ca)
HOST=${1}

# Find out whether Unix system is Linux or Mac
SYSTEM=`uname -a | awk '{print $1}'`
if [ $SYSTEM == "Linux" ]; then
  cmd=date
else
  # Must have coreutils installed on Mac!
  cmd=gdate
fi

# Monitor latency to host (indefinitely)
IP=`ping -c 1 ${HOST} | head -1 | awk '{print $3}'`
echo "# PING ${HOST} ${IP}"
echo "# Time_s Latency_ms"
while (true) do
  t=`${cmd} +%s.%N` # remove .%N if you don't have gdate on Mac
  p=`ping -c 1 ${HOST} | head -2 | tail -1 | sed -n 's/.*time=\(.*\)\ ms.*/\1/p'`
  if [ -z $p ]; then p=-1; fi # set ping to -1 in case of failure
  echo $t $p
  sleep 1
done


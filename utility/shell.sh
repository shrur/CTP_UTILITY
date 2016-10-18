#!/bin/sh
echo "copying from Log Folder"
cd /opt/app/brm/7.5/sys/test
echo "Started the transferring files from PROD to FTP server"
echo "transferring Started"
user=pin
pass=Unix@123
host=10.75.253.102
scp -r test9 $user@$host:/home/pin/portal/7.5/sys/test
if [ $? -eq 0 ]
then
  echo "Success.log">>success.log
else 
echo "Error.log">>Error.log
fi

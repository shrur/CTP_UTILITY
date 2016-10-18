#!/bin/bash
exec 3>&1 4>&2
trap 'exec 2>&4 1>&3' 0 1 2 3
exec 1>log.out1 2>&1
echo "pin_export export utility started...."
mkdir -p /opt/app/brm/7.5/sys/test/pin_export_files1
while read -r LINE; do
 cp $LINE /opt/app/brm/7.5/sys/test/pin_export_files1
done < files.txt

IFS=':' read f1 f2 <<< "$(head -n 1 files.txt)" 
        echo "User     : $f1"
        echo "Hostname: $f2"
# cd /opt/app/brm/7.5/sys/test
 tar -czvf pin_export_files1.tar.gz pin_export_files1
 echo "Started the transferring files from PROD to FTP server"
 echo "transferring Started"
 scp pin_export_files.tar.gz $f1@$f2:/tmp
 echo "transfer done"
cd /opt/app/brm/7.5/sys/test/utility
##done < files.txt 

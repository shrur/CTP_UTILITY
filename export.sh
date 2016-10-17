#!/bin/bash
exec 3>&1 4>&2
trap 'exec 2>&4 1>&3' 0 1 2 3
exec 1>log.out 2>&1
echo "pin_export export utility started...."
mkdir -p "$PIN_HOME"/sys/test/pin_export_files
now=$(date +"%Y.%m.%d.%S.%N")
sed -n '2,$ p' pin_export.conf | while read -r LINE 
do
 pin=`echo $PIN_HOME`
 echo "$pin"
 FILE="$pin/$LINE"
 echo $FILE
 if [ -f "$FILE" ];
 then
	echo "File $FILE exist."
	echo "Copying $FILE"
 	cp $FILE "$PIN_HOME"/sys/test/pin_export_files
	echo "$FILE - File Copied" 
	if [ $? -eq 0 ]
	then
	  echo "$FILE , Success ">>Success.$now.log
	fi
 else
	echo "File $FILE Does Not Exist" 
	if [ $? -eq 0 ]
	then
	  echo "$FILE , File not Found ">>Error.$now.log
 	fi
 fi
done

IFS=':' read f1 f2 <<< "$(head -n 1 pin_export.conf)"
        echo "User     : $f1"
        echo "Hostname : $f2"
cp pin_export.conf "$PIN_HOME"/sys/test/pin_export_files 
cd "$PIN_HOME"/sys/test
	#tar -czvf "$PIN_HOME"/sys/test/pin_export_files/pin_export_files.tar.gz pin_export_files
	tar -czvf pin_export_files.tar.gz pin_export_files
	echo "Started the transferring files from PROD to FTP server...."
	echo "Transferring Started...."
cd "$PIN_HOME"/sys/test
 	scp pin_export_files.tar.gz $f1@$f2:/tmp
 	echo "Transfer Done......"
cd "$PIN_HOME"/sys/test
#	rm -rf pin_export_files

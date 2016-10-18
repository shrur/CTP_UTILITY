#!/bin/bash
exec 3>&1 4>&2
trap 'exec 2>&4 1>&3' 0 1 2 3
exec 1>import_log.out 2>&1
echo "pin_import import utility started...."
cp /tmp/pin_export_files.tar.gz "$PIN_HOME"/sys/test
cd "$PIN_HOME"/sys/test
tar -xvzf pin_export_files.tar.gz
now=$(date +"%Y.%m.%d.%S")
sed -n '2,$ p' "$PIN_HOME"/sys/test/pin_export_files/pin_export.conf | while read -r LINE
do
 #FILE=$(cat "$LINE")
 #echo "$FILE"
 pin=`echo $PIN_HOME`
 echo "$pin"
 FILE="$pin/$LINE"
 echo $FILE
 if [ -f "$FILE" ];
 then
	
        echo "File $FILE exist."
        echo "$FILE Backup started"
        cp $FILE $FILE"_$now" 
        echo "$FILE - Backup completed"
	dirname=`dirname $FILE`
	echo "Directory Name: $dirname"
	fname=`basename $FILE`
	echo "Filename : $fname"
	cp "$PIN_HOME"/sys/test/pin_export_files/$fname $dirname
	cd "$PIN_HOME"/sys/test
	case $fname in
	*"pin_glid"*)
				echo "Deploying pin_glid......"
				load_pin_glid -dv $FILE
				;;
	*"pin_rum"*)
				echo "Deploying pin_rum....."
				load_pin_rum -dv $FILE	
				;;

	*"pin_usage_map"*)
				echo "Deploying pin_usage_map....."
				load_event_map -dv $FILE
				;;
	*"pin_beid"*)
				echo "Deploying pin_beid...."
				load_pin_beid  -dv $FILE
				;;
	*"pin_spec_rates"*)
				echo "Deploying pin_spec_rates...."
				load_pin_spec_rates $FILE
				;;

	*"pin_impact_category"*)
				echo "Deploying pin_impact_category....."
				load_pin_impact_category $FILE
				;;

	*"bus_params"*)
				echo "Deploying bus_params....."
				pin_bus_params $FILE
				;;

	*"pin_glchartaccts"*)
				echo "Deploying pin_glchartaccts....."
				load_pin_glchartaccts -dv $FILE
				;;
	*"en_US"*)
				echo "Deploying reason_code....."
				load_localized_strings -vf $FILE
				;;

	*"item_tags"*)
				cho "Deploying item_tags....."
				load_config_item_tags -v $FILE
				;;

	*"item_types"*)
				echo "Deploying item_types....."
				load_config_item_types -v $FILE
				;;

	*)
			echo "No files matching"
			;;
	esac
        if [ $? -eq 0 ]
        then
          echo "$FILE , Success ">>"$PIN_HOME"/sys/test/utility/Success_import.$now.log
        fi
 else
        echo "File $FILE Does Not Exist"
        if [ $? -eq 0 ]
        then
          echo "$FILE , File not Found ">>"$PIN_HOME"/sys/test/utility/Error_import.$now.log
        fi
 fi
done

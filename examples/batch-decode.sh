#!/bin/bash

dirflag='false'
fileflag=''
files=''
msg_type='rrc'
log_format='json'
total_file=0
counter=0

out_file="$PWD/batch-decode-log.txt"

while getopts 'd:f:l:t:o:vh' opt; do
  case "${opt}" in
    d)
        dirflag='true'
        dir_in="${OPTARG}" ;;
    l) in_file="${OPTARG}" ;;
    f) log_format="${OPTARG}" ;;
    t) msg_type="${OPTARG}" ;;
    o) out_file="${OPTARG}" ;;
    v) verboseflag='true' ;;
    h)
        echo "** Batch cellular log decoder for MobileInsight **"
        echo " "
        echo "  Author : Zengwen Yuan"
        echo "  Date   : 2016-04-22"
        echo "  Rev    : 1.1"
        echo "  Usage  : ./batch-decode.sh -d [dir] (-f [decode_format] -t [msg_type] -o [output_file])"
        echo " "
        echo "Options:"
        echo "  -h, --help              show this brief help"
        echo "  -d, --dir=DIR           decode all logs inside a directory"
        echo "  -l, --log=FILE_NAME     specify a single log file to decode (not implemented at this moment)"
        echo "  -f, --format=LOG_FORMAT specify decoded log format. Default: xml, options: [raw|xml|json]"
        echo "  -t, --type=MSG_TYPE     specify decoding message type. Default: rrc, options: [rrc|nas|mac|phy|rrcnas|4g|3g]"
        echo "  -o, --ouput=OUT_FILE    specify output filename. Default: [current_dir]/batch-decode-log.txt"
        exit 0
        ;;
    *) error "Unexpected option ${flag}" ;;
  esac
done

function loop() {
    for file in "$1"/*
    do
        if [ -d "$file" ]; then
            # echo "$file"
            # python kpi-manager-test.py ${file} >> $out_file
            loop "$file"
        elif [[ ${file: -5} == ".qmdl" || ${file: -7} == ".mi2log" || ${file: -7} == ".mi3log"  ]]; then
            echo "###[new log] $file###" >> $out_file
            # python decode-milog.py ${file} ${msg_type} ${log_format} >> $out_file
            # python lte_ho_config_analyzer.py ${file} ${msg_type} ${log_format} >> $out_file
            python kpi-manager-test.py ${file} ${msg_type} ${log_format} >> $out_file
            # python decode-milog-ho.py ${file} ${msg_type} ${log_format} >> $out_file
            counter=$((counter+1))
            echo "###end $file###" >> $out_file
            echo "[$counter/$total_file] Processing $file"
        else
            echo "Skipping for $file"
            # total_file=$((total_file-1))
        fi
    done
}

start_time="`date +[%Y-%m-%d\ %H:%M:%S]`";
echo "Decoding msg_type = ["$msg_type"] to" $out_file  >> $out_file;
echo "Log root directory:" $dir_in >> $out_file;

if $dirflag; then
    total_file=$(find $dir_in -type d | wc -l)
    loop $dir_in
fi

complete_time="`date +[%Y-%m-%d\ %H:%M:%S]`";
echo "Job finished! Start time = "$start_time", Complete time =" $complete_time >> $out_file;
echo "Job finished! Start time = "$start_time", Complete time =" $complete_time

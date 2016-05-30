#!/bin/bash

CH_PATH=$1
MD5=ABCD
TMPMD5=EFGH
NUM=0
OKNUM=0
FAILEDNUM=0

if [ -z "${CH_PATH}" ]
then
        echo "Please input the path!"
        exit
fi

if [ ! -d "${CH_PATH}" ]
then
        echo "${CH_PATH} is not a directry!"
        exit
fi

X=${CH_PATH: -1}
if [ $X = "/" ]
then
        FILES=`ls ${CH_PATH}/*`
fi

#echo ${FILES}


for FILE in $FILES
do
#        echo "${FILE}"

        if [ -z "${FILE}" ]
        then
                continue
        fi

        if [ ! -f ${FILE} ]
        then
                echo "${FLLE} is not a regula file!"
                continue
        fi

        if [ "${MD5}" = "ABCD" ]
        then
                MD5=`md5sum ${FILE}|cut -d' ' -f 1`
                echo "The standard md5 is ${MD5}"
                let NUM=$NUM+1
                continue
        fi

        TMPMD5=`md5sum ${FILE}|cut -d' ' -f 1`
        FFF=`echo ${FILE}|awk -F '//' '{print $NF}'`
        if [ "${TMPMD5}" = "$MD5" ]
        then
                let NUM=$NUM+1
                printf "%-10s %-20s %-42s%s\n" "ID: $NUM" "Filename: ${FFF}" "MD5: ${TMPMD5}" "Status: OK!"
        else
                let NUM=$NUM+1
                let FAILEDNUM=$FAILEDNUM+1
                printf "%-10s %-20s %-42s%s\n" "ID: $NUM" "Filename: ${FFF}" "MD5: ${TMPMD5}" "Status: Failed!"
        fi

done

let OKNUM=$NUM-$FAILEDNUM
if (($OKNUM==$NUM && $NUM!=0))
then
        RES_STR="All successful!"
else
        RES_STR="FAILED!"
fi

echo ""
echo "Sumary:"
echo "          Files : ${NUM}"
echo "          OK    : ${OKNUM}"
echo "          Failed: ${FAILEDNUM}"
echo ""
echo "          Result: ${RES_STR}"


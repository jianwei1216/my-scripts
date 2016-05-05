#!/bin/sh
filepath=$(cd "$(dirname "$0")"; pwd)
cp -r "$filepath"/etc/snmpd* /etc/
cp -r "$filepath"/etc/snmp/* /etc/snmp/
cp -r "$filepath"/etc/udev/rules.d/* /etc/udev/rules.d/
cp -r "$filepath"/usr/lib/* /usr/lib/
cp -r "$filepath"/usr/local/* /usr/local/
cp -r "$filepath"/usr/sbin/* /usr/sbin/
cp -r "$filepath"/usr/share/mibs/* /usr/share/snmp/mibs/
chmod +x /usr/local/digiserver/failure.pyc /usr/local/digiserver/mdadmclient.pyc /usr/local/digiserver/mdadm.sh /usr/local/digiserver/udevclient.pyc
chmod +x /usr/lib/hacking-bash.sh /usr/lib/snmpd-connector-lib.sh
chmod +x /usr/sbin/snmpd-mdraid-connector /usr/sbin/snmpd-smartctl-connector /usr/sbin/update-smartctl-cache /usr/sbin/update-mdraid-cache /usr/sbin/interfacestate/getinterface.py /usr/sbin/storpoolused/getstorpoolused.py
chmod +x /usr/bin/report_client.py



Summary: report manager for digioceanfs
Name: digioceanfs_reporter
Version: 3.8.0
Release: 3.el7 
Source0: %{name}-%{version}.tar.gz
License: GPL
Requires: digioceanfs , net-snmp,  net-snmp-devel, net-snmp-utils, smartmontools
Group: Development/Tools
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-buildroot
%description
A long decription of the project

%prep

%setup -q -n digioceanfs_reporter-%{version}

%install
rm -rf $RPM_BUILD_ROOT
mkdir -p $RPM_BUILD_ROOT/etc/
mkdir -p $RPM_BUILD_ROOT/usr/bin/
mkdir -p $RPM_BUILD_ROOT/etc/systemd/system/multi-user.target.wants/
mkdir -p $RPM_BUILD_ROOT/usr/
mkdir -p $RPM_BUILD_ROOT/usr/share/snmp/mibs/
python mkpyc.py ./
cp -rf ./etc/* $RPM_BUILD_ROOT/etc/
cp -rf ./usr/* $RPM_BUILD_ROOT/usr/
cp -rf ./usr/share/mibs/* $RPM_BUILD_ROOT/usr/share/snmp/mibs/
cp ./digioceanfs-reporter $RPM_BUILD_ROOT/usr/bin/
cp ./digioceanfs-reporter.service $RPM_BUILD_ROOT/etc/systemd/system/multi-user.target.wants/

%post
#chmod exe file mode
chmod +x /usr/local/digiserver/failure.pyc /usr/local/digiserver/mdadmclient.pyc /usr/local/digiserver/mdadm.sh /usr/local/digiserver/udevclient.pyc
chmod +x /usr/lib/hacking-bash.sh /usr/lib/snmpd-connector-lib.sh
chmod +x /usr/sbin/snmpd-mdraid-connector /usr/sbin/snmpd-smartctl-connector /usr/sbin/update-smartctl-cache /usr/sbin/update-mdraid-cache /usr/sbin/interfacestate/getinterface.pyc /usr/sbin/storpoolused/getstorpoolused.pyc
chmod +x /usr/bin/report_client.pyc
#add service start when server start
chmod +x /etc/init.d/digioceanfs-reporter
chkconfig --add digioceanfs-reporter 
chkconfig --level 235 digioceanfs-reporter on
#update-rc.d node-manager defaults
ln -s /etc/init.d/digioceanfs-reporter /usr/bin/
cp /etc/snmp/snmpd.conf.replace /etc/snmp/snmpd.conf
cp /etc/snmp/snmptrapd.conf.replace /etc/snmp/snmptrapd.conf

chmod +x /usr/bin/digioceanfs-reporter
systemctl enable digioceanfs-reporter

%clean
rm -rf $RPM_BUILD_ROOT

%files
%attr(0755, apache, apache) /usr/local/digiserver/*
%attr(0440, root, root) /etc/snmp/*
%attr(0440, root, root) /usr/sbin/nodesnmpscript/*
%attr(0755, root, root) /usr/bin/digioceanfs-reporter
%attr(0755, root, root) /usr/lib/hacking-bash.sh 
%attr(0755, root, root) /usr/lib/snmpd-connector-lib.sh 
%attr(0755, root, root) /usr/sbin/snmpd-mdraid-connector 
%attr(0755, root, root) /usr/sbin/update-smartctl-cache 
%attr(0755, root, root) /usr/sbin/update-mdraid-cache 
%attr(0755, root, root) /usr/sbin/interfacestate/getinterface.pyc 
%attr(0755, root, root) /usr/sbin/storpoolused/getstorpoolused.pyc 
%attr(0755, root, root) /usr/bin/report_client.pyc 
%attr(0755, root, root) /etc/snmpd-mdraid-connector
%attr(0755, root, root) /etc/snmpd-smartctl-connector
%attr(0755, root, root) /etc/udev/rules.d/95-me.rules
%attr(0755, root, root) /etc/udev/rules.d/20-usb-serial-gsm.rules
%attr(0755, root, root) /usr/sbin/interfacestate/interface.pyc
%attr(0755, root, root) /usr/sbin/interfacestate/snmp_passpersist.pyc
%attr(0755, root, root) /usr/sbin/interfacestate/utils.pyc
%attr(0755, root, root) /usr/sbin/snmpd-smartctl-connector
%attr(0755, root, root) /usr/sbin/storpoolused/getused.pyc
%attr(0755, root, root) /usr/sbin/storpoolused/snmp_passpersist.pyc
%attr(0755, root, root) /usr/sbin/storpoolused/utils.pyc
%attr(0755, root, root) /usr/share/mibs/*
%attr(0755, root, root) /usr/share/snmp/mibs/HACKING-SNMP-MIB
%attr(0755, root, root) /usr/share/snmp/mibs/JS-DISKDROP-MIB
%attr(0755, root, root) /usr/share/snmp/mibs/JS-DISKWARNING-MIB
%attr(0755, root, root) /usr/share/snmp/mibs/JS-FILEWARNING-MIB
%attr(0755, root, root) /usr/share/snmp/mibs/JS-INTERFACESTATE-MIB
%attr(0755, root, root) /usr/share/snmp/mibs/JS-MD-RAID-MIB
%attr(0755, root, root) /usr/share/snmp/mibs/JS-NETLINK-MIB
%attr(0755, root, root) /usr/share/snmp/mibs/JS-PING-MIB
%attr(0755, root, root) /usr/share/snmp/mibs/JS-RAIDFAIL-MIB
%attr(0755, root, root) /usr/share/snmp/mibs/JS-RAIDWARNING-MIB
%attr(0755, root, root) /usr/share/snmp/mibs/JS-SMARTCTL-MIB
%attr(0755, root, root) /usr/share/snmp/mibs/JS-STORPOOLUSED-MIB
%attr(0755, root, root) /usr/share/snmp/mibs/JS-SYSTEMDISK-MIB
%attr(0755, root, root) /usr/share/snmp/mibs/JS-DISKTEMP-MIB
%attr(0755, root, root) /usr/share/snmp/mibs/JS-SYSMSG-MIB
%attr(0755, root, root) /usr/share/snmp/mibs/JS-GLOBALINFO-MIB
%attr(0755, root, root) /usr/share/snmp/mibs/JS-NODEINFO-MIB
%attr(0755, root, root) /usr/share/snmp/mibs/JS-SERVICEINFO-MIB

%attr(0755, root, root) /etc/systemd/system/multi-user.target.wants/digioceanfs-reporter.service

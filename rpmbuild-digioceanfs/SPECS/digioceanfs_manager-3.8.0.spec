Summary: cmd manager for digioceanfs
Name: digioceanfs_manager
Version: 3.8.0
Release: 3.el7 
Source0: %{name}-%{version}.tar.gz 
License: GPL 
Requires: cups-devel, lm_sensors-devel, lm_sensors-libs, lsof, mdadm, nfs-utils, nfs-utils-lib, openssl-devel, perl, pexpect, pkgconfig, policycoreutils-python, pyserial, python-flup, python-pyinotify, python-simplejson, python-sysv_ipc, samba, samba-common, samba-winbind-clients, smartmontools, xfsprogs, zlib-devel, libaio-devel, file, dmidecode, digioceanfs-cli, digioceanfs-server, digioceanfs-fuse, digioceanfs-api, digioceanfs-api-devel, digioceanfs-devel, digioceanfs-geo-replication, digioceanfs-libs, digioceanfs,digioceanfs_client , digioceanfs_reporter, ipmitool
Group: Development/Tools 
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-buildroot
%description 
A long decription of the project  

%prep 
%setup -q -n digioceanfs_manager-%{version}

%preun
node-manager stop

%pre
rm -r /usr/local/sbin/unfsd &>/dev/null
#unlink /sbin/showmount &>/dev/null
rm -r /etc/init.d/node-manager &>/dev/null
if [ ! -e /sbin/showmount ];then
   if which showmount;then
       real_path=`which showmount`
       ln -sf $real_path /sbin/showmount &> /dev/null
   else
       echo "** Error : Command showmount not found"
       exit 1
   fi
fi
sed -i '/bridge-nf-call/s/^/#/' /etc/sysctl.conf

sed -i /tcp_keepalive_intvl/d /etc/sysctl.conf
sed -i /tcp_keepalive_probes/d /etc/sysctl.conf
/sbin/sysctl -p

%post
echo "net.ipv4.tcp_keepalive_intvl = 30" >> /etc/sysctl.conf
echo "net.ipv4.tcp_keepalive_probes = 2" >> /etc/sysctl.conf
/sbin/sysctl -p
ldconfig
# rpc-auth-allow-insecure for digioceanfs
ok=`grep 'rpc-auth-allow-insecure' /etc/digioceanfs/digioceand.vol`
if [ -z "$ok" ]
then
	sed -i '4 a \ \ \ \ option rpc-auth-allow-insecure on' /etc/digioceanfs/digioceand.vol
fi

cd /usr/local/digioceanfs_manager &> /dev/null
bash dev.sh -d &> /dev/null 
bash dev.sh -s &> /dev/null 
bash dev.sh -n &> /dev/null

%install 
rm -rf $RPM_BUILD_ROOT
mkdir -p $RPM_BUILD_ROOT/usr/local/digioceanfs_manager/
mkdir -p $RPM_BUILD_ROOT/usr/local/digioceanfs_backup/
mkdir -p $RPM_BUILD_ROOT/etc/digioceanfs_manager/err_disks
mkdir -p $RPM_BUILD_ROOT/etc/digioceanfs_manager/reports
mkdir -p $RPM_BUILD_ROOT/etc/storage
mkdir -p $RPM_BUILD_ROOT/etc/cron.d/
mkdir -p $RPM_BUILD_ROOT/etc/logrotate.d/
mkdir -p $RPM_BUILD_ROOT/etc/systemd/system/multi-user.target.wants/
touch $RPM_BUILD_ROOT/etc/storage/newdevice
mkdir -p $RPM_BUILD_ROOT/root/.ssh/
touch $RPM_BUILD_ROOT/root/.ssh/id_rsa
touch $RPM_BUILD_ROOT/root/.ssh/authorized_keys

pushd ./conf/ssh
cat id_rsa > $RPM_BUILD_ROOT/root/.ssh/id_rsa
cat id_rsa.pub > $RPM_BUILD_ROOT/root/.ssh/authorized_keys
popd

pushd ./conf/etc
cp -r *  $RPM_BUILD_ROOT/etc/
popd

pushd ./core
python mkpyc.py ./
cp -r ./* $RPM_BUILD_ROOT/usr/local/digioceanfs_manager/
popd


%clean 
rm -rf $RPM_BUILD_ROOT  

%files
%attr(-,root,root) /usr/local/
%attr(-,root,root) /etc/storage/newdevice
%attr(-,root,root) /etc/digioceanfs_manager/
%attr(-,root,root) /etc/cron.d/logrotate
%attr(-,root,root) /etc/logrotate.d/messages
%attr(-,root,root) /etc/udev/rules.d/10-my.rules
%attr(-,root,root) /etc/systemd/system/multi-user.target.wants/node-manager.service 

%attr(0600, root, root) /root/.ssh/*

%changelog
* Wed Jul 17 2013 Luo Yi - 0.1.0-1
- Updated to release 1.0.0-3.
- Add marvell iocard support.
- Fix bug of wrong path of unfsd 
- Add afr nufa control 
- Add replace nodisk command 
- Add alarm feature(zxx-9-5)
- Fix bugs of deleting a service that used by ivcs

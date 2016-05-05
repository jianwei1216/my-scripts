Summary: web manager for digioceanfs
Name: digioceanfs_gui 
Version: 3.8.0
Release: 3.el7 
Source0: %{name}-%{version}.tar.gz 
License: GPL 
Requires: sudo, mod_fcgid, nginx, spawn-fcgi, libsemanage-python, digioceanfs_manager, digioceanfs
Group: Development/Tools 
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-buildroot
%description 
A long decription of the project  

%prep 

%setup -q -n digioceanfs_gui-%{version}

%install 
rm -rf $RPM_BUILD_ROOT
mkdir -p $RPM_BUILD_ROOT/usr/local/digioceanfs_gui/
mkdir -p $RPM_BUILD_ROOT/usr/local/digioceanfs_gui/sessions/
mkdir -p $RPM_BUILD_ROOT/etc/sudoers.d/
mkdir -p $RPM_BUILD_ROOT/etc/nginx/
mkdir -p $RPM_BUILD_ROOT/etc/nginx/conf.d/
mkdir -p $RPM_BUILD_ROOT/etc/systemd/system/multi-user.target.wants/
mkdir -p $RPM_BUILD_ROOT/usr/bin/
mkdir -p $RPM_BUILD_ROOT/var/log/digioceanfs_gui/
pushd ./core
python mkpyc.py ./
mkfifo libcommon/processing
mkfifo libcommon/monitor_pipe 
popd
cp -r ./core/* $RPM_BUILD_ROOT/usr/local/digioceanfs_gui/
cp -r ./install-conf/digioceanfs $RPM_BUILD_ROOT/etc/sudoers.d/
cp -r ./install-conf/OEM.conf $RPM_BUILD_ROOT/usr/local/digioceanfs_gui/conf
cp -r ./install-conf/nginx/digioceanfs.* $RPM_BUILD_ROOT/etc/nginx/
cp -r ./install-conf/nginx/digioceanfs.* $RPM_BUILD_ROOT/etc/nginx/
cp -r ./install-conf/nginx/gui-https.conf $RPM_BUILD_ROOT/etc/nginx/conf.d/
cp -r ./install-conf/digioceanfs-gui $RPM_BUILD_ROOT/usr/bin/
cp -r ./install-conf/digioceanfs-gui.service $RPM_BUILD_ROOT/etc/systemd/system/multi-user.target.wants/

%post
chown apache /var/log/digioceanfs_gui
chmod +x /usr/bin/digioceanfs-gui
digioceanfs-gui start

%clean 
rm -rf $RPM_BUILD_ROOT  

%files
%attr(0755, nginx, nginx) /usr/local/digioceanfs_gui/*
%attr(0755, nginx, nginx) /var/log/digioceanfs_gui
%attr(0440, root, root) /etc/sudoers.d/digioceanfs

%attr(0440, root, root) /usr/bin/digioceanfs-gui

%attr(0775, root, root) /etc/nginx/conf.d/gui-https.conf
%attr(0775, root, root) /etc/nginx/digioceanfs.crt
%attr(0775, root, root) /etc/nginx/digioceanfs.key
%attr(0775, root, root) /etc/systemd/system/multi-user.target.wants/digioceanfs-gui.service

%changelog
* Wed Jul 17 2013 Luo Yi - 0.1.0-1
- Add alarm feature(zxx-9-5)
- Fix bugs of deleting a service that used by ivcs 

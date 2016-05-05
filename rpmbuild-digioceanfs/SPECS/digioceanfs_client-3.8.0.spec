Summary: web manager for digioceanfs
Name: digioceanfs_client
Version: 3.8.0
Release: 3.el7 
Source0: %{name}-%{version}.tar.gz 
License: GPL 
Requires: digioceanfs
Group: Development/Tools 
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-buildroot
%description 
A long decription of the project  

%prep 

%setup -q -n digioceanfs_client-%{version}

%install 
rm -rf $RPM_BUILD_ROOT
mkdir -p $RPM_BUILD_ROOT/usr/local/digioceanfs_client/
mkdir -p $RPM_BUILD_ROOT/usr/bin/
mkdir -p $RPM_BUILD_ROOT/etc/systemd/system/multi-user.target.wants/
python mkpyc.py ./
cp -r ./* $RPM_BUILD_ROOT/usr/local/digioceanfs_client/
cp ./digioceanfs-client $RPM_BUILD_ROOT/usr/bin/
cp ./digioceanfs-client.service $RPM_BUILD_ROOT/etc/systemd/system/multi-user.target.wants/

%post
chmod +x /usr/bin/digioceanfs-client
systemctl enable digioceanfs-client

%clean 
rm -rf $RPM_BUILD_ROOT  

%files
%attr(0755, root, root) /usr/local/digioceanfs_client/*
%attr(0755, root, root) /etc/systemd/system/multi-user.target.wants/digioceanfs-client.service
%attr(0755, root, root) /usr/bin/digioceanfs-client

%changelog
* Wed Jul 17 2023 Luo Yi - 0.1.0-1
- Add alarm feature(zxx-9-5)
- Fix bugs of deleting a service that used by ivcs 

%global _hardened_build 1

%global _for_fedora_koji_builds 0

# uncomment and add '%' to use the prereltag for pre-releases
# %%global prereltag qa3

##-----------------------------------------------------------------------------
## All argument definitions should be placed here and keep them sorted
##

# if you wish to compile an rpm with debugging...
# rpmbuild -ta digioceanfs-3.8dev.tar.gz --with debug
%{?_with_debug:%global _with_debug --enable-debug}

# if you wish to compile an rpm with cmocka unit testing...
# rpmbuild -ta digioceanfs-3.8dev.tar.gz --with cmocka
%{?_with_cmocka:%global _with_cmocka --enable-cmocka}

# if you wish to compile an rpm without rdma support, compile like this...
# rpmbuild -ta digioceanfs-3.8dev.tar.gz --without rdma
%{?_without_rdma:%global _without_rdma --disable-ibverbs}

# No RDMA Support on s390(x)
%ifarch s390 s390x
%global _without_rdma --disable-ibverbs
%endif

# if you wish to compile an rpm without epoll...
# rpmbuild -ta digioceanfs-3.8dev.tar.gz --without epoll
%{?_without_epoll:%global _without_epoll --disable-epoll}

# if you wish to compile an rpm without fusermount...
# rpmbuild -ta digioceanfs-3.8dev.tar.gz --without fusermount
%{?_without_fusermount:%global _without_fusermount --disable-fusermount}

# if you wish to compile an rpm without geo-replication support, compile like this...
# rpmbuild -ta digioceanfs-3.8dev.tar.gz --without georeplication
%{?_without_georeplication:%global _without_georeplication --disable-georeplication}

# Disable geo-replication on EL5, as its default Python is too old
%if ( 0%{?rhel} && 0%{?rhel} < 6 )
%global _without_georeplication --disable-georeplication
%endif

# if you wish to compile an rpm without the OCF resource agents...
# rpmbuild -ta digioceanfs-3.8dev.tar.gz --without ocf
%{?_without_ocf:%global _without_ocf --without-ocf}

# if you wish to build rpms without syslog logging, compile like this
# rpmbuild -ta digioceanfs-3.8dev.tar.gz --without syslog
%{?_without_syslog:%global _without_syslog --disable-syslog}

# disable syslog forcefully as rhel <= 6 doesn't have rsyslog or rsyslog-mmcount
# Fedora deprecated syslog, see 
#  https://fedoraproject.org/wiki/Changes/NoDefaultSyslog
# (And what about RHEL7?)
%if ( 0%{?fedora} && 0%{?fedora} >= 20 ) || ( 0%{?rhel} && 0%{?rhel} <= 6 )
%global _without_syslog --disable-syslog
%endif

# if you wish to compile an rpm without the BD map support...
# rpmbuild -ta digioceanfs-3.8dev.tar.gz --without bd
%{?_without_bd:%global _without_bd --disable-bd-xlator}

%if ( 0%{?rhel} && 0%{?rhel} < 6 || 0%{?sles_version} )
%define _without_bd --disable-bd-xlator
%endif

# if you wish to compile an rpm without the qemu-block support...
# rpmbuild -ta digioceanfs-3.8dev.tar.gz --without qemu-block
%{?_without_qemu_block:%global _without_qemu_block --disable-qemu-block}

%if ( 0%{?rhel} && 0%{?rhel} < 6 )
# xlators/features/qemu-block fails to build on RHEL5, disable it
%define _without_qemu_block --disable-qemu-block
%endif

# Disable data-tiering on EL5, sqlite is too old
%if ( 0%{?rhel} && 0%{?rhel} < 6 )
%global _without_tiering --disable-tiering
%endif

##-----------------------------------------------------------------------------
## All %global definitions should be placed here and keep them sorted
##

%if ( 0%{?fedora} && 0%{?fedora} > 16 ) || ( 0%{?rhel} && 0%{?rhel} > 6 )
%global _with_systemd true
%endif

%if ( 0%{?fedora} ) || ( 0%{?rhel} && 0%{?rhel} >= 7 )
%global _with_firewalld --enable-firewalld
%endif

%if 0%{?_tmpfilesdir:1}
%define _with_tmpfilesdir --with-tmpfilesdir=%{_tmpfilesdir}
%else
%define _with_tmpfilesdir --without-tmpfilesdir
%endif

# there is no systemtap support! Perhaps some day there will be
%global _without_systemtap --enable-systemtap=no

# From https://fedoraproject.org/wiki/Packaging:Python#Macros
%if ( 0%{?rhel} && 0%{?rhel} <= 5 )
%{!?python_sitelib: %global python_sitelib %(python -c "from distutils.sysconfig import get_python_lib; print(get_python_lib())")}
%{!?python_sitearch: %global python_sitearch %(python -c "from distutils.sysconfig import get_python_lib; print(get_python_lib(1))")}
%endif

%if ( 0%{?_with_systemd:1} )
%define _init_enable()  /bin/systemctl enable %1.service ;
%define _init_disable() /bin/systemctl disable %1.service ;
%define _init_restart() /bin/systemctl try-restart %1.service ;
%define _init_start()   /bin/systemctl start %1.service ;
%define _init_stop()    /bin/systemctl stop %1.service ;
%define _init_install() install -D -p -m 0644 %1 %{buildroot}%{_unitdir}/%2.service ;
# can't seem to make a generic macro that works
%define _init_digioceand   %{_unitdir}/digioceand.service
%define _init_digioceanfsd %{_unitdir}/digioceanfsd.service
%else
%define _init_enable()  /sbin/chkconfig --add %1 ;
%define _init_disable() /sbin/chkconfig --del %1 ;
%define _init_restart() /sbin/service %1 condrestart &>/dev/null ;
%define _init_start()   /sbin/service %1 start &>/dev/null ;
%define _init_stop()    /sbin/service %1 stop &>/dev/null ;
%define _init_install() install -D -p -m 0755 %1 %{buildroot}%{_sysconfdir}/init.d/%2 ;
# can't seem to make a generic macro that works
%define _init_digioceand   %{_sysconfdir}/init.d/digioceand
%define _init_digioceanfsd %{_sysconfdir}/init.d/digioceanfsd
%endif

%if ( 0%{_for_fedora_koji_builds} )
%if ( 0%{?_with_systemd:1} )
%global digioceanfsd_service digioceanfsd.service
%else
%global digioceanfsd_service digioceanfsd.init
%endif
%endif

%{!?_pkgdocdir: %global _pkgdocdir %{_docdir}/%{name}-%{version}}

%if ( 0%{?rhel} && 0%{?rhel} < 6 )
   # _sharedstatedir is not provided by RHEL5
   %define _sharedstatedir /var/lib
%endif

# We do not want to generate useless provides and requires for xlator
# .so files to be set for digioceanfs packages.
# Filter all generated:
#
# TODO: RHEL5 does not have a convenient solution
%if ( 0%{?rhel} == 6 )
    # filter_setup exists in RHEL6 only
    %filter_provides_in %{_libdir}/digioceanfs/%{version}/
    %global __filter_from_req %{?__filter_from_req} | grep -v -P '^(?!lib).*\.so.*$'
    %filter_setup
%else
    # modern rpm and current Fedora do not generate requires when the
    # provides are filtered
    %global __provides_exclude_from ^%{_libdir}/digioceanfs/%{version}/.*$
%endif


##-----------------------------------------------------------------------------
## All package definitions should be placed here and keep them sorted
##
Summary:          Distributed File System
%if ( 0%{_for_fedora_koji_builds} )
Name:             digioceanfs
Version:          3.8.0
Release:          1 
Vendor:           Fedora Project
%else
Name:             digioceanfs
Version:          3.8.0
Release:          1 
Vendor:           Digiocean Community
%endif
License:          GPLv2 or LGPLv3+
Group:            System Environment/Base
URL:              http://www.digiocean.org/docs/index.php/DigioceanFS
%if ( 0%{_for_fedora_koji_builds} )
Source0:          http://bits.digiocean.org/pub/digiocean/digioceanfs/src/digioceanfs-%{version}%{?prereltag}.tar.gz
Source1:          digioceand.sysconfig
Source2:          digioceanfsd.sysconfig
Source6:          rhel5-load-fuse-modules
Source7:          digioceanfsd.service
Source8:          digioceanfsd.init
%else
Source0:          digioceanfs-3.8.0.tar.gz
%endif

BuildRoot:        %(mktemp -ud %{_tmppath}/%{name}-%{version}-%{release}-XXXXXX)

%if ( 0%{?rhel} && 0%{?rhel} <= 5 )
BuildRequires:    python-simplejson
%endif
%if ( 0%{?_with_systemd:1} )
BuildRequires:    systemd-units
%endif

Requires:         %{name}-libs = %{version}-%{release}
BuildRequires:    bison flex
BuildRequires:    gcc make automake libtool
BuildRequires:    ncurses-devel readline-devel
BuildRequires:    libxml2-devel openssl-devel
BuildRequires:    libaio-devel libacl-devel
BuildRequires:    python-devel
BuildRequires:    python-ctypes
BuildRequires:    userspace-rcu-devel >= 0.7
%if ( 0%{?rhel} && 0%{?rhel} <= 5 )
BuildRequires:    e2fsprogs-devel
%else
BuildRequires:    libuuid-devel
%endif
%if ( 0%{?_with_cmocka:1} )
BuildRequires:    libcmocka-devel >= 1.0.1
%endif
%if ( 0%{!?_without_tiering:1} )
BuildRequires:    sqlite-devel
%endif
%if ( 0%{!?_without_systemtap:1} )
BuildRequires:    systemtap-sdt-devel
%endif
%if ( 0%{!?_without_bd:1} )
BuildRequires:    lvm2-devel
%endif
%if ( 0%{!?_without_qemu_block:1} )
BuildRequires:    glib2-devel
%endif
%if ( 0%{!?_without_georeplication:1} )
BuildRequires:    libattr-devel
%endif

%if (0%{?_with_firewalld:1})
BuildRequires:    firewalld
%endif

Obsoletes:        hekafs
Obsoletes:        %{name}-common < %{version}-%{release}
Obsoletes:        %{name}-core < %{version}-%{release}
Obsoletes:        %{name}-ufo
Provides:         %{name}-common = %{version}-%{release}
Provides:         %{name}-core = %{version}-%{release}

%description
DigioceanFS is a distributed file-system capable of scaling to several
petabytes. It aggregates various storage bricks over Infiniband RDMA
or TCP/IP interconnect into one large parallel network file
system. DigioceanFS is one of the most sophisticated file systems in
terms of features and extensibility.  It borrows a powerful concept
called Translators from GNU Hurd kernel. Much of the code in DigioceanFS
is in user space and easily manageable.

This package includes the digioceanfs binary, the digioceanfsd daemon and the
libdigioceanfs and digioceanfs translator modules common to both DigioceanFS server
and client framework.

%package api
Summary:          DigioceanFS api library
Group:            System Environment/Daemons
Requires:         %{name} = %{version}-%{release}
Requires:         %{name}-client-xlators = %{version}-%{release}
# we provide the Python package/namespace 'digiocean'
#Provides:         python-digiocean = %{version}-%{release}

%description api
DigioceanFS is a distributed file-system capable of scaling to several
petabytes. It aggregates various storage bricks over Infiniband RDMA
or TCP/IP interconnect into one large parallel network file
system. DigioceanFS is one of the most sophisticated file systems in
terms of features and extensibility.  It borrows a powerful concept
called Translators from GNU Hurd kernel. Much of the code in DigioceanFS
is in user space and easily manageable.

This package provides the digioceanfs libdfapi library.

%package api-devel
Summary:          Development Libraries
Group:            Development/Libraries
Requires:         %{name} = %{version}-%{release}
Requires:         %{name}-devel = %{version}-%{release}
Requires:         libacl-devel

%description api-devel
DigioceanFS is a distributed file-system capable of scaling to several
petabytes. It aggregates various storage bricks over Infiniband RDMA
or TCP/IP interconnect into one large parallel network file
system. DigioceanFS is one of the most sophisticated file systems in
terms of features and extensibility.  It borrows a powerful concept
called Translators from GNU Hurd kernel. Much of the code in DigioceanFS
is in user space and easily manageable.

This package provides the api include files.

%package cli
Summary:          DigioceanFS CLI
Group:            Applications/File
Requires:         %{name}-libs = %{version}-%{release}

%description cli
DigioceanFS is a distributed file-system capable of scaling to several
petabytes. It aggregates various storage bricks over Infiniband RDMA
or TCP/IP interconnect into one large parallel network file
system. DigioceanFS is one of the most sophisticated file systems in
terms of features and extensibility.  It borrows a powerful concept
called Translators from GNU Hurd kernel. Much of the code in DigioceanFS
is in user space and easily manageable.

This package provides the DigioceanFS CLI application and its man page

%package devel
Summary:          Development Libraries
Group:            Development/Libraries
Requires:         %{name} = %{version}-%{release}
# Needed for the Glupy examples to work
Requires:         %{name}-extra-xlators = %{version}-%{release}

%description devel
DigioceanFS is a distributed file-system capable of scaling to several
petabytes. It aggregates various storage bricks over Infiniband RDMA
or TCP/IP interconnect into one large parallel network file
system. DigioceanFS is one of the most sophisticated file systems in
terms of features and extensibility.  It borrows a powerful concept
called Translators from GNU Hurd kernel. Much of the code in DigioceanFS
is in user space and easily manageable.

This package provides the development libraries and include files.

%package extra-xlators
Summary:          Extra Digiocean filesystem Translators
Group:            Applications/File
# We need python-digiocean rpm for digiocean module's __init__.py in Python
# site-packages area
Requires:         python-digiocean = %{version}-%{release}
Requires:         python python-ctypes

%description extra-xlators
DigioceanFS is a distributed file-system capable of scaling to several
petabytes. It aggregates various storage bricks over Infiniband RDMA
or TCP/IP interconnect into one large parallel network file
system. DigioceanFS is one of the most sophisticated file systems in
terms of features and extensibility.  It borrows a powerful concept
called Translators from GNU Hurd kernel. Much of the code in DigioceanFS
is in user space and easily manageable.

This package provides extra filesystem Translators, such as Glupy,
for DigioceanFS.

%package fuse
Summary:          Fuse client
Group:            Applications/File
BuildRequires:    fuse-devel
Requires:         attr

Requires:         %{name} = %{version}-%{release}
Requires:         %{name}-client-xlators = %{version}-%{release}

Obsoletes:        %{name}-client < %{version}-%{release}
Provides:         %{name}-client = %{version}-%{release}

%description fuse
DigioceanFS is a distributed file-system capable of scaling to several
petabytes. It aggregates various storage bricks over Infiniband RDMA
or TCP/IP interconnect into one large parallel network file
system. DigioceanFS is one of the most sophisticated file systems in
terms of features and extensibility.  It borrows a powerful concept
called Translators from GNU Hurd kernel. Much of the code in DigioceanFS
is in user space and easily manageable.

This package provides support to FUSE based clients and inlcudes the
digioceanfs(d) binary.

%package ganesha
Summary:          NFS-Ganesha configuration
Group:            Applications/File

Requires:         %{name}-server = %{version}-%{release}
Requires:         nfs-ganesha-digiocean
Requires:         pcs

%description ganesha
DigioceanFS is a distributed file-system capable of scaling to several
petabytes. It aggregates various storage bricks over Infiniband RDMA
or TCP/IP interconnect into one large parallel network file
system. DigioceanFS is one of the most sophisticated file systems in
terms of features and extensibility.  It borrows a powerful concept
called Translators from GNU Hurd kernel. Much of the code in DigioceanFS
is in user space and easily manageable.

This package provides the configuration and related files for using
NFS-Ganesha as the NFS server using DigioceanFS

%if ( 0%{!?_without_georeplication:1} )
%package geo-replication
Summary:          DigioceanFS Geo-replication
Group:            Applications/File
Requires:         %{name} = %{version}-%{release}
Requires:         %{name}-server = %{version}-%{release}
Requires:         python python-ctypes
Requires:         rsync

%description geo-replication
DigioceanFS is a distributed file-system capable of scaling to several
peta-bytes. It aggregates various storage bricks over Infiniband RDMA
or TCP/IP interconnect into one large parallel network file
system. DigioceanFS is one of the most sophisticated file system in
terms of features and extensibility.  It borrows a powerful concept
called Translators from GNU Hurd kernel. Much of the code in DigioceanFS
is in userspace and easily manageable.

This package provides support to geo-replication.
%endif

%package libs
Summary:          DigioceanFS common libraries
Group:            Applications/File
%if ( 0%{!?_without_syslog:1} )
%if ( 0%{?fedora} ) || ( 0%{?rhel} && 0%{?rhel} > 6 )
Requires:         rsyslog-mmjsonparse
%endif
%if ( 0%{?rhel} && 0%{?rhel} == 6 )
Requires:         rsyslog-mmcount
%endif
%endif

%description libs
DigioceanFS is a distributed file-system capable of scaling to several
petabytes. It aggregates various storage bricks over Infiniband RDMA
or TCP/IP interconnect into one large parallel network file
system. DigioceanFS is one of the most sophisticated file systems in
terms of features and extensibility.  It borrows a powerful concept
called Translators from GNU Hurd kernel. Much of the code in DigioceanFS
is in user space and easily manageable.

This package provides the base DigioceanFS libraries

%package -n python-digiocean
Summary:          DigioceanFS python library
Group:            Development/Tools
%if ( ! ( 0%{?rhel} && 0%{?rhel} < 6 || 0%{?sles_version} ) )
# EL5 does not support noarch sub-packages
BuildArch:        noarch
%endif
Requires:         python

%description -n python-digiocean
DigioceanFS is a distributed file-system capable of scaling to several
petabytes. It aggregates various storage bricks over Infiniband RDMA
or TCP/IP interconnect into one large parallel network file
system. DigioceanFS is one of the most sophisticated file systems in
terms of features and extensibility.  It borrows a powerful concept
called Translators from GNU Hurd kernel. Much of the code in DigioceanFS
is in user space and easily manageable.

This package contains the python modules of DigioceanFS and own digiocean
namespace.


%if ( 0%{!?_without_rdma:1} )
%package rdma
Summary:          DigioceanFS rdma support for ib-verbs
Group:            Applications/File
BuildRequires:    libibverbs-devel
BuildRequires:    librdmacm-devel >= 1.0.15
Requires:         %{name} = %{version}-%{release}

%description rdma
DigioceanFS is a distributed file-system capable of scaling to several
petabytes. It aggregates various storage bricks over Infiniband RDMA
or TCP/IP interconnect into one large parallel network file
system. DigioceanFS is one of the most sophisticated file systems in
terms of features and extensibility.  It borrows a powerful concept
called Translators from GNU Hurd kernel. Much of the code in DigioceanFS
is in user space and easily manageable.

This package provides support to ib-verbs library.
%endif

%package regression-tests
Summary:          Development Tools
Group:            Development/Tools
Requires:         %{name} = %{version}-%{release}
Requires:         %{name}-fuse = %{version}-%{release}
Requires:         %{name}-server = %{version}-%{release}
## thin provisioning support
Requires:         lvm2 >= 2.02.89
Requires:         perl(App::Prove) perl(Test::Harness) gcc util-linux-ng
Requires:         python attr dbench file git libacl-devel net-tools
Requires:         nfs-utils xfsprogs yajl

%description regression-tests
The Digiocean Test Framework, is a suite of scripts used for
regression testing of Digiocean.

%if ( 0%{!?_without_ocf:1} )
%package resource-agents
Summary:          OCF Resource Agents for DigioceanFS
License:          GPLv3+
%if ( ! ( 0%{?rhel} && 0%{?rhel} < 6 || 0%{?sles_version} ) )
# EL5 does not support noarch sub-packages
BuildArch:        noarch
%endif
# this Group handling comes from the Fedora resource-agents package
%if ( 0%{?fedora} || 0%{?centos_version} || 0%{?rhel} )
Group:            System Environment/Base
%else
Group:            Productivity/Clustering/HA
%endif
# for digioceand
Requires:         %{name}-server
# depending on the distribution, we need pacemaker or resource-agents
Requires:         %{_prefix}/lib/ocf/resource.d

%description resource-agents
DigioceanFS is a distributed file-system capable of scaling to several
petabytes. It aggregates various storage bricks over Infiniband RDMA
or TCP/IP interconnect into one large parallel network file
system. DigioceanFS is one of the most sophisticated file systems in
terms of features and extensibility.  It borrows a powerful concept
called Translators from GNU Hurd kernel. Much of the code in DigioceanFS
is in user space and easily manageable.

This package provides the resource agents which plug digioceand into
Open Cluster Framework (OCF) compliant cluster resource managers,
like Pacemaker.
%endif

%package server
Summary:          Clustered file-system server
Group:            System Environment/Daemons
Requires:         %{name} = %{version}-%{release}
Requires:         %{name}-libs = %{version}-%{release}
Requires:         %{name}-cli = %{version}-%{release}
# some daemons (like quota) use a fuse-mount, digioceanfsd is part of -fuse
Requires:         %{name}-fuse = %{version}-%{release}
# self-heal daemon, rebalance, nfs-server etc. are actually clients
Requires:         %{name}-client-xlators = %{version}-%{release}
# psmisc for killall, lvm2 for snapshot, and nfs-utils and
# rpcbind/portmap for gnfs server
Requires:         psmisc
Requires:         lvm2
Requires:         nfs-utils
%if ( 0%{?_with_systemd:1} )
Requires(post):   systemd-units
Requires(preun):  systemd-units
Requires(postun): systemd-units
%else
Requires(post):   /sbin/chkconfig
Requires(preun):  /sbin/service
Requires(preun):  /sbin/chkconfig
Requires(postun): /sbin/service
%endif
%if ( 0%{?fedora} ) || ( 0%{?rhel} && 0%{?rhel} >= 6 )
Requires:         rpcbind
%else
Requires:         portmap
%endif
%if ( 0%{?rhel} && 0%{?rhel} < 6 )
Obsoletes:        %{name}-geo-replication = %{version}-%{release}
%endif
%if ( 0%{?rhel} && 0%{?rhel} <= 6 )
Requires:         python-argparse
%endif
Requires:         pyxattr

%description server
DigioceanFS is a distributed file-system capable of scaling to several
petabytes. It aggregates various storage bricks over Infiniband RDMA
or TCP/IP interconnect into one large parallel network file
system. DigioceanFS is one of the most sophisticated file systems in
terms of features and extensibility.  It borrows a powerful concept
called Translators from GNU Hurd kernel. Much of the code in DigioceanFS
is in user space and easily manageable.

This package provides the digioceanfs server daemon.

%package client-xlators
Summary:          DigioceanFS client-side translators
Group:            Applications/File

%description client-xlators
DigioceanFS is a distributed file-system capable of scaling to several
petabytes. It aggregates various storage bricks over Infiniband RDMA
or TCP/IP interconnect into one large parallel network file
system. DigioceanFS is one of the most sophisticated file systems in
terms of features and extensibility.  It borrows a powerful concept
called Translators from GNU Hurd kernel. Much of the code in DigioceanFS
is in user space and easily manageable.

This package provides the translators needed on any DigioceanFS client.

%prep
%setup -q -n %{name}-%{version}%{?prereltag}

%build
./autogen.sh && %configure \
        %{?_with_cmocka} \
        %{?_with_debug} \
        %{?_with_tmpfilesdir} \
        %{?_without_bd} \
        %{?_without_epoll} \
        %{?_without_fusermount} \
        %{?_without_georeplication} \
        %{?_with_firewalld} \
        %{?_without_ocf} \
        %{?_without_qemu_block} \
        %{?_without_rdma} \
        %{?_without_syslog} \
        %{?_without_systemtap} \
        %{?_without_tiering}

# fix hardening and remove rpath in shlibs
%if ( 0%{?fedora} && 0%{?fedora} > 17 ) || ( 0%{?rhel} && 0%{?rhel} > 6 )
sed -i 's| \\\$compiler_flags |&\\\$LDFLAGS |' libtool
%endif
sed -i 's|^hardcode_libdir_flag_spec=.*|hardcode_libdir_flag_spec=""|' libtool
sed -i 's|^runpath_var=LD_RUN_PATH|runpath_var=DIE_RPATH_DIE|' libtool

make %{?_smp_mflags}

# Build Glupy
pushd xlators/features/glupy/src
FLAGS="$RPM_OPT_FLAGS" python setup.py build
popd

%check
make check

%install
rm -rf %{buildroot}
make install DESTDIR=%{buildroot}
# install the Glupy Python library in /usr/lib/python*/site-packages
pushd xlators/features/glupy/src
python setup.py install --skip-build --verbose --root %{buildroot}
popd
# Install include directory
mkdir -p %{buildroot}%{_includedir}/digioceanfs
install -p -m 0644 libdigioceanfs/src/*.h \
    %{buildroot}%{_includedir}/digioceanfs/
install -p -m 0644 contrib/uuid/*.h \
    %{buildroot}%{_includedir}/digioceanfs/
# Following needed by hekafs multi-tenant translator
mkdir -p %{buildroot}%{_includedir}/digioceanfs/rpc
install -p -m 0644 rpc/rpc-lib/src/*.h \
    %{buildroot}%{_includedir}/digioceanfs/rpc/
install -p -m 0644 rpc/xdr/src/*.h \
    %{buildroot}%{_includedir}/digioceanfs/rpc/
mkdir -p %{buildroot}%{_includedir}/digioceanfs/server
install -p -m 0644 xlators/protocol/server/src/*.h \
    %{buildroot}%{_includedir}/digioceanfs/server/
%if ( 0%{_for_fedora_koji_builds} )
install -D -p -m 0644 %{SOURCE1} \
    %{buildroot}%{_sysconfdir}/sysconfig/digioceand
install -D -p -m 0644 %{SOURCE2} \
    %{buildroot}%{_sysconfdir}/sysconfig/digioceanfsd
%else
install -D -p -m 0644 extras/digioceand-sysconfig \
    %{buildroot}%{_sysconfdir}/sysconfig/digioceand
%endif

%if ( 0%{_for_fedora_koji_builds} )
%if ( 0%{?rhel} && 0%{?rhel} <= 5 )
install -D -p -m 0755 %{SOURCE6} \
    %{buildroot}%{_sysconfdir}/sysconfig/modules/digioceanfs-fuse.modules
%endif
%endif

mkdir -p %{buildroot}%{_localstatedir}/log/digioceand
mkdir -p %{buildroot}%{_localstatedir}/log/digioceanfs
mkdir -p %{buildroot}%{_localstatedir}/log/digioceanfsd
mkdir -p %{buildroot}%{_localstatedir}/run/digiocean
touch %{buildroot}%{python_sitelib}/digiocean/__init__.py


# Remove unwanted files from all the shared libraries
find %{buildroot}%{_libdir} -name '*.a' -delete
find %{buildroot}%{_libdir} -name '*.la' -delete

# Remove installed docs, the ones we want are included by %%doc, in
# /usr/share/doc/digioceanfs or /usr/share/doc/digioceanfs-x.y.z depending
# on the distribution
%if ( 0%{?fedora} && 0%{?fedora} > 19 ) || ( 0%{?rhel} && 0%{?rhel} > 6 )
rm -rf %{buildroot}%{_pkgdocdir}/*
%else
rm -rf %{buildroot}%{_defaultdocdir}/%{name}
mkdir -p %{buildroot}%{_pkgdocdir}
%endif
head -50 ChangeLog > ChangeLog.head && mv ChangeLog.head ChangeLog
cat << EOM >> ChangeLog

More commit messages for this ChangeLog can be found at
https://forge.digiocean.org/digioceanfs-core/digioceanfs/commits/v%{version}%{?prereltag}
EOM

# Remove benchmarking and other unpackaged files
%if ( 0%{?rhel} && 0%{?rhel} < 6 )
rm -rf %{buildroot}/benchmarking
rm -f %{buildroot}/digioceanfs-mode.el
rm -f %{buildroot}/digioceanfs.vim
%else
# make install always puts these in %%{_defaultdocdir}/%%{name} so don't
# use %%{_pkgdocdir}; that will be wrong on later Fedora distributions
rm -rf %{buildroot}%{_defaultdocdir}/%{name}/benchmarking
rm -f %{buildroot}%{_defaultdocdir}/%{name}/digioceanfs-mode.el
rm -f %{buildroot}%{_defaultdocdir}/%{name}/digioceanfs.vim
%endif

# Create working directory
mkdir -p %{buildroot}%{_sharedstatedir}/digioceand

# Update configuration file to /var/lib working directory
sed -i 's|option working-directory /etc/digioceand|option working-directory %{_sharedstatedir}/digioceand|g' \
    %{buildroot}%{_sysconfdir}/digioceanfs/digioceand.vol

# Install digioceanfsd .service or init.d file
%if ( 0%{_for_fedora_koji_builds} )
%_init_install %{digioceanfsd_service} digioceanfsd
%endif

install -D -p -m 0644 extras/digioceanfs-logrotate \
    %{buildroot}%{_sysconfdir}/logrotate.d/digioceanfs

%if ( 0%{!?_without_georeplication:1} )
# geo-rep ghosts
mkdir -p %{buildroot}%{_sharedstatedir}/digioceand/geo-replication
touch %{buildroot}%{_sharedstatedir}/digioceand/geo-replication/gsyncd_template.conf
install -D -p -m 0644 extras/digioceanfs-georep-logrotate \
    %{buildroot}%{_sysconfdir}/logrotate.d/digioceanfs-georep
%endif

%if ( 0%{!?_without_syslog:1} )
%if ( 0%{?fedora} ) || ( 0%{?rhel} && 0%{?rhel} > 6 )
install -D -p -m 0644 extras/digiocean-rsyslog-7.2.conf \
    %{buildroot}%{_sysconfdir}/rsyslog.d/digiocean.conf.example
%endif

%if ( 0%{?rhel} && 0%{?rhel} == 6 )
install -D -p -m 0644 extras/digiocean-rsyslog-5.8.conf \
    %{buildroot}%{_sysconfdir}/rsyslog.d/digiocean.conf.example
%endif

%if ( 0%{?fedora} ) || ( 0%{?rhel} && 0%{?rhel} >= 6 )
install -D -p -m 0644 extras/logger.conf.example \
    %{buildroot}%{_sysconfdir}/digioceanfs/logger.conf.example
%endif
%endif

# the rest of the ghosts
touch %{buildroot}%{_sharedstatedir}/digioceand/digioceand.info
touch %{buildroot}%{_sharedstatedir}/digioceand/options
mkdir -p %{buildroot}%{_sharedstatedir}/digioceand/hooks
mkdir -p %{buildroot}%{_sharedstatedir}/digioceand/hooks/1
mkdir -p %{buildroot}%{_sharedstatedir}/digioceand/hooks/1/stop
mkdir -p %{buildroot}%{_sharedstatedir}/digioceand/hooks/1/stop/post
mkdir -p %{buildroot}%{_sharedstatedir}/digioceand/hooks/1/stop/pre
mkdir -p %{buildroot}%{_sharedstatedir}/digioceand/hooks/1/start
mkdir -p %{buildroot}%{_sharedstatedir}/digioceand/hooks/1/start/post
mkdir -p %{buildroot}%{_sharedstatedir}/digioceand/hooks/1/start/pre
mkdir -p %{buildroot}%{_sharedstatedir}/digioceand/hooks/1/reset
mkdir -p %{buildroot}%{_sharedstatedir}/digioceand/hooks/1/reset/post
mkdir -p %{buildroot}%{_sharedstatedir}/digioceand/hooks/1/reset/pre
mkdir -p %{buildroot}%{_sharedstatedir}/digioceand/hooks/1/remove-brick
mkdir -p %{buildroot}%{_sharedstatedir}/digioceand/hooks/1/remove-brick/post
mkdir -p %{buildroot}%{_sharedstatedir}/digioceand/hooks/1/remove-brick/pre
mkdir -p %{buildroot}%{_sharedstatedir}/digioceand/hooks/1/add-brick
mkdir -p %{buildroot}%{_sharedstatedir}/digioceand/hooks/1/add-brick/post
mkdir -p %{buildroot}%{_sharedstatedir}/digioceand/hooks/1/add-brick/pre
mkdir -p %{buildroot}%{_sharedstatedir}/digioceand/hooks/1/set
mkdir -p %{buildroot}%{_sharedstatedir}/digioceand/hooks/1/set/post
mkdir -p %{buildroot}%{_sharedstatedir}/digioceand/hooks/1/set/pre
mkdir -p %{buildroot}%{_sharedstatedir}/digioceand/hooks/1/create
mkdir -p %{buildroot}%{_sharedstatedir}/digioceand/hooks/1/create/post
mkdir -p %{buildroot}%{_sharedstatedir}/digioceand/hooks/1/create/pre
mkdir -p %{buildroot}%{_sharedstatedir}/digioceand/hooks/1/delete
mkdir -p %{buildroot}%{_sharedstatedir}/digioceand/hooks/1/delete/post
mkdir -p %{buildroot}%{_sharedstatedir}/digioceand/hooks/1/delete/pre
mkdir -p %{buildroot}%{_sharedstatedir}/digioceand/hooks/1/copy-file
mkdir -p %{buildroot}%{_sharedstatedir}/digioceand/hooks/1/copy-file/post
mkdir -p %{buildroot}%{_sharedstatedir}/digioceand/hooks/1/copy-file/pre
mkdir -p %{buildroot}%{_sharedstatedir}/digioceand/hooks/1/gsync-create
mkdir -p %{buildroot}%{_sharedstatedir}/digioceand/hooks/1/gsync-create/post
mkdir -p %{buildroot}%{_sharedstatedir}/digioceand/hooks/1/gsync-create/pre
mkdir -p %{buildroot}%{_sharedstatedir}/digioceand/digioceanshd
mkdir -p %{buildroot}%{_sharedstatedir}/digioceand/peers
mkdir -p %{buildroot}%{_sharedstatedir}/digioceand/vols
mkdir -p %{buildroot}%{_sharedstatedir}/digioceand/nfs/run
touch %{buildroot}%{_sharedstatedir}/digioceand/nfs/nfs-server.vol
touch %{buildroot}%{_sharedstatedir}/digioceand/nfs/run/nfs.pid

%{__install} -p -m 0744 extras/hook-scripts/start/post/*.sh   \
    %{buildroot}%{_sharedstatedir}/digioceand/hooks/1/start/post
%{__install} -p -m 0744 extras/hook-scripts/stop/pre/*.sh   \
    %{buildroot}%{_sharedstatedir}/digioceand/hooks/1/stop/pre
%{__install} -p -m 0744 extras/hook-scripts/set/post/*.sh   \
    %{buildroot}%{_sharedstatedir}/digioceand/hooks/1/set/post
%{__install} -p -m 0744 extras/hook-scripts/add-brick/post/*.sh   \
    %{buildroot}%{_sharedstatedir}/digioceand/hooks/1/add-brick/post
%{__install} -p -m 0744 extras/hook-scripts/add-brick/pre/*.sh   \
    %{buildroot}%{_sharedstatedir}/digioceand/hooks/1/add-brick/pre
%{__install} -p -m 0744 extras/hook-scripts/reset/post/*.sh   \
    %{buildroot}%{_sharedstatedir}/digioceand/hooks/1/reset/post


find ./tests ./run-tests.sh -type f | cpio -pd %{buildroot}%{_prefix}/share/digioceanfs

## Install bash completion for cli
install -p -m 0744 -D extras/command-completion/digiocean.bash \
    %{buildroot}%{_sysconfdir}/bash_completion.d/digiocean


%clean
rm -rf %{buildroot}

##-----------------------------------------------------------------------------
## All %post should be placed here and keep them sorted
##
%post
%if ( 0%{!?_without_syslog:1} )
%if ( 0%{?fedora} ) || ( 0%{?rhel} && 0%{?rhel} >= 6 )
%_init_restart rsyslog
%endif
%endif

%post api
/sbin/ldconfig

%post fuse
%if ( 0%{?rhel} == 5 )
modprobe fuse
%endif

%if ( 0%{!?_without_georeplication:1} )
%post geo-replication
#restart digioceand.
if [ $1 -ge 1 ]; then
    %_init_restart digioceand
fi
%endif

%post libs
/sbin/ldconfig

%post server
# Legacy server
%_init_enable digioceand
%_init_enable digioceanfsd
# ".cmd_log_history" is renamed to "cmd_history.log" in DigioceanFS-3.7 .
# While upgrading digioceanfs-server package form DigioceanFS version <= 3.6 to
# DigioceanFS version 3.7, ".cmd_log_history" should be renamed to
# "cmd_history.log" to retain cli command history contents.
if [ -f %{_localstatedir}/log/digioceanfs/.cmd_log_history ]; then
    mv %{_localstatedir}/log/digioceanfs/.cmd_log_history \
       %{_localstatedir}/log/digioceanfs/cmd_history.log
fi

# Genuine Fedora (and EPEL) builds never put digiocean files in /etc; if
# there are any files in /etc from a prior digiocean.org install, move them
# to /var/lib. (N.B. Starting with 3.3.0 all digiocean files are in /var/lib
# in digiocean.org RPMs.) Be careful to copy them on the off chance that
# /etc and /var/lib are on separate file systems
if [ -d /etc/digioceand -a ! -h %{_sharedstatedir}/digioceand ]; then
    mkdir -p %{_sharedstatedir}/digioceand
    cp -a /etc/digioceand %{_sharedstatedir}/digioceand
    rm -rf /etc/digioceand
    ln -sf %{_sharedstatedir}/digioceand /etc/digioceand
fi

# Rename old volfiles in an RPM-standard way.  These aren't actually
# considered package config files, so %%config doesn't work for them.
if [ -d %{_sharedstatedir}/digioceand/vols ]; then
    for file in $(find %{_sharedstatedir}/digioceand/vols -name '*.vol'); do
        newfile=${file}.rpmsave
        echo "warning: ${file} saved as ${newfile}"
        cp ${file} ${newfile}
    done
fi

# add marker translator
# but first make certain that there are no old libs around to bite us
# BZ 834847
if [ -e /etc/ld.so.conf.d/digioceanfs.conf ]; then
    rm -f /etc/ld.so.conf.d/digioceanfs.conf
    /sbin/ldconfig
fi

%if (0%{?_with_firewalld:1})
#reload service files if firewalld running
if $(systemctl is-active firewalld 1>/dev/null 2>&1); then
  #firewalld-filesystem is not available for rhel7, so command used for reload.
  firewall-cmd  --reload
fi
%endif

pidof -c -o %PPID -x digioceand &> /dev/null
if [ $? -eq 0 ]; then
    kill -9 `pgrep -f gsyncd.py` &> /dev/null

    killall --wait digioceand &> /dev/null
    digioceand --xlator-option *.upgrade=on -N

    #Cleaning leftover digioceand socket file which is created by digioceand in
    #rpm_script_t context.
    rm -rf /var/run/digioceand.socket

    # digioceand _was_ running, we killed it, it exited after *.upgrade=on,
    # so start it again
    %_init_start digioceand
else
    digioceand --xlator-option *.upgrade=on -N

    #Cleaning leftover digioceand socket file which is created by digioceand in
    #rpm_script_t context.
    rm -rf /var/run/digioceand.socket
fi

##-----------------------------------------------------------------------------
## All %preun should be placed here and keep them sorted
##
%preun server
if [ $1 -eq 0 ]; then
    if [ -f %_init_digioceanfsd ]; then
        %_init_stop digioceanfsd
    fi
    %_init_stop digioceand
    if [ -f %_init_digioceanfsd ]; then
        %_init_disable digioceanfsd
    fi
    %_init_disable digioceand
fi
if [ $1 -ge 1 ]; then
    if [ -f %_init_digioceanfsd ]; then
        %_init_restart digioceanfsd
    fi
    %_init_restart digioceand
fi

##-----------------------------------------------------------------------------
## All %postun should be placed here and keep them sorted
##
%postun
/sbin/ldconfig
%if ( 0%{!?_without_syslog:1} )
%if ( 0%{?fedora} ) || ( 0%{?rhel} && 0%{?rhel} >= 6 )
%_init_restart rsyslog
%endif
%endif

%postun api
/sbin/ldconfig

%postun server
%if (0%{?_with_firewalld:1})
#reload service files if firewalld running
if $(systemctl is-active firewalld 1>/dev/null 2>&1); then
    firewall-cmd  --reload
fi
%endif


%postun libs
/sbin/ldconfig

##-----------------------------------------------------------------------------
## All files should be placed here and keep them grouped
##
%files
%doc ChangeLog COPYING-GPLV2 COPYING-LGPLV3 INSTALL README.md THANKS
%if ( 0%{!?_without_syslog:1} )
%if ( 0%{?fedora} ) || ( 0%{?rhel} && 0%{?rhel} >= 6 )
%{_sysconfdir}/rsyslog.d/digiocean.conf.example
%endif
%endif
%{_mandir}/man8/*digiocean*.8*
%exclude %{_mandir}/man8/digiocean.8*
%dir %{_localstatedir}/log/digioceanfs
%if ( 0%{!?_without_rdma:1} )
%exclude %{_libdir}/digioceanfs/%{version}%{?prereltag}/rpc-transport/rdma*
%endif
%dir %{_datadir}/digioceanfs/scripts
%{_datadir}/digioceanfs/scripts/post-upgrade-script-for-quota.sh
%{_datadir}/digioceanfs/scripts/pre-upgrade-script-for-quota.sh
# xlators that are needed on the client- and on the server-side
%dir %{_libdir}/digioceanfs/%{version}%{?prereltag}/auth
%{_libdir}/digioceanfs/%{version}%{?prereltag}/auth/addr.so
%{_libdir}/digioceanfs/%{version}%{?prereltag}/auth/login.so
%dir %{_libdir}/digioceanfs/%{version}%{?prereltag}/rpc-transport
%{_libdir}/digioceanfs/%{version}%{?prereltag}/rpc-transport/socket.so
%dir %{_libdir}/digioceanfs/%{version}%{?prereltag}/xlator/debug
%{_libdir}/digioceanfs/%{version}%{?prereltag}/xlator/debug/error-gen.so
%{_libdir}/digioceanfs/%{version}%{?prereltag}/xlator/debug/io-stats.so
%{_libdir}/digioceanfs/%{version}%{?prereltag}/xlator/debug/trace.so
%if ( ! ( 0%{?rhel} && 0%{?rhel} < 6 ) )
# RHEL-5 based distributions have a too old openssl
%{_libdir}/digioceanfs/%{version}%{?prereltag}/xlator/encryption/crypt.so
%endif
%{_libdir}/digioceanfs/%{version}%{?prereltag}/xlator/features/access-control.so
%{_libdir}/digioceanfs/%{version}%{?prereltag}/xlator/features/barrier.so
%{_libdir}/digioceanfs/%{version}%{?prereltag}/xlator/features/cdc.so
%{_libdir}/digioceanfs/%{version}%{?prereltag}/xlator/features/changelog.so
%{_libdir}/digioceanfs/%{version}%{?prereltag}/xlator/features/gfid-access.so
%{_libdir}/digioceanfs/%{version}%{?prereltag}/xlator/features/read-only.so
%{_libdir}/digioceanfs/%{version}%{?prereltag}/xlator/features/shard.so
%{_libdir}/digioceanfs/%{version}%{?prereltag}/xlator/features/snapview-client.so
%{_libdir}/digioceanfs/%{version}%{?prereltag}/xlator/features/worm.so
%{_libdir}/digioceanfs/%{version}%{?prereltag}/xlator/meta.so
%{_libdir}/digioceanfs/%{version}%{?prereltag}/xlator/performance/io-cache.so
%{_libdir}/digioceanfs/%{version}%{?prereltag}/xlator/performance/io-threads.so
%{_libdir}/digioceanfs/%{version}%{?prereltag}/xlator/performance/md-cache.so
%{_libdir}/digioceanfs/%{version}%{?prereltag}/xlator/performance/open-behind.so
%{_libdir}/digioceanfs/%{version}%{?prereltag}/xlator/performance/quick-read.so
%{_libdir}/digioceanfs/%{version}%{?prereltag}/xlator/performance/read-ahead.so
%{_libdir}/digioceanfs/%{version}%{?prereltag}/xlator/performance/readdir-ahead.so
%{_libdir}/digioceanfs/%{version}%{?prereltag}/xlator/performance/stat-prefetch.so
%{_libdir}/digioceanfs/%{version}%{?prereltag}/xlator/performance/write-behind.so
%{_libdir}/digioceanfs/%{version}%{?prereltag}/xlator/system/posix-acl.so


%files api
%exclude %{_libdir}/*.so
# libdfapi files
%{_libdir}/libdfapi.*
%{_libdir}/digioceanfs/%{version}%{?prereltag}/xlator/mount/api.so

%files api-devel
%{_libdir}/pkgconfig/digioceanfs-api.pc
%{_libdir}/libdfapi.so
%{_includedir}/digioceanfs/api/*

%files cli
%{_sbindir}/digiocean
%{_mandir}/man8/digiocean.8*
%{_sysconfdir}/bash_completion.d/digiocean

%files devel
%{_includedir}/digioceanfs
%exclude %{_includedir}/digioceanfs/y.tab.h
%exclude %{_includedir}/digioceanfs/api
%exclude %{_libdir}/libdfapi.so
%{_libdir}/*.so
# Glupy Translator examples
%{_libdir}/digioceanfs/%{version}%{?prereltag}/xlator/features/glupy/debug-trace.*
%{_libdir}/digioceanfs/%{version}%{?prereltag}/xlator/features/glupy/helloworld.*
%{_libdir}/digioceanfs/%{version}%{?prereltag}/xlator/features/glupy/negative.*
%{_libdir}/pkgconfig/libgfchangelog.pc
%if ( 0%{!?_without_tiering:1} )
%{_libdir}/pkgconfig/libgfdb.pc
%endif

%files client-xlators
%{_libdir}/digioceanfs/%{version}%{?prereltag}/xlator/cluster/*.so
%exclude %{_libdir}/digioceanfs/%{version}%{?prereltag}/xlator/cluster/pump.so
%{_libdir}/digioceanfs/%{version}%{?prereltag}/xlator/features/ganesha.so
%if ( 0%{!?_without_qemu_block:1} )
%{_libdir}/digioceanfs/%{version}%{?prereltag}/xlator/features/qemu-block.so
%endif
%{_libdir}/digioceanfs/%{version}%{?prereltag}/xlator/protocol/client.so

%files extra-xlators
%{_libdir}/digioceanfs/%{version}%{?prereltag}/xlator/encryption/rot-13.so
%{_libdir}/digioceanfs/%{version}%{?prereltag}/xlator/features/glupy.so
%{_libdir}/digioceanfs/%{version}%{?prereltag}/xlator/features/mac-compat.so
%{_libdir}/digioceanfs/%{version}%{?prereltag}/xlator/features/prot_client.so
%{_libdir}/digioceanfs/%{version}%{?prereltag}/xlator/features/prot_dht.so
%{_libdir}/digioceanfs/%{version}%{?prereltag}/xlator/features/prot_server.so
%{_libdir}/digioceanfs/%{version}%{?prereltag}/xlator/features/quiesce.so
%{_libdir}/digioceanfs/%{version}%{?prereltag}/xlator/testing/features/template.so
%{_libdir}/digioceanfs/%{version}%{?prereltag}/xlator/testing/performance/symlink-cache.so
# Glupy Python files
%{python_sitelib}/digiocean/glupy/*
# Don't expect a .egg-info file on EL5
%if ( ! ( 0%{?rhel} && 0%{?rhel} < 6 ) )
%{python_sitelib}/digioceanfs_glupy*.egg-info
%endif

%files fuse
# digioceanfs is a symlink to digioceanfsd, -server depends on -fuse.
%{_sbindir}/digioceanfs
%{_sbindir}/digioceanfsd
%config(noreplace) %{_sysconfdir}/logrotate.d/digioceanfs
%{_libdir}/digioceanfs/%{version}%{?prereltag}/xlator/mount/fuse.so
/sbin/mount.digioceanfs
%if ( 0%{!?_without_fusermount:1} )
%{_bindir}/fusermount-digioceanfs
%endif
%if ( 0%{_for_fedora_koji_builds} )
%if ( 0%{?rhel} && 0%{?rhel} <= 5 )
%{_sysconfdir}/sysconfig/modules/digioceanfs-fuse.modules
%endif
%endif

%files ganesha
%{_sysconfdir}/ganesha/*
%attr(0755,-,-) %{_libexecdir}/ganesha/*
%attr(0755,-,-) %{_prefix}/lib/ocf/resource.d/heartbeat/*

%if ( 0%{!?_without_georeplication:1} )
%files geo-replication
%config(noreplace) %{_sysconfdir}/logrotate.d/digioceanfs-georep
%{_libexecdir}/digioceanfs/gsyncd
%{_libexecdir}/digioceanfs/python/syncdaemon/*
%{_libexecdir}/digioceanfs/gverify.sh
%{_libexecdir}/digioceanfs/set_geo_rep_pem_keys.sh
%{_libexecdir}/digioceanfs/peer_gsec_create
%{_libexecdir}/digioceanfs/peer_mountbroker
%ghost %dir %attr(0755,-,-) %{_sharedstatedir}/digioceand/geo-replication
%dir %{_sharedstatedir}/digioceand/hooks
%dir %{_sharedstatedir}/digioceand/hooks/1
%dir %{_sharedstatedir}/digioceand/hooks/1/gsync-create
%dir %{_sharedstatedir}/digioceand/hooks/1/gsync-create/post
%{_sharedstatedir}/digioceand/hooks/1/gsync-create/post/S56digioceand-geo-rep-create-post.sh
%{_datadir}/digioceanfs/scripts/get-gfid.sh
%{_datadir}/digioceanfs/scripts/slave-upgrade.sh
%{_datadir}/digioceanfs/scripts/gsync-upgrade.sh
%{_datadir}/digioceanfs/scripts/generate-gfid-file.sh
%{_datadir}/digioceanfs/scripts/gsync-sync-gfid
%ghost %attr(0644,-,-) %{_sharedstatedir}/digioceand/geo-replication/gsyncd_template.conf
%endif
%{_libexecdir}/digioceanfs/gfind_missing_files
%{_sbindir}/gfind_missing_files

%files libs
%{_libdir}/*.so.*
%exclude %{_libdir}/libdfapi.*
%if ( 0%{!?_without_tiering:1} )
# libgfdb is only needed server-side
%exclude %{_libdir}/libgfdb.*
%endif

%files -n python-digiocean
# introducing digioceanfs module in site packages.
# so that all other digiocean submodules can reside in the same namespace.
%{python_sitelib}/digiocean/__init__.*

%if ( 0%{!?_without_rdma:1} )
%files rdma
%{_libdir}/digioceanfs/%{version}%{?prereltag}/rpc-transport/rdma*
%endif

%files regression-tests
%{_prefix}/share/digioceanfs/run-tests.sh
%{_prefix}/share/digioceanfs/tests
%exclude %{_prefix}/share/digioceanfs/tests/basic/rpm.t

%if ( 0%{!?_without_ocf:1} )
%files resource-agents
# /usr/lib is the standard for OCF, also on x86_64
%{_prefix}/lib/ocf/resource.d/digioceanfs
%endif

%files server
%doc extras/clear_xattrs.sh
%config(noreplace) %{_sysconfdir}/sysconfig/digioceand
%config(noreplace) %{_sysconfdir}/digioceanfs
%dir %{_localstatedir}/run/digiocean
%if 0%{?_tmpfilesdir:1}
%{_tmpfilesdir}/digiocean.conf
%endif
%dir %{_sharedstatedir}/digioceand
%dir %{_sharedstatedir}/digioceand/groups
%config(noreplace) %{_sharedstatedir}/digioceand/groups/virt
# Legacy configs
%if ( 0%{_for_fedora_koji_builds} )
%config(noreplace) %{_sysconfdir}/sysconfig/digioceanfsd
%endif
%config %{_sharedstatedir}/digioceand/hooks/1/add-brick/post/disabled-quota-root-xattr-heal.sh
%config %{_sharedstatedir}/digioceand/hooks/1/add-brick/pre/S28Quota-enable-root-xattr-heal.sh
%config %{_sharedstatedir}/digioceand/hooks/1/set/post/S30samba-set.sh
%config %{_sharedstatedir}/digioceand/hooks/1/set/post/S32digiocean_enable_shared_storage.sh
%config %{_sharedstatedir}/digioceand/hooks/1/start/post/S29CTDBsetup.sh
%config %{_sharedstatedir}/digioceand/hooks/1/start/post/S30samba-start.sh
%config %{_sharedstatedir}/digioceand/hooks/1/start/post/S31ganesha-start.sh
%config %{_sharedstatedir}/digioceand/hooks/1/stop/pre/S30samba-stop.sh
%config %{_sharedstatedir}/digioceand/hooks/1/stop/pre/S29CTDB-teardown.sh
%config %{_sharedstatedir}/digioceand/hooks/1/reset/post/S31ganesha-reset.sh
# init files
%_init_digioceand
%if ( 0%{_for_fedora_koji_builds} )
%_init_digioceanfsd
%endif
# binaries
%{_sbindir}/digioceand
%{_sbindir}/dofsheal
# {_sbindir}/digioceanfsd is the actual binary, but digioceanfs (client) is a
# symlink. The binary itself (and symlink) are part of the digioceanfs-fuse
# package, because digioceanfs-server depends on that anyway.
%{_libdir}/digioceanfs/%{version}%{?prereltag}/xlator/cluster/pump.so
%{_libdir}/digioceanfs/%{version}%{?prereltag}/xlator/features/arbiter.so
%{_libdir}/digioceanfs/%{version}%{?prereltag}/xlator/features/bit-rot.so
%{_libdir}/digioceanfs/%{version}%{?prereltag}/xlator/features/bitrot-stub.so
%if ( 0%{!?_without_tiering:1} )
%{_libdir}/digioceanfs/%{version}%{?prereltag}/xlator/features/changetimerecorder.so
%endif
%{_libdir}/digioceanfs/%{version}%{?prereltag}/xlator/features/index.so
%{_libdir}/digioceanfs/%{version}%{?prereltag}/xlator/features/locks.so
%{_libdir}/digioceanfs/%{version}%{?prereltag}/xlator/features/posix*
%{_libdir}/digioceanfs/%{version}%{?prereltag}/xlator/features/snapview-server.so
%{_libdir}/digioceanfs/%{version}%{?prereltag}/xlator/features/marker.so
%{_libdir}/digioceanfs/%{version}%{?prereltag}/xlator/features/quota*
%{_libdir}/digioceanfs/%{version}%{?prereltag}/xlator/features/trash.so
%{_libdir}/digioceanfs/%{version}%{?prereltag}/xlator/features/upcall.so
%{_libdir}/digioceanfs/%{version}%{?prereltag}/xlator/mgmt*
%{_libdir}/digioceanfs/%{version}%{?prereltag}/xlator/nfs*
%{_libdir}/digioceanfs/%{version}%{?prereltag}/xlator/protocol/server*
%{_libdir}/digioceanfs/%{version}%{?prereltag}/xlator/storage*
%if ( 0%{!?_without_tiering:1} )
%{_libdir}/libgfdb.so.*
%endif

#snap_scheduler
%{_sbindir}/snap_scheduler.py
%{_sbindir}/gcron.py

#hookscripts
%dir %attr(0755,-,-) %{_sharedstatedir}/digioceand/hooks
%dir %attr(0755,-,-) %{_sharedstatedir}/digioceand/hooks/1
%dir %attr(0755,-,-) %{_sharedstatedir}/digioceand/hooks/1/add-brick
%dir %attr(0755,-,-) %{_sharedstatedir}/digioceand/hooks/1/add-brick/post
%dir %attr(0755,-,-) %{_sharedstatedir}/digioceand/hooks/1/add-brick/pre
%dir %attr(0755,-,-) %{_sharedstatedir}/digioceand/hooks/1/set
%dir %attr(0755,-,-) %{_sharedstatedir}/digioceand/hooks/1/set/post
%dir %attr(0755,-,-) %{_sharedstatedir}/digioceand/hooks/1/start
%dir %attr(0755,-,-) %{_sharedstatedir}/digioceand/hooks/1/start/post
%dir %attr(0755,-,-) %{_sharedstatedir}/digioceand/hooks/1/stop
%dir %attr(0755,-,-) %{_sharedstatedir}/digioceand/hooks/1/stop/pre
%dir %attr(0755,-,-) %{_sharedstatedir}/digioceand/hooks/1/delete
%dir %attr(0755,-,-) %{_sharedstatedir}/digioceand/hooks/1/delete/post

%ghost %attr(0644,-,-) %config(noreplace) %{_sharedstatedir}/digioceand/digioceand.info
%ghost %attr(0600,-,-) %{_sharedstatedir}/digioceand/options
# This is really ugly, but I have no idea how to mark these directories in
# any other way. They should belong to the digioceanfs-server package, but
# don't exist after installation. They are generated on the first start...
%ghost %dir %attr(0755,-,-) %{_sharedstatedir}/digioceand/hooks/1/stop/post
%ghost %dir %attr(0755,-,-) %{_sharedstatedir}/digioceand/hooks/1/start/pre
%ghost %dir %attr(0755,-,-) %{_sharedstatedir}/digioceand/hooks/1/remove-brick
%ghost %dir %attr(0755,-,-) %{_sharedstatedir}/digioceand/hooks/1/remove-brick/post
%ghost %dir %attr(0755,-,-) %{_sharedstatedir}/digioceand/hooks/1/remove-brick/pre
%ghost %dir %attr(0755,-,-) %{_sharedstatedir}/digioceand/hooks/1/set/pre
%ghost %dir %attr(0755,-,-) %{_sharedstatedir}/digioceand/hooks/1/create
%ghost %dir %attr(0755,-,-) %{_sharedstatedir}/digioceand/hooks/1/create/post
%ghost %dir %attr(0755,-,-) %{_sharedstatedir}/digioceand/hooks/1/create/pre
%ghost %dir %attr(0755,-,-) %{_sharedstatedir}/digioceand/hooks/1/delete/pre
%ghost %dir %attr(0755,-,-) %{_sharedstatedir}/digioceand/digioceanshd
%ghost %dir %attr(0755,-,-) %{_sharedstatedir}/digioceand/vols
%ghost %dir %attr(0755,-,-) %{_sharedstatedir}/digioceand/peers
%ghost %dir %attr(0755,-,-) %{_sharedstatedir}/digioceand/nfs
%ghost      %attr(0600,-,-) %{_sharedstatedir}/digioceand/nfs/nfs-server.vol
%ghost %dir %attr(0755,-,-) %{_sharedstatedir}/digioceand/nfs/run
%ghost      %attr(0600,-,-) %{_sharedstatedir}/digioceand/nfs/run/nfs.pid

# Extra utility script
%{_datadir}/digioceanfs/scripts/stop-all-digiocean-processes.sh

# Incrementalapi
%{_libexecdir}/digioceanfs/digioceanfind
%{_bindir}/digioceanfind
%{_libexecdir}/digioceanfs/peer_add_secret_pub
%{_sharedstatedir}/digioceand/hooks/1/delete/post/S57digioceanfind-delete-post.py

%if ( 0%{?_with_firewalld:1} )
/usr/lib/firewalld/services/digioceanfs.xml
%endif


%changelog
* Tue Sep 1 2015 Kaleb S. KEITHLEY <kkeithle@redhat.com>
- erroneous ghost of ../hooks/1/delete causes install failure (#1258975)

* Tue Aug 25 2015 Anand Nekkunti <anekkunt@redhat.com>
- adding digioceanfs-firewalld service (#1253967)

* Tue Aug 18 2015 Niels de Vos <ndevos@redhat.com>
- Include missing directories for digioceanfind hooks scripts (#1225465)

* Mon Jun 15 2015 Niels de Vos <ndevos@redhat.com>
- Replace hook script S31ganesha-set.sh by S31ganesha-start.sh (#1231738)

* Fri Jun 12 2015 Aravinda VK <avishwan@redhat.com>
- Added rsync as dependency to georeplication rpm (#1231205)

* Tue Jun 02 2015 Aravinda VK <avishwan@redhat.com>
- Added post hook for volume delete as part of digioceanfind (#1225465)

* Wed May 27 2015 Aravinda VK <avishwan@redhat.com>
- Added stop-all-digiocean-processes.sh in digioceanfs-server section (#1204641)

* Wed May 20 2015 Kaleb S. KEITHLEY <kkeithle@redhat.com>
- python-digiocean should be 'noarch' (#1219954)

* Wed May 20 2015 Kaleb S. KEITHLEY <kkeithle@redhat.com>
- move libgf{db,changelog}.pc from -api-devel to -devel (#1223385)

* Wed May 20 2015 Anand Nekkunti <anekkunt@redhat.com>
- digioceand.socket file cleanup during post run upgrade (#1210404)

* Tue May 19 2015 Avra Sengupta <asengupt@redhat.com>
- Added S32digiocean_enable_shared_storage.sh as volume set hook script (#1222013)

* Mon May 18 2015 Milind Changire <mchangir@redhat.com>
- Move file peer_add_secret_pub to the server RPM to support digioceanfind (#1221544)

* Sun May 17 2015 Niels de Vos <ndevos@redhat.com>
- Fix building on RHEL-5 based distributions (#1222317)

* Tue May 05 2015 Niels de Vos <ndevos@redhat.com>
- Introduce digioceanfs-client-xlators to reduce dependencies (#1195947)

* Wed Apr 15 2015 Humble Chirammal <hchiramm@redhat.com>
- Introducing python-digiocean package to own digiocean namespace in sitelib (#1211848)

* Sat Mar 28 2015 Mohammed Rafi KC <rkavunga@redhat.com>
- Add dependency for librdmacm version >= 1.0.15 (#1206744)

* Thu Mar 26 2015 Kaleb S. KEITHLEY <kkeithle@redhat.com>
- attr dependency (#1174627)

* Tue Mar 24 2015 Niels de Vos <ndevos@redhat.com>
- move libgfdb (with sqlite dependency) to -server subpackage (#1194753)

* Tue Mar 17 2015 Kaleb S. KEITHLEY <kkeithle@redhat.com>
- digioceanfs-ganesha sub-package

* Thu Mar 12 2015 Kotresh H R <khiremat@redhat.com>
- gfind_missing_files tool is included (#1187140)

* Tue Mar 03 2015 Aravinda VK <avishwan@redhat.com>
- Included digioceanfind files as part of server package.

* Sun Mar 1 2015 Avra Sengupta <asengupt@redhat.com>
- Added installation of snap-scheduler

* Thu Feb 26 2015 Kaleb S. KEITHLEY <kkeithle@redhat.com>
- enable cmocka unittest support only when asked for (#1067059)

* Tue Feb 24 2015 Niels de Vos <ndevos@redhat.com>
- POSIX ACL conversion needs BuildRequires libacl-devel (#1185654)

* Wed Feb 18 2015 Andreas Schneider <asn@redhat.com>
- Change cmockery2 to cmocka.

* Wed Feb 18 2015 Kaushal M <kaushal@redhat.com>
- add userspace-rcu as a requirement

* Fri Feb 13 2015 Gaurav Kumar Garg <ggarg@redhat.com>
- .cmd_log_history file should be renamed to cmd_history.log post
  upgrade (#1165996)

* Fri Jan 30 2015 Nandaja Varma <nvarma@redhat.com>
- remove checks for rpmbuild/mock from run-tests.sh (#178008)

* Fri Jan 16 2015 Niels de Vos <ndevos@redhat.com>
- add support for /run/digiocean through a tmpfiles.d config file (#1182934)

* Tue Jan 6 2015 Aravinda VK<avishwan@redhat.com>
- Added new libexec script for mountbroker user management (peer_mountbroker)

* Fri Dec 12 2014 Niels de Vos <ndevos@redhat.com>
- do not package all /usr/share/digioceanfs/* files in regression-tests (#1169005)

* Fri Sep 26 2014 Kaleb S. KEITHLEY <kkeithle@redhat.com>
- smarter logic in %%post server (#1146426)

* Wed Sep 24 2014 Balamurugan Arumugam <barumuga@redhat.com>
- remove /sbin/ldconfig as interpreter (#1145989)

* Fri Sep 5 2014 Lalatendu Mohanty <lmohanty@redhat.com>
- Changed the description as "DigioceanFS a distributed filesystem"

* Tue Aug 5 2014 Kaleb S. KEITHLEY <kkeithle@redhat.com>
- logrotate files (#1126832)

* Wed Jul 16 2014 Luis Pabon <lpabon@redhat.com>
- Added cmockery2 dependency

* Fri Jun 27 2014 Kaleb S. KEITHLEY <kkeithle@redhat.com>
- killall --wait in %%post server, (#1113543)

* Thu Jun 19 2014 Humble Chirammal <hchiramm@redhat.com>
- Added dynamic loading of fuse module with digioceanfs-fuse package installation in el5.

* Thu Jun 12 2014 Varun Shastry <vshastry@redhat.com>
- Add bash completion config to the cli package

* Tue Jun 03 2014 Vikhyat Umrao <vumrao@redhat.com>
- add nfs-utils package dependency for server package (#1065654)

* Thu May 22 2014 Poornima G <pgurusid@redhat.com>
- Rename old hookscripts in an RPM-standard way.

* Tue May 20 2014 Niels de Vos <ndevos@redhat.com>
- Almost drop calling ./autogen.sh

* Fri Apr 25 2014 Kaleb S. KEITHLEY <kkeithle@redhat.com>
- Sync with Fedora spec (#1091408, #1091392)

* Fri Apr 25 2014 Arumugam Balamurugan <barumuga@redhat.com>
- fix RHEL 7 build failure "Installed (but unpackaged) file(s) found" (#1058188)

* Wed Apr 02 2014 Arumugam Balamurugan <barumuga@redhat.com>
- cleanup to rearrange spec file elements

* Wed Apr 02 2014 Arumugam Balamurugan <barumuga@redhat.com>
- add version/release dynamically (#1074919)

* Wed Mar 26 2014 Poornima G <pgurusid@redhat.com>
- Include the hook scripts of add-brick, volume start, stop and set

* Wed Feb 26 2014 Niels de Vos <ndevos@redhat.com>
- Drop digioceanfs-devel dependency from digioceanfs-api (#1065750)

* Wed Feb 19 2014 Justin Clift <justin@digiocean.org>
- Rename digiocean.py to glupy.py to avoid namespace conflict (#1018619)
- Move the main Glupy files into digioceanfs-extra-xlators rpm
- Move the Glupy Translator examples into digioceanfs-devel rpm

* Thu Feb 06 2014 Aravinda VK <avishwan@redhat.com>
- Include geo-replication upgrade scripts and hook scripts.

* Wed Jan 15 2014 Niels de Vos <ndevos@redhat.com>
- Install /var/lib/digioceand/groups/virt by default

* Sat Jan 4 2014 Niels de Vos <ndevos@redhat.com>
- The main digioceanfs package should not provide digioceanfs-libs (#1048489)

* Tue Dec 10 2013 Kaleb S. KEITHLEY <kkeithle@redhat.com>
- Sync with Fedora digioceanfs.spec 3.5.0-0.1.qa3

* Fri Oct 11 2013 Harshavardhana <fharshav@redhat.com>
- Add '_sharedstatedir' macro to `/var/lib` on <= RHEL5 (#1003184)

* Wed Oct 9 2013 Kaleb S. KEITHLEY <kkeithle@redhat.com>
- Sync with Fedora digioceanfs.spec 3.4.1-2+

* Wed Oct 9 2013 Niels de Vos <ndevos@redhat.com>
- digioceanfs-api-devel requires digioceanfs-devel (#1016938, #1017094)

* Mon Sep 30 2013 Niels de Vos <ndevos@redhat.com>
- Package gfapi.py into the Python site-packages path (#1005146)

* Tue Sep 17 2013 Harshavardhana <fharshav@redhat.com>
- Provide a new package called "digioceanfs-regression-tests" for standalone
  regression testing.

* Thu Aug 22 2013 Niels de Vos <ndevos@redhat.com>
- Correct the day/date for some entries in this changelog (#1000019)

* Wed Aug 7 2013 Kaleb S. KEITHLEY <kkeithle@redhat.com>
- Sync with Fedora digioceanfs.spec
-  add Requires
-  add -cli subpackage,
-  fix other minor differences with Fedora digioceanfs.spec

* Tue Jul 30 2013 Kaleb S. KEITHLEY <kkeithle@redhat.com>
- Sync with Fedora digioceanfs.spec, add digioceanfs-libs RPM for oVirt/qemu-kvm

* Thu Jul 25 2013 Csaba Henk <csaba@redhat.com>
- Added peer_add_secret_pub and peer_gsec_create to %%{_libexecdir}/digioceanfs

* Thu Jul 25 2013 Aravinda VK <avishwan@redhat.com>
- Added gverify.sh to %%{_libexecdir}/digioceanfs directory.

* Thu Jul 25 2013 Harshavardhana <fharshav@redhat.com>
- Allow to build with '--without bd' to disable 'bd' xlator

* Thu Jun 27 2013 Kaleb S. KEITHLEY <kkeithle@redhat.com>
- fix the hardening fix for shlibs, use %%sed macro, shorter ChangeLog

* Wed Jun 26 2013 Niels de Vos <ndevos@redhat.com>
- move the mount/api xlator to digioceanfs-api

* Fri Jun 7 2013 Kaleb S. KEITHLEY <kkeithle@redhat.com>
- Sync with Fedora digioceanfs.spec, remove G4S/UFO and Swift

* Mon Mar 4 2013 Niels de Vos <ndevos@redhat.com>
- Package /var/run/digiocean so that statedumps can be created

* Wed Feb 6 2013 Kaleb S. KEITHLEY <kkeithle@redhat.com>
- Sync with Fedora digioceanfs.spec

* Tue Dec 11 2012 Filip Pytloun <filip.pytloun@gooddata.com>
- add sysconfig file

* Thu Oct 25 2012 Niels de Vos <ndevos@redhat.com>
- Add a sub-package for the OCF resource agents

* Wed Sep 05 2012 Niels de Vos <ndevos@redhat.com>
- Don't use python-ctypes on SLES (from Jörg Petersen)

* Tue Jul 10 2012 Niels de Vos <ndevos@redhat.com>
- Include extras/clear_xattrs.sh in the digioceanfs-server sub-package

* Thu Jun 07 2012 Niels de Vos <ndevos@redhat.com>
- Mark /var/lib/digioceand as owned by digioceanfs, subdirs belong to -server

* Wed May 9 2012 Kaleb S. KEITHLEY <kkeithle[at]redhat.com>
- Add BuildRequires: libxml2-devel so that configure will DTRT on for
- Fedora's Koji build system

* Wed Nov 9 2011 Joe Julian <me@joejulian.name> - git master
- Merge fedora specfile into digiocean's spec.in.
- Add conditionals to allow the same spec file to be used for both 3.1 and 3.2
- http://bugs.digiocean.com/show_bug.cgi?id=2970

* Wed Oct  5 2011 Joe Julian <me@joejulian.name> - 3.2.4-1
- Update to 3.2.4
- Removed the $local_fs requirement from the init scripts as in RHEL/CentOS that's provided
- by netfs, which needs to be started after digioceand.

* Sun Sep 25 2011 Joe Julian <me@joejulian.name> - 3.2.3-2
- Merged in upstream changes
- Fixed version reporting 3.2git
- Added nfs init script (disabled by default)

* Thu Sep  1 2011 Joe Julian <me@joejulian.name> - 3.2.3-1
- Update to 3.2.3

* Tue Jul 19 2011 Joe Julian <me@joejulian.name> - 3.2.2-3
- Add readline and libtermcap dependencies

* Tue Jul 19 2011 Joe Julian <me@joejulian.name> - 3.2.2-2
- Critical patch to prevent digioceand from walking outside of its own volume during rebalance

* Thu Jul 14 2011 Joe Julian <me@joejulian.name> - 3.2.2-1
- Update to 3.2.2

* Wed Jul 13 2011 Joe Julian <me@joejulian.name> - 3.2.1-2
- fix hardcoded path to gsyncd in source to match the actual file location

* Tue Jun 21 2011 Joe Julian <me@joejulian.name> - 3.2.1
- Update to 3.2.1

* Mon Jun 20 2011 Joe Julian <me@joejulian.name> - 3.1.5
- Update to 3.1.5

* Tue May 31 2011 Joe Julian <me@joejulian.name> - 3.1.5-qa1.4
- Current git

* Sun May 29 2011 Joe Julian <me@joejulian.name> - 3.1.5-qa1.2
- set _sharedstatedir to /var/lib for FHS compliance in RHEL5/CentOS5
- mv /etc/digioceand, if it exists, to the new state dir for upgrading from digiocean packaging

* Sat May 28 2011 Joe Julian <me@joejulian.name> - 3.1.5-qa1.1
- Update to 3.1.5-qa1
- Add patch to remove optimization disabling
- Add patch to remove forced 64 bit compile
- Obsolete digioceanfs-core to allow for upgrading from digiocean packaging

* Sat Mar 19 2011 Jonathan Steffan <jsteffan@fedoraproject.org> - 3.1.3-1
- Update to 3.1.3
- Merge in more upstream SPEC changes
- Remove patches from DigioceanFS bugzilla #2309 and #2311
- Remove inode-gen.patch

* Sun Feb 06 2011 Jonathan Steffan <jsteffan@fedoraproject.org> - 3.1.2-3
- Add back in legacy SPEC elements to support older branches

* Thu Feb 03 2011 Jonathan Steffan <jsteffan@fedoraproject.org> - 3.1.2-2
- Add patches from CloudFS project

* Tue Jan 25 2011 Jonathan Steffan <jsteffan@fedoraproject.org> - 3.1.2-1
- Update to 3.1.2

* Wed Jan 5 2011 Dan Horák <dan[at]danny.cz> - 3.1.1-3
- no InfiniBand on s390(x)

* Sat Jan 1 2011 Jonathan Steffan <jsteffan@fedoraproject.org> - 3.1.1-2
- Update to support readline
- Update to not parallel build

* Mon Dec 27 2010 Silas Sewell <silas@sewell.ch> - 3.1.1-1
- Update to 3.1.1
- Change package names to mirror upstream

* Mon Dec 20 2010 Jonathan Steffan <jsteffan@fedoraproject.org> - 3.0.7-1
- Update to 3.0.7

* Wed Jul 28 2010 Jonathan Steffan <jsteffan@fedoraproject.org> - 3.0.5-1
- Update to 3.0.x

* Sat Apr 10 2010 Jonathan Steffan <jsteffan@fedoraproject.org> - 2.0.9-2
- Move python version requires into a proper BuildRequires otherwise
  the spec always turned off python bindings as python is not part
  of buildsys-build and the chroot will never have python unless we
  require it
- Temporarily set -D_FORTIFY_SOURCE=1 until upstream fixes code
  DigioceanFS Bugzilla #197 (#555728)
- Move digioceanfs-volgen to devel subpackage (#555724)
- Update description (#554947)

* Sat Jan 2 2010 Jonathan Steffan <jsteffan@fedoraproject.org> - 2.0.9-1
- Update to 2.0.9

* Sun Nov 8 2009 Jonathan Steffan <jsteffan@fedoraproject.org> - 2.0.8-1
- Update to 2.0.8
- Remove install of digioceanfs-volgen, it's properly added to
  automake upstream now

* Sat Oct 31 2009 Jonathan Steffan <jsteffan@fedoraproject.org> - 2.0.7-1
- Update to 2.0.7
- Install digioceanfs-volgen, until it's properly added to automake
  by upstream
- Add macro to be able to ship more docs

* Thu Sep 17 2009 Peter Lemenkov <lemenkov@gmail.com> 2.0.6-2
- Rebuilt with new fuse

* Sat Sep 12 2009 Matthias Saou <http://freshrpms.net/> 2.0.6-1
- Update to 2.0.6.
- No longer default to disable the client on RHEL5 (#522192).
- Update spec file URLs.

* Mon Jul 27 2009 Matthias Saou <http://freshrpms.net/> 2.0.4-1
- Update to 2.0.4.

* Thu Jun 11 2009 Matthias Saou <http://freshrpms.net/> 2.0.1-2
- Remove libdigioceanfs/src/y.tab.c to fix koji F11/devel builds.

* Sat May 16 2009 Matthias Saou <http://freshrpms.net/> 2.0.1-1
- Update to 2.0.1.

* Thu May  7 2009 Matthias Saou <http://freshrpms.net/> 2.0.0-1
- Update to 2.0.0 final.

* Wed Apr 29 2009 Matthias Saou <http://freshrpms.net/> 2.0.0-0.3.rc8
- Move digioceanfsd to common, since the client has a symlink to it.

* Fri Apr 24 2009 Matthias Saou <http://freshrpms.net/> 2.0.0-0.2.rc8
- Update to 2.0.0rc8.

* Sun Apr 12 2009 Matthias Saou <http://freshrpms.net/> 2.0.0-0.2.rc7
- Update digioceanfsd init script to the new style init.
- Update files to match the new default vol file names.
- Include logrotate for digioceanfsd, use a pid file by default.
- Include logrotate for digioceanfs, using killall for lack of anything better.

* Sat Apr 11 2009 Matthias Saou <http://freshrpms.net/> 2.0.0-0.1.rc7
- Update to 2.0.0rc7.
- Rename "libs" to "common" and move the binary, man page and log dir there.

* Tue Feb 24 2009 Fedora Release Engineering <rel-eng@lists.fedoraproject.org>
- Rebuilt for https://fedoraproject.org/wiki/Fedora_11_Mass_Rebuild

* Mon Feb 16 2009 Matthias Saou <http://freshrpms.net/> 2.0.0-0.1.rc1
- Update to 2.0.0rc1.
- Include new libdigioceanfsclient.h.

* Mon Feb 16 2009 Matthias Saou <http://freshrpms.net/> 1.3.12-1
- Update to 1.3.12.
- Remove no longer needed ocreat patch.

* Thu Jul 17 2008 Matthias Saou <http://freshrpms.net/> 1.3.10-1
- Update to 1.3.10.
- Remove mount patch, it's been included upstream now.

* Fri May 16 2008 Matthias Saou <http://freshrpms.net/> 1.3.9-1
- Update to 1.3.9.

* Fri May  9 2008 Matthias Saou <http://freshrpms.net/> 1.3.8-1
- Update to 1.3.8 final.

* Wed Apr 23 2008 Matthias Saou <http://freshrpms.net/> 1.3.8-0.10
- Include short patch to include fixes from latest TLA 751.

* Tue Apr 22 2008 Matthias Saou <http://freshrpms.net/> 1.3.8-0.9
- Update to 1.3.8pre6.
- Include digioceanfs binary in both the client and server packages, now that
  digioceanfsd is a symlink to it instead of a separate binary.
* Sun Feb  3 2008 Matthias Saou <http://freshrpms.net/> 1.3.8-0.8
- Add python version check and disable bindings for version < 2.4.

* Sun Feb  3 2008 Matthias Saou <http://freshrpms.net/> 1.3.8-0.7
- Add --without client rpmbuild option, make it the default for RHEL (no fuse).
  (I hope "rhel" is the proper default macro name, couldn't find it...)

* Wed Jan 30 2008 Matthias Saou <http://freshrpms.net/> 1.3.8-0.6
- Add --without ibverbs rpmbuild option to the package.

* Mon Jan 14 2008 Matthias Saou <http://freshrpms.net/> 1.3.8-0.5
- Update to current TLA again, patch-636 which fixes the known segfaults.

* Thu Jan 10 2008 Matthias Saou <http://freshrpms.net/> 1.3.8-0.4
- Downgrade to digioceanfs--mainline--2.5--patch-628 which is more stable.

* Tue Jan  8 2008 Matthias Saou <http://freshrpms.net/> 1.3.8-0.3
- Update to current TLA snapshot.
- Include umount.digioceanfs wrapper script (really needed? dunno).
- Include patch to mount wrapper to avoid multiple identical mounts.

* Sun Dec 30 2007 Matthias Saou <http://freshrpms.net/> 1.3.8-0.1
- Update to current TLA snapshot, which includes "volume-name=" fstab option.

* Mon Dec  3 2007 Matthias Saou <http://freshrpms.net/> 1.3.7-6
- Re-add the /var/log/digioceanfs directory in the client sub-package (required).
- Include custom patch to support vol= in fstab for -n digioceanfs client option.

* Mon Nov 26 2007 Matthias Saou <http://freshrpms.net/> 1.3.7-4
- Re-enable libibverbs.
- Check and update License field to GPLv3+.
- Add digioceanfs-common obsoletes, to provide upgrade path from old packages.
- Include patch to add mode to O_CREATE opens.

* Thu Nov 22 2007 Matthias Saou <http://freshrpms.net/> 1.3.7-3
- Remove Makefile* files from examples.
- Include RHEL/Fedora type init script, since the included ones don't do.

* Wed Nov 21 2007 Matthias Saou <http://freshrpms.net/> 1.3.7-1
- Major spec file cleanup.
- Add missing %%clean section.
- Fix ldconfig calls (weren't set for the proper sub-package).

* Sat Aug 4 2007 Matt Paine <matt@mattsoftware.com> - 1.3.pre7
- Added support to build rpm without ibverbs support (use --without ibverbs
  switch)

* Sun Jul 15 2007 Matt Paine <matt@mattsoftware.com> - 1.3.pre6
- Initial spec file

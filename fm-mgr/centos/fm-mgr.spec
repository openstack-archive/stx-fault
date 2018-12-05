%define local_dir /usr/local
%define local_bindir %{local_dir}/bin

Summary: CGTS Platform Fault Manager Package
Name: fm-mgr
Version: 1.0
Release: %{tis_patch_ver}%{?_tis_dist}
License: Apache-2.0
Group: base
Packager: Wind River <info@windriver.com>
URL: unknown
Source0: %{name}-%{version}.tar.gz
BuildRequires: fm-common-dev
BuildRequires: systemd-devel
BuildRequires: libuuid-devel

%description
CGTS platform Fault Manager that serves the client
application fault management requests and raise/clear/update
alarms in the active alarm database.

%prep
%setup

%build
VER=%{version}
MAJOR=`echo $VER | awk -F . '{print $1}'`
MINOR=`echo $VER | awk -F . '{print $2}'`
make  MAJOR=$MAJOR MINOR=$MINOR %{?_smp_mflags}

%install
rm -rf %{buildroot}
VER=%{version}
MAJOR=`echo $VER | awk -F . '{print $1}'`
MINOR=`echo $VER | awk -F . '{print $2}'`
make DESTDIR=%{buildroot} \
     BINDIR=%{local_bindir} \
     SYSCONFDIR=%{_sysconfdir} \
     UNITDIR=%{_unitdir} \
     MAJOR=$MAJOR MINOR=$MINOR \
     install

%clean
rm -rf %{buildroot}


%files
%defattr(-,root,root,-)
%doc LICENSE
%{local_bindir}/fmManager
%_sysconfdir/init.d/fminit
%{_unitdir}/fminit.service
%config(noreplace) %{_sysconfdir}/logrotate.d/fm.logrotate

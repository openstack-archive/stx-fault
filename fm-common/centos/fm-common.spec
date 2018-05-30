%define local_dir /usr/local
%define local_bindir %{local_dir}/bin
%define cgcs_doc_deploy_dir /opt/deploy/cgcs_doc

Summary: CGTS Platform Fault Management Common Package
Name: fm-common
Version: 1.0
Release: %{tis_patch_ver}%{?_tis_dist}
License: Apache-2.0
Group: base
Packager: Wind River <info@windriver.com>
URL: unknown
Source0: %{name}-%{version}.tar.gz
BuildRequires: util-linux
BuildRequires: postgresql-devel
BuildRequires: libuuid-devel
BuildRequires: python-devel

%package -n fm-common-dev
Summary: CGTS Platform Fault Management Common Package - Development files
Group: devel
Requires: fm-common = %{version}-%{release}

%description
Titanium Cloud platform Fault Management Client Library that provides APIs for
applications to raise/clear/update active alarms.

%description -n fm-common-dev
Titanium Cloud platform Fault Management Client Library that provides APIs for
applications to raise/clear/update active alarms.  This package contains
symbolic links, header files, and related items necessary for software
development.

%package -n fm-common-doc
Summary: fm-common deploy doc
Group: doc

%description -n fm-common-doc
Contains fmAlarm.h which is to be used by fm-doc package to validate
the Alarms & Logs Doc Yaml file

%prep
%setup

%build
VER=%{version}
MAJOR=`echo $VER | awk -F . '{print $1}'`
MINOR=`echo $VER | awk -F . '{print $2}'`
make  MAJOR=$MAJOR MINOR=$MINOR %{?_smp_mflags}

%install
rm -rf $RPM_BUILD_ROOT
VER=%{version}
MAJOR=`echo $VER | awk -F . '{print $1}'`
MINOR=`echo $VER | awk -F . '{print $2}'`
make DEST_DIR=$RPM_BUILD_ROOT BIN_DIR=%{local_bindir} LIB_DIR=%{_libdir} INC_DIR=%{_includedir} MAJOR=$MAJOR MINOR=$MINOR install_non_bb

install -d $RPM_BUILD_ROOT/usr/bin
install -m 755 fm_db_sync_event_suppression.py $RPM_BUILD_ROOT/usr/bin/fm_db_sync_event_suppression.py

CGCS_DOC_DEPLOY=$RPM_BUILD_ROOT/%{cgcs_doc_deploy_dir}
install -d $CGCS_DOC_DEPLOY
# install fmAlarm.h in CGCS_DOC_DEPLOY_DIR
# used by fm-doc package to validate the Alarms & Logs Doc Yaml file
install -m 644 fmAlarm.h $CGCS_DOC_DEPLOY

%clean
rm -rf $RPM_BUILD_ROOT


%files
%defattr(-,root,root,-)
%doc LICENSE
%{local_bindir}/*
%{_libdir}/*.so.*
/usr/bin/fm_db_sync_event_suppression.py

%files -n fm-common-dev
%defattr(-,root,root,-)
%{_includedir}/*
%{_libdir}/*.so

%files -n fm-common-doc
%defattr(-,root,root,-)
%{cgcs_doc_deploy_dir}/*

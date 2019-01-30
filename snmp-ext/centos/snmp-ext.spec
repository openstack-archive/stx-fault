Summary: CGTS Platform SNMP extension Package
Name: snmp-ext
Version: 1.0
Release: %{tis_patch_ver}%{?_tis_dist}
License: Apache-2.0
Group: base
Packager: Wind River <info@windriver.com>
URL: unknown
Source0: %{name}-%{version}.tar.gz
BuildRequires: fm-common-dev
BuildRequires: net-snmp-devel
BuildRequires: libuuid-devel
Requires: fm-common
Requires: net-snmp

%define mib_ver 2.0
%define cgcs_sdk_deploy_dir /opt/deploy/cgcs_sdk

%description
Titanium Cloud platform SNMP extension provides Wind River enterprise MIBs support
and it serves as SNMP based alarm surveillance module for Network Manager
System.

%package -n snmp-ext-devel
Summary: Titanium Cloud Platform SNMP extension Package - Development files
Group: devel
Requires: snmp-ext = %{version}-%{release}

%description -n snmp-ext-devel
Titanium Cloud platform SNMP extension provides Wind River enterprise MIBs support
and it serves as SNMP based alarm surveillance module for Network Manager
System.  This package contains symbolic links, header files, and related
items necessary for software development.

%package -n %{name}-cgts-sdk
Summary: Titanium Cloud Platform SNMP extension Package - SDK
Group: devel

%description -n %{name}-cgts-sdk

%prep
%setup

%build
MAJOR=`echo %{version} | awk -F . '{print $1}'`
MINOR=`echo %{version} | awk -F . '{print $2}'`
make MAJOR=$MAJOR MINOR=$MINOR PATCH=%{tis_patch_ver} %{?_smp_mflags}

%install
MAJOR=`echo %{version} | awk -F . '{print $1}'`
MINOR=`echo %{version} | awk -F . '{print $2}'`
make DEST_DIR=%{buildroot} \
     LIB_DIR=%{_libdir} \
     MAJOR=$MAJOR \
     MINOR=$MINOR \
     SDK_DEPLOY_DIR=%{buildroot}%{cgcs_sdk_deploy_dir} \
     MIBVER=%{mib_ver} \
     PATCH=%{tis_patch_ver} install

%files
%defattr(-,root,root,-)
%doc LICENSE
%{_libdir}/*.so.*
%{_datadir}/snmp/mibs/*

%files -n snmp-ext-devel
%defattr(-,root,root,-)
%{_libdir}/*.so

%files -n %{name}-cgts-sdk
%{cgcs_sdk_deploy_dir}/wrs-snmp-mib-*.%{mib_ver}.tgz

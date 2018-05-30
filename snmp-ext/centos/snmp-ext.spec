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

Source1: wrsAlarmMib.mib.txt
Source2: wrsEnterpriseReg.mib.txt

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
VER=%{version}
MAJOR=`echo $VER | awk -F . '{print $1}'`
MINOR=`echo $VER | awk -F . '{print $2}'`
PATCHVER=` echo %{release} | awk -F r '{print $2}' | awk -F . '{print $1}'`
make  MAJOR=$MAJOR MINOR=$MINOR PATCH=$PATCHVER %{?_smp_mflags}

%install
rm -rf $RPM_BUILD_ROOT
VER=%{version}
MAJOR=`echo $VER | awk -F . '{print $1}'`
MINOR=`echo $VER | awk -F . '{print $2}'`

PATCHVER=` echo %{release} | awk -F r '{print $2}' | awk -F . '{print $1}'`
make DEST_DIR=$RPM_BUILD_ROOT LIB_DIR=%{_libdir} MAJOR=$MAJOR MINOR=$MINOR PATCH=$PATCHVER install_non_bb

MIBDIR=wrs-snmp-mib-${MAJOR}.%{mib_ver}
mkdir -p $RPM_BUILD_ROOT%{_datadir}/snmp/mibs
install -m 644 %{SOURCE1} $RPM_BUILD_ROOT%{_datadir}/snmp/mibs
install -m 644 %{SOURCE2} $RPM_BUILD_ROOT%{_datadir}/snmp/mibs
tar -zc --transform=s/^mibs/${MIBDIR}/ -f wrs-snmp-mib-${MAJOR}.%{mib_ver}.tgz -C $RPM_BUILD_ROOT%{_datadir}/snmp mibs
mkdir -p $RPM_BUILD_ROOT%{cgcs_sdk_deploy_dir}
install -m 644 wrs-snmp-mib-${MAJOR}.%{mib_ver}.tgz $RPM_BUILD_ROOT%{cgcs_sdk_deploy_dir}

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

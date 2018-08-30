Summary: Titanium Cloud Platform SNMP Audit Trail
Name: snmp-audittrail
Version: 1.0
Release: %{tis_patch_ver}%{?_tis_dist}
License: Apache-2.0
Group: base
Packager: Wind River <info@windriver.com>
URL: unknown
Source0: %{name}-%{version}.tar.gz
BuildRequires: net-snmp-devel
BuildRequires: fm-common-dev
BuildRequires: uuid-devel
BuildRequires: uuid
BuildRequires: libuuid-devel
Requires: net-snmp
Requires: uuid

%description
Titanium Cloud platform SNMP Audit Trail provides audit trail support for incoming
SNMP requests.

%package -n snmp-audittrail-devel
Summary: Titanium Cloud Platform SNMP Audit Trail Package - Development files
Group: devel
Requires: snmp-audittrail = %{version}-%{release}

%description -n snmp-audittrail-devel
Titanium Cloud platform SNMP Audit Trail provides audit trail support for incoming
SNMP requests.  This package contains symbolic links, header files, and related
items necessary for software development.

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

%files
%defattr(-,root,root,-)
%doc LICENSE
%{_libdir}/*.so.*

%files -n snmp-audittrail-devel
%defattr(-,root,root,-)
%{_libdir}/*.so

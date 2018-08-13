%global pypi_name fmclient

Summary: A python client library for Fault Management
Name: python-fmclient
Version: 1.0
Release: %{tis_patch_ver}%{?_tis_dist}
License: windriver
Group: base
Packager: Wind River <info@windriver.com>
URL: unknown
Source0: %{name}-%{version}.tar.gz

BuildRequires:  git
BuildRequires:  python-pbr >= 2.0.0
BuildRequires:  python-setuptools

Requires:       python-keystoneauth1 >= 3.1.0
Requires:       python-pbr >= 2.0.0
Requires:       python-six >= 1.9.0
Requires:       python-oslo-i18n >= 2.1.0
Requires:       python-oslo-utils >= 3.20.0
Requires:       python-requests

%description
A python client library for Fault Management

%define local_bindir /usr/bin/
%define pythonroot /usr/lib64/python2.7/site-packages

%define debug_package %{nil}

%package          sdk
Summary:          SDK files for %{name}

%description      sdk
Contains SDK files for %{name} package

%prep
%autosetup -n %{name}-%{version} -S git

# Remove bundled egg-info
rm -rf *.egg-info

%build
echo "Start build"

export PBR_VERSION=%{version}
%{__python} setup.py build

%install
echo "Start install"
export PBR_VERSION=%{version}
%{__python} setup.py install --root=%{buildroot} \
                             --install-lib=%{pythonroot} \
                             --prefix=/usr \
                             --install-data=/usr/share \
                             --single-version-externally-managed

# prep SDK package
mkdir -p %{buildroot}/usr/share/remote-clients
tar zcf %{buildroot}/usr/share/remote-clients/%{name}-%{version}.tgz --exclude='.gitignore' --exclude='.gitreview' -C .. %{name}-%{version}

%clean
echo "CLEAN CALLED"
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root,-)
%doc LICENSE
%{local_bindir}/*
%{pythonroot}/%{pypi_name}/*
%{pythonroot}/%{pypi_name}-%{version}*.egg-info

%files sdk
/usr/share/remote-clients/%{name}-%{version}.tgz


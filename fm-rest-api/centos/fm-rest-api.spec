Summary: Fault Management Openstack REST API
Name: fm-rest-api
Version: 1.0
Release: %{tis_patch_ver}%{?_tis_dist}
License: windriver
Group: base
Packager: Wind River <info@windriver.com>
URL: unknown
Source0: %{name}-%{version}.tar.gz


BuildRequires: python-setuptools
BuildRequires: python-oslo-config
BuildRequires: python-oslo-db
BuildRequires: python-oslo-log
BuildRequires: python-oslo-messaging
BuildRequires: python-oslo-middleware

Requires: python-eventlet
Requires: python-routes
Requires: python-webob
Requires: python-paste

BuildRequires: systemd

%description
Fault Management Openstack REST API Service

%define local_bindir /usr/bin/
%define local_initddir /etc/rc.d/init.d
%define pythonroot /usr/lib64/python2.7/site-packages
%define local_etc_pmond /etc/pmon.d/
%define debug_package %{nil}

%prep
%autosetup -n %{name}-%{version}

# Remove bundled egg-info
rm -rf *.egg-info

%build
echo "Start build"

export PBR_VERSION=%{version}
%{__python} setup.py build
PYTHONPATH=. oslo-config-generator --config-file=fm/config-generator.conf

%install
echo "Start install"
export PBR_VERSION=%{version}
%{__python} setup.py install --root=%{buildroot} \
                             --install-lib=%{pythonroot} \
                             --prefix=/usr \
                             --install-data=/usr/share \
                             --single-version-externally-managed

install -p -D -m 644 scripts/fm-api.service %{buildroot}%{_unitdir}/fm-api.service
install -d -m 755 %{buildroot}%{local_initddir}
install -p -D -m 755 scripts/fm-api %{buildroot}%{local_initddir}/fm-api

install -d -m 755 %{buildroot}%{local_etc_pmond}
install -p -D -m 644 fm-api-pmond.conf %{buildroot}%{local_etc_pmond}/fm-api.conf

# Install sql migration stuff that wasn't installed by setup.py
install -m 640 fm/db/sqlalchemy/migrate_repo/migrate.cfg %{buildroot}%{pythonroot}/fm/db/sqlalchemy/migrate_repo/migrate.cfg

# install default config files
cd %{_builddir}/%{name}-%{version} && oslo-config-generator --config-file fm/config-generator.conf --output-file %{_builddir}/%{name}-%{version}/fm.conf.sample
install -p -D -m 644 %{_builddir}/%{name}-%{version}/fm.conf.sample %{buildroot}%{_sysconfdir}/fm/fm.conf

%clean
echo "CLEAN CALLED"
rm -rf $RPM_BUILD_ROOT

%post
/bin/systemctl enable fm-api.service >/dev/null 2>&1

%files
%defattr(-,root,root,-)
%doc LICENSE

%{local_bindir}/*

%{local_initddir}/*

%{pythonroot}/fm/*

%{pythonroot}/fm-%{version}*.egg-info

%config(noreplace) %{_sysconfdir}/fm/fm.conf

# systemctl service files
%{_unitdir}/fm-api.service

# pmond config file
%{local_etc_pmond}/fm-api.conf


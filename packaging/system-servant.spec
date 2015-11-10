Name:       system-servant
Summary:    Servant application for System FW
Version:    0.2.0
Release:    1
Group:      System/Utilities
License:    Apache-2.0
Source0:    %{name}-%{version}.tar.gz
Source1:    %{name}.manifest
BuildRequires:  pkgconfig(elementary)
BuildRequires:  pkgconfig(appcore-efl)
BuildRequires:  pkgconfig(syspopup)
BuildRequires:  pkgconfig(syspopup-caller)
BuildRequires:  pkgconfig(notification) 
BuildRequires:  pkgconfig(vconf)
BuildRequires:  pkgconfig(dlog)
BuildRequires:  pkgconfig(dbus-1)
BuildRequires:  pkgconfig(edbus)
BuildRequires:  pkgconfig(libtzplatform-config)
BuildRequires:  cmake
BuildRequires:  gettext-devel

%description
System applications such as app-launcher
and service file for dbus activation

%prep
%setup -q

%build
cp %{SOURCE1} .

cmake . \
		-DCMAKE_INSTALL_PREFIX=%{_prefix} \
		-DPKGNAME=%{name} \
		-DTZ_SYS_RO_APP=%{TZ_SYS_RO_APP} \
		-DTZ_SYS_RO_PACKAGES=%{TZ_SYS_RO_PACKAGES} \
		-DTZ_SYS_SMACK=%{TZ_SYS_SMACK} \
		-DTZ_SYS_SHARE=%{TZ_SYS_SHARE}

make %{?jobs:-j%jobs}

%install
rm -rf %{buildroot}
%make_install


%files
%manifest %{name}.manifest
%license LICENSE
%{_bindir}/sysapp-launcher
%{_datadir}/license/sysapp-launcher
%{_datadir}/dbus-1/system-services/org.tizen.system.popup.service
%{_sysconfdir}/smack/accesses.d/system-servant.efl


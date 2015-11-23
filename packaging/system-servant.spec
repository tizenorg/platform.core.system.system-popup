%bcond_with x
%bcond_with wayland
%bcond_with emulator

%define PROFILE common

#Main applications
%define poweroff_popup on

%if "%{?tizen_profile_name}" == "mobile"
%define PROFILE mobile
#Main applicaitons
%define poweroff_popup on
%endif

%if "%{?tizen_profile_name}" == "wearable"
%define PROFILE wearable
#Main applicaitons
%define poweroff_popup on
%endif

%if "%{?tizen_profile_name}" == "tv"
%define PROFILE tv
%endif

Name:       system-servant
Summary:    Servant application for System FW
Version:    0.2.0
Release:    1
Group:      System/Utilities
License:    Apache-2.0
Source0:    %{name}-%{version}.tar.gz
Source1:    %{name}.manifest
Source1001:    org.tizen.poweroff-syspopup.manifest
BuildRequires:  pkgconfig(elementary)
BuildRequires:  pkgconfig(appcore-efl)
BuildRequires:  pkgconfig(syspopup)
BuildRequires:  pkgconfig(syspopup-caller)
BuildRequires:  pkgconfig(notification) 
BuildRequires:  pkgconfig(vconf)
BuildRequires:  pkgconfig(glib-2.0)
BuildRequires:  pkgconfig(dlog)
BuildRequires:  pkgconfig(dbus-1)
BuildRequires:  pkgconfig(edbus)
BuildRequires:  pkgconfig(deviced)
BuildRequires:  pkgconfig(feedback)
BuildRequires:  pkgconfig(efl-extension)
BuildRequires:  pkgconfig(libtzplatform-config)
%if %{with x}
BuildRequires:  pkgconfig(ecore-x)
BuildRequires:  pkgconfig(utilX)
%endif
BuildRequires:  cmake
BuildRequires:  gettext-devel

%description
System applications such as app-launcher
and service file for dbus activation

%if %{?poweroff_popup} == on
%package -n org.tizen.poweroff-syspopup
Summary:    poweroff-popup application
Group:      main
Requires:   %{name} = %{version}-%{release}

%description -n org.tizen.poweroff-syspopup
poweroff-popup application.
%endif

%prep
%setup -q

%build
cp %{SOURCE1} .

%if %{poweroff_popup} == on
cp %{SOURCE1001} .
%endif

%define DPMS none
%if %{with x}
%define DPMS x
%endif
%if %{with wayland}
%define DPMS wayland
%endif

%cmake . \
		-DCMAKE_INSTALL_PREFIX=%{_prefix} \
		-DPKGNAME=%{name} \
		-DPROFILE=%{PROFILE} \
		-DDPMS=%{DPMS} \
		-DTZ_SYS_RO_APP=%{TZ_SYS_RO_APP} \
		-DTZ_SYS_RO_PACKAGES=%{TZ_SYS_RO_PACKAGES} \
		-DTZ_SYS_SMACK=%{TZ_SYS_SMACK} \
		-DTZ_SYS_SHARE=%{TZ_SYS_SHARE} \
		-DTZ_SYS_RO_APP=%{TZ_SYS_RO_APP} \
		-DPOWEROFF_POPUP=%{poweroff_popup} \

make %{?jobs:-j%jobs}

%install
rm -rf %{buildroot}
%make_install


%files
%manifest %{name}.manifest
%{_bindir}/sysapp-launcher
%{_datadir}/license/sysapp-launcher
%{_datadir}/dbus-1/system-services/org.tizen.system.popup.service

#po files to support multi-languages
%lang(ar) %{_datadir}/locale/ar/LC_MESSAGES/system-servant.mo
%lang(az) %{_datadir}/locale/az/LC_MESSAGES/system-servant.mo
%lang(bg) %{_datadir}/locale/bg/LC_MESSAGES/system-servant.mo
%lang(bn) %{_datadir}/locale/bn/LC_MESSAGES/system-servant.mo
%lang(ca) %{_datadir}/locale/ca/LC_MESSAGES/system-servant.mo
%lang(cs) %{_datadir}/locale/cs/LC_MESSAGES/system-servant.mo
%lang(da) %{_datadir}/locale/da/LC_MESSAGES/system-servant.mo
%lang(de) %{_datadir}/locale/de/LC_MESSAGES/system-servant.mo
%lang(el_GR) %{_datadir}/locale/el_GR/LC_MESSAGES/system-servant.mo
%lang(en_PH) %{_datadir}/locale/en_PH/LC_MESSAGES/system-servant.mo
%lang(en) %{_datadir}/locale/en/LC_MESSAGES/system-servant.mo
%lang(en_US) %{_datadir}/locale/en_US/LC_MESSAGES/system-servant.mo
%lang(es_ES) %{_datadir}/locale/es_ES/LC_MESSAGES/system-servant.mo
%lang(es_US) %{_datadir}/locale/es_US/LC_MESSAGES/system-servant.mo
%lang(et) %{_datadir}/locale/et/LC_MESSAGES/system-servant.mo
%lang(eu) %{_datadir}/locale/eu/LC_MESSAGES/system-servant.mo
%lang(fa) %{_datadir}/locale/fa/LC_MESSAGES/system-servant.mo
%lang(fi) %{_datadir}/locale/fi/LC_MESSAGES/system-servant.mo
%lang(fr_CA) %{_datadir}/locale/fr_CA/LC_MESSAGES/system-servant.mo
%lang(fr) %{_datadir}/locale/fr/LC_MESSAGES/system-servant.mo
%lang(ga) %{_datadir}/locale/ga/LC_MESSAGES/system-servant.mo
%lang(gl) %{_datadir}/locale/gl/LC_MESSAGES/system-servant.mo
%lang(gu) %{_datadir}/locale/gu/LC_MESSAGES/system-servant.mo
%lang(he) %{_datadir}/locale/he/LC_MESSAGES/system-servant.mo
%lang(hi) %{_datadir}/locale/hi/LC_MESSAGES/system-servant.mo
%lang(hr) %{_datadir}/locale/hr/LC_MESSAGES/system-servant.mo
%lang(hu) %{_datadir}/locale/hu/LC_MESSAGES/system-servant.mo
%lang(hy) %{_datadir}/locale/hy/LC_MESSAGES/system-servant.mo
%lang(is) %{_datadir}/locale/is/LC_MESSAGES/system-servant.mo
%lang(it_IT) %{_datadir}/locale/it_IT/LC_MESSAGES/system-servant.mo
%lang(ja_JP) %{_datadir}/locale/ja_JP/LC_MESSAGES/system-servant.mo
%lang(ka) %{_datadir}/locale/ka/LC_MESSAGES/system-servant.mo
%lang(kk) %{_datadir}/locale/kk/LC_MESSAGES/system-servant.mo
%lang(kn) %{_datadir}/locale/kn/LC_MESSAGES/system-servant.mo
%lang(lt) %{_datadir}/locale/lt/LC_MESSAGES/system-servant.mo
%lang(lv) %{_datadir}/locale/lv/LC_MESSAGES/system-servant.mo
%lang(mk) %{_datadir}/locale/mk/LC_MESSAGES/system-servant.mo
%lang(ml) %{_datadir}/locale/ml/LC_MESSAGES/system-servant.mo
%lang(nb) %{_datadir}/locale/nb/LC_MESSAGES/system-servant.mo
%lang(nl) %{_datadir}/locale/nl/LC_MESSAGES/system-servant.mo
%lang(pl) %{_datadir}/locale/pl/LC_MESSAGES/system-servant.mo
%lang(pt_BR) %{_datadir}/locale/pt_BR/LC_MESSAGES/system-servant.mo
%lang(pt_PT) %{_datadir}/locale/pt_PT/LC_MESSAGES/system-servant.mo
%lang(ro) %{_datadir}/locale/ro/LC_MESSAGES/system-servant.mo
%lang(ru_RU) %{_datadir}/locale/ru_RU/LC_MESSAGES/system-servant.mo
%lang(si) %{_datadir}/locale/si/LC_MESSAGES/system-servant.mo
%lang(sk) %{_datadir}/locale/sk/LC_MESSAGES/system-servant.mo
%lang(sl) %{_datadir}/locale/sl/LC_MESSAGES/system-servant.mo
%lang(sr) %{_datadir}/locale/sr/LC_MESSAGES/system-servant.mo
%lang(sv) %{_datadir}/locale/sv/LC_MESSAGES/system-servant.mo
%lang(ta) %{_datadir}/locale/ta/LC_MESSAGES/system-servant.mo
%lang(te) %{_datadir}/locale/te/LC_MESSAGES/system-servant.mo
%lang(th) %{_datadir}/locale/th/LC_MESSAGES/system-servant.mo
%lang(tr_TR) %{_datadir}/locale/tr_TR/LC_MESSAGES/system-servant.mo
%lang(uk) %{_datadir}/locale/uk/LC_MESSAGES/system-servant.mo
%lang(ur) %{_datadir}/locale/ur/LC_MESSAGES/system-servant.mo
%lang(uz) %{_datadir}/locale/uz/LC_MESSAGES/system-servant.mo
%lang(zh_CN) %{_datadir}/locale/zh_CN/LC_MESSAGES/system-servant.mo
%lang(zh_HK) %{_datadir}/locale/zh_HK/LC_MESSAGES/system-servant.mo
%lang(zh_TW) %{_datadir}/locale/zh_TW/LC_MESSAGES/system-servant.mo


%if %{poweroff_popup} == on
%files -n org.tizen.poweroff-syspopup
%manifest org.tizen.poweroff-syspopup.manifest
%license LICENSE
%defattr(-,root,root,-)
%{TZ_SYS_RO_APP}/org.tizen.poweroff-syspopup/bin/poweroff-popup
%{TZ_SYS_SHARE}/packages/org.tizen.poweroff-syspopup.xml
%endif

%bcond_with x
%bcond_with wayland
%bcond_with emulator

%define PROFILE common
%define FORMFACTOR none

#Main applications
%define powerkey_popup off
%define crash_popup off
%define system_popup off
%define notification_service off
%define signal_sender off
#sub-popups of system-popup
%define battery_popup off
%define mmc_popup off
%define usb_popup off
%define watchdog_popup off
%define overheat_popup off
%define storage_popup off

%if "%{?profile}" == "mobile"
%define PROFILE mobile
#Main applicaitons
%define powerkey_popup on
%define crash_popup on
%define system_popup on
%define notification_service on
%define signal_sender on
#sub-popups of system-popup
%define battery_popup on
%define mmc_popup on
%define usb_popup on
%define watchdog_popup on
%if %{?system_popup} == on
%define overheat_popup on
%endif
%define storage_popup on
%endif

%if "%{?profile}" == "wearable"
%define PROFILE wearable
%if "%_repository" == "target-circle" || "%_repository" == "emulator-circle"
	%define FORMFACTOR circle
%else
	%define FORMFACTOR rectangle
%endif
#Main applicaitons
%define powerkey_popup on
%define crash_popup on
%define system_popup on
#sub-popups of system-popup
%define storage_popup on
%define watchdog_popup on
%define battery_popup on
%if %{?system_popup} == on
%define overheat_popup on
%endif
%endif

%if "%{?profile}" == "tv"
%define PROFILE tv
#Main applications
%define crash_popup on
#sub-popups of system-popup
%endif

Name:       system-servant
Summary:    Servant application for System FW
Version:    0.2.0
Release:    1
Group:      System/Utilities
License:    Apache-2.0
Source0:    %{name}-%{version}.tar.gz
Source1:    %{name}.manifest
Source1001:    org.tizen.powerkey-syspopup.manifest
Source1015:    org.tizen.crash-syspopup.manifest
Source2001:    org.tizen.system-syspopup.manifest
Source2003:    org.tizen.system-signal-sender.manifest
BuildRequires:  pkgconfig(appsvc)
BuildRequires:  pkgconfig(pkgmgr-info)
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
BuildRequires:  edje-bin

%if %{with x}
BuildRequires:  pkgconfig(ecore-x)
BuildRequires:  pkgconfig(utilX)
%endif
BuildRequires:  cmake
BuildRequires:  gettext-devel

%description
System applications such as app-launcher
and service file for dbus activation

%if %{?crash_popup} == on
%package -n org.tizen.crash-syspopup
Summary:    System popup application (crash system popup)
Group:      System/Utilities
Requires:   %{name} = %{version}-%{release}

%description -n org.tizen.crash-syspopup
to inform user crash information. It is activated
when crash event is happend
%endif

%if %{?powerkey_popup} == on
%package -n org.tizen.powerkey-syspopup
Summary:    Powerkey-popup application
Group:      System/Utilities
Requires:   %{name} = %{version}-%{release}

%description -n org.tizen.powerkey-syspopup
to inform user powerkey information. It is activated
when user power key event is happened
%endif

%if %{?signal_sender} == on
%package -n org.tizen.system-signal-sender
Summary:    System FW signal sender
Group:      System/Utilities
Requires:   %{name} = %{version}-%{release}

%description -n org.tizen.system-signal-sender
to inform user system FW signal sender. It is activated
when system event is happend
%endif

%if %{?system_popup} == on
%package -n org.tizen.system-syspopup
Summary:    System popup application
Group:      System/Utilities
Requires:   %{name} = %{version}-%{release}

%description -n org.tizen.system-syspopup
to inform user system information. It is activated
when system event is happend

%endif # system_popup

%prep
%setup -q

%build
chmod 0644 %{SOURCE1}
cp %{SOURCE1} .

%if %{powerkey_popup} == on
chmod 0644 %{SOURCE1001}
cp %{SOURCE1001} .
%endif

%if %{crash_popup} == on
chmod 0644 %{SOURCE1015}
cp %{SOURCE1015} .
%endif

%if %{system_popup} == on
chmod 0644 %{SOURCE2001}
cp %{SOURCE2001} .
%endif

%if %{signal_sender} == on
chmod 0644 %{SOURCE2003}
cp %{SOURCE2003} .
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
		-DFORMFACTOR=%{FORMFACTOR} \
		-DDPMS=%{DPMS} \
		-DTZ_SYS_RO_APP=%{TZ_SYS_RO_APP} \
		-DTZ_SYS_RO_PACKAGES=%{TZ_SYS_RO_PACKAGES} \
		-DTZ_SYS_SMACK=%{TZ_SYS_SMACK} \
		-DTZ_SYS_SHARE=%{TZ_SYS_SHARE} \
		-DTZ_SYS_RO_SHARE=%{TZ_SYS_RO_SHARE} \
		-DTZ_SYS_RO_APP=%{TZ_SYS_RO_APP} \
		-DPOWERKEY_POPUP=%{powerkey_popup} \
		-DCRASH_POPUP=%{crash_popup} \
		-DNOTIFICATION_SERVICE=%{notification_service} \
		-DBATTERY_POPUP=%{battery_popup} \
		-DSYSTEM_POPUP=%{system_popup} \
		-DSIGNAL_SENDER=%{signal_sender} \
		-DMMC_POPUP=%{mmc_popup} \
		-DSTORAGE_POPUP=%{storage_popup} \
		-DUSB_POPUP=%{usb_popup} \
		-DWATCHDOG_POPUP=%{watchdog_popup} \
		-DOVERHEAT_POPUP=%{overheat_popup} \

make %{?jobs:-j%jobs}

%install
rm -rf %{buildroot}
%make_install


%files
%manifest %{name}.manifest
%{_bindir}/sysapp-launcher
%{_datadir}/license/sysapp-launcher
%{_datadir}/dbus-1/system-services/org.tizen.system.popup.service
%config %{_sysconfdir}/dbus-1/system.d/launcher.conf

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

%if %{notification_service} == on
%{_datadir}/system-apps/res/icons/batt_full_icon.png
%{_datadir}/system-apps/res/icons/batt_full_indicator.png
%{TZ_SYS_RO_SHARE}/system-apps/res/icons/datausage_warning.png
%{TZ_SYS_RO_SHARE}/system-apps/res/icons/led_torch.png
%endif

%if %{crash_popup} == on
%files -n org.tizen.crash-syspopup
%manifest org.tizen.crash-syspopup.manifest
%license LICENSE
%defattr(-,root,root,-)
%{TZ_SYS_RO_APP}/org.tizen.crash-syspopup/bin/crash-popup
%{TZ_SYS_RO_SHARE}/packages/org.tizen.crash-syspopup.xml
%endif

%if %{system_popup} == on
%files -n org.tizen.system-syspopup
%manifest org.tizen.system-syspopup.manifest
%defattr(-,root,root,-)
%{TZ_SYS_RO_APP}/org.tizen.system-syspopup/bin/system-syspopup
%if %{overheat_popup} == on
%{TZ_SYS_RO_APP}/org.tizen.system-syspopup/shared/res/system-syspopup.edj
%endif
%{TZ_SYS_RO_SHARE}/packages/org.tizen.system-syspopup.xml
%endif

%if %{powerkey_popup} == on
%files -n org.tizen.powerkey-syspopup
%manifest org.tizen.powerkey-syspopup.manifest
%license LICENSE
%defattr(-,root,root,-)
%{TZ_SYS_RO_APP}/org.tizen.powerkey-syspopup/bin/powerkey-popup
%{TZ_SYS_RO_SHARE}/packages/org.tizen.powerkey-syspopup.xml
%{TZ_SYS_RO_APP}/org.tizen.powerkey-syspopup/res/circle_btn_check.png
%{TZ_SYS_RO_APP}/org.tizen.powerkey-syspopup/res/circle_btn_delete.png
%endif

%if %{signal_sender} == on
%files -n org.tizen.system-signal-sender
%manifest org.tizen.system-signal-sender.manifest
%defattr(-,root,root,-)
%{TZ_SYS_RO_APP}/org.tizen.system-signal-sender/bin/system-signal-sender
%{TZ_SYS_RO_SHARE}/packages/org.tizen.system-signal-sender.xml
%endif

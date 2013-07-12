Name:       system-popup
Summary:    system-popup application (poweroff popup,sysevent-alert)
Version: 0.1.16
Release:    2
Group:      framework/system
License:    Apache License, Version 2.0
Source0:    %{name}-%{version}.tar.bz2
Source1001:    %{name}.manifest
Source1007:    org.tizen.poweroff-syspopup.rule
Source1008:    org.tizen.lowmem-syspopup.rule
Source1009:    org.tizen.lowbat-syspopup.rule
Source1010:    org.tizen.mmc-syspopup.rule
Source1011:    org.tizen.usb-syspopup.rule
Source1012:    org.tizen.usbotg-syspopup.rule
BuildRequires:  pkgconfig(evas)
BuildRequires:  pkgconfig(ecore-input)
BuildRequires:  pkgconfig(ethumb)
BuildRequires:  pkgconfig(elementary)
BuildRequires:  pkgconfig(efreet)
BuildRequires:  pkgconfig(sysman)
BuildRequires:  pkgconfig(mm-sound)
BuildRequires:  pkgconfig(sensor)
BuildRequires:  pkgconfig(devman_haptic)
BuildRequires:  pkgconfig(appcore-efl)
BuildRequires:  pkgconfig(devman)
BuildRequires:  pkgconfig(utilX)
BuildRequires:  pkgconfig(syspopup)
BuildRequires:  pkgconfig(syspopup-caller)
BuildRequires:  pkgconfig(notification) 
BuildRequires:  pkgconfig(pmapi)
BuildRequires:  pkgconfig(appsvc)
BuildRequires:  pkgconfig(svi)
BuildRequires:  pkgconfig(bundle)
BuildRequires:  pkgconfig(vconf)
BuildRequires:  pkgconfig(dlog)
BuildRequires:  cmake
BuildRequires:  edje-bin
BuildRequires:  embryo-bin
BuildRequires:  gettext-devel

%description
system-popup application (poweroff popup,sysevent-alert).


%package -n org.tizen.poweroff-syspopup
Summary:    system-popup application (poweroff popup,sysevent-alert)
Group:      main
Requires:   %{name} = %{version}-%{release}

%description -n org.tizen.poweroff-syspopup
system-popup application (poweroff popup,sysevent-alert).

%package -n org.tizen.lowmem-syspopup
Summary:    system-popup application (lowbatt popup)
Group:      main
Requires:   %{name} = %{version}-%{release}

%description -n org.tizen.lowmem-syspopup
system-popup application (lowbatt popup).

%package -n org.tizen.lowbat-syspopup
Summary:    system-popup application (lowmem  popup)
Group:      main
Requires:   %{name} = %{version}-%{release}

%description -n org.tizen.lowbat-syspopup
system-popup application (lowmem  popup).

%package -n org.tizen.mmc-syspopup
Summary:    system-popup application (mmc  popup)
Group:      main
Requires:   %{name} = %{version}-%{release}

%description -n org.tizen.mmc-syspopup
system-popup application (mmc  popup).

%package -n org.tizen.usb-syspopup
Summary:    system-popup application (usb popup)
Group:      main
Requires:   %{name} = %{version}-%{release}

%description -n org.tizen.usb-syspopup
system-popup application (usb popup).

%package -n org.tizen.usbotg-syspopup
Summary:    system-popup application (usb otg popup)
Group:      main
Requires:   %{name} = %{version}-%{release}

%description -n org.tizen.usbotg-syspopup
system-popup application (usb otg popup).

%prep
%setup -q


%build
cp %{SOURCE1001} .
cp %{SOURCE1007} .
cp %{SOURCE1008} .
cp %{SOURCE1009} .
cp %{SOURCE1010} .
cp %{SOURCE1011} .
cp %{SOURCE1012} .

cmake . -DCMAKE_INSTALL_PREFIX=%{_prefix}
make %{?jobs:-j%jobs}

%install
rm -rf %{buildroot}
%make_install

%post
vconftool set -t int db/setting/select_popup_btn "0" -u 5000 -f

%files
%manifest %{name}.manifest
%defattr(-,root,root,-)


%files -n org.tizen.poweroff-syspopup
%manifest %{name}.manifest
%defattr(-,root,root,-)
/usr/apps/org.tizen.poweroff-syspopup/bin/poweroff-popup
/usr/apps/org.tizen.poweroff-syspopup/res/edje/poweroff/poweroff.edj
/usr/apps/org.tizen.poweroff-syspopup/res/icon/org.tizen.poweroff-syspopup.png
/usr/share/packages/org.tizen.poweroff-syspopup.xml
/usr/share/process-info/poweroff-popup.ini
/usr/apps/org.tizen.poweroff-syspopup/res/locale/*/LC_MESSAGES/*.mo
/opt/etc/smack/accesses.d/org.tizen.poweroff-syspopup.rule

%files -n org.tizen.lowmem-syspopup
%manifest %{name}.manifest
%defattr(-,root,root,-)
/usr/apps/org.tizen.lowmem-syspopup/bin/lowmem-popup
/usr/apps/org.tizen.lowmem-syspopup/res/keysound/02_Warning.wav
/usr/apps/org.tizen.lowmem-syspopup/res/edje/lowmem/lowmem.edj
/usr/apps/org.tizen.lowmem-syspopup/res/icon/org.tizen.lowmem-syspopup.png
/usr/share/packages/org.tizen.lowmem-syspopup.xml
/usr/share/process-info/lowmem-popup.ini
/usr/apps/org.tizen.lowmem-syspopup/res/locale/*/LC_MESSAGES/*.mo
/opt/etc/smack/accesses.d/org.tizen.lowmem-syspopup.rule

%files -n org.tizen.lowbat-syspopup
%manifest %{name}.manifest
%defattr(-,root,root,-)
/usr/apps/org.tizen.lowbat-syspopup/bin/lowbatt-popup
/usr/apps/org.tizen.lowbat-syspopup/res/edje/lowbatt/lowbatt.edj
/usr/apps/org.tizen.lowbat-syspopup/res/locale/*/LC_MESSAGES/*.mo
/usr/apps/org.tizen.lowbat-syspopup/res/icon/org.tizen.lowbat-syspopup.png
/usr/share/packages/org.tizen.lowbat-syspopup.xml
/usr/share/process-info/lowbatt-popup.ini
/opt/etc/smack/accesses.d/org.tizen.lowbat-syspopup.rule

%files -n org.tizen.mmc-syspopup
%manifest %{name}.manifest
%defattr(-,root,root,-)
/usr/apps/org.tizen.mmc-syspopup/bin/mmc-popup
/usr/share/packages/org.tizen.mmc-syspopup.xml
/usr/share/process-info/mmc-popup.ini
/usr/apps/org.tizen.mmc-syspopup/res/locale/*/LC_MESSAGES/*.mo
/opt/etc/smack/accesses.d/org.tizen.mmc-syspopup.rule

%files -n org.tizen.usb-syspopup
%manifest %{name}.manifest
%defattr(440,root,root,-)
%attr(555,app,app) /usr/apps/org.tizen.usb-syspopup/bin/usb-syspopup
%attr(440,app,app) /usr/apps/org.tizen.usb-syspopup/res/locale/*/LC_MESSAGES/usb-syspopup.mo
/usr/share/packages/org.tizen.usb-syspopup.xml
/opt/etc/smack/accesses.d/org.tizen.usb-syspopup.rule

%files -n org.tizen.usbotg-syspopup
%manifest %{name}.manifest
%defattr(440,root,root,-)
%attr(555,app,app) /usr/apps/org.tizen.usbotg-syspopup/bin/usbotg-syspopup
/usr/share/packages/org.tizen.usbotg-syspopup.xml
/opt/etc/smack/accesses.d/org.tizen.usbotg-syspopup.rule

Name:       system-popup
Summary:    system-popup application (poweroff popup,sysevent-alert)
Version: 0.1.13
Release:    2
Group:      framework-system
License:    APLv2
Source0:    %{name}-%{version}.tar.bz2
Source1001:    org.tizen.poweroff-syspopup.manifest
Source1002:    org.tizen.lowmem-syspopup.manifest
Source1003:    org.tizen.lowbat-syspopup.manifest
Source1004:    org.tizen.usbotg-syspopup.manifest
Source1005:    org.tizen.usbotg-unmount-popup.manifest
Source1006:    org.tizen.mmc-syspopup.manifest
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

%package -n org.tizen.usbotg-syspopup
Summary:    system-popup application (usbotg  popup)
Group:      main
Requires:   %{name} = %{version}-%{release}

%description -n org.tizen.usbotg-syspopup
system-popup application (usbotg  popup).

%package -n org.tizen.usbotg-unmount-popup
Summary:    system-popup application (usbotg unmount popup)
Group:      main
Requires:   %{name} = %{version}-%{release}

%description -n org.tizen.usbotg-unmount-popup
system-popup application (usbotg unmount popup).

%package -n org.tizen.mmc-syspopup
Summary:    system-popup application (mmc  popup)
Group:      main
Requires:   %{name} = %{version}-%{release}

%description -n org.tizen.mmc-syspopup
system-popup application (mmc  popup).

%prep
%setup -q


%build
cp %{SOURCE1001} .
cp %{SOURCE1002} .
cp %{SOURCE1003} .
cp %{SOURCE1004} .
cp %{SOURCE1005} .
cp %{SOURCE1006} .
cmake . -DCMAKE_INSTALL_PREFIX=/usr
make %{?jobs:-j%jobs}

%install
rm -rf %{buildroot}
%make_install


%files
%defattr(-,root,root,-)


%files -n org.tizen.poweroff-syspopup
%manifest org.tizen.poweroff-syspopup.manifest
%defattr(-,root,root,-)
/usr/apps/org.tizen.poweroff-syspopup/bin/poweroff-popup
/usr/apps/org.tizen.poweroff-syspopup/res/edje/poweroff/poweroff.edj
/usr/apps/org.tizen.poweroff-syspopup/res/icon/org.tizen.poweroff-syspopup.png
/usr/share/packages/org.tizen.poweroff-syspopup.xml
/usr/share/process-info/poweroff-popup.ini
/usr/apps/org.tizen.poweroff-syspopup/res/locale/*/LC_MESSAGES/*.mo

%files -n org.tizen.lowmem-syspopup
%manifest org.tizen.lowmem-syspopup.manifest
%defattr(-,root,root,-)
/usr/apps/org.tizen.lowmem-syspopup/bin/lowmem-popup
/usr/apps/org.tizen.lowmem-syspopup/res/keysound/02_Warning.wav
/usr/apps/org.tizen.lowmem-syspopup/res/edje/lowmem/lowmem.edj
/usr/apps/org.tizen.lowmem-syspopup/res/icon/org.tizen.lowmem-syspopup.png
/usr/share/packages/org.tizen.lowmem-syspopup.xml
/usr/share/process-info/lowmem-popup.ini
/usr/apps/org.tizen.lowmem-syspopup/res/locale/*/LC_MESSAGES/*.mo

%files -n org.tizen.lowbat-syspopup
%manifest org.tizen.lowbat-syspopup.manifest
%defattr(-,root,root,-)
/usr/apps/org.tizen.lowbat-syspopup/bin/lowbatt-popup
/usr/apps/org.tizen.lowbat-syspopup/res/edje/lowbatt/lowbatt.edj
/usr/apps/org.tizen.lowbat-syspopup/res/locale/*/LC_MESSAGES/*.mo
/usr/apps/org.tizen.lowbat-syspopup/res/icon/org.tizen.lowbat-syspopup.png
/usr/share/packages/org.tizen.lowbat-syspopup.xml
/usr/share/process-info/lowbatt-popup.ini

%files -n org.tizen.usbotg-syspopup
%manifest org.tizen.usbotg-syspopup.manifest
%defattr(-,root,root,-)
/usr/apps/org.tizen.usbotg-syspopup/bin/usbotg-popup
/usr/apps/org.tizen.usbotg-syspopup/res/keysound/02_Warning.wav
/usr/apps/org.tizen.usbotg-syspopup/res/edje/usbotg/usbotg.edj
/usr/apps/org.tizen.usbotg-syspopup/res/icons/usb_icon.png
/usr/apps/org.tizen.usbotg-syspopup/res/icon/org.tizen.usbotg-syspopup.png
/usr/share/packages/org.tizen.usbotg-syspopup.xml
/usr/share/process-info/usbotg-popup.ini
/usr/apps/org.tizen.usbotg-syspopup/res/locale/*/LC_MESSAGES/*.mo

%files -n org.tizen.usbotg-unmount-popup
%manifest org.tizen.usbotg-unmount-popup.manifest
%defattr(-,root,root,-)
/usr/apps/org.tizen.usbotg-unmount-popup/bin/usbotg-unmount-popup
/usr/apps/org.tizen.usbotg-unmount-popup/res/keysound/02_Warning.wav
/usr/apps/org.tizen.usbotg-unmount-popup/res/edje/usbotg-unmount/usbotg-unmount.edj
/usr/apps/org.tizen.usbotg-unmount-popup/res/icon/org.tizen.usbotg-unmount-popup.png
/usr/share/packages/org.tizen.usbotg-unmount-popup.xml
/usr/share/process-info/usbotg-unmount-popup.ini
/usr/apps/org.tizen.usbotg-unmount-popup/res/locale/*/LC_MESSAGES/*.mo
%files -n org.tizen.mmc-syspopup
%manifest org.tizen.mmc-syspopup.manifest
%defattr(-,root,root,-)
/usr/apps/org.tizen.mmc-syspopup/bin/mmc-popup
/usr/share/packages/org.tizen.mmc-syspopup.xml
/usr/share/process-info/mmc-popup.ini
/usr/apps/org.tizen.mmc-syspopup/res/locale/*/LC_MESSAGES/*.mo

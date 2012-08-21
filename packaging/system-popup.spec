Name:       system-popup
Summary:    system-popup application (poweroff popup,sysevent-alert)
Version: 0.1.7
Release:    1
Group:      main
License:    Flora Software License
Source0:    %{name}-%{version}.tar.bz2
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


%prep
%setup -q


%build

cmake . -DCMAKE_INSTALL_PREFIX=/usr
make %{?jobs:-j%jobs}

%install
rm -rf %{buildroot}
%make_install


%files
%defattr(-,root,root,-)


%files -n org.tizen.poweroff-syspopup
%defattr(-,root,root,-)
/opt/apps/org.tizen.poweroff-syspopup/bin/poweroff-popup
/opt/apps/org.tizen.poweroff-syspopup/res/edje/poweroff/poweroff.edj
/opt/apps/org.tizen.poweroff-syspopup/res/icon/org.tizen.poweroff-syspopup.png
/opt/share/packages/org.tizen.poweroff-syspopup.xml
/opt/share/process-info/poweroff-popup.ini
/opt/apps/org.tizen.poweroff-syspopup/res/locale/*/LC_MESSAGES/*.mo

%files -n org.tizen.lowmem-syspopup
%defattr(-,root,root,-)
/opt/apps/org.tizen.lowmem-syspopup/bin/lowmem-popup
/opt/apps/org.tizen.lowmem-syspopup/res/keysound/02_Warning.wav
/opt/apps/org.tizen.lowmem-syspopup/res/edje/lowmem/lowmem.edj
/opt/apps/org.tizen.lowmem-syspopup/res/icon/org.tizen.lowmem-syspopup.png
/opt/share/packages/org.tizen.lowmem-syspopup.xml
/opt/share/process-info/lowmem-popup.ini
/opt/apps/org.tizen.lowmem-syspopup/res/locale/*/LC_MESSAGES/*.mo

%files -n org.tizen.lowbat-syspopup
%defattr(-,root,root,-)
/opt/apps/org.tizen.lowbat-syspopup/bin/lowbatt-popup
/opt/apps/org.tizen.lowbat-syspopup/res/edje/lowbatt/lowbatt.edj
/opt/apps/org.tizen.lowbat-syspopup/res/locale/*/LC_MESSAGES/*.mo
/opt/apps/org.tizen.lowbat-syspopup/res/icon/org.tizen.lowbat-syspopup.png
/opt/share/packages/org.tizen.lowbat-syspopup.xml
/opt/share/process-info/lowbatt-popup.ini

%files -n org.tizen.usbotg-syspopup
%defattr(-,root,root,-)
/opt/apps/org.tizen.usbotg-syspopup/bin/usbotg-popup
/opt/apps/org.tizen.usbotg-syspopup/res/keysound/02_Warning.wav
/opt/apps/org.tizen.usbotg-syspopup/res/edje/usbotg/usbotg.edj
/opt/apps/org.tizen.usbotg-syspopup/res/icons/usb_icon.png
/opt/apps/org.tizen.usbotg-syspopup/res/icon/org.tizen.usbotg-syspopup.png
/opt/share/packages/org.tizen.usbotg-syspopup.xml
/opt/share/process-info/usbotg-popup.ini
/opt/apps/org.tizen.usbotg-syspopup/res/locale/*/LC_MESSAGES/*.mo

%files -n org.tizen.usbotg-unmount-popup
%defattr(-,root,root,-)
/opt/apps/org.tizen.usbotg-unmount-popup/bin/usbotg-unmount-popup
/opt/apps/org.tizen.usbotg-unmount-popup/res/keysound/02_Warning.wav
/opt/apps/org.tizen.usbotg-unmount-popup/res/edje/usbotg-unmount/usbotg-unmount.edj
/opt/apps/org.tizen.usbotg-unmount-popup/res/icon/org.tizen.usbotg-unmount-popup.png
/opt/share/packages/org.tizen.usbotg-unmount-popup.xml
/opt/share/process-info/usbotg-unmount-popup.ini
/opt/apps/org.tizen.usbotg-unmount-popup/res/locale/*/LC_MESSAGES/*.mo

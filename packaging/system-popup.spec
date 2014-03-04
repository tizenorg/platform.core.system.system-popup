Name:       system-popup
Summary:    System Popup application (poweroff popup,sysevent-alert)
Version:    0.1.17
Release:    1
Group:      System/Utilities
License:    Apache-2.0
Source0:    %{name}-%{version}.tar.bz2
Source1001:    %{name}.manifest
Source1:    system-apps.manifest
Source1002:    org.tizen.lowmem-syspopup.manifest
Source1003:    org.tizen.lowbat-syspopup.manifest
Source1004:    org.tizen.mmc-syspopup.manifest
Source1005:    org.tizen.usb-syspopup.manifest
Source1006:    org.tizen.usbotg-syspopup.manifest
Source1007:    org.tizen.poweroff-syspopup.rule
Source1008:    org.tizen.lowmem-syspopup.rule
Source1009:    org.tizen.lowbat-syspopup.rule
Source1010:    org.tizen.mmc-syspopup.rule
Source1011:    org.tizen.usb-syspopup.rule
Source1012:    org.tizen.usbotg-syspopup.rule
Source1013:    org.tizen.poweroff-syspopup.manifest
BuildRequires:  pkgconfig(evas)
BuildRequires:  pkgconfig(ecore-input)
BuildRequires:  pkgconfig(elementary)
BuildRequires:  pkgconfig(deviced)
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
Requires(post): /usr/bin/vconftool

%description
System applications such as popup-launcher
and service file for dbus activation

%package -n org.tizen.poweroff-syspopup
Summary:    System Popup application (poweroff popup,sysevent-alert)
Requires:   %{name} = %{version}-%{release}

%description -n org.tizen.poweroff-syspopup
Power off popup application which is launched by dbus activation

%package -n org.tizen.lowmem-syspopup
Summary:    System Popup application (lowmem popup)
Requires:   %{name} = %{version}-%{release}

%description -n org.tizen.lowmem-syspopup
Low memory popup application which is launched by dbus activation

%package -n org.tizen.lowbat-syspopup
Summary:    System Popup application (lowbat popup)
Requires:   %{name} = %{version}-%{release}

%description -n org.tizen.lowbat-syspopup
Low battery popup application which is launched by dbus activation

%package -n org.tizen.mmc-syspopup
Summary:    System Popup application (mmc popup)
Requires:   %{name} = %{version}-%{release}

%description -n org.tizen.mmc-syspopup
Mmc popup application which is launched by dbus activation

%package -n org.tizen.usb-syspopup
Summary:    System Popup application (usb popup)
Requires:   %{name} = %{version}-%{release}

%description -n org.tizen.usb-syspopup
Usb popup application which is launched by dbus activation

%package -n org.tizen.usbotg-syspopup
Summary:    System Popup application (usb otg popup)
Requires:   %{name} = %{version}-%{release}

%description -n org.tizen.usbotg-syspopup
Usb otg popup application which is launched by dbus activation

%prep
%setup -q


%build
cp %{SOURCE1001} .
cp %{SOURCE1002} .
cp %{SOURCE1003} .
cp %{SOURCE1004} .
cp %{SOURCE1005} .
cp %{SOURCE1006} .
cp %{SOURCE1007} .
cp %{SOURCE1008} .
cp %{SOURCE1009} .
cp %{SOURCE1010} .
cp %{SOURCE1011} .
cp %{SOURCE1012} .
cp %{SOURCE1013} .

cmake . -DCMAKE_INSTALL_PREFIX=%{_prefix}
make %{?jobs:-j%jobs}

%install
rm -rf %{buildroot}
%make_install

mkdir -p %{buildroot}/usr/share/license
cp LICENSE.Apache_v2 %{buildroot}/usr/share/license/system-popup
cp LICENSE.Apache_v2 %{buildroot}/usr/share/license/org.tizen.poweroff-syspopup
cp LICENSE.Apache_v2 %{buildroot}/usr/share/license/org.tizen.lowbat-syspopup
cp LICENSE.Apache_v2 %{buildroot}/usr/share/license/org.tizen.lowmem-syspopup
cp LICENSE.Apache_v2 %{buildroot}/usr/share/license/org.tizen.mmc-syspopup
cp LICENSE.Apache_v2 %{buildroot}/usr/share/license/org.tizen.usb-syspopup
cp LICENSE.Apache_v2 %{buildroot}/usr/share/license/org.tizen.usbotg-syspopup


%files
%manifest %{name}.manifest
%defattr(-,root,root,-)
%{_datadir}/locale/*/LC_MESSAGES/*.mo
/usr/share/system-apps/res/icons/datausage_warning.png
/usr/share/system-apps/res/icons/led_torch.png
/usr/share/system-apps/res/icons/sdcard_encryption.png
/usr/share/system-apps/res/icons/sdcard_decryption.png
/usr/share/system-apps/res/icons/sdcard_encryption_error.png
/usr/share/system-apps/res/icons/sdcard_decryption_error.png
/usr/share/system-apps/res/icons/usb.png
/usr/share/system-apps/res/icons/battery_full_noti.png
/usr/share/system-apps/res/icons/battery_full_indi.png
%{_bindir}/popup-launcher
/usr/share/dbus-1/services/org.tizen.system.popup.service
/etc/smack/accesses2.d/system-apps.rule
/usr/share/license/system-popup

%files -n org.tizen.poweroff-syspopup
%manifest %{name}.manifest
%defattr(-,root,root,-)
/usr/apps/org.tizen.poweroff-syspopup/bin/poweroff-popup
/usr/share/packages/org.tizen.poweroff-syspopup.xml
/opt/etc/smack/accesses.d/org.tizen.poweroff-syspopup.rule
/usr/share/license/org.tizen.poweroff-syspopup

%files -n org.tizen.lowmem-syspopup
%manifest %{name}.manifest
%defattr(-,root,root,-)
/usr/apps/org.tizen.lowmem-syspopup/bin/lowmem-popup
/usr/apps/org.tizen.lowmem-syspopup/res/edje/lowmem/lowmem.edj
/usr/share/packages/org.tizen.lowmem-syspopup.xml
/opt/etc/smack/accesses.d/org.tizen.lowmem-syspopup.rule
/usr/share/license/org.tizen.lowmem-syspopup

%files -n org.tizen.lowbat-syspopup
%manifest %{name}.manifest
%defattr(-,root,root,-)
/usr/apps/org.tizen.lowbat-syspopup/bin/lowbatt-popup
/usr/share/packages/org.tizen.lowbat-syspopup.xml
/opt/etc/smack/accesses.d/org.tizen.lowbat-syspopup.rule
/usr/share/license/org.tizen.lowbat-syspopup

%files -n org.tizen.mmc-syspopup
%manifest %{name}.manifest
%defattr(-,root,root,-)
/usr/apps/org.tizen.mmc-syspopup/bin/mmc-popup
/usr/share/packages/org.tizen.mmc-syspopup.xml
/opt/etc/smack/accesses.d/org.tizen.mmc-syspopup.rule
/usr/share/license/org.tizen.mmc-syspopup

%files -n org.tizen.usb-syspopup
%manifest %{name}.manifest
%defattr(-,root,root,-)
/usr/apps/org.tizen.usb-syspopup/bin/usb-syspopup
/usr/share/packages/org.tizen.usb-syspopup.xml
/opt/etc/smack/accesses.d/org.tizen.usb-syspopup.rule
/usr/share/license/org.tizen.usb-syspopup

%files -n org.tizen.usbotg-syspopup
%manifest %{name}.manifest
%defattr(-,root,root,-)
/usr/apps/org.tizen.usbotg-syspopup/bin/usbotg-syspopup
/usr/share/packages/org.tizen.usbotg-syspopup.xml
/opt/etc/smack/accesses.d/org.tizen.usbotg-syspopup.rule
/usr/share/license/org.tizen.usbotg-syspopup

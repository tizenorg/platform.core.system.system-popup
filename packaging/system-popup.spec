Name:       system-popup
Summary:    system-popup application (poweroff popup,sysevent-alert)
Version:    0.1.7
Release:    1
Group:      main
License:    Flora Software License
Source0:    %{name}-%{version}.tar.bz2
Source1001: packaging/system-popup.manifest 
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
BuildRequires:  pkgconfig(notification) 
BuildRequires:  pkgconfig(pmapi)

BuildRequires:  cmake
BuildRequires:  edje-bin
BuildRequires:  embryo-bin
BuildRequires:  gettext-devel

%description
system-popup application (poweroff popup,sysevent-alert).


%package -n org.tizen.poweroff-syspopup
Summary:    system-popup application (poweroff popup,sysevent-alert)
Group:      main

%description -n org.tizen.poweroff-syspopup
system-popup application (poweroff popup,sysevent-alert).

%package -n org.tizen.lowmem-syspopup
Summary:    system-popup application (lowbatt popup)
Group:      main

%description -n org.tizen.lowmem-syspopup
system-popup application (lowbatt popup).

%package -n org.tizen.lowbat-syspopup
Summary:    system-popup application (lowmem  popup)
Group:      main

%description -n org.tizen.lowbat-syspopup
system-popup application (lowmem  popup).


%prep
%setup -q 


%build
cp %{SOURCE1001} .
cmake . -DCMAKE_INSTALL_PREFIX=/usr
make %{?jobs:-j%jobs}

%install
%make_install


%files -n org.tizen.poweroff-syspopup
%manifest system-popup.manifest
/opt/apps/org.tizen.poweroff-syspopup/bin/poweroff-popup
/opt/apps/org.tizen.lowbat-syspopup/res/icons/batt_full_icon.png
/opt/share/applications/org.tizen.poweroff-syspopup.desktop
/opt/apps/org.tizen.poweroff-syspopup/res/edje/poweroff/poweroff.edj
/opt/apps/org.tizen.poweroff-syspopup/res/icon/org.tizen.poweroff-syspopup.png
/opt/apps/org.tizen.poweroff-syspopup/res/locale/*/*/poweroff-popup.mo
/opt/share/process-info/poweroff-popup.ini


%files -n org.tizen.lowmem-syspopup
%manifest system-popup.manifest
/opt/apps/org.tizen.lowmem-syspopup/bin/lowmem-popup
/opt/share/applications/org.tizen.lowmem-syspopup.desktop
/opt/apps/org.tizen.lowmem-syspopup/res/edje/lowmem/lowmem.edj
/opt/apps/org.tizen.lowmem-syspopup/res/icon/org.tizen.lowmem-syspopup.png
/opt/apps/org.tizen.lowmem-syspopup/res/locale/*/*/lowmem-popup.mo
/opt/share/process-info/lowmem-popup.ini
/opt/apps/org.tizen.lowmem-syspopup/res/keysound/02_Warning.wav

%files -n org.tizen.lowbat-syspopup
%manifest system-popup.manifest
/opt/apps/org.tizen.lowbat-syspopup/bin/lowbatt-popup
/opt/share/applications/org.tizen.lowbat-syspopup.desktop
/opt/apps/org.tizen.lowbat-syspopup/res/edje/lowbatt/lowbatt.edj
/opt/apps/org.tizen.lowbat-syspopup/res/icon/org.tizen.lowbat-syspopup.png
/opt/apps/org.tizen.lowbat-syspopup/res/locale/*/*/lowbatt-popup.mo
/opt/share/process-info/lowbatt-popup.ini
/opt/apps/org.tizen.lowbat-syspopup/res/keysound/09_Low_Battery.wav

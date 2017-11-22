Summary: The GLE Tubing and Extrusion Library for OpenGL
Name: gle
Version: 3.1.0
Release: 1
Epoch: 1
Copyright: Mix+match open source
Group: System Environment/Libraries
URL: http://www.linas.org/gle/
Packager: Rob Knop <rknop@pobox.com>
Source: http://www.linas.org/gle/pub/gle-%{version}.tar.gz
BuildPrereq: Mesa
Requires: Mesa
Provides: GLE-%{version}
AutoReqProv: yes
#Obsoletes:
Buildroot: /var/tmp/gle-root
Prefix: /usr

%description
The GLE Tubing and Extrusion Library consists of a number of "C"
language subroutines for drawing tubing and extrusions. It is a very
fast implementation of these shapes, outperforming all other
implementations, most by orders of magnitude. It uses the
OpenGL (TM) programming API to perform the actual drawing of the tubing
and extrusions.

%package devel
Requires: Mesa-devel GLE-%{version}
Summary: GLE includes and development libraries
Group: Development/Libraries

%description devel
Includes, man pages, and development libraries for the GLE Tubing and
Extrusion Library.

%prep
%setup -q

%build
./configure --prefix=${prefix}
make

%install
rm -rf $RPM_BUILD_ROOT
mkdir -p $RPM_BUILD_ROOT%{prefix}/lib
mkdir -p $RPM_BUILD_ROOT%{prefix}/include
mkdir -p $RPM_BUILD_ROOT%{prefix}/include/GL
mkdir -p $RPM_BUILD_ROOT%{prefix}/man
mkdir -p $RPM_BUILD_ROOT%{prefix}/man/man3
mkdir -p $RPM_BUILD_ROOT%{prefix}/share
mkdir -p $RPM_BUILD_ROOT%{prefix}/share/doc
mkdir -p $RPM_BUILD_ROOT%{prefix}/share/doc/examples
mkdir -p $RPM_BUILD_ROOT%{prefix}/share/doc/html
make prefix=$RPM_BUILD_ROOT%{prefix} install

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root)
%{prefix}/lib/*.so*
%{prefix}/share/doc/gle/AUTHORS
%{prefix}/share/doc/gle/COPYING*
%{prefix}/share/doc/gle/README

%files devel
%{prefix}/include/GL/*
%{prefix}/lib/*a
%{prefix}/man/man3/*
%{prefix}/share/doc/gle/examples/*
%{prefix}/share/doc/gle/html/*


%changelog
* Tue Mar 05 2002 Linas Vepstas <linas@linas.org>
- changes since June 2001 include:
- configure fixes
- ms windows related fixes
- python binding fixes
* Thu Jul 20 2001 Linas Vepstas <linas@linas.org>
- Added doc subdirectory
* Thu Jul 05 2001 Rob Knop <rknop@pobox.com>
- Created the spec file

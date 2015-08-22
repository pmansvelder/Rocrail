#!/bin/sh


DIST=$1
ARCH=$2

if [ !  $1 ]; then
  echo "usage  : mkdeb.sh dist arch"
  echo "example: mkdeb.sh ubuntu1310 i386"
  exit $?
fi

if [ ! $2 ]; then
  ARCH="i386"
fi

echo "Getting revision number..."
if which git > /dev/null
then
  BAZAARREV=`git rev-list --count HEAD`
  echo "    Revision number is $BAZAARREV"
  echo ""
else
  BAZAARREV="user"
  echo "    Revision number not found, using \"user\""
  echo ""
fi

sed s/\<BZR\>/$BAZAARREV/ < ../rocrail/package/control.template > ../rocrail/package/control.tmp
sed s/\<ARCH\>/$ARCH/ < ../rocrail/package/control.tmp > ../rocrail/package/control
rm ../rocrail/package/control.tmp


if [ ! -e ../package ] ; then
	mkdir ../package
fi
cd ../package
rm -Rf *.deb
mkdir -p debian/DEBIAN
mkdir -p debian/opt/rocrail/default
mkdir -p debian/opt/rocrail/stylesheets
mkdir -p debian/opt/rocrail/decspecs
mkdir -p debian/opt/rocrail/web
mkdir -p debian/opt/rocrail/images
mkdir -p debian/opt/rocrail/svg
mkdir -p debian/usr/share/applications
mkdir -p debian/usr/share/desktop-directories
mkdir -p debian/etc/init.d

cp ../rocrail/package/control debian/DEBIAN/control

cp ../unxbin/rocrail debian/opt/rocrail
cp ../unxbin/rocview debian/opt/rocrail
cp ../unxbin/rocnetnode debian/opt/rocrail
cp ../unxbin/*.so debian/opt/rocrail

strip debian/opt/rocrail/rocrail
strip debian/opt/rocrail/rocview
strip debian/opt/rocrail/*.so

cp ../rocrail/package/Rocrail.directory debian/usr/share/desktop-directories
cp ../rocrail/package/Roc*.desktop debian/usr/share/applications
cp ../rocrail/package/roc*.sh debian/opt/rocrail
cp ../rocrail/package/rocraild debian/etc/init.d
chmod +x debian/opt/rocrail/*.sh
cp ../rocrail/package/rocraild.png debian/opt/rocrail
cp ../rocrail/package/rocrail.xpm debian/opt/rocrail
cp ../rocrail/package/rocrail.mib debian/opt/rocrail
cp -u ../rocrail/package/plan.xml debian/opt/rocrail/default

cp -R ../rocrail/package/images/* debian/opt/rocrail/images
cp -R ../stylesheets/* debian/opt/rocrail/stylesheets
cp -R ../decspecs/* debian/opt/rocrail/decspecs
cp -R ../rocview/svg/* debian/opt/rocrail/svg
cp -R ../rocrail/impl/web/html/* debian/opt/rocrail/web
cp -R ../COPYING debian/opt/rocrail

fakeroot 'dpkg-deb --build debian'
mv debian.deb rocrail-$BAZAARREV-$DIST-$ARCH.deb
rm -Rf debian
cd ../rocrail

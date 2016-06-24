# Maintainer: Yannick Pflanzer <www.neo-engine.de>
pkgname=neo-game-engine
pkgver=$(git describe --long --tags | sed 's/\([^-]*-g\)/r\1/;s/-/./g')
pkgrel=1
pkgdesc="The Neo 3D Game Engine and Tools"
arch=('i686' 'x86_64')
url="http://www.neo-engine.de>"
license=('unknown')
groups=()
depends=('qt5-base' 'sdl2')
makedepends=('qt5-base' 'libogg' 'sdl2' 'swig' 'doxygen')
checkdepends=()
optdepends=()
provides=()
conflicts=()
replaces=()
backup=()
options=()
install=
changelog=
source=()
noextract=()
md5sums=()
validpgpkeys=()

prepare() {
		  ln -snf "$startdir" "$srcdir/$pkgname"
}

build() {
	cd "$pkgname"

	if [ ! -d "build" ]; then
	   mkdir build
	fi
	
	cd build
	echo $PWD

	cmake ../ -DCMAKE_INSTALL_PREFIX="$pkgdir/usr"
	make
}

package() {
	cd "$pkgname"
	cd build
	make install
}

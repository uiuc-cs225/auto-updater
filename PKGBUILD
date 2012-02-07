# Maintainer: William Kennington <kenning1@illinois.edu>
pkgname=cs225
pkgver=0.2.1
pkgrel=1
epoch=0
pkgdesc="Auto updater for archlinux virtual machine"
arch=('i686' 'x86_64')
url="http://www.wkennington.com/cs225/"
license=('GPLv2')
groups=('cs225')
depends=('pacman' 'gtk2' 'glibc')
makedepends=('gtk2')
checkdepends=()
optdepends=()
provides=()
conflicts=()
replaces=()
backup=()
options=('strip' 'zipman')
source=("http://www.wkennington.com/cs225/src/${pkgname}-${pkgver}.tar.gz")
noextract=()
sha256sums=('639d985550fec358abcb784781b158891143dd653f0bc0699caf90539d06a51e')

build() {
	cd "$srcdir/$pkgname-$pkgver"
	./configure --prefix=/usr
	make
}
check() {
	cd "$srcdir/$pkgname-$pkgver"
	make -k check
}
package() {
	cd "$srcdir/$pkgname-$pkgver"
	make DESTDIR="$pkgdir/" install
}

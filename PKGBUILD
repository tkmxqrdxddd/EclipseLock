# Maintainer: Jan Fidra <tkmxqrd@gmail.com>
pkgname=eclipselock
pkgver=1.0.0
pkgrel=1
pkgdesc="A command-line tool for file encryption using AES-256-CBC"
arch=('x86_64')
url="https://github.com/janfidra/eclipselock"
license=('MIT')
depends=('openssl')
makedepends=('gcc' 'make')
source=("$pkgname-$pkgver.tar.gz")
sha256sums=('a0081fa955576a83eb95204eeceaaa474124411f9e1c1d02d293777ade2138a3')

build() {
    cd "$pkgname-$pkgver"
    make
}

package() {
    cd "$pkgname-$pkgver"
    install -Dm755 "bin/$pkgname" "$pkgdir/usr/bin/$pkgname"
    install -Dm644 "LICENSE" "$pkgdir/usr/share/licenses/$pkgname/LICENSE"
}

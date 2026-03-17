# Maintainer: Jan Fidra <tkmxqrd@gmail.com>
pkgname=eclipselock
pkgver=1.1.0
pkgrel=1
pkgdesc="A command-line and GUI tool for file encryption using AES-256-CBC"
arch=('x86_64')
url="https://github.com/tkmxqrdxddd/eclipselock"
license=('MIT')
depends=('openssl' 'wxwidgets' 'gtk3')
makedepends=('cmake' 'gcc' 'make')
source=("$pkgname-$pkgver.tar.gz")
sha256sums=('e1528d9525809afea6e9ae21e6c36a694d47f9b47090b07e73dfb1a9e53af166')

build() {
    cd "$pkgname-$pkgver"
    mkdir -p build && cd build
    cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr
    make
}

package() {
    cd "$pkgname-$pkgver/build"
    make DESTDIR="$pkgdir" install
    install -Dm644 "../LICENSE" "$pkgdir/usr/share/licenses/$pkgname/LICENSE"
}

# Windows cross-compilation package (mingw-w64)
pkgname_windows=eclipselock-windows

build_windows() {
    cd "$pkgname-$pkgver"
    mkdir -p build-windows && cd build-windows
    x86_64-w64-mingw32-cmake .. \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_SYSTEM_NAME=Windows \
        -DCMAKE_C_COMPILER=x86_64-w64-mingw32-gcc \
        -DCMAKE_CXX_COMPILER=x86_64-w64-mingw32-g++ \
        -DCMAKE_RC_COMPILER=x86_64-w64-mingw32-windres
    make
}

package_windows() {
    cd "$pkgname-$pkgver/build-windows"
    install -Dm755 "eclipselock.exe" "$pkgdir/usr/bin/eclipselock.exe"
    install -Dm644 "../LICENSE" "$pkgdir/usr/share/licenses/$pkgname/LICENSE"
}

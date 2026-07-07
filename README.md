# EclipseLock

EclipseLock is a secure file encryption tool that uses AES-256-CBC encryption with PKCS7 padding. It features both a modern graphical interface and command-line options, designed to provide robust protection for sensitive files while maintaining ease of use.

## Features

- **AES-256-CBC encryption/decryption** with OpenSSL EVP API
- **PBKDF2 key derivation** with random salt and 100,000 iterations
- **HMAC-SHA256 authentication** to detect tampering
- **Secure random IV generation** for each encryption operation
- **PKCS7 padding** for block alignment
- **Dual interface**: GTK GUI and Command-line interface
- **Support for any file type** with original extension preservation
- **Real-time status updates** with progress indication

## Prerequisites

### Build Dependencies
- C++ compiler with C++17 support
- CMake 3.15+
- OpenSSL development libraries
- GTKmm 3.0 development libraries
- GTK3 development libraries

### Installation Dependencies by Platform

#### Debian/Ubuntu

```bash
sudo apt-get install libssl-dev libgtkmm-3.0-dev libgtk-3-dev cmake g++
```

#### Arch Linux

```bash
sudo pacman -S openssl gtkmm3 gtk3 cmake gcc make
```

#### Fedora

```bash
sudo dnf install openssl-devel gtkmm30-devel gtk3-devel cmake gcc-c++ make
```

#### Windows (Native Build)

```powershell
# Using vcpkg
git clone https://github.com/Microsoft/vcpkg.git
.\vcpkg\bootstrap-vcpkg.bat
vcpkg install openssl:x64-windows

# Then build with CMake
.\build-windows-native.ps1
```

#### Windows (MinGW Cross-Compile from Linux)

```bash
# Using Podman
podman build -f Containerfile.windows -t eclipselock-windows .
podman run --rm eclipselock-windows cat /output/eclipselock.exe > eclipselock.exe
```

## Installation

### From Source (CMake)

```bash
# Clone the repository
git clone https://github.com/tkmxqrdxddd/eclipselock.git
cd eclipselock

# Build
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make

# Run
./eclipselock
```

### Using Nix

```bash
# Enter development shell
nix-shell

# Build
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make
```

### Using Podman/Docker

```bash
# Build container
podman build -t eclipselock .

# Run CLI
podman run --rm eclipselock help

# Run GUI (requires X11)
podman run --rm -e DISPLAY=$DISPLAY -v /tmp/.X11-unix:/tmp/.X11-unix eclipselock
```

#### Debian/Ubuntu
```bash
# Install the .deb package
sudo dpkg -i eclipselock_1.0.0_amd64.deb
sudo apt-get install -f  # Install dependencies if needed
```

#### Arch Linux
```bash
# Using yay
yay -S eclipselock

# Or manually from AUR
git clone https://aur.archlinux.org/eclipselock.git
cd eclipselock
makepkg -si
```

#### NixOS
```bash
nix-build eclipselock.nix
nix-env -i ./result
```

#### Windows

**Option 1: Using the Installer (Recommended)**
```powershell
# Download and run the installer
eclipselock_1.1.0_windows_installer.exe

# Or using Chocolatey (if available)
choco install eclipselock
```

**Option 2: Using the Portable Executable**
```powershell
# Download eclipselock.exe and run directly
.\eclipselock.exe
```

**Option 3: Build from Source**
```powershell
# Install dependencies via vcpkg
.\build-windows-native.ps1 -InstallDeps

# Build
.\build-windows-native.ps1

# Run
.\bin\eclipselock.exe
```

**Option 4: Using NSIS Installer**
```powershell
# After building, create the installer
makensis installer.nsi

# Run the generated installer
.\eclipselock_1.1.0_windows_installer.exe
```

## Usage

### Graphical Interface

Launch the GUI:
```bash
./eclipselock
```

The GUI provides:
- **File picker** - Select files to encrypt/decrypt
- **Password field** - Enter your encryption key
- **Encrypt button** - Encrypt the selected file
- **Decrypt button** - Decrypt .enc files
- **Progress indicator** - Visual feedback during operations
- **Status messages** - Real-time operation status

### Command-Line Interface

Basic command structure:
```bash
eclipselock <command> <file> <key>
```

Commands:
```
encrypt <file> <key>   Encrypt the specified file
decrypt <file> <key>   Decrypt the specified file
help                   Display help message
```

### Examples

```bash
# GUI Mode
./eclipselock

# CLI - Encrypt a PDF file
./eclipselock encrypt secret.pdf "my_secure_password"
# Creates: secret.pdf.enc

# CLI - Decrypt the file
./eclipselock decrypt secret.pdf.enc "my_secure_password"
# Restores: secret.pdf

# CLI - Display help
./eclipselock help
```

## Security Features

1. **Strong Encryption**: Uses AES-256-CBC encryption from OpenSSL's EVP API
2. **PBKDF2 Key Derivation**: Derives keys using PBKDF2-HMAC-SHA256 with 100,000 iterations and random salt
3. **HMAC Authentication**: HMAC-SHA256 tag for integrity verification and tamper detection
4. **Random IV**: Generates a unique random IV for each encryption operation
5. **PKCS7 Padding**: Implements proper block padding for secure encryption
6. **Memory Safety**: Sensitive key material is securely wiped from memory after use

## Implementation Details

The project consists of three main components:

1. **AESKey Class**: Manages encryption keys with PBKDF2 key derivation and secure cleanup
2. **AESCrypt Class**: Handles AES-256-CBC encryption/decryption with RAII resource management
3. **encrypt/decrypt functions**: File-level operations with HMAC authentication and error handling

See the source files in `src/` for detailed implementation.

## Security Considerations

- Keep encryption keys secure and use strong passwords
- Encrypted files contain the IV in the header for decryption
- Original files are not automatically deleted after encryption
- Each file is encrypted with a unique IV
- The implementation uses industry-standard cryptographic practices

## Contributing

1. Fork the repository
2. Create your feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add some amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

### Development Guidelines

- Follow modern C++ best practices
- Maintain memory safety
- Add error handling for all operations
- Include tests for new features
- Document security-critical code
- Follow existing code style

### CI/CD

The project uses GitHub Actions for continuous integration and deployment:

- **build-release.yml** - Runs on every push/PR:
  - Builds on Ubuntu
  - Runs static analysis with CodeQL
  - Runs unit tests
  - Builds .deb package on version tags

- **build-deb.yml** - Runs on version tags (v*):
  - Builds .deb package for Debian/Ubuntu
  - Creates GitHub release with artifacts
  - Uploads package and checksums

To trigger a release build:
```bash
git tag v1.0.0
git push origin v1.0.0
```

## Testing

Tests are written using the Catch2 framework and located in the `tests/` directory.
Run them with:

```bash
mkdir build && cd build
cmake .. -DBUILD_TESTS=ON
make && ctest
```

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Author

Jan Fidra <tkmxqrd@gmail.com>

## Acknowledgments

- OpenSSL for cryptographic operations
- The C++ community for best practices and guidance
- Contributors and users who help improve the project

## Support

For bugs, feature requests, or questions:
1. Open an issue on GitHub
2. Contact the maintainer directly
3. Check existing documentation and issues

Remember to never share encryption keys or sensitive files when reporting issues.
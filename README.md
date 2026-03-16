# EclipseLock

EclipseLock is a secure file encryption tool that uses AES-256-CBC encryption with PKCS7 padding. It features both a modern graphical interface and command-line options, designed to provide robust protection for sensitive files while maintaining ease of use.

## Features

- **Modern Dark Theme GUI** with gradient backgrounds and styled buttons
- **AES-256-CBC encryption/decryption** with OpenSSL EVP API
- **Secure random IV generation** for each encryption operation
- **PKCS7 padding** for block alignment
- **Dual interface**: Modern GUI and Command-line options
- **Support for any file type** with original extension preservation
- **Memory-safe implementation** using modern C++17
- **Hover effects** on buttons for better UX
- **Real-time status updates** with progress indication

## Prerequisites

### Build Dependencies
- C++ compiler with C++17 support
- CMake 3.15+
- OpenSSL development libraries
- wxWidgets 3.2+ (GTK3)
- GTK3 development libraries

### Installation Dependencies by Platform

#### Debian/Ubuntu

```bash
sudo apt-get install libssl-dev libwxgtk3.2-dev libgtk-3-dev cmake g++
```

#### Arch Linux

```bash
sudo pacman -S openssl wxgtk3.2 gtk3 cmake gcc make
```

#### Fedora

```bash
sudo dnf install openssl-devel wxGTK-devel gtk3-devel cmake gcc-c++ make
```

## Installation

### From Source (CMake)

```bash
# Clone the repository
git clone https://github.com/janfidra/eclipselock.git
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

## Usage

### Graphical Interface

Launch the GUI:
```bash
./eclipselock
```

The modern dark-themed GUI provides:
- **File picker** - Select files to encrypt/decrypt
- **Password field** - Enter your encryption key
- **Encrypt button** (blue) - Encrypt the selected file
- **Decrypt button** (green) - Decrypt .enc files
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
2. **Random IV**: Generates a unique random IV for each encryption operation
3. **PKCS7 Padding**: Implements proper block padding for secure encryption
4. **Memory Safety**: Utilizes modern C++ features for memory-safe operations
5. **Key Handling**: Securely processes encryption keys with zero-initialization

## Implementation Details

The project consists of three main components:

1. **AESKey Class**: Manages encryption keys with secure initialization
```cpp
startLine: 12
endLine: 20
```

2. **AESCrypt Class**: Handles encryption/decryption operations using OpenSSL
```cpp
startLine: 23
endLine: 96
```

3. **FileHandler Class**: Manages file operations with proper error handling
```cpp
startLine: 99
endLine: 125
```

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

- **build.yml** - Runs on every push/PR:
  - Builds on Fedora 39 container
  - Builds with nix-shell (NixOS 24.05)
  - Runs CLI encryption/decryption tests
  - Builds and tests Podman container

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

The project includes comprehensive testing for:
- Encryption/decryption operations
- File handling
- Key management
- Error conditions
- Memory safety

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
# EclipseLock

EclipseLock is a secure command-line file encryption tool that uses AES-256-CBC encryption with PKCS7 padding. It's designed to provide robust protection for sensitive files while maintaining ease of use.

## Features

- AES-256-CBC encryption/decryption
- Secure random IV generation for each encryption
- PKCS7 padding for block alignment
- Command-line interface with intuitive options
- Support for any file type
- Original file extension preservation
- Memory-safe implementation using modern C++
- OpenSSL EVP API for cryptographic operations

## Prerequisites

### Build Dependencies
- C++ compiler with C++17 support
- OpenSSL development libraries
- Make

### Installation Dependencies by Platform

#### Debian/Ubuntu

```bash
sudo apt-get install libssl-dev
```
#### Arch Linux

```bash
sudo pacman -S openssl gcc openssl
```

## Installation

### From Source

```bash
# Clone the repository
git clone https://github.com/janfidra/eclipselock.git
cd eclipselock

# Build
make

# Install (optional)
sudo make install
```

### Package Installation

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

Basic command structure:
```bash
eclipselock [-e|-d] <file> -k <encryption_key>
```

Options:
```
-e, --encrypt <file>  Encrypt the specified file
-d, --decrypt <file>  Decrypt the specified file
-k, --key <key>      Set the encryption key
-h, --help           Display help message
```

### Examples

```bash
# Encrypt a PDF file
eclipselock -e secret.pdf -k "my_secure_password"
# Creates: secret.pdf.enc

# Decrypt the file
eclipselock -d secret.pdf.enc -k "my_secure_password"
# Restores: secret.pdf

# Display help
eclipselock -h
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
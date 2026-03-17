# EclipseLock Windows Native Build Script
# Requires: Visual Studio 2019/2022, vcpkg with wxwidgets and openssl

param(
    [string]$BuildType = "Release",
    [switch]$InstallDeps,
    [switch]$Clean
)

$ErrorActionPreference = "Stop"

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "EclipseLock Windows Build Script" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

# Configuration
$BuildDir = "build-windows-native"
$VcpkgTriplet = "x64-windows"

# Function to check if command exists
function Test-Command($cmdname) {
    return $null -ne (Get-Command $cmdname -ErrorAction SilentlyContinue)
}

# Install dependencies via vcpkg
if ($InstallDeps) {
    Write-Host "Installing dependencies via vcpkg..." -ForegroundColor Yellow
    
    if (-not (Test-Command vcpkg)) {
        Write-Host "vcpkg not found. Please install vcpkg first:" -ForegroundColor Red
        Write-Host "  git clone https://github.com/Microsoft/vcpkg.git" -ForegroundColor Yellow
        Write-Host "  .\vcpkg\bootstrap-vcpkg.bat" -ForegroundColor Yellow
        exit 1
    }
    
    Write-Host "Installing wxwidgets and openssl..." -ForegroundColor Yellow
    vcpkg install wxwidgets:$VcpkgTriplet
    vcpkg install openssl:$VcpkgTriplet
    
    Write-Host "Dependencies installed successfully!" -ForegroundColor Green
    exit 0
}

# Clean previous build
if ($Clean) {
    Write-Host "Cleaning previous build..." -ForegroundColor Yellow
    if (Test-Path $BuildDir) {
        Remove-Item -Recurse -Force $BuildDir
    }
    if (Test-Path "bin") {
        Remove-Item -Recurse -Force bin
    }
}

# Check for Visual Studio
if (-not (Test-Command cl.exe)) {
    Write-Host "Visual Studio C++ compiler not found in PATH." -ForegroundColor Yellow
    Write-Host "Please run this script from a Visual Studio Developer Command Prompt," -ForegroundColor Yellow
    Write-Host "or run:" -ForegroundColor Yellow
    Write-Host '  & "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat"' -ForegroundColor Yellow
    Write-Host ""
    
    # Try to find and use VS 2022
    $vsPaths = @(
        "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat",
        "C:\Program Files\Microsoft Visual Studio\2022\Professional\Common7\Tools\VsDevCmd.bat",
        "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\Common7\Tools\VsDevCmd.bat",
        "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\Common7\Tools\VsDevCmd.bat",
        "C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional\Common7\Tools\VsDevCmd.bat"
    )
    
    $foundVs = $false
    foreach ($vsPath in $vsPaths) {
        if (Test-Path $vsPath) {
            Write-Host "Found Visual Studio: $vsPath" -ForegroundColor Green
            Write-Host "Please run this script from that VS Developer Command Prompt." -ForegroundColor Yellow
            $foundVs = $true
            break
        }
    }
    
    if (-not $foundVs) {
        Write-Host "Visual Studio not found. Please install Visual Studio 2019/2022 with C++ support." -ForegroundColor Red
    }
    exit 1
}

# Check for CMake
if (-not (Test-Command cmake)) {
    Write-Host "CMake not found. Please install CMake from https://cmake.org/download/" -ForegroundColor Red
    exit 1
}

# Create build directory
Write-Host "Creating build directory..." -ForegroundColor Yellow
if (-not (Test-Path $BuildDir)) {
    New-Item -ItemType Directory -Path $BuildDir | Out-Null
}

# Configure with CMake
Write-Host "Configuring with CMake..." -ForegroundColor Yellow
Set-Location $BuildDir

$cmakeArgs = @(
    "..",
    "-G", "Visual Studio 17 2022",
    "-A", "x64",
    "-DCMAKE_BUILD_TYPE=$BuildType",
    "-DCMAKE_TOOLCHAIN_FILE=[vcpkg root]/scripts/buildsystems/vcpkg.cmake"
)

# Try to find vcpkg integration
$vcpkgRoot = $env:VCPKG_ROOT
if (-not $vcpkgRoot) {
    $vcpkgRoot = "C:\vcpkg"
}

if (Test-Path "$vcpkgRoot\scripts\buildsystems\vcpkg.cmake") {
    Write-Host "Using vcpkg from: $vcpkgRoot" -ForegroundColor Green
    $cmakeArgs[4] = "-DCMAKE_TOOLCHAIN_FILE=$vcpkgRoot\scripts\buildsystems\vcpkg.cmake"
} else {
    Write-Host "vcpkg not found. Building without vcpkg integration." -ForegroundColor Yellow
    Write-Host "Please ensure wxWidgets and OpenSSL are installed and in PATH." -ForegroundColor Yellow
    $cmakeArgs = @(
        "..",
        "-G", "Visual Studio 17 2022",
        "-A", "x64",
        "-DCMAKE_BUILD_TYPE=$BuildType"
    )
}

& cmake @cmakeArgs

if ($LASTEXITCODE -ne 0) {
    Write-Host "CMake configuration failed!" -ForegroundColor Red
    Set-Location ..
    exit 1
}

# Build
Write-Host "Building $BuildType..." -ForegroundColor Yellow
& cmake --build . --config $BuildType

if ($LASTEXITCODE -ne 0) {
    Write-Host "Build failed!" -ForegroundColor Red
    Set-Location ..
    exit 1
}

# Copy output
Write-Host "Copying output..." -ForegroundColor Yellow
$exePath = ".\$BuildType\eclipselock.exe"
if (Test-Path $exePath) {
    if (-not (Test-Path "..\bin")) {
        New-Item -ItemType Directory -Path "..\bin" | Out-Null
    }
    Copy-Item $exePath "..\bin\eclipselock.exe"
    Write-Host "Build successful! Output: ..\bin\eclipselock.exe" -ForegroundColor Green
} else {
    Write-Host "Build completed but executable not found at $exePath" -ForegroundColor Yellow
}

Set-Location ..

Write-Host ""
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "Build Complete!" -ForegroundColor Green
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""
Write-Host "To run the application:" -ForegroundColor Yellow
Write-Host "  .\bin\eclipselock.exe" -ForegroundColor White
Write-Host ""
Write-Host "To create installer (requires NSIS):" -ForegroundColor Yellow
Write-Host "  makensis installer.nsi" -ForegroundColor White
Write-Host ""

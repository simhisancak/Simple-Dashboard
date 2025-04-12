#!powershell.exe
# Execution policy bypass for this script
Set-ExecutionPolicy Bypass -Scope Process -Force

# PowerShell script for building and running Fracq projects

function Show-Usage {
    Write-Host "Usage:"
    Write-Host "  .\build.ps1 [options]"
    Write-Host ""
    Write-Host "Options:"
    Write-Host "  build client    - Build only the client"
    Write-Host "  build server    - Build only the server"
    Write-Host "  build all       - Build both client and server"
    Write-Host "  run server      - Run the server"
    Write-Host "  clean          - Clean build directory"
    Write-Host "  help           - Show this help message"
}

# Check if CMake is installed
if (-not (Get-Command cmake -ErrorAction SilentlyContinue)) {
    Write-Host "Error: CMake is not installed or not in PATH"
    exit 1
}

# Check if CMakeLists.txt exists
if (-not (Test-Path "CMakeLists.txt")) {
    Write-Host "Error: CMakeLists.txt not found in current directory"
    exit 1
}

# Create build directory if it doesn't exist
if (-not (Test-Path "build")) {
    New-Item -ItemType Directory -Path "build" | Out-Null
}

# Change to build directory
Push-Location build

# Initialize CMake if not already done
if (-not (Test-Path "CMakeCache.txt")) {
    Write-Host "Initializing CMake..."
    cmake -G "Visual Studio 17 2022" -A Win32 -DCMAKE_BUILD_TYPE=Release -DCMAKE_CONFIGURATION_TYPES=Release ..
    if ($LASTEXITCODE -ne 0) {
        Write-Host "Error: CMake configuration failed"
        Pop-Location
        exit 1
    }
}

# Process commands
$command = $args[0]
$target = $args[1]

switch ($command) {
    "build" {
        switch ($target) {
            "client" {
                Write-Host "Building client..."
                cmake --build . --config Release --target FracqClient -- /p:RuntimeLibrary=MultiThreaded
            }
            "server" {
                Write-Host "Building server..."
                cmake --build . --config Release --target FracqServer -- /p:RuntimeLibrary=MultiThreaded
            }
            "all" {
                Write-Host "Building all projects..."
                Write-Host "Building client..."
                cmake --build . --config Release --target FracqClient -- /p:RuntimeLibrary=MultiThreaded
                if ($LASTEXITCODE -eq 0) {
                    Write-Host "Building server..."
                    cmake --build . --config Release --target FracqServer -- /p:RuntimeLibrary=MultiThreaded
                }
            }
            default {
                Write-Host "Invalid build target. Use 'client', 'server', or 'all'"
                Show-Usage
                Pop-Location
                exit 1
            }
        }
    }
    "run" {
        switch ($target) {
            "" {
                Write-Host "Running server..."
                $serverPath = Join-Path $PWD "bin\Release\FracqServer.exe"
                if (Test-Path $serverPath) {
                    Start-Process -FilePath $serverPath -NoNewWindow
                } else {
                    Write-Host "Error: Server executable not found at $serverPath"
                    exit 1
                }
            }
            default {
                Write-Host "Invalid run target. Use 'server'"
                Show-Usage
                Pop-Location
                exit 1
            }
        }
    }
    "clean" {
        Write-Host "Cleaning build directory..."
        Pop-Location
        if (Test-Path "build") {
            # Remove all files and directories in build folder
            Get-ChildItem -Path "build" -Recurse | Remove-Item -Force -Recurse
            # Remove the build directory itself
            Remove-Item -Path "build" -Force
            # Create a fresh build directory
            New-Item -ItemType Directory -Path "build" | Out-Null
            # Reinitialize CMake
            Push-Location build
            Write-Host "Reinitializing CMake..."
            cmake -G "Visual Studio 17 2022" -A Win32 -DCMAKE_BUILD_TYPE=Release -DCMAKE_CONFIGURATION_TYPES=Release ..
        }
    }
    "help" {
        Show-Usage
    }
    default {
        Write-Host "Invalid command. Use 'build', 'run', 'clean', or 'help'"
        Show-Usage
        Pop-Location
        exit 1
    }
}

Pop-Location 
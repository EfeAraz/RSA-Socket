#!/bin/bash
MAIN_DIR="RSA-Socket"

# Check if OpenSSL is installed, if not install OpenSSL
if ! command -v openssl &> /dev/null; then
    echo "OpenSSL is not installed. Installing..."
    sudo pacman -Syu --needed openssl
    if [ $? -eq 0 ]; then
        echo "OpenSSL has been successfully installed."
    else
        echo "Failed to install OpenSSL. Please check your network or package manager."
    fi
        exit 1
else
    echo "OpenSSL is already installed."
fi

# Maybe find if already in rsa-socket or you have rsa-socket in subdirectories first then cd into it
# I'm too lazy for it rn
cd "$MAIN_DIR"

chmod +x keygen.sh;
./keygen.sh;

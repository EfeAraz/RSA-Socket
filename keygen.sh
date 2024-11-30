#!/bin/bash
# Directories
KEYS_DIR="keys"

# Files
PRIVATE_KEY="keys/private.key"
PUBLIC_KEY="keys/public.key"

# Key length (2048 bits for security)
KEY_LENGTH=2048

echo "Generating a $KEY_LENGTH-bit RSA private key..."
openssl genpkey -algorithm RSA -out $PRIVATE_KEY -pkeyopt rsa_keygen_bits:$KEY_LENGTH

echo "Extracting the public key from the private key..."
openssl rsa -pubout -in $PRIVATE_KEY -out $PUBLIC_KEY

echo "Keys generated successfully!"

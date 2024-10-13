# Encryption & Decryption 

This C++ project provides a simple and secure way to encrypt and decrypt files of any format (e.g., .pdf, .pptx, .docx, .txt). The tool generates encrypted files with a `.enc` extension, allowing users to share files securely. The recipient can decrypt the file using the correct key, restoring the file to its original format.

## Features
- **File Encryption:** Supports multiple file formats (.pdf, .pptx, .docx, .txt, etc.) and saves encrypted files with a `.enc` extension.
- **File Decryption:** Restores the original file after decryption, with all content intact (text, images, etc.).
- **Cross-platform Compatibility:** Works on Windows, Linux, and macOS.
- **Single Key Encryption:** Use the same key for encryption and decryption.

## How It Works
1. **Encryption**: The tool encrypts the contents of any file and saves it with the `.enc` extension.
2. **Decryption**: The encrypted file is decrypted using the original key, and the file is restored to its original format.
3. **File Transfer**: Encrypted files can be shared via email, Bluetooth, or any other method, providing a secure way to transfer files.

## Usage

### To Encrypt:
```bash
./main.exe encrypt <input_file> <output_file.enc> <key>

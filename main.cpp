#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <limits>
#include <cstdint>

// Function to perform XOR encryption/decryption
void xorCipher(std::vector<char>& data, const std::string& key) {
    size_t keyLength = key.size();
    for (size_t i = 0; i < data.size(); ++i) {
        data[i] ^= key[i % keyLength];
    }
}

// Function to read a file into a vector<char>
bool readFile(const std::string& filename, std::vector<char>& data) {
    std::ifstream inFile(filename, std::ios::binary);
    if (!inFile) {
        std::cerr << "Error: Cannot open input file \"" << filename << "\".\n";
        return false;
    }

    // Read file into vector
    inFile.seekg(0, std::ios::end);
    std::streamsize size = inFile.tellg();
    if (size < 0) {
        std::cerr << "Error: Failed to read the file size.\n";
        return false;
    }
    inFile.seekg(0, std::ios::beg);

    data.resize(static_cast<size_t>(size));
    if (!inFile.read(data.data(), size)) {
        std::cerr << "Error: Failed to read the file data.\n";
        return false;
    }

    inFile.close();
    return true;
}

// Function to write a vector<char> to a file
bool writeFile(const std::string& filename, const std::vector<char>& data) {
    std::ofstream outFile(filename, std::ios::binary);
    if (!outFile) {
        std::cerr << "Error: Cannot open output file \"" << filename << "\".\n";
        return false;
    }

    outFile.write(data.data(), data.size());
    if (!outFile) {
        std::cerr << "Error: Failed to write data to file.\n";
        return false;
    }

    outFile.close();
    return true;
}

// Function to get user input with a prompt
std::string getInput(const std::string& prompt) {
    std::string input;
    std::cout << prompt;
    // Use getline to allow spaces in file paths and keys
    std::getline(std::cin, input);
    return input;
}

// Function to extract the filename from a full path
std::string extractFilename(const std::string& filepath) {
    size_t pos = filepath.find_last_of("/\\");
    if (pos == std::string::npos)
        return filepath; // No directory component
    else
        return filepath.substr(pos + 1);
}

// Function to concatenate directory path and filename
std::string concatenatePath(const std::string& directory, const std::string& filename) {
    if (directory.empty())
        return filename;
    char lastChar = directory[directory.length() - 1];
    if (lastChar == '/' || lastChar == '\\')
        return directory + filename;
    else
        return directory + "/" + filename;
}

// Function to convert size_t to 4-byte little endian
std::vector<char> sizeToBytes(uint32_t size) {
    std::vector<char> bytes(4);
    bytes[0] = size & 0xFF;
    bytes[1] = (size >> 8) & 0xFF;
    bytes[2] = (size >> 16) & 0xFF;
    bytes[3] = (size >> 24) & 0xFF;
    return bytes;
}

// Function to convert 4-byte little endian to uint32_t
uint32_t bytesToSize(const std::vector<char>& bytes, size_t start) {
    if (start + 4 > bytes.size()) return 0;
    uint32_t size = 0;
    size |= static_cast<unsigned char>(bytes[start]);
    size |= static_cast<unsigned char>(bytes[start + 1]) << 8;
    size |= static_cast<unsigned char>(bytes[start + 2]) << 16;
    size |= static_cast<unsigned char>(bytes[start + 3]) << 24;
    return size;
}

int main() {
    while (true) {
        std::cout << "========================================\n";
        std::cout << "     File Encryptor/Decryptor Tool      \n";
        std::cout << "========================================\n";
        std::cout << "Please choose an option:\n";
        std::cout << "1. Encrypt a File\n";
        std::cout << "2. Decrypt a File\n";
        std::cout << "3. Exit\n";
        std::cout << "Enter your choice (1/2/3): ";

        int choice;
        std::cin >> choice;

        // Clear the newline character left in the input buffer
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (choice == 1) {
            // Encryption
            std::cout << "\n--- File Encryption ---\n";
            std::string inputFile = getInput("Enter the full path of the file to encrypt: ");

            // Extract the original filename from the input path
            std::string originalFilename = extractFilename(inputFile);

            std::string outputFile = getInput("Enter the desired output file path (with .enc extension): ");

            if (outputFile.empty()) {
                // Default to original filename with .enc extension in the same directory
                outputFile = inputFile + ".enc";
                std::cout << "Output file not specified. Using \"" << outputFile << "\" by default.\n";
            }

            std::string key = getInput("Enter the encryption key: ");

            if (key.empty()) {
                std::cerr << "Error: Key cannot be empty.\n\n";
                continue;
            }

            std::vector<char> inputData;
            if (!readFile(inputFile, inputData)) {
                std::cerr << "Encryption failed due to input file error.\n\n";
                continue;
            }

            // Create header
            uint32_t filenameLength = static_cast<uint32_t>(originalFilename.size());
            std::vector<char> header = sizeToBytes(filenameLength);
            header.insert(header.end(), originalFilename.begin(), originalFilename.end());

            // Concatenate header and data
            std::vector<char> combinedData = header;
            combinedData.insert(combinedData.end(), inputData.begin(), inputData.end());

            // Encrypt the combined data
            xorCipher(combinedData, key);

            if (!writeFile(outputFile, combinedData)) {
                std::cerr << "Encryption failed due to output file error.\n\n";
                continue;
            }

            std::cout << "Encryption successful! Encrypted file saved as \"" << outputFile << "\".\n\n";
        }
        else if (choice == 2) {
            // Decryption
            std::cout << "\n--- File Decryption ---\n";
            std::string inputFile = getInput("Enter the full path of the file to decrypt (.enc): ");

            std::string key = getInput("Enter the decryption key: ");

            if (key.empty()) {
                std::cerr << "Error: Key cannot be empty.\n\n";
                continue;
            }

            std::vector<char> inputData;
            if (!readFile(inputFile, inputData)) {
                std::cerr << "Decryption failed due to input file error.\n\n";
                continue;
            }

            // Decrypt the data
            xorCipher(inputData, key);

            // Extract header
            if (inputData.size() < 4) {
                std::cerr << "Error: Encrypted file is corrupted or not in the correct format.\n\n";
                continue;
            }

            uint32_t filenameLength = bytesToSize(inputData, 0);
            if (filenameLength == 0 || 4 + filenameLength > inputData.size()) {
                std::cerr << "Error: Invalid filename length in encrypted file.\n\n";
                continue;
            }

            std::string originalFilename(inputData.begin() + 4, inputData.begin() + 4 + filenameLength);

            // Extract file data
            std::vector<char> fileData(inputData.begin() + 4 + filenameLength, inputData.end());

            // Prompt user for output directory
            std::string outputDirPath = getInput("Enter the full path of the output directory where the decrypted file will be saved: ");

            // Construct full output file path
            std::string outputFilePath = concatenatePath(outputDirPath, originalFilename);

            // Check if file already exists
            std::ifstream checkFile(outputFilePath, std::ios::binary);
            if (checkFile.good()) {
                std::cout << "Warning: File \"" << outputFilePath << "\" already exists. Overwrite? (y/n): ";
                char overwriteChoice;
                std::cin >> overwriteChoice;
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                if (overwriteChoice != 'y' && overwriteChoice != 'Y') {
                    std::cerr << "Decryption aborted to prevent overwriting the existing file.\n\n";
                    continue;
                }
            }

            if (!writeFile(outputFilePath, fileData)) {
                std::cerr << "Decryption failed due to output file error.\n\n";
                continue;
            }

            std::cout << "Decryption successful! Decrypted file saved as \"" << outputFilePath << "\".\n\n";
        }
        else if (choice == 3) {
            // Exit
            std::cout << "Exiting the program. Goodbye!\n";
            break;
        }
        else {
            std::cerr << "Invalid choice. Please enter 1, 2, or 3.\n\n";
        }
    }

    return 0;
}

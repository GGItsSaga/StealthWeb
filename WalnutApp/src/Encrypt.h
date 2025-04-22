#pragma once
#ifndef ENCRYPT_H
#define ENCRYPT_H
#include <sodium.h>
#include <fstream>
#include <iostream>
#include <vector>
#include <filesystem>
#include <stdexcept>

class Encrypt {
private:
    std::vector<std::filesystem::path> filePaths;
    std::string passkey;

    std::string readFile(const std::filesystem::path& filePath) {
        std::ifstream file(filePath, std::ios::binary);
        if (!file.is_open()) {
            throw std::runtime_error("Error opening file: " + filePath.string());
        }
        return std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    }

    std::string encryptText(const std::string& plainText) {
        unsigned char nonce[crypto_secretbox_NONCEBYTES];
        randombytes_buf(nonce, sizeof(nonce));

        std::vector<unsigned char> key = deriveKey(passkey);

        std::vector<unsigned char> cipherText(plainText.size() + crypto_secretbox_MACBYTES);
        if (crypto_secretbox_easy(cipherText.data(),
            reinterpret_cast<const unsigned char*>(plainText.data()),
            plainText.size(),
            nonce,
            key.data()) != 0) {
            throw std::runtime_error("Encryption failed");
        }

        std::vector<unsigned char> finalOutput;
        finalOutput.insert(finalOutput.end(), nonce, nonce + crypto_secretbox_NONCEBYTES);
        finalOutput.insert(finalOutput.end(), cipherText.begin(), cipherText.end());

        return std::string(reinterpret_cast<const char*>(finalOutput.data()), finalOutput.size());
    }

    std::vector<unsigned char> deriveKey(const std::string& passphrase) {
        std::vector<unsigned char> key(crypto_secretbox_KEYBYTES);
        crypto_generichash(key.data(), key.size(),
            reinterpret_cast<const unsigned char*>(passphrase.data()), passphrase.size(),
            nullptr, 0);
        return key;
    }

    void writeFile(const std::filesystem::path& filePath, const std::string& content) {
        std::ofstream file(filePath, std::ios::binary);
        if (!file.is_open()) {
            throw std::runtime_error("File write error: " + filePath.string());
        }

        file.write(content.data(), content.size());
        file.close();
    }

public:
    Encrypt() {
        if (sodium_init() < 0) {
            throw std::runtime_error("Libsodium could not be initialized");
        }
    }

    ~Encrypt() {
        sodium_memzero(&passkey[0], passkey.size());
    }

    void setPassKey(const std::string& p) {
        passkey = p;
    }

    void appendFile(const std::string& fileName) {
        std::filesystem::path filePath(fileName);
        filePaths.emplace_back(filePath);
    }

    const std::string& getKey() const {
        return passkey;
    }

    void encryptAndSave() {
        for (const auto& filePath : filePaths) {
            std::string plainText = readFile(filePath);
            std::string encryptedText = encryptText(plainText);

            std::filesystem::path outputFile = filePath.parent_path() / ("Encrypted_" + filePath.filename().string());
            writeFile(outputFile, encryptedText);
        }
    }
};

#endif

#pragma once
#ifndef DECRYPT_H
#define DECRYPT_H

#include <sodium.h>
#include <fstream>
#include <iostream>
#include <vector>
#include <stdexcept>
#include <filesystem>

class Decrypt {
private:
    std::vector<std::string> filenames_;
    std::string passkey_;

    std::string readFile(const std::string& filename) {
        std::ifstream file(filename, std::ios::binary);
        if (!file) {
            throw std::runtime_error("Error opening file: " + filename);
        }
        return std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    }

    void writeFile(const std::string& originalPath, const std::string& content) {
        std::filesystem::path inputPath(originalPath);
        std::filesystem::path outputPath = inputPath.parent_path() / ("decrypted_" + inputPath.filename().string());

        std::ofstream file(outputPath, std::ios::binary);
        if (!file) {
            throw std::runtime_error("Error creating or opening file: " + outputPath.string());
        }

        file.write(content.data(), content.size());
        file.close();
    }


    std::vector<unsigned char> deriveKey(const std::string& passphrase) {
        std::vector<unsigned char> key(crypto_secretbox_KEYBYTES);
        if (crypto_generichash(
            key.data(), key.size(),
            reinterpret_cast<const unsigned char*>(passphrase.data()), passphrase.size(),
            nullptr, 0) != 0) {
            throw std::runtime_error("Key derivation failed");
        }
        return key;
    }

    void decryptAndSave(const std::string& ciphertext, const std::string& passkey, const std::string& filename) {
        const size_t expectedSize = crypto_secretbox_NONCEBYTES + crypto_secretbox_MACBYTES;
        if (ciphertext.size() < expectedSize) {
            throw std::runtime_error("Insufficient ciphertext size for decryption");
        }

        std::vector<unsigned char> nonce(ciphertext.begin(), ciphertext.begin() + crypto_secretbox_NONCEBYTES);
        std::vector<unsigned char> cipherData(ciphertext.begin() + crypto_secretbox_NONCEBYTES, ciphertext.end());
        std::vector<unsigned char> decryptedText(cipherData.size() - crypto_secretbox_MACBYTES);

        std::vector<unsigned char> key = deriveKey(passkey);

        if (crypto_secretbox_open_easy(
            decryptedText.data(), cipherData.data(), cipherData.size(),
            nonce.data(), key.data()) != 0) {
            throw std::runtime_error("Error decrypting file. Wrong passkey.");
        }

        std::string result(reinterpret_cast<const char*>(decryptedText.data()), decryptedText.size());
        writeFile(filename, result);
    }

public:
    Decrypt() {
        if (sodium_init() < 0) {
            throw std::runtime_error("Error initializing Libsodium");
        }
    }

    ~Decrypt() {
        sodium_memzero(&passkey_[0], passkey_.size());
    }

    void setPasskey(const std::string& p) {
        passkey_ = p;
    }

    void appendFile(const std::string& filename) {
        filenames_.push_back(filename);
    }

    void decryptAndSaveAll() {
        for (const auto& filename : filenames_) {
            try {
                std::string ciphertext = readFile(filename);
                decryptAndSave(ciphertext, passkey_, filename);
            }
            catch (const std::exception& e) {
                std::cerr << "Error processing file: " << filename << " - " << e.what() << std::endl;
            }
        }
    }
};

#endif

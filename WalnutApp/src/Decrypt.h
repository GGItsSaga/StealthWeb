#pragma once
#ifndef DECRYPT_H
#define DECRYPT_H
#include <sodium.h>
#include <fstream>
#include <iostream>
#include <vector>
#include <stdexcept>

class Decrypt {
private:
    std::vector<std::string> filenames_;
    std::string passkey_; // Passkey for decryption

    // Reading the file into a string
    std::string readFile(const std::string& filename) {
        std::ifstream file(filename, std::ios::binary);
        if (!file) {
            throw std::runtime_error("Error opening file: " + filename);
        }

        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        file.close();
        return content;
    }

    // Writing the contents on file in binary mode, just as we did in encryption
    void writeFile(const std::string& filename, const std::string& content) {
        std::ofstream file(filename, std::ios::binary);
        if (!file) {
            throw std::runtime_error("Error creating or opening file: " + filename);
        }

        file.write(content.data(), content.size());
        file.close();
    }
    // Decrypting ciphertext file with crypto_secretbox_easy with the provided passkey:
    void decryptAndSave(const std::string& ciphertext, const std::string& passkey, const std::string& filename) {
        try {
            const size_t expectedSize = crypto_secretbox_NONCEBYTES + crypto_secretbox_MACBYTES;
            if (ciphertext.size() < expectedSize) {
                throw std::runtime_error("Insufficient ciphertext size for decryption");
            }

            // Extract nonce from the beginning of the ciphertext:
            std::vector<unsigned char> nonce(ciphertext.begin(), ciphertext.begin() + crypto_secretbox_NONCEBYTES);

            // Decrypt the rest of the ciphertext since the remaining bytes are as such
            std::vector<unsigned char> cipherData(ciphertext.begin() + crypto_secretbox_NONCEBYTES, ciphertext.end());

            std::vector<unsigned char> decryptedText(cipherData.size() - crypto_secretbox_MACBYTES);
            if (crypto_secretbox_open_easy(
                decryptedText.data(),cipherData.data(),cipherData.size(),nonce.data(),
                reinterpret_cast<const unsigned char*>(passkey.data())) != 0) {
                throw std::runtime_error("Error decrypting file. Wrong passkey.");
            }

            std::string result(reinterpret_cast<const char*>(decryptedText.data()), decryptedText.size());
            writeFile("decrypted_" + filename, result);

        }
        catch (const std::exception& e) {
            std::cerr << "Exception during decryption: " << e.what() << std::endl;
            throw;  // Re-throw the exception after logging
        }
    }

public:
    Decrypt() { // Default constructor
        if (sodium_init() < 0) {
            throw std::runtime_error("Error initializing Libsodium");
        }
    }

    ~Decrypt() { // Destructor
        sodium_memzero(&passkey_[0], passkey_.size());
    }

    // Setting the decryption key
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
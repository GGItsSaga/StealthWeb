#pragma once
#ifndef ENCRYPT_H
#define ENCRYPT_H
#include <sodium.h>
#include <fstream>
#include <iostream>
#include <vector>
#include <filesystem>
#include <stdexcept>

class Encrypt
{
private:
	std::vector<std::filesystem::path> filePaths; // Filepath of all files we want to encrypt
	std::string passkey; // Passkey for encryption

	// the following function returns the entire files contents as a single string
	std::string readFile(const std::filesystem::path& filePath)
	{
		std::ifstream file(filePath, std::ios::binary);
		if (!file.is_open())
		{
			// Throw exception if file cannot be read
			throw std::runtime_error("Error opening file: " + filePath.string());
		}
		else
		{
			// the line below reads the entire file contents from the buffer using an iterator (iterator is like a pointer)
			// the first argument is an iterator starting from the beginning, the second points to the eof (end of file)
			// so basically, this string holds the entire files contents
			std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
			file.close();
			return content;
		}
	}

	// Encrypts the plaintext using crypto_secretbox_easy with the passkey as the symmetric key
	std::string encryptText(const std::string& plainText, const std::string& key)
	{
		unsigned char nonce[crypto_secretbox_NONCEBYTES];
		randombytes_buf(nonce, sizeof(nonce));

		// New implementation: Encrypting plainText using crypto_secretbox_easy w/ passkey
		std::vector<unsigned char> cipherText(plainText.size() + crypto_secretbox_MACBYTES);
		if (crypto_secretbox_easy(cipherText.data(),
			reinterpret_cast<const unsigned char*>(plainText.data()),
			plainText.size(),
			nonce,
			reinterpret_cast<const unsigned char*>(key.data())) != 0) {
			
			throw std::runtime_error("Encryption failed");
		}

		// Prepends nonce to ciphertext (needed for decryption)
		std::vector<unsigned char> finalOutput;
		finalOutput.insert(finalOutput.end(), nonce, nonce + crypto_secretbox_NONCEBYTES);
		finalOutput.insert(finalOutput.end(), cipherText.begin(), cipherText.end());

		return std::string(reinterpret_cast<const char*>(finalOutput.data()), finalOutput.size());
	}


	// this function writes our encrypted text from the above function into a file
	void writeFile(const std::filesystem::path& filePath, const std::string& content)
	{
		// Opens and encrypts file in binary mode:
		std::ofstream file(filePath, std::ios::binary);
		if (!file.is_open())
		{
			throw std::runtime_error("File write error: " + filePath.string());
		}
		else
		{
			file.write(content.data(), content.size());
			file.close();
			
		}
	}

public:
	Encrypt() // default constructor
	{
		if (sodium_init() < 0)
		{
			throw std::runtime_error("Libsodium could not be initialized");
		}
	}

	// destructor to free dynamically allocated memory
	~Encrypt()
	{
		sodium_memzero(&passkey[0], passkey.size());
	}

	// Setting passkey for user
	void setPassKey(const std::string& p) 
	{
		passkey = p;
	}

	// add files to our vector. Appends file for encryption
	void appendFile(const std::string& fileName)
	{
		std::filesystem::path filePath(fileName);
		filePaths.emplace_back(filePath);
	}

	// returns current passkey
	// we will need it for decryption
	const std::string& getKey() const
	{
		return passkey;
	}

	// this encrypts/saves each file in our vector
	// the files are stored as .bin
	void encryptAndSave()
	{
		for (const auto& filePath : filePaths)
		{
			std::string plainText = readFile(filePath);
			std::string encryptedText = encryptText(plainText, passkey);

			std::filesystem::path outputFile = filePath.parent_path() / ("Encrypted_" + filePath.filename().string());
			writeFile(outputFile, encryptedText);
		}
	}
};


#endif
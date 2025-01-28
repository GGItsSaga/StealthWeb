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
	std::vector<std::filesystem::path> filePaths; // all of the files we want to encrypt
	std::string key; // secret key 

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

	// libsodium requires us to generate a key for encryption, thats what this function does.
	// basically, we are initializing the key private variable
	void generateRandomKey()
	{
		key = std::string(crypto_secretbox_KEYBYTES, 0); // create a string of nullbytes (0 bytes) equal to the size of the secret key

		// the randombytes_buf function overwrites the null bytes with a random byte
		// the first argument, &key[0], is the address of the first byte in the key string
		// remember, a string is just an array of const chars, so key[0] is just the first char/byte
		// the last argument is the size of the key, so that the function knows how many bytes to randomize
		randombytes_buf(reinterpret_cast<unsigned char*>(&key[0]), key.size());
	}


	std::string encryptText(const std::string& plainText, const std::string& key)
	{
		unsigned char nonce[crypto_secretbox_NONCEBYTES];
		randombytes_buf(nonce, sizeof(nonce));

		std::vector<unsigned char> cypherText(plainText.size() + crypto_secretbox_MACBYTES);
		crypto_secretbox_easy(cypherText.data(), reinterpret_cast<const unsigned char*>(plainText.data()), plainText.size(),
			nonce, reinterpret_cast<const unsigned char*>(key.data()));

		// Include the nonce at the beginning of the cypherText
		cypherText.insert(cypherText.begin(), nonce, nonce + crypto_secretbox_NONCEBYTES);

		// Convert the vector to a string
		std::string result(reinterpret_cast<const char*>(cypherText.data()), cypherText.size());

		return result;
	}


	// this function writes our encrypted text from the above function into a file
	// you might be wondering, why do I keep typing "const datatype& variableName" so much
	// its for code efficiency/security. 
	// we are passing by reference (&) so that we dont create unnecessary copies of variables
	// we are using const so that we dont accidently modify our arguements. 
	void writeFile(const std::filesystem::path& filePath, const std::string& content)
	{
		// we open the file in binary mode
		// why?
		// because its common practice to do so in encryption
		// the encrypted file is usually in raw bits (look at the code above, everytime we use crypt_secretbox its using bits)
		// if we did in regular text mode, it could mess up our encryption when converting to text
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
			std::cerr << "Error initializing libsodium" << std::endl;
			throw std::runtime_error("Libsodium could not be initialized");
		}
	}
	//// parameterized constructor
	//Encrypt(const std::string& fileName)
	//{
	//	fileNames.emplace_back(fileName);
	//	generateRandomKey();
	//}

	// destructor to free dynamically allocated memory
	~Encrypt()
	{
		sodium_memzero(&key[0], key.size());
	}

	// add files to our vector
	void appendFile(const std::string& fileName)
	{
		std::filesystem::path filePath(fileName);
		filePaths.emplace_back(filePath);
		generateRandomKey();
		
	}

	// get the key
	// we will need it for decryption
	const std::string& getKey() const
	{
		return key;
	}

	// this encrypts/saves each file in our vector
	// the files are stored as .bin
	void encryptAndSave()
	{
		for (const auto& filePath : filePaths)
		{
			std::string plainText = readFile(filePath);
			std::string encryptedText = encryptText(plainText, key);

			std::filesystem::path outputFile = filePath.parent_path() / ("Encrypted_" + filePath.filename().string());
			writeFile(outputFile, encryptedText);
		}
	}

	//const std::vector<std::string>& getEncryptedFileNames() const
	//{
	//	return fileNames;
	//}
};


#endif
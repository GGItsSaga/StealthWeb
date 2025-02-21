#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"
#include "MainLayer.h"
#include "Encrypt.h"
#include "Decrypt.h"
#include "EncryptButton.h"
#include "DecryptButton.h"

#include <iostream>
#include <fstream>

MainLayer::MainLayer() {
    e = std::make_unique<Encrypt>();  
    d = std::make_unique<Decrypt>();  
}

// Encryption callback
void MainLayer::encryptCallback(const char* inputFileEncrypt, const std::string& passkey) {
    e->setPassKey(passkey);
    e->appendFile(inputFileEncrypt);
    e->encryptAndSave();
}

// Decryption callback
void MainLayer::decryptCallback(const char* inputFileDecrypt, const std::string& passkey) {
    try {
        d->setPasskey(passkey);
        // Appends file and decrypt the encrypted file
        d->appendFile(inputFileDecrypt);
        d->decryptAndSaveAll(); // Assuming this method decrypts and saves the decrypted content
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error during decryption: " << e.what() << std::endl;
        // Handle decryption error if needed
    }
}

bool MainLayer::fileExists(const char* fileName) {
    std::ifstream file(fileName);
    return file.good();
}


Walnut::Application* Walnut::CreateApplication(int argc, char** argv) {
    Walnut::ApplicationSpecification spec;
    spec.Name = "Open-Source Encryption/Decryption";
    spec.CustomTitlebar = true;

    Walnut::Application* app = new Walnut::Application(spec);
    std::shared_ptr<MainLayer> mainLayer = std::make_shared<MainLayer>();
    app->PushLayer(mainLayer);
    app->SetMenubarCallback([app, mainLayer]() {
        if (ImGui::BeginMenu("Options")) {
            if (ImGui::MenuItem("Exit")) {
                app->Close();
            }
            ImGui::EndMenu();
        }
        });
    return app;
}

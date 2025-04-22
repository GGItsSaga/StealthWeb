#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"
#include "MainLayer.h"
#include "Encrypt.h"
#include "Decrypt.h"
#include "EncryptButton.h"
#include "DecryptButton.h"

#include <iostream>
#include <fstream>
#include <GLFW/glfw3.h> // ✅Required for GLFW drop callback

MainLayer::MainLayer() {
    e = std::make_unique<Encrypt>();
    d = std::make_unique<Decrypt>();
}

// Encryption callback
void MainLayer::encryptCallback(const char* inputFileEncrypt, const std::string& passkey) {
    try {
        e->setPassKey(passkey);
        e->appendFile(inputFileEncrypt);
        e->encryptAndSave();
        showEncryptionSuccess = true;
    }
    catch (const std::exception& e) {
        errorMessage = e.what();
        showErrorPopup = true;
    }
}

// Decryption callback
void MainLayer::decryptCallback(const char* inputFileDecrypt, const std::string& passkey) {
    try {
        d->setPasskey(passkey);
        d->appendFile(inputFileDecrypt);
        d->decryptAndSaveAll();
        showDecryptionSuccess = true;
    }
    catch (const std::exception& e) {
        errorMessage = e.what();
        showErrorPopup = true;
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

    // ✅ GLFW file drag-and-drop support
    GLFWwindow* window = static_cast<GLFWwindow*>(Walnut::Application::Get().GetWindowHandle());
    glfwSetDropCallback(window, [](GLFWwindow*, int count, const char** paths) {
        auto& layerStack = Walnut::Application::Get().GetLayerStack();
        if (!layerStack.empty()) {
            auto* mainLayer = dynamic_cast<MainLayer*>(layerStack.front().get());
            if (mainLayer) {
                mainLayer->OnDragDrop(count, paths);
            }
        }
        });

    app->SetMenubarCallback([app]() {
        if (ImGui::BeginMenu("Options")) {
            if (ImGui::MenuItem("Exit")) {
                app->Close();
            }
            ImGui::EndMenu();
        }
        });

    return app;
}

void MainLayer::OnDragDrop(int count, const char** paths) {
    if (count > 0 && paths != nullptr) {
        std::strncpy(m_inputFileEncrypt, paths[0], sizeof(m_inputFileEncrypt));
        std::strncpy(m_inputFileDecrypt, paths[0], sizeof(m_inputFileDecrypt));
    }
}

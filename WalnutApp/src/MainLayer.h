#ifndef MAIN_LAYER_H
#define MAIN_LAYER_H

#include "Walnut/Layer.h"
#include "Walnut/UI/UI.h"
#include <string>

#include "EncryptButton.h"
#include "DecryptButton.h"

#include "Encrypt.h"
#include "Decrypt.h"

class MainLayer : public Walnut::Layer {
private:
    std::unique_ptr<Encrypt> e;
    std::unique_ptr<Decrypt> d;
    char m_inputFileEncrypt[128] = "";
    char m_inputFileDecrypt[128] = "";
    std::string m_PasskeyInput = std::string(128, '\0');
    std::string encryptError, decryptError;

public:
    MainLayer();
    inline virtual void OnUIRender() override; // Declare and implement the inline function

    void encryptCallback(const char* inputFile, const std::string& passkey);
    void decryptCallback(const char* inputFile, const std::string& passkey);
    bool fileExists(const char* fileName);
};

inline void MainLayer::OnUIRender() {
    ImVec2 windowSize = ImGui::GetIO().DisplaySize;
    ImGui::SetNextWindowSize(windowSize, ImGuiCond_Always);
    ImGui::Begin("Main Menu");

    // Center allignment for Encrypt and Decrypt buttons
    float windowWidth = ImGui::GetWindowWidth();
    float buttonWidth = 300.0f;
    float buttonHeight = 100.0f;
    float vertPadding = (ImGui::GetWindowSize().y - buttonHeight * 2) / 3.0f;

    // Vertical spacing for both buttons
    ImGui::Spacing();
    ImGui::Dummy(ImVec2(0.0f, vertPadding));

    // Horizontal position of buttons for center allignment
    float horiPadding = (windowWidth - buttonWidth * 2) / 2.0f;
    ImGui::SetCursorPosX(horiPadding);

    // Encrypt Button
    encryptButton(m_inputFileEncrypt, encryptError, m_PasskeyInput,
        [this](const char* file, const std::string& key) { 
            encryptCallback(file, key); },
        [this](const char* file) { 
            return fileExists(file); });

    ImGui::SameLine();

    // Decrypt Button
    decryptButton(m_inputFileDecrypt, decryptError, m_PasskeyInput, 
        [this](const char* file, const std::string& key) { 
            decryptCallback(file, key); },
        [this](const char* file) { 
            return fileExists(file); });

    ImGui::End();
}

#endif // MAIN_LAYER_H

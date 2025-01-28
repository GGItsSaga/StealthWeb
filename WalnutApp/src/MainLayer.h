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
    char inputFileEncrypt[512] = "";
    char inputFileDecrypt[512] = "";
    std::string encryptError, decryptError;

    void encryptCallback(const char* inputFile);
    void decryptCallback(const char* inputFile);
    bool fileExists(const char* fileName);

public:
    MainLayer();
    inline virtual void OnUIRender() override; // Declare and implement the inline function
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
    encryptButton(inputFileEncrypt, encryptError,
        [this](const char* file) { encryptCallback(file); },
        [this](const char* file) { return fileExists(file); });

    ImGui::SameLine();

    // Decrypt Button
    decryptButton(inputFileDecrypt, decryptError,
        [this](const char* file) { decryptCallback(file); },
        [this](const char* file) { return fileExists(file); });

    //// Input field for decryption key
    //ImGui::InputText("Decryption Key", decryptionKeyInput, sizeof(decryptionKeyInput));

    ImGui::End();
}

#endif // MAIN_LAYER_H


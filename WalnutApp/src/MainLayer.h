#ifndef MAIN_LAYER_H
#define MAIN_LAYER_H

#include "Walnut/Layer.h"
#include "Walnut/UI/UI.h"
#include <string>
#include <cstring>

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
    std::string m_PasskeyInput;
    std::string encryptError, decryptError;
    bool showEncryptionSuccess = false;
    bool showDecryptionSuccess = false;
    bool showErrorPopup = false;
    std::string errorMessage;


public:
    MainLayer();
    inline virtual void OnUIRender() override;

    void encryptCallback(const char* inputFile, const std::string& passkey);
    void decryptCallback(const char* inputFile, const std::string& passkey);
    bool fileExists(const char* fileName);
    void OnDragDrop(int count, const char** paths);
};

inline void MainLayer::OnUIRender() {
    ImVec2 windowSize = ImGui::GetIO().DisplaySize;
    ImGui::SetNextWindowSize(windowSize, ImGuiCond_Always);
    ImGui::Begin("Main Menu");

    // Center alignment setup
    float windowWidth = ImGui::GetWindowWidth();
    float buttonWidth = 300.0f;
    float buttonHeight = 100.0f;
    float vertPadding = (ImGui::GetWindowSize().y - buttonHeight * 2) / 3.0f;
    float horiPadding = (windowWidth - buttonWidth * 2) / 2.0f;

    ImGui::Spacing();
    ImGui::Dummy(ImVec2(0.0f, vertPadding));
    ImGui::SetCursorPosX(horiPadding);

    // Encrypt Button
    encryptButton(m_inputFileEncrypt, encryptError, m_PasskeyInput,
        [this](const char* file, const std::string& key) {
            encryptCallback(file, key);
        },
        [this](const char* file) {
            return fileExists(file);
        });

    ImGui::SameLine();

    // Decrypt Button
    decryptButton(m_inputFileDecrypt, decryptError, m_PasskeyInput,
        [this](const char* file, const std::string& key) {
            decryptCallback(file, key);
        },
        [this](const char* file) {
            return fileExists(file);
        });

    ImGui::Dummy(ImVec2(0.0f, 20.0f));

    // ?? Drop area UI container with glow effect
    ImVec2 boxSize = ImVec2(900, buttonHeight);
    ImVec2 boxPos = ImVec2(
        (ImGui::GetWindowWidth() - boxSize.x) * 0.5f,
        ImGui::GetCursorPosY()
    );
    ImGui::SetCursorPos(boxPos);

    // ?? Light blue glow effect
    ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(100, 180, 255, 50));
    ImGui::BeginChild("DropArea", boxSize, true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
    ImGui::PopStyleColor();

    ImGui::SetCursorPosY(20);

    std::string fileDisplay = m_inputFileEncrypt[0] != '\0'
        ? std::string("Selected: ") + m_inputFileEncrypt
        : "Drop a file here...";

    float labelWidth = ImGui::CalcTextSize(fileDisplay.c_str()).x;
    float centerX = (boxSize.x - labelWidth - 20.0f) * 0.5f;
    ImGui::SetCursorPosX(centerX);

    ImGui::TextWrapped("%s", fileDisplay.c_str());

    if (m_inputFileEncrypt[0] != '\0') {
        ImGui::SameLine();
        if (ImGui::SmallButton("X")) {
            std::memset(m_inputFileEncrypt, 0, sizeof(m_inputFileEncrypt));
            std::memset(m_inputFileDecrypt, 0, sizeof(m_inputFileDecrypt));
        }
    }

    ImGui::EndChild();

    // Encryption success popup
    if (showEncryptionSuccess) {
        ImGui::OpenPopup("Encryption Complete");
        showEncryptionSuccess = false;
    }

    if (ImGui::BeginPopupModal("Encryption Complete", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Your file has been encrypted successfully!");
        if (ImGui::Button("OK")) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    // Decryption success popup
    if (showDecryptionSuccess) {
        ImGui::OpenPopup("Decryption Complete");
        showDecryptionSuccess = false;
    }

    if (ImGui::BeginPopupModal("Decryption Complete", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Your file has been decrypted successfully!");
        if (ImGui::Button("OK")) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    // Error popups
    if (showErrorPopup) {
        ImGui::OpenPopup("Error");
        showErrorPopup = false;
    }

    if (ImGui::BeginPopupModal("Error", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::TextWrapped("%s", errorMessage.c_str());
        if (ImGui::Button("OK")) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }



    ImGui::End();
}

#endif // MAIN_LAYER_H

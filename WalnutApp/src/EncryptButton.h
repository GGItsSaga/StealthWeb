#pragma once

#include "imgui.h"
#include <string>

// Encrypt button
inline void encryptButton(char* inputFileEncrypt, std::string& encryptError, std::string& passkeyInput,
    const std::function<void(const char*, const std::string&)>& encryptCallback,
    const std::function<bool(const char*)>& fileExists) {

    if (ImGui::Button("Encrypt", ImVec2(300.0f, 100.0f))) {
        ImGui::OpenPopup("Encrypt Files");
    }

    if (ImGui::BeginPopupModal("Encrypt Files", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {

        ImGui::Text("Enter file name for Encryption:");
        ImGui::InputText("##File to Encrypt", inputFileEncrypt, 128);

        ImGui::Text("Enter Encryption passkey:");
        static char tempPasskey[128] = "";
        ImGui::InputText("###Passkey:", tempPasskey, sizeof(tempPasskey), ImGuiInputTextFlags_Password);

        if (ImGui::Button("Encrypt")) {
            if (fileExists(inputFileEncrypt)) {
                passkeyInput = std::string(tempPasskey);
                encryptCallback(inputFileEncrypt, passkeyInput);
                ImGui::CloseCurrentPopup();

                std::memset(inputFileEncrypt, 0, 128);
                passkeyInput.clear();
                std::memset(tempPasskey, 0, sizeof(tempPasskey));
            }
            else {
                encryptError = "Error: File not found.";
            }
        }

        ImGui::SameLine();

        if (ImGui::Button("Cancel")) {
            ImGui::CloseCurrentPopup();

            std::memset(inputFileEncrypt, 0, 128);
            passkeyInput.clear();
            std::memset(tempPasskey, 0, sizeof(tempPasskey));
        }

        if (!encryptError.empty()) {
            ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), encryptError.c_str());
        }
        ImGui::EndPopup();
    }
}

#pragma once

#include "imgui.h"
#include <string>

// Encrypt button
inline void encryptButton(char* inputFileEncrypt, std::string& encryptError, std::string& passkeyInput,
    const std::function<void(const char*, const std::string&)>& encryptCallback,
    const std::function<bool(const char*)>& fileExists){

    if (ImGui::Button("Encrypt", ImVec2(300.0f, 100.0f))) {
        ImGui::OpenPopup("Encrypt Files");
    }

    if (ImGui::BeginPopupModal("Encrypt Files", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {

        // File name input
        ImGui::Text("Enter file name for Encryption:");
        ImGui::InputText("##File to Encrypt", inputFileEncrypt, 128);

        // Passkey input
        ImGui::Text("Enter Encryption passkey:");
        ImGui::InputText("###Passkey:", &passkeyInput[0], passkeyInput.capacity());

        if (ImGui::Button("Encrypt")) {
            if (fileExists(inputFileEncrypt)) {
                encryptCallback(inputFileEncrypt, passkeyInput);
                ImGui::CloseCurrentPopup();

                // Clears input fields on close
                std::memset(inputFileEncrypt, 0, 128);
                passkeyInput.clear();
            }
            else {
                encryptError = "Error: File not found.";
            }
        }

        ImGui::SameLine();

        if (ImGui::Button("Cancel")) {
            ImGui::CloseCurrentPopup();

            // Clears input fields on close
            std::memset(inputFileEncrypt, 0, 128);
            passkeyInput.clear();
        }

        if (!encryptError.empty()) {
            ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), encryptError.c_str());
        }
        ImGui::EndPopup();
    }
}
#pragma once

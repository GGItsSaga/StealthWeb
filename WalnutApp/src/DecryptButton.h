#pragma once

#include "imgui.h"
#include <string>

// Decrypt button
inline void decryptButton(char* inputFileDecrypt, std::string& decryptError, std:: string& passkeyInput,
    const std::function<void(const char*,std::string&)>& decryptCallback,
    const std::function<bool(const char*)>& fileExists) {

    if (ImGui::Button("Decrypt", ImVec2(300.0f, 100.0f))) {
        ImGui::OpenPopup("Decrypt Files");
    }

    if (ImGui::BeginPopupModal("Decrypt Files", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Enter file name for Decryption:");
        ImGui::InputText("##File to Decrypt", inputFileDecrypt, 128);

        ImGui::Text("Enter Decryption Passkey:");
        ImGui::InputText("###Passkey:", &passkeyInput[0], 128);

        if (ImGui::Button("Decrypt")) {
            if (fileExists(inputFileDecrypt)) {
                try {
                    decryptCallback(inputFileDecrypt, passkeyInput);
                    ImGui::CloseCurrentPopup();

                    // Clears input fields on close
                    std::memset(inputFileDecrypt, 0, 128);
                    passkeyInput.clear();
                }
                catch (const std::exception& e) {
                    decryptError = "Error: Wrong passkey.";
                }
                
            }
            else {
                decryptError = "Error: File not found.";
            }
        }

        ImGui::SameLine();

        if (ImGui::Button("Cancel")) {
            ImGui::CloseCurrentPopup();

            // Clears input fields on close
            std::memset(inputFileDecrypt, 0, 128);
            passkeyInput.clear();
        }

        if (!decryptError.empty()) {
            ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), decryptError.c_str());
        }
        ImGui::EndPopup();
    }
}
#pragma once

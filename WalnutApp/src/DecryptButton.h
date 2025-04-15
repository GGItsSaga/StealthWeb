#pragma once

#include "imgui.h"
#include <string>

// Decrypt button
inline void decryptButton(char* inputFileDecrypt, std::string& decryptError, std::string& passkeyInput,
    const std::function<void(const char*, std::string&)>& decryptCallback,
    const std::function<bool(const char*)>& fileExists) {

    if (ImGui::Button("Decrypt", ImVec2(300.0f, 100.0f))) {
        ImGui::OpenPopup("Decrypt Files");
    }

    if (ImGui::BeginPopupModal("Decrypt Files", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Enter file name for Decryption:");
        ImGui::InputText("##File to Decrypt", inputFileDecrypt, 128);

        // Temporary local buffer for safe user passkey input
        ImGui::Text("Enter Decryption Passkey:");
        static char tempPasskey[128] = "";
        ImGui::InputText("###Passkey:", tempPasskey, sizeof(tempPasskey), ImGuiInputTextFlags_Password);

        if (ImGui::Button("Decrypt")) {
            if (fileExists(inputFileDecrypt)) {
                try {
                    passkeyInput = std::string(tempPasskey);  // Clean transfer
                    decryptCallback(inputFileDecrypt, passkeyInput);
                    ImGui::CloseCurrentPopup();

                    // Clear all inputs on success
                    std::memset(inputFileDecrypt, 0, 128);
                    passkeyInput.clear();
                    std::memset(tempPasskey, 0, sizeof(tempPasskey));
                    decryptError.clear();
                }
                catch (const std::exception&) {
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

            // Clear all inputs on cancel
            std::memset(inputFileDecrypt, 0, 128);
            passkeyInput.clear();
            std::memset(tempPasskey, 0, sizeof(tempPasskey));
            decryptError.clear();
        }

        if (!decryptError.empty()) {
            ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), decryptError.c_str());
        }

        ImGui::EndPopup();
    }
}

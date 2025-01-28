#pragma once

#include "imgui.h"
#include <string>

// Decrypt button
inline void decryptButton(char* inputFileDecrypt, std::string& decryptError,
    const std::function<void(const char*)>& decryptCallback,
    const std::function<bool(const char*)>& fileExists) {
    if (ImGui::Button("Decrypt", ImVec2(300.0f, 100.0f))) {
        ImGui::OpenPopup("Decrypt Files");
    }

    if (ImGui::BeginPopupModal("Decrypt Files", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Enter file name for Decryption:");
        ImGui::InputText("##File to Decrypt", inputFileDecrypt, 128);

        if (ImGui::Button("Decrypt")) {
            if (fileExists(inputFileDecrypt)) {
                decryptCallback(inputFileDecrypt);
                ImGui::CloseCurrentPopup();
            }
            else {
                decryptError = "Error: File not found.";
            }
        }

        ImGui::SameLine();

        if (ImGui::Button("Cancel")) {
            ImGui::CloseCurrentPopup();
        }

        if (!decryptError.empty()) {
            ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), decryptError.c_str());
        }
        ImGui::EndPopup();
    }
}
#pragma once

#pragma once

#include "imgui.h"
#include <string>

// Encrypt button
inline void encryptButton(char* inputFileEncrypt, std::string& encryptError,
    const std::function<void(const char*)>& encryptCallback,
    const std::function<bool(const char*)>& fileExists) {
    if (ImGui::Button("Encrypt", ImVec2(300.0f, 100.0f))) {
        ImGui::OpenPopup("Encrypt Files");
    }

    if (ImGui::BeginPopupModal("Encrypt Files", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {

        ImGui::Text("Enter file name for Encryption:");
        ImGui::InputText("##File to Encrypt", inputFileEncrypt, 128);

        if (ImGui::Button("Encrypt")) {
            if (fileExists(inputFileEncrypt)) {
                encryptCallback(inputFileEncrypt);
                ImGui::CloseCurrentPopup();
            }
            else {
                encryptError = "Error: File not found.";
            }
        }

        ImGui::SameLine();

        if (ImGui::Button("Cancel")) {
            ImGui::CloseCurrentPopup();
        }

        if (!encryptError.empty()) {
            ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), encryptError.c_str());
        }
        ImGui::EndPopup();
    }
}
#pragma once

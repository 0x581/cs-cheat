#include "../../overlay.hpp"

#include "config.hpp"
#include "render_data.hpp"

void c_overlay::draw_bomb()
{
    ImGui::SetNextWindowPos(cfg.persistent.bombWindow.position, ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(bombWindowSize);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

    ImGui::Begin(
        "Bomb GUI",
        0,
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse
        //| (show_menu ? 0 : ImGuiWindowFlags_NoBackground)
    );

    cfg.persistent.bombWindow.position = ImGui::GetWindowPos();

    ImGui::PushFont(bomb_font);

    std::vector<std::string> lines;

    const auto &bomb = renderData.bomb;

    if (!bomb.planted)
    {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));
        lines.push_back(std::string("Bomb is not planted."));
    }
    if (bomb.planted)
    {
        if (bomb.defused)
        {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.0f, 1.0f));
            lines.push_back(std::string("Bomb has been defused."));
        }
        else if (bomb.timeRemaining < 0.f)
        {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
            lines.push_back(std::string("Bum"));
        }
        else
        {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
            lines.push_back(std::string("Bomb has been planted on ") + (bomb.site ? "B." : "A."));
            lines.push_back(std::format("{:.2f} seconds remaining.", bomb.timeRemaining));
        }
    }

    const auto start_cursor_pos = ImGui::GetCursorPos();
    const auto available = ImGui::GetContentRegionAvail();

    const auto font_size = ImGui::GetFontSize();
    const auto total_height = font_size * lines.size();
    float current_height = (available.y - total_height) * 0.5f;

    for (const auto &line : lines)
    {
        const auto line_size = ImGui::CalcTextSize(line.c_str());
        const auto text_x = (available.x - line_size.x) * 0.5f;

        ImGui::SetCursorPos(start_cursor_pos + ImVec2(text_x, current_height));

        ImGui::TextUnformatted(line.c_str());

        current_height += font_size;
    }

    // line1_text += std::string("\nLocal Player Can Defuse: ") + (bomb.localPlayerCanDefuse ? "Yes" : "No");
    // line1_text += std::string("\nBeing Defused: ") + (bomb.beingDefused ? "Yes" : "No");
    // line1_text += std::string("\nDefuser: ") + (bomb.defuser ? std::to_string(bomb.defuser) : "None");

    ImGui::PopStyleColor();

    ImGui::PopFont();
    ImGui::End();

    ImGui::PopStyleVar();
}

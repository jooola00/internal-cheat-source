struct g_p
{
	bool memory = true;
	bool corner = false;
	bool lines = false;
	bool aimbot_fov = true;
	bool box = false;
	bool outlined = false;
	bool distance = false;
	bool shadow = false;
	float smoothness = 1;
	float radius = 100;
	bool menu = true;
	int tabs;
	bool debug = false;
	bool vischeck = true;
	bool skeleton = false;
	bool RapidFire = false;
    float rapidfire = 50;
	bool crosshair = false;
	bool instarevive = false;
	bool mouseaim = false;
	float Mouseaim = 50;
};
g_p settings;

#include <sstream>

namespace fn
{
	namespace general_overlay
	{
		std::string string_To_UTF8(const std::string& str)
		{
			int nwLen = ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);
			wchar_t* pwBuf = new wchar_t[nwLen + 1];
			ZeroMemory(pwBuf, nwLen * 2 + 2);
			::MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.length(), pwBuf, nwLen);
			int nLen = ::WideCharToMultiByte(CP_UTF8, 0, pwBuf, -1, NULL, NULL, NULL, NULL);
			char* pBuf = new char[nLen + 1];
			ZeroMemory(pBuf, nLen + 1);
			::WideCharToMultiByte(CP_UTF8, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);
			std::string retStr(pBuf);
			delete[]pwBuf;
			delete[]pBuf;
			pwBuf = NULL;
			pBuf = NULL;
			return retStr;
		}
		// Drawings for custom menus + Toggle Button's / Checkboxes ETC
		void RegularRGBText(int x, int y, ImColor color, const char* str)
		{
			ImFont a;
			std::string utf_8_1 = std::string(str);
			std::string utf_8_2 = string_To_UTF8(utf_8_1);
			ImGui::GetOverlayDrawList()->AddText(ImVec2(x, y), ImColor(color), utf_8_2.c_str());
		}
		void Test(int x, int y, ImColor color, const char* str)
		{
			ImFont a;
			std::string utf_8_1 = std::string(str);
			std::string utf_8_2 = string_To_UTF8(utf_8_1);
			ImGui::GetOverlayDrawList()->AddText(ImVec2(x, y), ImGui::ColorConvertFloat4ToU32(ImVec4(color)), utf_8_2.c_str());
		}
		void ShadowRGBText(int x, int y, ImColor color, const char* str)
		{
			ImFont a;
			std::string utf_8_1 = std::string(str);
			std::string utf_8_2 = string_To_UTF8(utf_8_1);
			ImGui::GetOverlayDrawList()->AddText(ImVec2(x + 2, y + 2), ImGui::ColorConvertFloat4ToU32(ImColor(0, 0, 0, 240)), utf_8_2.c_str());
			ImGui::GetOverlayDrawList()->AddText(ImVec2(x + 2, y + 2), ImGui::ColorConvertFloat4ToU32(ImColor(0, 0, 0, 240)), utf_8_2.c_str());
			ImGui::GetOverlayDrawList()->AddText(ImVec2(x, y), ImGui::ColorConvertFloat4ToU32(ImVec4(color)), utf_8_2.c_str());
		}
		void OutlinedRBGText(int x, int y, ImColor color, const char* str)
		{
			ImFont a;
			std::string utf_8_1 = std::string(str);
			std::string utf_8_2 = string_To_UTF8(utf_8_1);
			ImGui::GetOverlayDrawList()->AddText(ImVec2(x + 1, y + 2), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, 30 / 30.0)), utf_8_2.c_str());
			ImGui::GetOverlayDrawList()->AddText(ImVec2(x + 1, y + 2), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, 30 / 30.0)), utf_8_2.c_str());
			ImGui::GetOverlayDrawList()->AddText(ImVec2(x, y), ImGui::ColorConvertFloat4ToU32(ImVec4(color)), utf_8_2.c_str());
			ImGui::GetOverlayDrawList()->AddText(ImVec2(x, y), ImGui::ColorConvertFloat4ToU32(ImVec4(color)), utf_8_2.c_str());
		}
		void Rect(int x, int y, int w, int h, ImColor color, int thickness)
		{
			ImGui::GetOverlayDrawList()->AddRect(ImVec2(x, y), ImVec2(x + w, y + h), ImGui::ColorConvertFloat4ToU32(ImVec4(color)), 0, 0, thickness);
		}
		void FilledRect(int x, int y, int w, int h, ImColor color)
		{
			ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h), ImGui::ColorConvertFloat4ToU32(ImVec4(color)), 0, 0);
		}

		void OutlinedString(ImGuiWindow& windowshit, std::string str, ImVec2 loc, ImU32 colr, bool centered = false)
		{
			ImVec2 size = { 0,0 };
			float minx = 0;
			float miny = 0;
			if (centered)
			{
				size = ImGui::GetFont()->CalcTextSizeA(windowshit.DrawList->_Data->FontSize, 0x7FFFF, 0, str.c_str());
				minx = size.x / 2.f;
				miny = size.y / 2.f;
			}

			windowshit.DrawList->AddText(ImVec2((loc.x - 1) - minx, (loc.y - 1) - miny), ImGui::GetColorU32({ 0.f, 0.f, 0.f, 1.f }), str.c_str());
			windowshit.DrawList->AddText(ImVec2((loc.x + 1) - minx, (loc.y + 1) - miny), ImGui::GetColorU32({ 0.f, 0.f, 0.f, 1.f }), str.c_str());
			windowshit.DrawList->AddText(ImVec2((loc.x + 1) - minx, (loc.y - 1) - miny), ImGui::GetColorU32({ 0.f, 0.f, 0.f, 1.f }), str.c_str());
			windowshit.DrawList->AddText(ImVec2((loc.x - 1) - minx, (loc.y + 1) - miny), ImGui::GetColorU32({ 0.f, 0.f, 0.f, 1.f }), str.c_str());
			windowshit.DrawList->AddText(ImVec2(loc.x - minx, loc.y - miny), colr, str.c_str());
		}
		float DrawOutlinedText(ImFont* pFont, const std::string& text, const ImVec2& pos, float size, ImU32 color, bool center)
		{
			ImGuiWindow* window = ImGui::GetCurrentWindow();

			std::stringstream stream(text);
			std::string line;

			float y = 0.0f;
			int i = 0;

			while (std::getline(stream, line))
			{
				ImVec2 textSize = pFont->CalcTextSizeA(size, FLT_MAX, 0.0f, line.c_str());

				if (center)
				{
					window->DrawList->AddText(pFont, size, ImVec2((pos.x - textSize.x / 2.0f) + 1, (pos.y + textSize.y * i) + 1), ImGui::GetColorU32(ImVec4(0, 0, 0, 255)), line.c_str());
					window->DrawList->AddText(pFont, size, ImVec2((pos.x - textSize.x / 2.0f) - 1, (pos.y + textSize.y * i) - 1), ImGui::GetColorU32(ImVec4(0, 0, 0, 255)), line.c_str());
					window->DrawList->AddText(pFont, size, ImVec2((pos.x - textSize.x / 2.0f) + 1, (pos.y + textSize.y * i) - 1), ImGui::GetColorU32(ImVec4(0, 0, 0, 255)), line.c_str());
					window->DrawList->AddText(pFont, size, ImVec2((pos.x - textSize.x / 2.0f) - 1, (pos.y + textSize.y * i) + 1), ImGui::GetColorU32(ImVec4(0, 0, 0, 255)), line.c_str());

					window->DrawList->AddText(pFont, size, ImVec2(pos.x - textSize.x / 2.0f, pos.y + textSize.y * i), ImGui::GetColorU32(color), line.c_str());
				}
				else
				{
					window->DrawList->AddText(pFont, size, ImVec2((pos.x) + 1, (pos.y + textSize.y * i) + 1), ImGui::GetColorU32(ImVec4(0, 0, 0, 255)), line.c_str());
					window->DrawList->AddText(pFont, size, ImVec2((pos.x) - 1, (pos.y + textSize.y * i) - 1), ImGui::GetColorU32(ImVec4(0, 0, 0, 255)), line.c_str());
					window->DrawList->AddText(pFont, size, ImVec2((pos.x) + 1, (pos.y + textSize.y * i) - 1), ImGui::GetColorU32(ImVec4(0, 0, 0, 255)), line.c_str());
					window->DrawList->AddText(pFont, size, ImVec2((pos.x) - 1, (pos.y + textSize.y * i) + 1), ImGui::GetColorU32(ImVec4(0, 0, 0, 255)), line.c_str());

					window->DrawList->AddText(pFont, size, ImVec2(pos.x, pos.y + textSize.y * i), ImGui::GetColorU32(color), line.c_str());
				}

				y = pos.y + textSize.y * (i + 1);
				i++;
			}
			return y;
		}
		float DrawNormalText(ImFont* pFont, const std::string& text, const ImVec2& pos, float size, ImU32 color, bool center)
		{
			ImGuiWindow* window = ImGui::GetCurrentWindow();

			std::stringstream stream(text);
			std::string line;

			float y = 0.0f;
			int i = 0;

			while (std::getline(stream, line))
			{
				ImVec2 textSize = pFont->CalcTextSizeA(size, FLT_MAX, 0.0f, line.c_str());

				if (center)
				{
					window->DrawList->AddText(pFont, size, ImVec2(pos.x - textSize.x / 2.0f, pos.y + textSize.y * i), ImGui::GetColorU32(color), line.c_str());
				}
				else
				{
					window->DrawList->AddText(pFont, size, ImVec2(pos.x, pos.y + textSize.y * i), ImGui::GetColorU32(color), line.c_str());
				}

				y = pos.y + textSize.y * (i + 1);
				i++;
			}
			return y;
		}
		void DrawLine(int x1, int y1, int x2, int y2, const ImVec4& color, int thickness)
		{
			ImGui::GetOverlayDrawList()->AddLine(ImVec2(x1, y1), ImVec2(x2, y2), ImGui::GetColorU32(color), thickness);
		}
		void DrawCorneredBox(int X, int Y, int W, int H, ImColor color, int thickness) {
			float lineW = (W / 3);
			float lineH = (H / 3);

			DrawLine(X, Y, X, Y + lineH, color, thickness);
			DrawLine(X, Y, X + lineW, Y, color, thickness);
			DrawLine(X + W - lineW, Y, X + W, Y, color, thickness);
			DrawLine(X + W, Y, X + W, Y + lineH, color, thickness);
			DrawLine(X, Y + H - lineH, X, Y + H, color, thickness);
			DrawLine(X, Y + H, X + lineW, Y + H, color, thickness);
			DrawLine(X + W - lineW, Y + H, X + W, Y + H, color, thickness);
			DrawLine(X + W, Y + H - lineH, X + W, Y + H, color, thickness);
		}
		void Outline(int X, int Y, int W, int H, ImColor color) {
			float lineW = (W / 3);
			float lineH = (H / 3);

			DrawLine(X, Y, X, Y + lineH, color, 2);
			DrawLine(X, Y, X + lineW, Y, color, 2);
			DrawLine(X + W - lineW, Y, X + W, Y, color, 2);
			DrawLine(X + W, Y, X + W, Y + lineH, color, 2);
			DrawLine(X, Y + H - lineH, X, Y + H, color, 2);
			DrawLine(X, Y + H, X + lineW, Y + H, color, 2);
			DrawLine(X + W - lineW, Y + H, X + W, Y + H, color, 2);
			DrawLine(X + W, Y + H - lineH, X + W, Y + H, color, 2);
		}

	}
	void keys();
	bool actorloop(ImGuiWindow& window);
	void menu();
	void render(ImGuiWindow& window);
}

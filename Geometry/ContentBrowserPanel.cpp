#include "pch.h"
#include "ContentBrowserPanel.h"
#include "ViewPortPanel.h"
#include "ImGuIRenderClass.h"
#include "Core.h"
#include "ResMgrClass.h"
#include "TextureClass.h"

extern IMGUI_WINDOW_TYPE g_focusedWindow;
extern IMGUI_WINDOW_TYPE g_iCurMousePosWindowID;

wstring g_resourceRelativePath = L"";
RESOURCE_TYPE g_rosourceType = RESOURCE_TYPE::END;

// project root directory
string g_contentsPath = "contents";

ContentBrowserPanel::ContentBrowserPanel()
{
	m_type == IMGUI_WINDOW_TYPE::CONTENTBROWSER;

	// folder in Output Directory 
	m_CurrentDirectory = g_contentsPath;

	m_folderImage = ImGuIRenderClass::GetTexture(L"default\\texture\\folder_icon.png");
	m_folder_open_img = ImGuIRenderClass::GetTexture(L"default\\texture\\folder_open.png");
	m_open_button_img = ImGuIRenderClass::GetTexture(L"default\\texture\\open_button.png");
	m_open_hover_button_img = ImGuIRenderClass::GetTexture(L"default\\texture\\open_hover_button.png");
	m_close_button_img = ImGuIRenderClass::GetTexture(L"default\\texture\\close_button.png");
	m_close_hover_button_img = ImGuIRenderClass::GetTexture(L"default\\texture\\close_hover_button.png");
	m_folder_close_img = ImGuIRenderClass::GetTexture(L"default\\texture\\folder_close.png");
	m_search_directory_button_img = ImGuIRenderClass::GetTexture(L"default\\texture\\search_directory_button_img.png");
	m_search_directory_cancel_button_img = ImGuIRenderClass::GetTexture(L"default\\texture\\search_directory_cancel_button_img.png");


	m_fileImage = ImGuIRenderClass::GetTexture(L"default\\texture\\file_icon.png");
	m_fbxFileImage = ImGuIRenderClass::GetTexture(L"default\\texture\\file_icon_fbx.png");
	m_openedDir.push_back(g_contentsPath);
}

ContentBrowserPanel::~ContentBrowserPanel()
{
}

void ContentBrowserPanel::Shutdown()
{
}

void ContentBrowserPanel::Render()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));   // padding 값 설정
	if (ImGui::Begin("Content Browser"))
	{

		if (IMGUI_WINDOW_FOCUS_CHECK)
		{
			g_iCurMousePosWindowID = IMGUI_WINDOW_TYPE::SCENE;
			if (ImGui::IsMouseClicked(0))
				g_focusedWindow = IMGUI_WINDOW_TYPE::SCENE;
		}

		ShowContents();

	}
	ImGui::End();
	ImGui::PopStyleVar();   // padding 값 설정s
}

void ContentBrowserPanel::ShowContents()
{
	int padding = 10.0f;

	// Top
	{
		ImGui::BeginGroup();
		ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 2.0f);
		ImVec2 postion = ImGui::GetCursorPos();
		ImGui::SetCursorPos(ImVec2(postion.x + padding, postion.y + padding));
		ImGui::BeginChild("ContentsBrowserTopPanel", ImVec2(ImGui::GetWindowSize().x - 2 * padding, 30.0f), true);

		string currentPath = m_CurrentDirectory.string();
		vector<string> directoryNames;
		char* fileName = (char*)currentPath.c_str();
		char* token = strtok(fileName, "\\");
		while (token != NULL) {
			directoryNames.push_back(token);
			token = strtok(NULL, "\\");
		}

		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5.0f);
		for (int i = 0; i < directoryNames.size(); i++)
		{
			if (ImGui::Button(directoryNames[i].c_str()))
			{
				string path = "";
				for (int j = 0; j <= i; j++)
				{
					if (j == i)
						path += directoryNames[j];
					else
						path += directoryNames[j] + "\\";
				}

				m_CurrentDirectory = path;
				break;
			}
			ImGui::SameLine();
		}

		ImGui::EndChild();
		ImGui::PopStyleVar();
		ImGui::EndGroup();
	}

	//Left
	{
		ImGui::BeginGroup();
		ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 2.0f);
		ImVec2 postion = ImGui::GetCursorPos();
		ImGui::SetCursorPos(ImVec2(postion.x + padding, postion.y + padding));
		ImGui::BeginChild("ContentsBrowserLeftPanel", ImVec2(200, -ImGui::GetFrameHeightWithSpacing() + padding), true);

		ImGui::SetCursorPos(ImVec2(10.0f, 10.0f));
		static char buffer[255] = "";
		static string searchStr = "";

		ID3D11ShaderResourceView* searchButton;
		ImGui::SetItemAllowOverlap();
		ImGui::SetNextItemWidth(180.0f);
		if (ImGui::InputTextWithHint("##DirectorySearch", "Search Directory", buffer, sizeof(buffer)))
			searchStr = buffer;
		ImGui::SameLine();
		ImGui::SetItemAllowOverlap();
		ImGui::SetCursorPosX(180.f - ImGui::GetFontSize());
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
		searchButton = strcmp(buffer, "") == 0 ? m_search_directory_button_img : m_search_directory_cancel_button_img;
		if (ImGui::ImageButton((ImTextureID)searchButton, ImVec2(ImGui::GetFontSize(), ImGui::GetFontSize())))
		{
			if (searchButton == m_search_directory_button_img)
				searchStr = buffer;
			else if (searchButton == m_search_directory_cancel_button_img)
			{
				strcpy(buffer, "");
				searchStr = "";
			}
		}
		ImGui::PopStyleColor();
		if (ImGui::BeginTable("directorytable", 1))
		{
			ShowDirectoryTree(g_contentsPath, searchStr);
			ImGui::EndTable();
		}
		ImGui::EndChild();
		ImGui::PopStyleVar();
		ImGui::EndGroup();
	}
	ImGui::SameLine();
	// Right
	{
		ImGui::BeginGroup();
		ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 2.0f);
		ImVec2 postion = ImGui::GetCursorPos();
		ImGui::SetCursorPos(ImVec2(postion.x + 10.f, postion.y + 10.f));
		ImGui::BeginChild("ContentsBrowserRightPanel", ImVec2(-padding, -ImGui::GetFrameHeightWithSpacing() + padding), true);
		// C++ 17부터 제공되는 Directory를 순회할 수 있는 filesystem를 사용 
		if (m_CurrentDirectory != std::filesystem::path(g_contentsPath))
		{
			// move to parent directory 
			if (ImGui::Button("<-"))
			{
				string path = m_CurrentDirectory.string();
				for (int i = path.length() - 1; i >= 0; i--)
				{
					if (path[i] == '\\')
					{
						path = path.substr(0, i);
						m_CurrentDirectory = path;
						break;
					}
				}
			}
		}

		static float padding = 8.0f;
		static float thumbnailSize = 100.0f;
		float cellSize = thumbnailSize + padding;

		float panelWidth = ImGui::GetContentRegionAvail().x;
		int columnCount = (int)panelWidth / cellSize;

		ImGui::Columns(columnCount, 0, false);
		int id = 0;
		for (auto& directoryEntry : std::filesystem::directory_iterator(m_CurrentDirectory))
		{
			const auto& path = directoryEntry.path();
			auto relativePath = std::filesystem::relative(path, g_contentsPath);
			string relativePathString = relativePath.string();
			string fileNameString = relativePathString;
			for (int i = relativePathString.length() - 1; i >= 0; i--)
			{
				if (relativePathString[i] == '\\')
				{
					fileNameString = relativePathString.substr(i + 1, relativePathString.length() - 1);
					break;
				}
			}

			if (directoryEntry.is_directory())  // if path is directory
			{
				ImGui::PushID(id);
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
				if (ImGui::ImageButton((ImTextureID)m_folderImage, ImVec2(thumbnailSize, thumbnailSize)))
				{
					// if button click, change current driectory
					m_CurrentDirectory += "\\";
					m_CurrentDirectory += directoryEntry.path().filename().string();
				}
				ImGui::PopStyleColor();
				ImGui::PopID();
				CenterTextWrapped(fileNameString.c_str(), thumbnailSize);
				id++;
			}
			else  // if path is not directory
			{
				ID3D11ShaderResourceView* fileImg;
				SetFileImage(path.c_str(), &fileImg);

				ImGui::PushID(id);
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));

				ImGui::ImageButton((ImTextureID)fileImg, ImVec2(thumbnailSize, thumbnailSize));
				if (ImGui::BeginDragDropSource())
				{
					ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", path.c_str(), (wcslen(path.c_str()) + 1) * sizeof(wchar_t));
					ImGui::Text(fileNameString.c_str());
					ImGui::EndDragDropSource();
				}
				ImGui::PopStyleColor();
				ImGui::PopID();
				CenterTextWrapped(fileNameString.c_str(), thumbnailSize);
				id++;
			}

			ImGui::NextColumn();
		}
		ImGui::EndChild();
		ImGui::PopStyleVar();
		ImGui::EndGroup();
	}
}

void ContentBrowserPanel::ShowDirectoryTree(std::filesystem::path path, string searchStr)
{
	ID3D11ShaderResourceView* buttonImg;
	ID3D11ShaderResourceView* folderImg;

	static std::filesystem::path selectedDirPath;

	bool isOpenDirectory = false;

	int searchTextIdx = -1;

	if (SearchDiectory(path, searchStr, searchTextIdx))
	{
		// item insert to column
		int indent_cnt = 0;
		int indent_size = 10.0f;
		for (int i = 0; i < path.string().size(); i++)
		{
			if (path.string()[i] == '\\')
				indent_cnt++;
		}
		ImGui::TableNextColumn();
		ImGui::Dummy(ImVec2(indent_cnt * indent_size, 0.0f));
		ImGui::SameLine();

		folderImg = m_folder_close_img;
		buttonImg = m_close_button_img;

		bool isHasDir = false;
		for (auto& iter : std::filesystem::directory_iterator(path))
		{
			if (iter.is_directory())
			{
				isHasDir = true;
				break;
			}
		}
		if (isHasDir)  // if child directory exist
		{
			ImVec2 buttonSize = ImVec2(ImGui::GetFontSize() / 1.8f, ImGui::GetFontSize() / 1.8f);
			ImVec2 buttonPos = ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY());
			for (int i = 0; i < m_openedDir.size(); i++)
			{
				if (m_openedDir[i] == path)
				{
					folderImg = m_folder_open_img;
					buttonImg = m_open_button_img;
					break;
				}
			}
			ImGui::SetCursorPos(buttonPos);
			ImGui::Dummy(buttonSize);
			ImGui::SameLine();
			ImGui::SetItemAllowOverlap();  // dummy is overlaped with button to check hovering on button
			if (ImGui::IsItemHovered())
			{
				if (buttonImg == m_open_button_img)
					buttonImg = m_open_hover_button_img;
				else if (buttonImg == m_close_button_img)
					buttonImg = m_close_hover_button_img;

				if (ImGui::IsItemClicked(0))
				{
					isOpenDirectory = true;  // arrow clicked
					m_CurrentDirectory = path;
				}
			}
			ImGui::SetCursorPos(buttonPos);
			ImGui::Image((ImTextureID)buttonImg, buttonSize);
			ImGui::SetItemAllowOverlap();
			ImGui::SameLine();
		}
		else
		{
			ImGui::Dummy(ImVec2(ImGui::GetFontSize() / 1.8f, 0.0f));
			ImGui::SameLine();
		}

		ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() - 3.0f, ImGui::GetCursorPosY()));
		ImGui::Image((ImTextureID)folderImg, ImVec2(ImGui::GetFontSize(), ImGui::GetFontSize()));
		ImGui::SameLine();
		if (searchTextIdx != -1)  // 검색시
		{
			ImVec2 textPos = ImGui::CalcTextSize(path.filename().string().substr(0, searchTextIdx).c_str());
			ImVec2 textSize = ImGui::CalcTextSize(searchStr.c_str());
			ImGui::SetItemAllowOverlap();
			ImVec2 searchMin = ImVec2(ImGui::GetCursorScreenPos().x + textPos.x, ImGui::GetCursorScreenPos().y);
			ImVec2 searchMax = ImVec2(searchMin.x + textSize.x, searchMin.y + ImGui::GetFontSize());
			ImGui::GetForegroundDrawList()->AddRect(searchMin, searchMax, IM_COL32(255, 255, 0, 100));
			ImGui::GetForegroundDrawList()->AddRectFilled(searchMin, searchMax, IM_COL32(255, 255, 0, 100));
			ImGui::SameLine();
			ImGui::SetItemAllowOverlap();
		}
		if (ImGui::Selectable(path.filename().string().c_str(), m_CurrentDirectory.filename().string() == path.filename().string(), ImGuiSelectableFlags_SpanAllColumns))
		{
			selectedDirPath = path;
			m_CurrentDirectory = path;
		}
		if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
			isOpenDirectory = true;  // double clicked
		//

		// double click -> open folder or arrow clicked
		if (isOpenDirectory)
		{
			bool isOpened = false;
			for (int j = 0; j < m_openedDir.size(); j++)
			{
				if (m_openedDir[j] == selectedDirPath)
				{
					// 이미 열려있는 폴더일때는 닫아준다.
					m_openedDir.erase(m_openedDir.begin() + j);
					isOpened = true;
					break;
				}
			}
			if (!isOpened)
				m_openedDir.push_back(selectedDirPath);

		}

		// directory가 오픈되어있으면 다시 한번 더 순회
		for (int i = 0; i < m_openedDir.size(); i++)
		{
			if (m_openedDir[i] == path.string())
			{
				for (auto& iter : std::filesystem::directory_iterator(path))
				{
					if (iter.is_directory())
					{
						std::filesystem::path newPath = iter.path();

						ShowDirectoryTree(newPath, searchStr);
					}
				}
				break;
			}
		}
	}

}

bool ContentBrowserPanel::SearchDiectory(std::filesystem::path path, string searchStr, int& idx)
{
	std::transform(searchStr.begin(), searchStr.end(), searchStr.begin(), ::tolower);

	bool result = false;

	static vector<string> searchLog;  // 검색 로그 
	static bool searchEnd = false;

	// 검색 종료시 로그를 정리해주는 코드 
	// 검색할때 열어줬던 Directory를 로그와 비교해 닫아준다.
	// 단 CurrentDirectory의 path는 닫아주지 않는다.
	if (!searchEnd && searchLog.size() != 0)
	{
		string currentPath = m_CurrentDirectory.string();
		vector<string> directoryNames;
		char* fileName = (char*)currentPath.c_str();
		char* token = strtok(fileName, "\\");
		while (token != NULL) {
			directoryNames.push_back(token);
			token = strtok(NULL, "\\");
		}

		for (int i = 0; i < searchLog.size(); i++)
		{
			for (int j = 0; j < m_openedDir.size(); j++)
			{
				if (searchLog[i] == m_openedDir[j])
				{
					bool IsDelete = true;
					for (int k = 0; k < directoryNames.size(); k++)
					{
						if (directoryNames[k] == m_openedDir[j].filename().string())
							IsDelete = false;
					}
					if (IsDelete)
						m_openedDir.erase(m_openedDir.begin() + j);
				}
			}
		}

		for (int i = searchLog.size() - 1; i >= 0; i--)
		{
			searchLog.erase(searchLog.begin() + i);
		}
	}

	// 해당 path가 directory list에 출력될지를 결정하는 함수다.
	if (searchStr == "")
	{
		searchEnd = false;  // 검색 종료
		return true;
	}
	else
		searchEnd = true; // 검색 시작

	// 자식들을 순회
	for (auto& iter : std::filesystem::recursive_directory_iterator(path))
	{
		if (iter.is_directory())
		{
			string fileName = iter.path().filename().string();
			std::transform(fileName.begin(), fileName.end(), fileName.begin(), ::tolower);
			if (fileName.find(searchStr) != string::npos)
			{
				// 자식들에서 검색이 성공하면 부모 폴더를 열어준다.
				bool isOpened = true;
				for (int i = 0; i < m_openedDir.size(); i++)
				{
					if (path == m_openedDir[i])
						break;

					if (i >= m_openedDir.size() - 1)
						isOpened = false;
				}
				if (!isOpened)
				{
					searchLog.push_back(path.string());  // 나중에 검색이 끝나면 닫아줘야하기때문에 로그를 기록한다.
					m_openedDir.push_back(path);
				}
				result = true;
			}
		}
	}

	string fileName = path.filename().string();
	std::transform(fileName.begin(), fileName.end(), fileName.begin(), ::tolower);
	if (fileName.find(searchStr) != string::npos) // path 자신이 serachStr을 포함하고 있을때
	{
		idx = fileName.find(searchStr);
		result = true;
	}

	return result;
}

// file image을 결정
void ContentBrowserPanel::SetFileImage(wstring filePath, ID3D11ShaderResourceView** view)
{
	wstring fileExtension = Core::GetFileExtension(filePath);

	for (int i = 0; i < IM_ARRAYSIZE(g_modelExtension); i++)
	{
		if (fileExtension == g_modelExtension[i])
		{
			*view = m_fbxFileImage;
			return;
		}
	}

	for (int i = 0; i < IM_ARRAYSIZE(g_textureExtension); i++)
	{
		if (fileExtension == g_textureExtension[i])
		{
			TextureClass* texture = ResMgrClass::GetInst()->LoadTexture(Core::GetDevice(), Core::GetFileName(filePath), filePath);
			*view = texture->GetTexture();
			return;
		}
	}

	*view = m_fileImage;
	return;
}

void ContentBrowserPanel::CenterTextWrapped(string str, float width)
{
	float font_size;
	int line_font_cnt = ((int)width / ImGui::GetFontSize()) * 2;  // line안 최대 폰트수 
	if (strlen(str.c_str() - 1) < line_font_cnt)
		font_size = strlen(str.c_str() - 1) * ImGui::GetFontSize();
	else
		font_size = line_font_cnt * ImGui::GetFontSize();

	ImGui::SetCursorPosX(ImGui::GetCursorPosX() + width / 2 - (font_size / 4));
	ImVec2 size = ImGui::GetItemRectSize();
	ImGui::TextWrapped(str.c_str());
}
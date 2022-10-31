#pragma once
#include "Panel.h"

class ContentBrowserPanel : public Panel
{
private:
    // Once we have projects, change this
    std::filesystem::path m_CurrentDirectory;

    vector<std::filesystem::path> m_openedDir;

    ID3D11ShaderResourceView* m_folderImage;
    ID3D11ShaderResourceView* m_folder_open_img;
    ID3D11ShaderResourceView* m_open_button_img;
    ID3D11ShaderResourceView* m_open_hover_button_img;
    ID3D11ShaderResourceView* m_close_button_img;
    ID3D11ShaderResourceView* m_close_hover_button_img;
    ID3D11ShaderResourceView* m_folder_close_img;
    ID3D11ShaderResourceView* m_fileImage;
    ID3D11ShaderResourceView* m_fbxFileImage;
    ID3D11ShaderResourceView* m_search_directory_button_img;
    ID3D11ShaderResourceView* m_search_directory_cancel_button_img;

public:
    ContentBrowserPanel();
    ~ContentBrowserPanel();
public:
    virtual void Shutdown();
    virtual void Render();

    void ShowContents();
    void ShowDirectoryTree(std::filesystem::path path, string searchStr);
    bool SearchDiectory(std::filesystem::path path, string searchStr, int& idx);

    void SetFileImage(wstring filePath, ID3D11ShaderResourceView** view);

    void CenterTextWrapped(string str, float width);
};


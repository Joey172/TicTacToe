#include "ttt_util.h"

Tile IncrementTurn(Tile tile, unsigned numPlayers)
{
  tile = Tile(1 + (tile % numPlayers));
  return tile;
}

Tile DecrementTurn(Tile tile, unsigned numPlayers)
{
  tile = Tile((tile - 2 + numPlayers) % numPlayers + 1);
  return tile;
}

#include <windows.h>
#include <shobjidl.h>
#include <string>

std::string WindowsOnlyOpenFileDialog() {
  HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED |
    COINIT_DISABLE_OLE1DDE);
  PWSTR pszFilePath;
  std::string ret = "";
  if (SUCCEEDED(hr))
  {
    IFileOpenDialog* pFileOpen;

    // Create the FileOpenDialog object.
    hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL,
      IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

    if (SUCCEEDED(hr))
    {
      // Show the Open dialog box.
      hr = pFileOpen->Show(NULL);

      // Get the file name from the dialog box.
      if (SUCCEEDED(hr))
      {
        IShellItem* pItem;
        hr = pFileOpen->GetResult(&pItem);
        if (SUCCEEDED(hr))
        {

          hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

          // Display the file name to the user.
          if (SUCCEEDED(hr))
          {
            std::wstring ws(pszFilePath);
            ret = std::string(ws.begin(), ws.end());
            //MessageBox(NULL, pszFilePath, L"File Path", MB_OK);
            CoTaskMemFree(pszFilePath);
          }
          pItem->Release();
        }
      }
      pFileOpen->Release();
    }
    CoUninitialize();
  }
  return ret;
}


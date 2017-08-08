#include <fstream>
#include <metahost.h>
#pragma comment(lib, "mscoree.lib")

using namespace std;

HMODULE hModule;

//��ȡDllĿ¼
wstring GetDllDir()
{
	wchar_t chrPath[MAX_PATH];
	wstring path;

	GetModuleFileName(hModule, chrPath, sizeof(chrPath));
	//��ȡDInvokerX.dll��·��
	path = (wstring)chrPath;
	//ת�����ַ���
	path.replace(path.rfind(L"\\") + 1, path.length(), L"");
	//������DInvokerX.dll�滻Ϊ��
	return path;
}

//��ȡCLR�汾
wstring GetCLRVer()
{
	wchar_t chrPath[MAX_PATH];
	wstring dllName;

	GetModuleFileName(hModule, chrPath, sizeof(chrPath));
	//��ȡDInvokerX.dll��·��
	dllName = (wstring)chrPath;
	dllName = dllName.substr(dllName.rfind(L"\\") + 1);
	if (dllName == L"DInvoker4.dll")
		return L"v4.0.30319";
	else if (dllName == L"DInvoker2.dll")
		return L"v2.0.50727";
	else
		return L"v4.0.30319";
}

//����CLR
DWORD WINAPI LoadCLR(LPVOID lpThreadParameter)
{
	ICLRMetaHost *pMetaHost = nullptr;
	ICLRMetaHostPolicy *pMetaHostPolicy = nullptr;
	ICLRRuntimeHost *pRuntimeHost = nullptr;
	ICLRRuntimeInfo *pRuntimeInfo = nullptr;
	HRESULT result;

	result = CLRCreateInstance(CLSID_CLRMetaHost, IID_PPV_ARGS(&pMetaHost));
	result = pMetaHost->GetRuntime(GetCLRVer().data(), IID_PPV_ARGS(&pRuntimeInfo));
	result = pRuntimeInfo->GetInterface(CLSID_CLRRuntimeHost, IID_PPV_ARGS(&pRuntimeHost));
	result = pRuntimeHost->Start();
	result = pRuntimeHost->ExecuteInDefaultAppDomain(GetDllDir().append(L"netdll.dll").data(), L"Injecting.Injector", L"DllMain", nullptr, nullptr);
	return result;
}

BOOL APIENTRY DllMain(HMODULE hModule1, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		hModule = hModule1;
		CreateThread(0, 0, LoadCLR, 0, 0, 0);
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

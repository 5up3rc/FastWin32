#include <fstream>
#include <metahost.h>
#pragma comment(lib, "mscoree.lib")

using namespace std;

//��ȡDllĿ¼
wstring GetDllDir()
{
	HMODULE hModule;
	wchar_t chrPath[MAX_PATH];
	wstring path;
	const size_t lenOfName = sizeof("DInvoker.dll") - 1;

	hModule = GetModuleHandle(L"DInvoker.dll");
	//��ȡDInvoker.dll��ģ����
	GetModuleFileName(hModule, chrPath, sizeof(chrPath));
	//��ȡDInvoker.dll��·��
	path = (wstring)chrPath;
	//ת�����ַ���
	path.replace(path.length() - lenOfName, path.length(), L"");
	//������DInvoker.dll�滻Ϊ��
	return path;
}

//��ȡCLR�汾
wstring GetCLRVer()
{
	ifstream ifs;
	wstring result;

	ifs.open(GetDllDir().append(L"clrver"));
	//���ļ���
	result = wstring((istreambuf_iterator<char>(ifs)), istreambuf_iterator<char>());
	//��ȡ�����ַ�
	ifs.close();
	//�ر���
	return result;
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
	return pRuntimeHost->ExecuteInDefaultAppDomain(GetDllDir().append(L"netdll.dll").data(), L"Injecting.Injector", L"DllMain", nullptr, nullptr);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		//wchar_t chrPath[MAX_PATH];

		//GetModuleFileName(hModule, chrPath, sizeof(chrPath));
		////��ȡDInvoker.dll��·��
		//MessageBox(0, wstring(chrPath).data(), L"DEBUG", 0);
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

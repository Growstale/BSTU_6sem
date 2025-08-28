#include <windows.h>
#include <combaseapi.h> 
#include <strsafe.h>
#include <stdio.h>
#include <initguid.h>   // Для определения GUIDов (должен быть перед заголовочными файлами с GUIDами)
#include "IComTest_h.h" // IComTest и IVAVTest
#include <ComSampleServerGuids.h> 
#include <ComSampleServiceGuids.h> 
#include <winerror.h> 

HRESULT Execute(_In_ const IID& rclsid, _In_ DWORD dwCoInit, _In_ DWORD dwClsContext)
{
    HRESULT hr = CoInitializeEx(NULL, dwCoInit);
    if (SUCCEEDED(hr))
    {
        IComTest* pComTest = nullptr; 
        IVAVTest* pVAVTest = nullptr; 

        hr = CoCreateInstance(rclsid,               
            NULL, 
            dwClsContext,       
            IID_PPV_ARGS(&pComTest)
        );

        if (SUCCEEDED(hr))
        {
            LPWSTR pwszWhoAmI = nullptr;
            HRESULT hrWhoAmI = pComTest->WhoAmI(&pwszWhoAmI);
            if (SUCCEEDED(hrWhoAmI))
            {
                wprintf(L"%s. Client calling from %s. COM Server running %s.\n",
                    pwszWhoAmI,
                    (dwCoInit == COINIT_MULTITHREADED) ? L"MTA" : L"STA",
                    (dwClsContext == CLSCTX_INPROC_SERVER) ? L"in-process" : L"out-of-process");
                CoTaskMemFree(pwszWhoAmI)
            }
            else
            {
                wprintf(L"Error: WhoAmI failed with HRESULT=0x%08X\n", hrWhoAmI);
            }

            HRESULT hrQI = pComTest->QueryInterface(IID_IVAVTest, (void**)&pVAVTest);
            if (SUCCEEDED(hrQI))
            {
                LONG lNumberToSum = 789; 
                LONG lCalculatedSum = 0;

                HRESULT hrCalc = pVAVTest->CalculateSumOfDigits(lNumberToSum, &lCalculatedSum);
                if (SUCCEEDED(hrCalc))
                {
                    wprintf(L" -> CalculateSumOfDigits(%ld) returned sum: %ld\n",
                        lNumberToSum, lCalculatedSum);
                }
                else
                {
                    wprintf(L" -> CalculateSumOfDigits failed with HRESULT=0x%08X\n", hrCalc);
                }

                pVAVTest->Release();
                pVAVTest = nullptr; 
            }
            else
            {
                wprintf(L" -> Error: QueryInterface for IVAVTest failed with HRESULT=0x%08X\n", hrQI);
            }

            pComTest->Release();
            pComTest = nullptr;
        }
        else 
        {
            wprintf(L"Error: CoCreateInstance failed for %s with HRESULT=0x%08X\n",
                (dwClsContext == CLSCTX_INPROC_SERVER) ? L"in-process" : L"out-of-process",
                hr);
        }

        CoUninitialize();
    }
    else 
    {
        wprintf(L"Error: CoInitializeEx failed with HRESULT=0x%08X\n", hr);
    }

    wprintf(L"----------------------------------------------------\n");
    return hr; 
}

int __cdecl main()
{
    Execute(CLSID_CComServerTest, COINIT_MULTITHREADED, CLSCTX_INPROC_SERVER);
    Execute(CLSID_CComServerTest, COINIT_MULTITHREADED, CLSCTX_LOCAL_SERVER); 
    Execute(CLSID_CComServerTest, COINIT_APARTMENTTHREADED, CLSCTX_INPROC_SERVER);
    Execute(CLSID_CComServerTest, COINIT_APARTMENTTHREADED, CLSCTX_LOCAL_SERVER); 
    Execute(CLSID_CComServiceTest, COINIT_MULTITHREADED, CLSCTX_LOCAL_SERVER); 
    Execute(CLSID_CComServiceTest, COINIT_APARTMENTTHREADED, CLSCTX_LOCAL_SERVER);

    wprintf(L"Client execution finished. Press Enter to exit...\n");
    getchar();

    return 0;
}
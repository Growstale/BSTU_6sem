

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.01.0628 */
/* at Tue Jan 19 06:14:07 2038
 */
/* Compiler settings for D:/6sem_BSTU/SP/Lab4/Lab4/ComSampleProxy/IComTest.idl:
    Oicf, W1, Zp8, env=Win64 (32b run), target_arch=AMD64 8.01.0628 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif /* __RPCNDR_H_VERSION__ */

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __IComTest_h_h__
#define __IComTest_h_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#ifndef DECLSPEC_XFGVIRT
#if defined(_CONTROL_FLOW_GUARD_XFG)
#define DECLSPEC_XFGVIRT(base, func) __declspec(xfg_virtual(base, func))
#else
#define DECLSPEC_XFGVIRT(base, func)
#endif
#endif

/* Forward Declarations */ 

#ifndef __IComTest_FWD_DEFINED__
#define __IComTest_FWD_DEFINED__
typedef interface IComTest IComTest;

#endif 	/* __IComTest_FWD_DEFINED__ */


#ifndef __IVAVTest_FWD_DEFINED__
#define __IVAVTest_FWD_DEFINED__
typedef interface IVAVTest IVAVTest;

#endif 	/* __IVAVTest_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __IComTest_INTERFACE_DEFINED__
#define __IComTest_INTERFACE_DEFINED__

/* interface IComTest */
/* [helpstring][version][uuid][object] */ 


EXTERN_C const IID IID_IComTest;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("db24e16f-aa47-4fae-83bb-9f18ebd1e9bc")
    IComTest : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE WhoAmI( 
            /* [out] */ LPWSTR *pwszWhoAmI) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IComTestVtbl
    {
        BEGIN_INTERFACE
        
        DECLSPEC_XFGVIRT(IUnknown, QueryInterface)
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IComTest * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        DECLSPEC_XFGVIRT(IUnknown, AddRef)
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IComTest * This);
        
        DECLSPEC_XFGVIRT(IUnknown, Release)
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IComTest * This);
        
        DECLSPEC_XFGVIRT(IComTest, WhoAmI)
        HRESULT ( STDMETHODCALLTYPE *WhoAmI )( 
            IComTest * This,
            /* [out] */ LPWSTR *pwszWhoAmI);
        
        END_INTERFACE
    } IComTestVtbl;

    interface IComTest
    {
        CONST_VTBL struct IComTestVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IComTest_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IComTest_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IComTest_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IComTest_WhoAmI(This,pwszWhoAmI)	\
    ( (This)->lpVtbl -> WhoAmI(This,pwszWhoAmI) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IComTest_INTERFACE_DEFINED__ */


#ifndef __IVAVTest_INTERFACE_DEFINED__
#define __IVAVTest_INTERFACE_DEFINED__

/* interface IVAVTest */
/* [helpstring][version][uuid][object] */ 


EXTERN_C const IID IID_IVAVTest;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("704e2ec6-a123-4631-b2a7-6afb30d43a45")
    IVAVTest : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE CalculateSumOfDigits( 
            /* [in] */ LONG lInputNumber,
            /* [out] */ LONG *plSumOfDigits) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IVAVTestVtbl
    {
        BEGIN_INTERFACE
        
        DECLSPEC_XFGVIRT(IUnknown, QueryInterface)
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IVAVTest * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        DECLSPEC_XFGVIRT(IUnknown, AddRef)
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IVAVTest * This);
        
        DECLSPEC_XFGVIRT(IUnknown, Release)
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IVAVTest * This);
        
        DECLSPEC_XFGVIRT(IVAVTest, CalculateSumOfDigits)
        HRESULT ( STDMETHODCALLTYPE *CalculateSumOfDigits )( 
            IVAVTest * This,
            /* [in] */ LONG lInputNumber,
            /* [out] */ LONG *plSumOfDigits);
        
        END_INTERFACE
    } IVAVTestVtbl;

    interface IVAVTest
    {
        CONST_VTBL struct IVAVTestVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IVAVTest_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IVAVTest_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IVAVTest_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IVAVTest_CalculateSumOfDigits(This,lInputNumber,plSumOfDigits)	\
    ( (This)->lpVtbl -> CalculateSumOfDigits(This,lInputNumber,plSumOfDigits) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IVAVTest_INTERFACE_DEFINED__ */


/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif





/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 6.00.0366 */
/* at Tue Mar 03 17:55:43 2009
 */
/* Compiler settings for .\msgdll.idl:
    Oicf, W1, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 440
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __msgdll_h__
#define __msgdll_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __Inagevlogmsg_FWD_DEFINED__
#define __Inagevlogmsg_FWD_DEFINED__
typedef interface Inagevlogmsg Inagevlogmsg;
#endif 	/* __Inagevlogmsg_FWD_DEFINED__ */


#ifndef __nagevlogmsg_FWD_DEFINED__
#define __nagevlogmsg_FWD_DEFINED__

#ifdef __cplusplus
typedef class nagevlogmsg nagevlogmsg;
#else
typedef struct nagevlogmsg nagevlogmsg;
#endif /* __cplusplus */

#endif 	/* __nagevlogmsg_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void * ); 

#ifndef __Inagevlogmsg_INTERFACE_DEFINED__
#define __Inagevlogmsg_INTERFACE_DEFINED__

/* interface Inagevlogmsg */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_Inagevlogmsg;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("5AF9B1DB-B469-4A5D-A18E-5928883ACF8F")
    Inagevlogmsg : public IDispatch
    {
    public:
    };
    
#else 	/* C style interface */

    typedef struct InagevlogmsgVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            Inagevlogmsg * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            Inagevlogmsg * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            Inagevlogmsg * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            Inagevlogmsg * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            Inagevlogmsg * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            Inagevlogmsg * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            Inagevlogmsg * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } InagevlogmsgVtbl;

    interface Inagevlogmsg
    {
        CONST_VTBL struct InagevlogmsgVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define Inagevlogmsg_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define Inagevlogmsg_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define Inagevlogmsg_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define Inagevlogmsg_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define Inagevlogmsg_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define Inagevlogmsg_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define Inagevlogmsg_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __Inagevlogmsg_INTERFACE_DEFINED__ */



#ifndef __msgdllLib_LIBRARY_DEFINED__
#define __msgdllLib_LIBRARY_DEFINED__

/* library msgdllLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_msgdllLib;

EXTERN_C const CLSID CLSID_nagevlogmsg;

#ifdef __cplusplus

class DECLSPEC_UUID("FAA3D89D-43ED-47FC-9D24-30D437C6C0AC")
nagevlogmsg;
#endif
#endif /* __msgdllLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif



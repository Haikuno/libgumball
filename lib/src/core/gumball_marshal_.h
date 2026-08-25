#ifndef GUMBALL_MARSHAL_PRIVATE_H
#define GUMBALL_MARSHAL_PRIVATE_H

#include <gimbal/meta/signals/gimbal_c_closure.h>
#include <gimbal/meta/signals/gimbal_marshal.h>
#include <gimbal/meta/types/gimbal_variant.h>

static GBL_RESULT GUM_Marshal_CClosure_VOID__INSTANCE_POINTER_SIZE_(GblClosure* pClosure,
                                                                    GblVariant* pRetValue,
                                                                    size_t argCount,
                                                                    GblVariant* pArgs,
                                                                    GblPtr marshalData) {
    GBL_UNUSED(pRetValue);

    if (!pClosure || !pArgs)
        return GBL_RESULT_ERROR_INVALID_POINTER;
    if (argCount < 3)
        return GBL_RESULT_ERROR_INVALID_ARG;

    GblFnPtr pFnCallback = marshalData.pFunc;
    if (!pFnCallback)
        pFnCallback = GblCClosure_callback((GblCClosure*)pClosure);
    if (!pFnCallback)
        return GBL_RESULT_ERROR_INVALID_OPERATION;

    typedef void (*GUM_MarshalCallback_)(GblInstance*, void*, size_t);
    ((GUM_MarshalCallback_)pFnCallback)(GblVariant_toPointer(&pArgs[0]),
                                        GblVariant_toPointer(&pArgs[1]),
                                        GblVariant_toSize(&pArgs[2]));
    return GBL_RESULT_SUCCESS;
}

#endif // GUMBALL_MARSHAL_PRIVATE_H

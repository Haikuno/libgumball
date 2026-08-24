#include "core/gumball_manager_test_suite.h"
#include <gimbal/test/gimbal_test_macros.h>
#include <gumball/gumball.h>
#include <gumball/core/gumball_backend.h>

#ifdef _WIN32
#include <direct.h>
#include <stdlib.h>
#include <string.h>
#endif

#define GUM_DEFAULT_FONT_PROBE_TYPE (GBL_TYPEID(GUM_DefaultFontProbe))
#define GUM_DEFAULT_FONT_PROBE(self) (GBL_CAST(GUM_DefaultFontProbe, self))

GBL_FORWARD_DECLARE_STRUCT(GUM_DefaultFontProbe);
GblType GUM_DefaultFontProbe_type(void) GBL_NOEXCEPT;

GBL_CLASS_DERIVE_EMPTY(GUM_DefaultFontProbe, GUM_Font)
GBL_INSTANCE_DERIVE_EMPTY(GUM_DefaultFontProbe, GUM_Font)

static GUM_Font* pRetiringFont_ = nullptr;
static GUM_Font* pObservedFont_ = nullptr;

static GBL_RESULT GUM_DefaultFontProbe_unload_(GUM_IResource* pResource) {
    if (GUM_FONT(pResource) == pRetiringFont_)
        pObservedFont_ = GUM_Font_default();
    return GBL_RESULT_SUCCESS;
}

static GBL_RESULT GUM_DefaultFontProbeClass_init_(GblClass* pClass, const void* pData) {
    GBL_UNUSED(pData);
    GUM_IRESOURCE_CLASS(pClass)->pFnUnload = GUM_DefaultFontProbe_unload_;
    return GBL_RESULT_SUCCESS;
}

GblType GUM_DefaultFontProbe_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    if (type == GBL_INVALID_TYPE) {
        type = GblType_register(GblQuark_internStatic("GUM_DefaultFontProbe"),
                                GUM_FONT_TYPE,
                                &(static GblTypeInfo){
                                    .classSize    = sizeof(GUM_DefaultFontProbeClass),
                                    .instanceSize = sizeof(GUM_DefaultFontProbe),
                                    .pFnClassInit = GUM_DefaultFontProbeClass_init_
                                },
                                GBL_TYPE_FLAG_TYPEINFO_STATIC);
    }

    return type;
}

#define GBL_SELF_TYPE GUM_ManagerTestSuite

GBL_TEST_FIXTURE_NONE
GBL_TEST_INIT_NONE
GBL_TEST_FINAL_NONE

GBL_TEST_CASE(lifecycle)
    GUM_Manager_deinit();
    GBL_TEST_VERIFY(!GblModule_find("GUM_Manager"));

    GUM_IResource* pOut = (GUM_IResource*)1;
    GBL_TEST_COMPARE(GUM_Manager_loadEx(nullptr, &pOut), GBL_RESULT_ERROR_INVALID_ARG);
    GBL_TEST_VERIFY(!pOut);
    GBL_TEST_COMPARE(GUM_Manager_loadEx("koslogo.png", nullptr), GBL_RESULT_ERROR_INVALID_POINTER);
    GBL_TEST_COMPARE(GUM_Manager_setBasePath(nullptr), GBL_RESULT_ERROR_INVALID_ARG);
    GBL_TEST_COMPARE(GUM_Manager_setBasePath(""), GBL_RESULT_ERROR_INVALID_ARG);

    pOut = (GUM_IResource*)1;
    GBL_TEST_COMPARE(GUM_Manager_loadEx("resource.unsupported", &pOut), GBL_RESULT_UNSUPPORTED);
    GBL_TEST_VERIFY(!pOut);

    GUM_Root* pRoot = GUM_Root_create();
    GUM_IResource* pTexture = pRoot ? GUM_Manager_load("koslogo.png") : nullptr;
    GBL_TEST_VERIFY(pRoot && pTexture);

    const GUM_Vector2 size = GUM_Texture_size(GUM_TEXTURE(pTexture));
    GBL_TEST_VERIFY(size.x > 0 && size.y > 0);

    GblModule* pModule = GblModule_find("GUM_Manager");
    GBL_TEST_VERIFY(pModule);
    GBL_TEST_VERIFY(GblType_check(GBL_TYPEOF(pModule), GUM_MANAGER_TYPE));
    GBL_TEST_VERIFY(GblModule_isLoaded(pModule));
    GBL_TEST_COMPARE(GblModule_useCount(pModule), 1u);

    GUM_Manager_deinit();
    GBL_TEST_VERIFY(!GblModule_find("GUM_Manager"));
    GBL_TEST_COMPARE(GUM_Texture_size(GUM_TEXTURE(pTexture)).x, size.x);
    GBL_TEST_COMPARE(GUM_Texture_size(GUM_TEXTURE(pTexture)).y, size.y);
    GUM_Manager_deinit();

    GUM_IResource_unref(pTexture);
    GUM_unref(pRoot);
GBL_TEST_CASE_END

GBL_TEST_CASE(basePath)
    GUM_Manager_deinit();

    GblStringRef* pInitial = GUM_Manager_basePath();
    GBL_TEST_VERIFY(pInitial && pInitial[0]);
    GblStringRef* pInitialCopy = GblStringRef_create(pInitial);
    GBL_TEST_VERIFY(pInitialCopy);

    GUM_Root* pRoot = GUM_Root_create();
    GUM_IResource* pFirst = pRoot ? GUM_Manager_load("koslogo.png") : nullptr;
    GBL_TEST_VERIFY(pRoot && pFirst);

    GBL_TEST_CALL(GUM_Manager_setBasePath(".."));
    GBL_TEST_VERIFY(GUM_Manager_basePath());
    GBL_TEST_VERIFY(strcmp(GUM_Manager_basePath(), pInitialCopy) != 0);

    GBL_TEST_CALL(GUM_Manager_setBasePath(pInitialCopy));
    GBL_TEST_COMPARE(strcmp(GUM_Manager_basePath(), pInitialCopy), 0);

    GUM_IResource* pSecond = GUM_Manager_load("koslogo.png");
    GBL_TEST_COMPARE(pSecond, pFirst);

    GUM_IResource_unref(pFirst);
    GUM_IResource_unref(pSecond);
    GblStringRef_unref(pInitialCopy);
    GUM_unref(pRoot);
GBL_TEST_CASE_END

GBL_TEST_CASE(resourceType)
    GBL_TEST_COMPARE(GUM_Backend_resourceType("koslogo.png"), GUM_TEXTURE_TYPE);
    GBL_TEST_COMPARE(GUM_Backend_resourceType("FONT.TTF"), GUM_FONT_TYPE);
    GBL_TEST_COMPARE(GUM_Backend_resourceType("resource.unsupported"), GBL_INVALID_TYPE);
    GBL_TEST_COMPARE(GUM_Backend_resourceType(nullptr), GBL_INVALID_TYPE);
GBL_TEST_CASE_END

GBL_TEST_CASE(cache)
    GUM_Root* pRoot = GUM_Root_create();
    GUM_IResource* pFirst  = pRoot ? GUM_Manager_load("koslogo.png") : nullptr;
    GUM_IResource* pSecond = pRoot ? GUM_Manager_load("./koslogo.png") : nullptr;
    GUM_IResource* pThird  = pRoot ? GUM_Manager_load("unused/../koslogo.png") : nullptr;
    GBL_TEST_VERIFY(pRoot && pFirst && pSecond && pThird);

    GBL_TEST_COMPARE(pFirst, pSecond);
    GBL_TEST_COMPARE(pSecond, pThird);
    GBL_TEST_COMPARE(GblBox_refCount(GBL_BOX(pFirst)), 4u);

    const GUM_Vector2 size = GUM_Texture_size(GUM_TEXTURE(pFirst));
    GBL_TEST_CALL(GUM_Manager_evict(pFirst));
    GBL_TEST_COMPARE(GUM_Manager_evict(pFirst), GBL_RESULT_NOT_FOUND);
    GBL_TEST_COMPARE(GUM_Texture_size(GUM_TEXTURE(pFirst)).x, size.x);
    GBL_TEST_COMPARE(GUM_Texture_size(GUM_TEXTURE(pFirst)).y, size.y);
    GBL_TEST_COMPARE(GblBox_refCount(GBL_BOX(pFirst)), 3u);

    GUM_IResource* pFresh = GUM_Manager_load("koslogo.png");
    GBL_TEST_VERIFY(pFresh && pFresh != pFirst);

    GUM_IResource_unref(pFirst);
    GUM_IResource_unref(pSecond);
    GUM_IResource_unref(pThird);
    GUM_IResource_unref(pFresh);
    GUM_unref(pRoot);
GBL_TEST_CASE_END

GBL_TEST_CASE(restart)
    GUM_Root* pRoot = GUM_Root_create();
    GUM_IResource* pOld = pRoot ? GUM_Manager_load("koslogo.png") : nullptr;
    GBL_TEST_VERIFY(pRoot && pOld);

    const GUM_Vector2 size = GUM_Texture_size(GUM_TEXTURE(pOld));
    GUM_Manager_deinit();
    GBL_TEST_COMPARE(GUM_Texture_size(GUM_TEXTURE(pOld)).x, size.x);
    GBL_TEST_COMPARE(GUM_Texture_size(GUM_TEXTURE(pOld)).y, size.y);

    GUM_IResource* pFresh = GUM_Manager_load("koslogo.png");
    GBL_TEST_VERIFY(pFresh && pFresh != pOld);
    GBL_TEST_COMPARE(GUM_Texture_size(GUM_TEXTURE(pOld)).x, size.x);
    GBL_TEST_COMPARE(GUM_Texture_size(GUM_TEXTURE(pOld)).y, size.y);

    GUM_IResource_unref(pOld);
    GUM_IResource_unref(pFresh);
    GUM_unref(pRoot);
GBL_TEST_CASE_END

GBL_TEST_CASE(defaultFont)
    GUM_Font* pFirst  = GUM_FONT(GblBox_create(GUM_FONT_TYPE));
    GUM_Font* pSecond = GUM_FONT(GblBox_create(GUM_FONT_TYPE));
    GBL_TEST_VERIFY(pFirst && pSecond);

    GUM_Font_setDefault(pFirst);
    GBL_TEST_COMPARE(GblBox_refCount(GBL_BOX(pFirst)), 2u);
    GUM_Font_setDefault(pSecond);
    GBL_TEST_COMPARE(GblBox_refCount(GBL_BOX(pFirst)), 1u);
    GBL_TEST_COMPARE(GblBox_refCount(GBL_BOX(pSecond)), 2u);
    GUM_Font_setDefault(pSecond);
    GBL_TEST_COMPARE(GblBox_refCount(GBL_BOX(pSecond)), 2u);

    GUM_IResource_unref(GUM_IRESOURCE(pFirst));
    GUM_IResource_unref(GUM_IRESOURCE(pSecond));
    GBL_TEST_COMPARE(GUM_Font_default(), pSecond);
    GBL_TEST_COMPARE(GblBox_refCount(GBL_BOX(pSecond)), 1u);
    GUM_Font_setDefault(nullptr);
    GBL_TEST_VERIFY(!GUM_Font_default());

    pObservedFont_ = nullptr;
    GUM_DefaultFontProbe* pRetiring = GUM_DEFAULT_FONT_PROBE(GblBox_create(GUM_DEFAULT_FONT_PROBE_TYPE));
    GUM_DefaultFontProbe* pReplacement = GUM_DEFAULT_FONT_PROBE(GblBox_create(GUM_DEFAULT_FONT_PROBE_TYPE));
    GBL_TEST_VERIFY(pRetiring && pReplacement);

    pRetiringFont_ = GUM_FONT(pRetiring);
    GUM_Font_setDefault(GUM_FONT(pRetiring));
    GUM_IResource_unref(GUM_IRESOURCE(pRetiring));

    GUM_Font_setDefault(GUM_FONT(pReplacement));
    GBL_TEST_COMPARE(pObservedFont_, GUM_FONT(pReplacement));
    GBL_TEST_COMPARE(GUM_Font_default(), GUM_FONT(pReplacement));

    pRetiringFont_ = nullptr;
    GUM_Font_setDefault(nullptr);
    GUM_IResource_unref(GUM_IRESOURCE(pReplacement));
GBL_TEST_CASE_END

GBL_TEST_CASE(rootRestart)
    GUM_Root* pRoot = GUM_Root_create();
    GUM_IResource* pTexture = pRoot ? GUM_Manager_load("koslogo.png") : nullptr;
    GBL_TEST_VERIFY(pRoot && pTexture);

    const GUM_Vector2 size = GUM_Texture_size(GUM_TEXTURE(pTexture));
    GUM_unref(pRoot);
    GBL_TEST_COMPARE(GUM_Texture_size(GUM_TEXTURE(pTexture)).x, size.x);
    GBL_TEST_COMPARE(GUM_Texture_size(GUM_TEXTURE(pTexture)).y, size.y);

    pRoot = GUM_Root_create();
    GBL_TEST_VERIFY(pRoot);
    GBL_TEST_COMPARE(GUM_Texture_size(GUM_TEXTURE(pTexture)).x, size.x);
    GBL_TEST_COMPARE(GUM_Texture_size(GUM_TEXTURE(pTexture)).y, size.y);

    GUM_IResource_unref(pTexture);
    GUM_unref(pRoot);
GBL_TEST_CASE_END

#ifdef _WIN32
GBL_TEST_CASE(windowsPath)
    char* pOriginalCwd = _getcwd(nullptr, 0);
    char* pAbsolute = _fullpath(nullptr, "koslogo.png", 0);
    GUM_Root* pRoot = GUM_Root_create();
    GUM_IResource* pRelative = pRoot ? GUM_Manager_load("koslogo.png") : nullptr;
    GUM_IResource* pQualified = pAbsolute ? GUM_Manager_load(pAbsolute) : nullptr;
    GBL_TEST_VERIFY(pOriginalCwd && pAbsolute && pRoot && pRelative && pQualified);
    GBL_TEST_COMPARE(pRelative, pQualified);

    GBL_TEST_VERIFY(_chdir("..") == 0);
    GUM_IResource* pAfterCwdChange = GUM_Manager_load("koslogo.png");
    GBL_TEST_COMPARE(pAfterCwdChange, pRelative);

    GUM_IResource* pPartial = (GUM_IResource*)1;
    GBL_TEST_COMPARE(GUM_Manager_loadEx("\\koslogo.png", &pPartial), GBL_RESULT_UNSUPPORTED);
    GBL_TEST_VERIFY(!pPartial);

    char driveRelative[] = "C:koslogo.png";
    if (strlen(pOriginalCwd) >= 2 && pOriginalCwd[1] == ':')
        driveRelative[0] = pOriginalCwd[0];

    pPartial = (GUM_IResource*)1;
    GBL_TEST_COMPARE(GUM_Manager_loadEx(driveRelative, &pPartial), GBL_RESULT_UNSUPPORTED);
    GBL_TEST_VERIFY(!pPartial);

    GBL_TEST_VERIFY(_chdir(pOriginalCwd) == 0);
    GUM_IResource_unref(pRelative);
    GUM_IResource_unref(pQualified);
    GUM_IResource_unref(pAfterCwdChange);
    GUM_unref(pRoot);
    free(pAbsolute);
    free(pOriginalCwd);
GBL_TEST_CASE_END
#endif

#ifdef _WIN32
GBL_TEST_REGISTER(lifecycle,
                  basePath,
                  resourceType,
                  cache,
                  restart,
                  defaultFont,
                  rootRestart,
                  windowsPath)
#else
GBL_TEST_REGISTER(lifecycle,
                  basePath,
                  resourceType,
                  cache,
                  restart,
                  defaultFont,
                  rootRestart)
#endif

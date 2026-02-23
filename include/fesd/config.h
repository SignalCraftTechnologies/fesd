#ifndef FESD_CONFIG_H
#define FESD_CONFIG_H


#if defined(FESD_SHARED_BUILD)
#if defined(_MSC_VER) || defined(__MINGW32__)
#define FESD_EXPORT __declspec(dllexport)
#define FESD_IMPORT __declspec(dllimport)
#elif defined(__clang__) || defined(__GNUC__)
#define FESD_EXPORT __attribute__((visibility("default")))
#define FESD_IMPORT __attribute__((visibility("default")))
#endif
#elif defined(FESD_STATIC_BUILD)
#define FESD_EXPORT
#define FESD_IMPORT
#else
#define FESD_EXPORT
#define FESD_IMPORT
#endif

#ifdef FESD_EXPORTS
#define FESD_API FESD_EXPORT
#else
#define FESD_API FESD_IMPORT
#endif

#endif // FESD_CONFIG_H
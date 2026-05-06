#ifndef OGC_PORTRAYAL_EXPORT_H
#define OGC_PORTRAYAL_EXPORT_H

#ifdef OGC_SYMBOLOGY_EXPORTS
#define OGC_PORTRAYAL_API __declspec(dllexport)
#else
#define OGC_PORTRAYAL_API __declspec(dllimport)
#endif

#endif

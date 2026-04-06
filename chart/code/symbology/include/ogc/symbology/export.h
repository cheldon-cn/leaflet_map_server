#ifndef OGC_SYMBOLOGY_EXPORT_H
#define OGC_SYMBOLOGY_EXPORT_H

#ifdef OGC_SYMBOLOGY_EXPORTS
#define OGC_SYMBOLOGY_API __declspec(dllexport)
#else
#define OGC_SYMBOLOGY_API __declspec(dllimport)
#endif

#endif

#ifndef OGC_ALERT_EXPORT_H
#define OGC_ALERT_EXPORT_H

#ifdef OGC_ALERT_EXPORTS
#define OGC_ALERT_API __declspec(dllexport)
#else
#define OGC_ALERT_API __declspec(dllimport)
#endif

#endif

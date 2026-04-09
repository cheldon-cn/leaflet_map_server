#ifndef CHART_FORMAT_H
#define CHART_FORMAT_H

#include <string>

namespace chart {
namespace parser {

enum class ChartFormat {
    Unknown = 0,
    S57,
    S100,
    S101,
    S102,
    S111,
    S112
};

inline std::string ChartFormatToString(ChartFormat format) {
    switch (format) {
        case ChartFormat::S57:   return "S57";
        case ChartFormat::S100:  return "S100";
        case ChartFormat::S101:  return "S101";
        case ChartFormat::S102:  return "S102";
        case ChartFormat::S111:  return "S111";
        case ChartFormat::S112:  return "S112";
        default:                 return "Unknown";
    }
}

inline ChartFormat StringToChartFormat(const std::string& str) {
    if (str == "S57" || str == "s57")   return ChartFormat::S57;
    if (str == "S100" || str == "s100") return ChartFormat::S100;
    if (str == "S101" || str == "s101") return ChartFormat::S101;
    if (str == "S102" || str == "s102") return ChartFormat::S102;
    if (str == "S111" || str == "s111") return ChartFormat::S111;
    if (str == "S112" || str == "s112") return ChartFormat::S112;
    return ChartFormat::Unknown;
}

} // namespace parser
} // namespace chart

#endif // CHART_FORMAT_H

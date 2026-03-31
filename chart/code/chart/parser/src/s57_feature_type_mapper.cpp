#include "chart_parser/s57_feature_type_mapper.h"
#include "chart_parser/error_handler.h"

namespace chart {
namespace parser {

S57FeatureTypeMapper::S57FeatureTypeMapper() {
    InitializeMapping();
}

S57FeatureTypeMapper::~S57FeatureTypeMapper() {
}

void S57FeatureTypeMapper::InitializeMapping() {
    m_typeMapping["SOUNDG"] = FeatureType::SOUNDG;
    m_typeMapping["LIGHTS"] = FeatureType::LIGHTS;
    m_typeMapping["BOYLAT"] = FeatureType::BOYLAT;
    m_typeMapping["BOYSAW"] = FeatureType::BOYSAW;
    m_typeMapping["BCNLAT"] = FeatureType::BCNLAT;
    m_typeMapping["BCNSAW"] = FeatureType::BCNSAW;
    m_typeMapping["DEPARE"] = FeatureType::DEPARE;
    m_typeMapping["DEPCNT"] = FeatureType::DEPCNT;
    m_typeMapping["LNDARE"] = FeatureType::LNDARE;
    m_typeMapping["LNDELV"] = FeatureType::LNDELV;
    m_typeMapping["OBSTRN"] = FeatureType::OBSTRN;
    m_typeMapping["WRECKS"] = FeatureType::WRECKS;
    m_typeMapping["UWTROC"] = FeatureType::UWTROC;
    m_typeMapping["COALNE"] = FeatureType::COALNE;
    m_typeMapping["SLCONS"] = FeatureType::SLCONS;
    m_typeMapping["BUAARE"] = FeatureType::BUAARE;
    m_typeMapping["RESARE"] = FeatureType::RESARE;
    m_typeMapping["NAVLNE"] = FeatureType::NAVLNE;
    m_typeMapping["RADSTA"] = FeatureType::RADSTA;
    m_typeMapping["RTPBCN"] = FeatureType::RTPBCN;
    m_typeMapping["SBDARE"] = FeatureType::SBDARE;
    m_typeMapping["SEAARE"] = FeatureType::SEAARE;
    m_typeMapping["TSSLPT"] = FeatureType::TSSLPT;
    m_typeMapping["TSEZNE"] = FeatureType::TSEZNE;
    m_typeMapping["DRGARE"] = FeatureType::DRGARE;
    m_typeMapping["FAIRWY"] = FeatureType::FAIRWY;
    m_typeMapping["HULKES"] = FeatureType::HULKES;
    m_typeMapping["PONTON"] = FeatureType::PONTON;
    m_typeMapping["PILPNT"] = FeatureType::PILPNT;
    m_typeMapping["MORFAC"] = FeatureType::MORFAC;
    m_typeMapping["GATCON"] = FeatureType::GATCON;
    m_typeMapping["LOCK"] = FeatureType::LOCK;
    m_typeMapping["CBLARE"] = FeatureType::CBLARE;
    m_typeMapping["CBLOHD"] = FeatureType::CBLOHD;
    m_typeMapping["BERTHS"] = FeatureType::BERTHS;
    m_typeMapping["BRIDGE"] = FeatureType::BRIDGE;
    m_typeMapping["CAUSWY"] = FeatureType::CAUSWY;
    m_typeMapping["DISMAR"] = FeatureType::DISMAR;
    m_typeMapping["FLODOC"] = FeatureType::FLODOC;
    m_typeMapping["HRBARE"] = FeatureType::HRBARE;
    m_typeMapping["HRBFAC"] = FeatureType::HRBFAC;
    m_typeMapping["LOKBSN"] = FeatureType::LOKBSN;
    m_typeMapping["PRDARE"] = FeatureType::PRDARE;
    m_typeMapping["PYLONS"] = FeatureType::PYLONS;
    m_typeMapping["TUNNEL"] = FeatureType::TUNNEL;
    m_typeMapping["WTWARE"] = FeatureType::WTWARE;
    m_typeMapping["WTWAXS"] = FeatureType::WTWAXS;
    m_typeMapping["MIPARE"] = FeatureType::MIPARE;
    m_typeMapping["OSPARE"] = FeatureType::OSPARE;
    m_typeMapping["OSTRBN"] = FeatureType::OSTRBN;
    m_typeMapping["RDOSTA"] = FeatureType::RDOSTA;
    m_typeMapping["RDOCAL"] = FeatureType::RDOCAL;
    m_typeMapping["RETRFL"] = FeatureType::RETRFL;
    m_typeMapping["SILTNK"] = FeatureType::SILTNK;
    m_typeMapping["SISTAT"] = FeatureType::SISTAT;
    m_typeMapping["TOPMAR"] = FeatureType::TOPMAR;
    m_typeMapping["VEGATN"] = FeatureType::VEGATN;
    m_typeMapping["WATFAL"] = FeatureType::WATFAL;
    m_typeMapping["WEDKLP"] = FeatureType::WEDKLP;
    m_typeMapping["WEDRTW"] = FeatureType::WEDRTW;
}

FeatureType S57FeatureTypeMapper::MapFeatureType(const std::string& className) {
    auto it = m_typeMapping.find(className);
    if (it != m_typeMapping.end()) {
        return it->second;
    }
    
    LOG_DEBUG("Unknown S57 object class: {}", className);
    return FeatureType::Unknown;
}

} // namespace parser
} // namespace chart

#include <gtest/gtest.h>
#include "ogc/navi/positioning/nmea_parser.h"

using namespace ogc::navi;

class NmeaParserTest : public ::testing::Test {
protected:
    void SetUp() override {
        parser = &NmeaParser::Instance();
    }
    
    NmeaParser* parser;
};

TEST_F(NmeaParserTest, ValidateChecksum_Valid) {
    std::string sentence = "$GPGGA,092750.000,5321.6802,N,00630.3372,W,1,8,1.03,61.7,M,55.2,M,,*76";
    EXPECT_TRUE(parser->ValidateChecksum(sentence));
}

TEST_F(NmeaParserTest, ValidateChecksum_Invalid) {
    std::string sentence = "$GPGGA,092750.000,5321.6802,N,00630.3372,W,1,8,1.03,61.7,M,55.2,M,,*77";
    EXPECT_FALSE(parser->ValidateChecksum(sentence));
}

TEST_F(NmeaParserTest, ValidateChecksum_NoAsterisk) {
    std::string sentence = "$GPGGA,092750.000,5321.6802,N,00630.3372,W,1,8,1.03,61.7,M,55.2,M,,";
    EXPECT_FALSE(parser->ValidateChecksum(sentence));
}

TEST_F(NmeaParserTest, CalculateChecksum) {
    std::string sentence = "$GPGGA,092750.000,5321.6802,N,00630.3372,W,1,8,1.03,61.7,M,55.2,M,,";
    std::string checksum = parser->CalculateChecksum(sentence);
    EXPECT_EQ(checksum, "76");
}

TEST_F(NmeaParserTest, ParseGGA_Valid) {
    std::string sentence = "$GPGGA,092750.000,5321.6802,N,00630.3372,W,1,8,1.03,61.7,M,55.2,M,,*76";
    
    GgaData data;
    EXPECT_TRUE(parser->ParseGGA(sentence, data));
    
    EXPECT_NEAR(data.utc_time, 9 * 3600 + 27 * 60 + 50.0, 0.001);
    EXPECT_NEAR(data.latitude, 53.361337, 0.0001);
    EXPECT_NEAR(data.longitude, -6.505620, 0.0001);
    EXPECT_EQ(data.gps_quality, 1);
    EXPECT_EQ(data.satellite_count, 8);
    EXPECT_NEAR(data.hdop, 1.03, 0.01);
    EXPECT_NEAR(data.altitude, 61.7, 0.1);
}

TEST_F(NmeaParserTest, ParseGGA_WithDGPS) {
    std::string sentence = "$GPGGA,092750.000,5321.6802,N,00630.3372,W,2,8,1.03,61.7,M,55.2,M,,*76";
    
    GgaData data;
    EXPECT_TRUE(parser->ParseGGA(sentence, data));
    EXPECT_EQ(data.gps_quality, 2);
}

TEST_F(NmeaParserTest, ParseRMC_Valid) {
    std::string sentence = "$GPRMC,092750.000,A,5321.6802,N,00630.3372,W,0.02,31.66,280511,,,A*45";
    
    RmcData data;
    EXPECT_TRUE(parser->ParseRMC(sentence, data));
    
    EXPECT_NEAR(data.utc_time, 9 * 3600 + 27 * 60 + 50.0, 0.001);
    EXPECT_EQ(data.status, 'A');
    EXPECT_NEAR(data.latitude, 53.361337, 0.0001);
    EXPECT_NEAR(data.longitude, -6.505620, 0.0001);
    EXPECT_NEAR(data.speed_knots, 0.02, 0.01);
    EXPECT_NEAR(data.track_angle, 31.66, 0.01);
}

TEST_F(NmeaParserTest, ParseRMC_InvalidStatus) {
    std::string sentence = "$GPRMC,092750.000,V,5321.6802,N,00630.3372,W,0.02,31.66,280511,,,A*45";
    
    RmcData data;
    EXPECT_TRUE(parser->ParseRMC(sentence, data));
    EXPECT_EQ(data.status, 'V');
}

TEST_F(NmeaParserTest, ParseVTG_Valid) {
    std::string sentence = "$GPVTG,054.7,T,034.4,M,005.5,N,010.2,K*48";
    
    VtgData data;
    EXPECT_TRUE(parser->ParseVTG(sentence, data));
    
    EXPECT_NEAR(data.track_true, 54.7, 0.1);
    EXPECT_NEAR(data.track_magnetic, 34.4, 0.1);
    EXPECT_NEAR(data.speed_knots, 5.5, 0.1);
    EXPECT_NEAR(data.speed_kph, 10.2, 0.1);
}

TEST_F(NmeaParserTest, ParseGSA_Valid) {
    std::string sentence = "$GPGSA,A,3,04,05,,09,12,,,24,,,,,2.5,1.3,2.1*39";
    
    GsaData data;
    EXPECT_TRUE(parser->ParseGSA(sentence, data));
    
    EXPECT_EQ(data.mode, 'A');
    EXPECT_EQ(data.fix_type, 3);
    EXPECT_EQ(data.satellites[0], 4);
    EXPECT_EQ(data.satellites[1], 5);
    EXPECT_EQ(data.satellites[2], 0);
    EXPECT_EQ(data.satellites[3], 9);
    EXPECT_NEAR(data.pdop, 2.5, 0.1);
    EXPECT_NEAR(data.hdop, 1.3, 0.1);
    EXPECT_NEAR(data.vdop, 2.1, 0.1);
}

TEST_F(NmeaParserTest, ParseGSV_Valid) {
    std::string sentence = "$GPGSV,2,1,08,01,40,083,46,02,17,308,41,12,07,344,39,14,22,228,45*75";
    
    GsvData data;
    EXPECT_TRUE(parser->ParseGSV(sentence, data));
    
    EXPECT_EQ(data.total_messages, 2);
    EXPECT_EQ(data.message_number, 1);
    EXPECT_EQ(data.satellites_in_view, 8);
    EXPECT_EQ(data.satellites[0].prn, 1);
    EXPECT_EQ(data.satellites[0].elevation, 40);
    EXPECT_EQ(data.satellites[0].azimuth, 83);
    EXPECT_EQ(data.satellites[0].snr, 46);
}

TEST_F(NmeaParserTest, Parse_GGA) {
    std::string base = "$GPGGA,092750.000,5321.6802,N,00630.3372,W,1,8,1.03,61.7,M,55.2,M,,";
    std::string checksum = parser->CalculateChecksum(base);
    std::string sentence = base + "*" + checksum;
    
    PositionData data;
    EXPECT_TRUE(parser->Parse(sentence, data));
    
    EXPECT_NEAR(data.latitude, 53.361337, 0.0001);
    EXPECT_NEAR(data.longitude, -6.505620, 0.0001);
    EXPECT_EQ(data.quality, PositionQuality::Gps);
    EXPECT_TRUE(data.valid);
}

TEST_F(NmeaParserTest, Parse_RMC) {
    std::string base = "$GPRMC,092750.000,A,5321.6802,N,00630.3372,W,0.02,31.66,280511,,,A";
    std::string checksum = parser->CalculateChecksum(base);
    std::string sentence = base + "*" + checksum;
    
    PositionData data;
    EXPECT_TRUE(parser->Parse(sentence, data));
    
    EXPECT_NEAR(data.latitude, 53.361337, 0.0001);
    EXPECT_NEAR(data.longitude, -6.505620, 0.0001);
    EXPECT_NEAR(data.speed, 0.02, 0.01);
    EXPECT_NEAR(data.course, 31.66, 0.01);
    EXPECT_TRUE(data.valid);
}

TEST_F(NmeaParserTest, Parse_InvalidChecksum) {
    std::string sentence = "$GPGGA,092750.000,5321.6802,N,00630.3372,W,1,8,1.03,61.7,M,55.2,M,,*77";
    
    PositionData data;
    EXPECT_FALSE(parser->Parse(sentence, data));
}

TEST_F(NmeaParserTest, Parse_Shanghai) {
    std::string base = "$GPGGA,092750.000,3113.822,N,12128.422,E,1,8,1.03,10.0,M,55.2,M,,";
    std::string checksum = parser->CalculateChecksum(base);
    std::string sentence = base + "*" + checksum;
    
    PositionData data;
    EXPECT_TRUE(parser->Parse(sentence, data));
    
    EXPECT_NEAR(data.latitude, 31.230367, 0.0001);
    EXPECT_NEAR(data.longitude, 121.473700, 0.0001);
}

TEST_F(NmeaParserTest, ParseGGA_EmptyString_ReturnsFalse) {
    std::string sentence = "";
    GgaData data;
    EXPECT_FALSE(parser->ParseGGA(sentence, data));
}

TEST_F(NmeaParserTest, ParseGGA_MissingFields_ReturnsFalse) {
    std::string sentence = "$GPGGA,092750.000*76";
    GgaData data;
    EXPECT_FALSE(parser->ParseGGA(sentence, data));
}

TEST_F(NmeaParserTest, Parse_InvalidChecksum_ReturnsFalse) {
    std::string sentence = "$GPGGA,092750.000,5321.6802,N,00630.3372,W,1,8,1.03,61.7,M,55.2,M,,*00";
    PositionData data;
    EXPECT_FALSE(parser->Parse(sentence, data));
}

TEST_F(NmeaParserTest, ParseRMC_EmptyString_ReturnsFalse) {
    std::string sentence = "";
    RmcData data;
    EXPECT_FALSE(parser->ParseRMC(sentence, data));
}

TEST_F(NmeaParserTest, ParseRMC_MissingFields_ReturnsFalse) {
    std::string sentence = "$GPRMC,092750.000*45";
    RmcData data;
    EXPECT_FALSE(parser->ParseRMC(sentence, data));
}

TEST_F(NmeaParserTest, ParseVTG_EmptyString_ReturnsFalse) {
    std::string sentence = "";
    VtgData data;
    EXPECT_FALSE(parser->ParseVTG(sentence, data));
}

TEST_F(NmeaParserTest, ParseGSA_EmptyString_ReturnsFalse) {
    std::string sentence = "";
    GsaData data;
    EXPECT_FALSE(parser->ParseGSA(sentence, data));
}

TEST_F(NmeaParserTest, ParseGSV_EmptyString_ReturnsFalse) {
    std::string sentence = "";
    GsvData data;
    EXPECT_FALSE(parser->ParseGSV(sentence, data));
}

TEST_F(NmeaParserTest, Parse_EmptyString_ReturnsFalse) {
    std::string sentence = "";
    PositionData data;
    EXPECT_FALSE(parser->Parse(sentence, data));
}

TEST_F(NmeaParserTest, Parse_UnknownSentence_ReturnsFalse) {
    std::string sentence = "$GPXXX,092750.000,5321.6802,N,00630.3372,W*76";
    PositionData data;
    EXPECT_FALSE(parser->Parse(sentence, data));
}

TEST_F(NmeaParserTest, ValidateChecksum_EmptyString_ReturnsFalse) {
    std::string sentence = "";
    EXPECT_FALSE(parser->ValidateChecksum(sentence));
}

TEST_F(NmeaParserTest, CalculateChecksum_EmptyString_ReturnsEmpty) {
    std::string sentence = "";
    std::string checksum = parser->CalculateChecksum(sentence);
    EXPECT_EQ(checksum, "00");
}

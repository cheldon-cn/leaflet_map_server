#include <gtest/gtest.h>
#include <chrono>
#include <vector>
#include <cmath>
#include "ogc/navi/positioning/coordinate_converter.h"
#include "ogc/navi/positioning/nmea_parser.h"
#include "ogc/navi/track/track.h"
#include "ogc/navi/track/track_point.h"
#include "ogc/navi/navigation/navigation_calculator.h"
#include "test_constants.h"

using namespace ogc::navi;
namespace tc = ogc::navi::test_constants;

class PerformanceTest : public ::testing::Test {
protected:
    void SetUp() override {
        converter = &CoordinateConverter::Instance();
        parser = &NmeaParser::Instance();
        nav_calculator = &NavigationCalculator::Instance();
    }
    
    void TearDown() override {
        if (track) {
            track->ReleaseReference();
            track = nullptr;
        }
    }
    
    double MeasureExecutionTime(std::function<void()> func, int iterations) {
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < iterations; ++i) {
            func();
        }
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> duration = end - start;
        return duration.count() / iterations;
    }
    
    CoordinateConverter* converter;
    NmeaParser* parser;
    NavigationCalculator* nav_calculator;
    Track* track = nullptr;
};

TEST_F(PerformanceTest, GreatCircleDistance_Performance) {
    const int iterations = 10000;
    
    double avg_time_ms = MeasureExecutionTime([this]() {
        converter->CalculateGreatCircleDistance(
            tc::SHANGHAI_LAT, tc::SHANGHAI_LON,
            tc::BEIJING_LAT, tc::BEIJING_LON
        );
    }, iterations);
    
    EXPECT_LT(avg_time_ms, 0.01) << "GreatCircleDistance should complete in < 0.01ms per call";
    
    double operations_per_second = 1000.0 / avg_time_ms;
    EXPECT_GT(operations_per_second, 100000) << "Should handle > 100,000 operations per second";
}

TEST_F(PerformanceTest, CalculateBearing_Performance) {
    const int iterations = 10000;
    
    double avg_time_ms = MeasureExecutionTime([this]() {
        converter->CalculateBearing(
            tc::SHANGHAI_LAT, tc::SHANGHAI_LON,
            tc::BEIJING_LAT, tc::BEIJING_LON
        );
    }, iterations);
    
    EXPECT_LT(avg_time_ms, 0.01) << "CalculateBearing should complete in < 0.01ms per call";
}

TEST_F(PerformanceTest, CalculateDestination_Performance) {
    const int iterations = 10000;
    
    double avg_time_ms = MeasureExecutionTime([this]() {
        double lat, lon;
        converter->CalculateDestination(
            tc::SHANGHAI_LAT, tc::SHANGHAI_LON,
            45.0, 100000.0, lat, lon
        );
    }, iterations);
    
    EXPECT_LT(avg_time_ms, 0.01) << "CalculateDestination should complete in < 0.01ms per call";
}

TEST_F(PerformanceTest, NmeaParseGGA_Performance) {
    const int iterations = 5000;
    std::string sentence = "$GPGGA,092750.000,5321.6802,N,00630.3372,W,1,8,1.03,61.7,M,55.2,M,,*76";
    
    double avg_time_ms = MeasureExecutionTime([this, &sentence]() {
        GgaData data;
        parser->ParseGGA(sentence, data);
    }, iterations);
    
    EXPECT_LT(avg_time_ms, 0.05) << "ParseGGA should complete in < 0.05ms per call";
}

TEST_F(PerformanceTest, NmeaParseRMC_Performance) {
    const int iterations = 5000;
    std::string sentence = "$GPRMC,092751.000,A,5321.6802,N,00630.3372,W,12.5,45.0,191124,,,A*5C";
    
    double avg_time_ms = MeasureExecutionTime([this, &sentence]() {
        RmcData data;
        parser->ParseRMC(sentence, data);
    }, iterations);
    
    EXPECT_LT(avg_time_ms, 0.05) << "ParseRMC should complete in < 0.05ms per call";
}

TEST_F(PerformanceTest, TrackSimplify_SmallTrack_Performance) {
    track = Track::Create();
    
    const int point_count = 100;
    for (int i = 0; i < point_count; ++i) {
        TrackPointData tp_data;
        tp_data.longitude = tc::SHANGHAI_LON + i * 0.001;
        tp_data.latitude = tc::SHANGHAI_LAT + i * 0.001;
        tp_data.timestamp = 1000.0 + i * 60.0;
        tp_data.speed = 10.0;
        tp_data.course = 45.0;
        
        TrackPoint* tp = TrackPoint::Create(tp_data);
        track->AddPoint(tp);
        tp->ReleaseReference();
    }
    
    const int iterations = 100;
    
    double avg_time_ms = MeasureExecutionTime([this]() {
        track->Simplify(10.0);
    }, iterations);
    
    EXPECT_LT(avg_time_ms, 5.0) << "TrackSimplify (100 points) should complete in < 5ms per call";
}

TEST_F(PerformanceTest, TrackSimplify_MediumTrack_Performance) {
    track = Track::Create();
    
    const int point_count = 1000;
    for (int i = 0; i < point_count; ++i) {
        TrackPointData tp_data;
        tp_data.longitude = tc::SHANGHAI_LON + i * 0.0001;
        tp_data.latitude = tc::SHANGHAI_LAT + i * 0.0001;
        tp_data.timestamp = 1000.0 + i * 60.0;
        tp_data.speed = 10.0;
        tp_data.course = 45.0;
        
        TrackPoint* tp = TrackPoint::Create(tp_data);
        track->AddPoint(tp);
        tp->ReleaseReference();
    }
    
    const int iterations = 10;
    
    double avg_time_ms = MeasureExecutionTime([this]() {
        track->Simplify(10.0);
    }, iterations);
    
    EXPECT_LT(avg_time_ms, 100.0) << "TrackSimplify (1000 points) should complete in < 100ms per call";
}

TEST_F(PerformanceTest, NavigationCalculator_CrossTrackError_Performance) {
    const int iterations = 10000;
    
    GeoPoint position(tc::SHANGHAI_LON, tc::SHANGHAI_LAT);
    GeoPoint start(tc::SHANGHAI_LON + 0.5, tc::SHANGHAI_LAT + 0.5);
    GeoPoint end(tc::BEIJING_LON, tc::BEIJING_LAT);
    
    double avg_time_ms = MeasureExecutionTime([this, &position, &start, &end]() {
        nav_calculator->CalculateCrossTrackError(position, start, end);
    }, iterations);
    
    EXPECT_LT(avg_time_ms, 0.01) << "CalculateCrossTrackError should complete in < 0.01ms per call";
}

TEST_F(PerformanceTest, NavigationCalculator_TimeToWaypoint_Performance) {
    const int iterations = 10000;
    
    double avg_time_ms = MeasureExecutionTime([this]() {
        nav_calculator->CalculateTimeToWaypoint(100000.0, 15.0);
    }, iterations);
    
    EXPECT_LT(avg_time_ms, 0.001) << "CalculateTimeToWaypoint should complete in < 0.001ms per call";
}

TEST_F(PerformanceTest, CoordinateConverter_BatchOperations) {
    const int batch_size = 1000;
    std::vector<std::pair<double, double>> coords;
    
    for (int i = 0; i < batch_size; ++i) {
        coords.push_back({
            tc::SHANGHAI_LAT + (i % 100) * 0.01,
            tc::SHANGHAI_LON + (i / 100) * 0.01
        });
    }
    
    auto start = std::chrono::high_resolution_clock::now();
    
    double total_distance = 0;
    for (size_t i = 0; i < coords.size() - 1; ++i) {
        total_distance += converter->CalculateGreatCircleDistance(
            coords[i].first, coords[i].second,
            coords[i + 1].first, coords[i + 1].second
        );
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration = end - start;
    
    EXPECT_LT(duration.count(), 50.0) << "Batch of 1000 distance calculations should complete in < 50ms";
    EXPECT_GT(total_distance, 0);
}

TEST_F(PerformanceTest, NmeaParser_BatchParsing) {
    const int batch_size = 1000;
    std::vector<std::string> sentences;
    
    for (int i = 0; i < batch_size; ++i) {
        int hour = (i / 3600) % 24;
        int min = (i / 60) % 60;
        int sec = i % 60;
        
        char sentence[128];
        sprintf_s(sentence, sizeof(sentence), 
                "$GPGGA,%02d%02d%02d.000,5321.6802,N,00630.3372,W,1,8,1.03,61.7,M,55.2,M,,*76",
                hour, min, sec);
        sentences.push_back(sentence);
    }
    
    auto start = std::chrono::high_resolution_clock::now();
    
    int success_count = 0;
    for (const auto& sentence : sentences) {
        GgaData data;
        if (parser->ParseGGA(sentence, data)) {
            success_count++;
        }
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration = end - start;
    
    EXPECT_LT(duration.count(), 100.0) << "Batch of 1000 NMEA parsing should complete in < 100ms";
    EXPECT_EQ(success_count, batch_size);
}

TEST_F(PerformanceTest, TrackGetTotalDistance_Performance) {
    track = Track::Create();
    
    const int point_count = 500;
    for (int i = 0; i < point_count; ++i) {
        TrackPointData tp_data;
        tp_data.longitude = tc::SHANGHAI_LON + i * 0.001;
        tp_data.latitude = tc::SHANGHAI_LAT + i * 0.001;
        tp_data.timestamp = 1000.0 + i * 60.0;
        tp_data.speed = 10.0;
        tp_data.course = 45.0;
        
        TrackPoint* tp = TrackPoint::Create(tp_data);
        track->AddPoint(tp);
        tp->ReleaseReference();
    }
    
    const int iterations = 100;
    
    double avg_time_ms = MeasureExecutionTime([this]() {
        track->GetTotalDistance();
    }, iterations);
    
    EXPECT_LT(avg_time_ms, 10.0) << "GetTotalDistance (500 points) should complete in < 10ms per call";
}

TEST_F(PerformanceTest, MemoryEfficiency_TrackPoints) {
    const int point_count = 10000;
    track = Track::Create();
    
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < point_count; ++i) {
        TrackPointData tp_data;
        tp_data.longitude = tc::SHANGHAI_LON + i * 0.0001;
        tp_data.latitude = tc::SHANGHAI_LAT + i * 0.0001;
        tp_data.timestamp = 1000.0 + i * 60.0;
        tp_data.speed = 10.0;
        tp_data.course = 45.0;
        
        TrackPoint* tp = TrackPoint::Create(tp_data);
        track->AddPoint(tp);
        tp->ReleaseReference();
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration = end - start;
    
    EXPECT_EQ(track->GetPointCount(), point_count);
    EXPECT_LT(duration.count(), 5000.0) << "Creating 10000 track points should complete in < 5s";
}

TEST_F(PerformanceTest, StressTest_CoordinateConversions) {
    const int iterations = 100000;
    int success_count = 0;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < iterations; ++i) {
        double lat1 = -90.0 + (i % 180);
        double lon1 = -180.0 + (i % 360);
        double lat2 = lat1 + 0.1;
        double lon2 = lon1 + 0.1;
        
        double distance = converter->CalculateGreatCircleDistance(lat1, lon1, lat2, lon2);
        if (distance >= 0) {
            success_count++;
        }
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration = end - start;
    
    double ops_per_second = iterations / (duration.count() / 1000.0);
    
    EXPECT_GT(ops_per_second, 50000) << "Should handle > 50,000 coordinate conversions per second";
    EXPECT_EQ(success_count, iterations);
}

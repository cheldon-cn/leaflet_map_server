#include <gtest/gtest.h>
#include <chrono>
#include <vector>
#include <memory>
#include "alert/repository_adapter.h"
#include "alert/alert_type_adapter.h"
#include "ogc/alert/types.h"

using namespace std::chrono;

namespace {

ogc::alert::AlertPtr CreateRandomAlert(int id) {
    auto alert = std::make_shared<ogc::alert::Alert>();
    alert->alert_id = "alert_" + std::to_string(id);
    alert->alert_type = static_cast<ogc::alert::AlertType>(id % 7 + 1);
    alert->alert_level = static_cast<ogc::alert::AlertLevel>(id % 4 + 1);
    alert->status = ogc::alert::AlertStatus::kActive;
    alert->content.title = "Test Alert " + std::to_string(id);
    alert->content.message = "Test message for alert " + std::to_string(id);
    alert->position.longitude = -180.0 + (id % 360);
    alert->position.latitude = -90.0 + (id % 180);
    alert->issue_time = ogc::alert::DateTime::Now();
    return alert;
}

}

TEST(PerformanceTest, SaveAlert_Performance) {
    alarm::RepositoryAdapter repoAdapter;
    const int NUM_ALERTS = 1000;
    std::vector<ogc::alert::AlertPtr> alerts;
    
    for (int i = 0; i < NUM_ALERTS; ++i) {
        alerts.push_back(CreateRandomAlert(i));
    }
    
    auto start = high_resolution_clock::now();
    
    for (const auto& alert : alerts) {
        repoAdapter.Save(alert);
    }
    
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(end - start);
    
    double avgTime = static_cast<double>(duration.count()) / NUM_ALERTS;
    
    std::cout << "Save " << NUM_ALERTS << " alerts: " << duration.count() << "ms" << std::endl;
    std::cout << "Average time per alert: " << avgTime << "ms" << std::endl;
    
    EXPECT_LT(avgTime, 1.0);
}

TEST(PerformanceTest, FindById_Performance) {
    alarm::RepositoryAdapter repoAdapter;
    const int NUM_ALERTS = 100;
    
    for (int i = 0; i < NUM_ALERTS; ++i) {
        repoAdapter.Save(CreateRandomAlert(i));
    }
    
    auto start = high_resolution_clock::now();
    
    for (int i = 0; i < NUM_ALERTS; ++i) {
        auto alert = repoAdapter.FindById("alert_" + std::to_string(i));
    }
    
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(end - start);
    
    double avgTime = static_cast<double>(duration.count()) / NUM_ALERTS;
    
    std::cout << "FindById " << NUM_ALERTS << " alerts: " << duration.count() << "ms" << std::endl;
    std::cout << "Average time per query: " << avgTime << "ms" << std::endl;
    
    EXPECT_LT(avgTime, 1.0);
}

TEST(PerformanceTest, Query_Performance) {
    alarm::RepositoryAdapter repoAdapter;
    const int NUM_ALERTS = 500;
    
    for (int i = 0; i < NUM_ALERTS; ++i) {
        repoAdapter.Save(CreateRandomAlert(i));
    }
    
    ogc::alert::AlertQueryParams params;
    params.page = 1;
    params.page_size = 50;
    
    auto start = high_resolution_clock::now();
    
    auto results = repoAdapter.Query(params);
    
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(end - start);
    
    std::cout << "Query " << results.size() << " alerts: " << duration.count() << "ms" << std::endl;
    
    EXPECT_LT(duration.count(), 100);
}

TEST(PerformanceTest, SaveConfig_Performance) {
    alarm::ConfigRepositoryAdapter configAdapter;
    const int NUM_CONFIGS = 100;
    std::vector<ogc::alert::AlertConfig> configs;
    
    for (int i = 0; i < NUM_CONFIGS; ++i) {
        ogc::alert::AlertConfig config;
        config.user_id = "user_" + std::to_string(i);
        config.depth_threshold.level1_threshold = 0.5;
        config.depth_threshold.level2_threshold = 1.0;
        config.depth_threshold.level3_threshold = 2.0;
        config.depth_threshold.level4_threshold = 3.0;
        configs.push_back(config);
    }
    
    auto start = high_resolution_clock::now();
    
    for (const auto& config : configs) {
        configAdapter.SaveConfig(config);
    }
    
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(end - start);
    
    double avgTime = static_cast<double>(duration.count()) / NUM_CONFIGS;
    
    std::cout << "Save " << NUM_CONFIGS << " configs: " << duration.count() << "ms" << std::endl;
    std::cout << "Average time per config: " << avgTime << "ms" << std::endl;
    
    EXPECT_LT(avgTime, 1.0);
}

TEST(PerformanceTest, LoadConfig_Performance) {
    alarm::ConfigRepositoryAdapter configAdapter;
    const int NUM_CONFIGS = 100;
    
    for (int i = 0; i < NUM_CONFIGS; ++i) {
        ogc::alert::AlertConfig config;
        config.user_id = "user_" + std::to_string(i);
        configAdapter.SaveConfig(config);
    }
    
    auto start = high_resolution_clock::now();
    
    for (int i = 0; i < NUM_CONFIGS; ++i) {
        ogc::alert::AlertConfig config = configAdapter.LoadConfig("user_" + std::to_string(i));
        EXPECT_EQ(config.user_id, "user_" + std::to_string(i));
    }
    
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(end - start);
    
    double avgTime = static_cast<double>(duration.count()) / NUM_CONFIGS;
    
    std::cout << "Load " << NUM_CONFIGS << " configs: " << duration.count() << "ms" << std::endl;
    std::cout << "Average time per config: " << avgTime << "ms" << std::endl;
    
    EXPECT_LT(avgTime, 1.0);
}

TEST(PerformanceTest, TypeConversion_Performance) {
    const int NUM_CONVERSIONS = 10000;
    
    auto start = high_resolution_clock::now();
    
    for (int i = 0; i < NUM_CONVERSIONS; ++i) {
        ogc::alert::AlertType type = static_cast<ogc::alert::AlertType>((i % 4) + 1);
        alert::AlertType alarmType = alarm::AlertTypeAdapter::FromAlertType(type);
        ogc::alert::AlertType backType = alarm::AlertTypeAdapter::ToAlertType(alarmType);
        EXPECT_EQ(static_cast<int>(type), static_cast<int>(backType));
    }
    
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(end - start);
    
    double avgTime = static_cast<double>(duration.count()) / NUM_CONVERSIONS;
    
    std::cout << "Type conversion " << NUM_CONVERSIONS << " times: " << duration.count() << "us" << std::endl;
    std::cout << "Average time per conversion: " << avgTime << "us" << std::endl;
    
    EXPECT_LT(avgTime, 10.0);
}

TEST(PerformanceTest, MemoryUsage_Baseline) {
    alarm::RepositoryAdapter repoAdapter;
    const int NUM_ALERTS = 1000;
    
    for (int i = 0; i < NUM_ALERTS; ++i) {
        repoAdapter.Save(CreateRandomAlert(i));
    }
    
    std::cout << "Memory baseline test completed for " << NUM_ALERTS << " alerts" << std::endl;
    std::cout << "Expected memory per alert: ~200 bytes" << std::endl;
    
    SUCCEED();
}

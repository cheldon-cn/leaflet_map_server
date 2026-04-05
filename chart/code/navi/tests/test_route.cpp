#include <gtest/gtest.h>
#include "ogc/navi/route/route.h"
#include "ogc/navi/route/waypoint.h"

using namespace ogc::navi;

class RouteTest : public ::testing::Test {
protected:
    void SetUp() override {
        route = Route::Create();
        route->SetName("TestRoute");
        
        wp1 = Waypoint::Create();
        wp1->SetPosition(121.4737, 31.2304);
        wp1->SetName("Shanghai");
        
        wp2 = Waypoint::Create();
        wp2->SetPosition(122.0, 32.0);
        wp2->SetName("Waypoint2");
        
        wp3 = Waypoint::Create();
        wp3->SetPosition(123.0, 33.0);
        wp3->SetName("Waypoint3");
    }
    
    void TearDown() override {
        if (route) {
            route->ReleaseReference();
        }
    }
    
    Route* route = nullptr;
    Waypoint* wp1 = nullptr;
    Waypoint* wp2 = nullptr;
    Waypoint* wp3 = nullptr;
};

TEST_F(RouteTest, Create) {
    Route* r = Route::Create();
    EXPECT_EQ(r->GetWaypointCount(), 0);
    EXPECT_DOUBLE_EQ(r->GetTotalDistance(), 0.0);
    r->ReleaseReference();
}

TEST_F(RouteTest, SetAndGetName) {
    route->SetName("NewName");
    EXPECT_EQ(route->GetName(), "NewName");
}

TEST_F(RouteTest, SetAndGetDescription) {
    route->SetDescription("Test description");
    EXPECT_EQ(route->GetDescription(), "Test description");
}

TEST_F(RouteTest, SetAndGetId) {
    route->SetId("route-001");
    EXPECT_EQ(route->GetId(), "route-001");
}

TEST_F(RouteTest, SetAndGetStatus) {
    route->SetStatus(RouteStatus::Active);
    EXPECT_EQ(route->GetStatus(), RouteStatus::Active);
}

TEST_F(RouteTest, SetAndGetDeparture) {
    route->SetDeparture("Shanghai");
    EXPECT_EQ(route->GetDeparture(), "Shanghai");
}

TEST_F(RouteTest, SetAndGetDestination) {
    route->SetDestination("Tokyo");
    EXPECT_EQ(route->GetDestination(), "Tokyo");
}

TEST_F(RouteTest, AddWaypoint) {
    route->AddWaypoint(wp1);
    wp1 = nullptr;  // Ownership transferred
    EXPECT_EQ(route->GetWaypointCount(), 1);
    EXPECT_EQ(route->GetWaypoint(0)->GetName(), "Shanghai");
}

TEST_F(RouteTest, AddMultipleWaypoints) {
    route->AddWaypoint(wp1);
    wp1 = nullptr;
    route->AddWaypoint(wp2);
    wp2 = nullptr;
    route->AddWaypoint(wp3);
    wp3 = nullptr;
    
    EXPECT_EQ(route->GetWaypointCount(), 3);
}

TEST_F(RouteTest, InsertWaypoint) {
    route->AddWaypoint(wp1);
    wp1 = nullptr;
    route->AddWaypoint(wp3);
    wp3 = nullptr;
    
    Waypoint* newWp = Waypoint::Create();
    newWp->SetPosition(122.0, 32.0);
    newWp->SetName("Waypoint2");
    route->InsertWaypoint(1, newWp);
    
    EXPECT_EQ(route->GetWaypointCount(), 3);
    EXPECT_EQ(route->GetWaypoint(1)->GetName(), "Waypoint2");
}

TEST_F(RouteTest, RemoveWaypoint) {
    route->AddWaypoint(wp1);
    wp1 = nullptr;
    route->AddWaypoint(wp2);
    wp2 = nullptr;
    route->AddWaypoint(wp3);
    wp3 = nullptr;
    
    route->RemoveWaypoint(1);
    
    EXPECT_EQ(route->GetWaypointCount(), 2);
    EXPECT_EQ(route->GetWaypoint(1)->GetName(), "Waypoint3");
}

TEST_F(RouteTest, ClearWaypoints) {
    route->AddWaypoint(wp1);
    wp1 = nullptr;
    route->AddWaypoint(wp2);
    wp2 = nullptr;
    
    route->ClearWaypoints();
    
    EXPECT_EQ(route->GetWaypointCount(), 0);
}

TEST_F(RouteTest, GetTotalDistance) {
    route->AddWaypoint(wp1);
    wp1 = nullptr;
    route->AddWaypoint(wp2);
    wp2 = nullptr;
    route->AddWaypoint(wp3);
    wp3 = nullptr;
    
    double distance = route->CalculateTotalDistance();
    EXPECT_GT(distance, 0.0);
}

TEST_F(RouteTest, GetLegDistance) {
    route->AddWaypoint(wp1);
    wp1 = nullptr;
    route->AddWaypoint(wp2);
    wp2 = nullptr;
    
    double leg_distance = route->CalculateLegDistance(0);
    EXPECT_GT(leg_distance, 0.0);
}

TEST_F(RouteTest, GetLegBearing) {
    route->AddWaypoint(wp1);
    wp1 = nullptr;
    route->AddWaypoint(wp2);
    wp2 = nullptr;
    
    double bearing = route->CalculateLegBearing(0);
    EXPECT_GE(bearing, 0.0);
    EXPECT_LT(bearing, 360.0);
}

TEST_F(RouteTest, Reverse) {
    route->AddWaypoint(wp1);
    wp1 = nullptr;
    route->AddWaypoint(wp2);
    wp2 = nullptr;
    route->AddWaypoint(wp3);
    wp3 = nullptr;
    
    route->Reverse();
    
    EXPECT_EQ(route->GetWaypoint(0)->GetName(), "Waypoint3");
    EXPECT_EQ(route->GetWaypoint(2)->GetName(), "Shanghai");
}

TEST_F(RouteTest, ToDataFromData) {
    route->SetId("test-id");
    route->SetName("Test Route");
    route->SetDescription("Test Description");
    route->SetDeparture("Shanghai");
    route->SetDestination("Tokyo");
    route->AddWaypoint(wp1);
    wp1 = nullptr;
    
    RouteData data = route->ToData();
    
    EXPECT_EQ(data.id, "test-id");
    EXPECT_EQ(data.name, "Test Route");
    EXPECT_EQ(data.description, "Test Description");
    EXPECT_EQ(data.departure, "Shanghai");
    EXPECT_EQ(data.destination, "Tokyo");
    
    Route* route2 = Route::Create(data);
    EXPECT_EQ(route2->GetId(), "test-id");
    EXPECT_EQ(route2->GetName(), "Test Route");
    EXPECT_EQ(route2->GetWaypointCount(), 1);
    
    route2->ReleaseReference();
}

TEST_F(RouteTest, CreateWithData) {
    RouteData data;
    data.id = "route-123";
    data.name = "My Route";
    data.description = "Test route";
    data.departure = "Port A";
    data.destination = "Port B";
    data.status = RouteStatus::Planned;
    
    Route* r = Route::Create(data);
    
    EXPECT_EQ(r->GetId(), "route-123");
    EXPECT_EQ(r->GetName(), "My Route");
    EXPECT_EQ(r->GetDescription(), "Test route");
    EXPECT_EQ(r->GetDeparture(), "Port A");
    EXPECT_EQ(r->GetDestination(), "Port B");
    EXPECT_EQ(r->GetStatus(), RouteStatus::Planned);
    
    r->ReleaseReference();
}

TEST_F(RouteTest, GetWaypoint_EmptyRoute_ReturnsNull) {
    Waypoint* wp = route->GetWaypoint(0);
    EXPECT_EQ(wp, nullptr);
}

TEST_F(RouteTest, GetWaypoint_IndexOutOfRange_ReturnsNull) {
    route->AddWaypoint(wp1);
    wp1 = nullptr;
    
    Waypoint* wp = route->GetWaypoint(10);
    EXPECT_EQ(wp, nullptr);
}

TEST_F(RouteTest, GetWaypoint_NegativeIndex_ReturnsNull) {
    route->AddWaypoint(wp1);
    wp1 = nullptr;
    
    Waypoint* wp = route->GetWaypoint(-1);
    EXPECT_EQ(wp, nullptr);
}

TEST_F(RouteTest, RemoveWaypoint_EmptyRoute_HandlesGracefully) {
    EXPECT_NO_THROW(route->RemoveWaypoint(0));
}

TEST_F(RouteTest, RemoveWaypoint_IndexOutOfRange_HandlesGracefully) {
    route->AddWaypoint(wp1);
    wp1 = nullptr;
    
    EXPECT_NO_THROW(route->RemoveWaypoint(10));
}

TEST_F(RouteTest, InsertWaypoint_EmptyRoute_InsertsAtZero) {
    route->InsertWaypoint(0, wp1);
    wp1 = nullptr;
    
    EXPECT_EQ(route->GetWaypointCount(), 1);
}

TEST_F(RouteTest, InsertWaypoint_IndexOutOfRange_DoesNothing) {
    route->AddWaypoint(wp1);
    wp1 = nullptr;
    
    route->InsertWaypoint(10, wp2);
    wp2 = nullptr;
    
    EXPECT_EQ(route->GetWaypointCount(), 1);
}

TEST_F(RouteTest, CalculateTotalDistance_EmptyRoute_ReturnsZero) {
    double distance = route->CalculateTotalDistance();
    EXPECT_DOUBLE_EQ(distance, 0.0);
}

TEST_F(RouteTest, CalculateTotalDistance_SingleWaypoint_ReturnsZero) {
    route->AddWaypoint(wp1);
    wp1 = nullptr;
    
    double distance = route->CalculateTotalDistance();
    EXPECT_DOUBLE_EQ(distance, 0.0);
}

TEST_F(RouteTest, CalculateLegDistance_EmptyRoute_ReturnsZero) {
    double distance = route->CalculateLegDistance(0);
    EXPECT_DOUBLE_EQ(distance, 0.0);
}

TEST_F(RouteTest, CalculateLegDistance_IndexOutOfRange_ReturnsZero) {
    route->AddWaypoint(wp1);
    wp1 = nullptr;
    
    double distance = route->CalculateLegDistance(10);
    EXPECT_DOUBLE_EQ(distance, 0.0);
}

TEST_F(RouteTest, CalculateLegBearing_EmptyRoute_ReturnsZero) {
    double bearing = route->CalculateLegBearing(0);
    EXPECT_DOUBLE_EQ(bearing, 0.0);
}

TEST_F(RouteTest, Reverse_EmptyRoute_HandlesGracefully) {
    EXPECT_NO_THROW(route->Reverse());
}

TEST_F(RouteTest, Reverse_SingleWaypoint_HandlesGracefully) {
    route->AddWaypoint(wp1);
    wp1 = nullptr;
    
    EXPECT_NO_THROW(route->Reverse());
    EXPECT_EQ(route->GetWaypointCount(), 1);
}

#include <gtest/gtest.h>

#ifdef DRAW_HAS_QT

#include "ogc/draw/qt_event_adapter.h"
#include "ogc/draw/qt_window_manager.h"
#include "ogc/draw/qt_resource_manager.h"
#include <QApplication>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QWheelEvent>

using namespace ogc::draw;

class QtPlatformTest : public ::testing::Test {
protected:
    void SetUp() override {
        if (!QApplication::instance()) {
            int argc = 1;
            char* argv[] = {const_cast<char*>("test")};
            static QApplication app(argc, argv);
        }
    }
    
    void TearDown() override {
    }
};

TEST_F(QtPlatformTest, EventAdapterCreate) {
    QtEventAdapter adapter;
    EXPECT_FALSE(adapter.IsEnabled());
    adapter.SetEnabled(true);
    EXPECT_TRUE(adapter.IsEnabled());
}

TEST_F(QtPlatformTest, EventAdapterMouseTracking) {
    QtEventAdapter adapter;
    EXPECT_FALSE(adapter.IsMouseTracking());
    adapter.SetMouseTracking(true);
    EXPECT_TRUE(adapter.IsMouseTracking());
}

TEST_F(QtPlatformTest, EventAdapterTouchEnabled) {
    QtEventAdapter adapter;
    EXPECT_TRUE(adapter.IsTouchEnabled());
    adapter.SetTouchEnabled(false);
    EXPECT_FALSE(adapter.IsTouchEnabled());
}

TEST_F(QtPlatformTest, EventAdapterGestureEnabled) {
    QtEventAdapter adapter;
    EXPECT_TRUE(adapter.IsGestureEnabled());
    adapter.SetGestureEnabled(false);
    EXPECT_FALSE(adapter.IsGestureEnabled());
}

TEST_F(QtPlatformTest, EventAdapterCallback) {
    QtEventAdapter adapter;
    adapter.SetEnabled(true);
    
    bool callbackCalled = false;
    adapter.SetEventCallback(InteractionEventType::kMouseDown, 
        [&callbackCalled](const InteractionEvent& event) {
            callbackCalled = true;
            return true;
        });
    
    QMouseEvent mouseEvent(QEvent::MouseButtonPress, 
                          QPointF(100, 200), 
                          Qt::LeftButton, 
                          Qt::LeftButton, 
                          Qt::NoModifier);
    
    adapter.eventFilter(nullptr, &mouseEvent);
    EXPECT_TRUE(callbackCalled);
}

TEST_F(QtPlatformTest, EventAdapterClearCallback) {
    QtEventAdapter adapter;
    
    adapter.SetEventCallback(InteractionEventType::kMouseDown, 
        [](const InteractionEvent& event) { return true; });
    
    adapter.ClearEventCallback(InteractionEventType::kMouseDown);
}

TEST_F(QtPlatformTest, EventAdapterClearAllCallbacks) {
    QtEventAdapter adapter;
    
    adapter.SetEventCallback(InteractionEventType::kMouseDown, 
        [](const InteractionEvent& event) { return true; });
    adapter.SetEventCallback(InteractionEventType::kMouseUp, 
        [](const InteractionEvent& event) { return true; });
    
    adapter.ClearAllCallbacks();
}

TEST_F(QtPlatformTest, WindowManagerInstance) {
    auto& manager = QtWindowManager::Instance();
    EXPECT_FALSE(manager.IsInitialized());
}

TEST_F(QtPlatformTest, WindowManagerInitialize) {
    auto& manager = QtWindowManager::Instance();
    EXPECT_TRUE(manager.Initialize());
    EXPECT_TRUE(manager.IsInitialized());
    manager.Finalize();
    EXPECT_FALSE(manager.IsInitialized());
}

TEST_F(QtPlatformTest, WindowManagerCreateMainWindow) {
    auto& manager = QtWindowManager::Instance();
    manager.Initialize();
    
    int windowId = manager.CreateMainWindow("Test Window", QRect(0, 0, 800, 600));
    EXPECT_GT(windowId, 0);
    
    auto window = manager.GetWindow(windowId);
    EXPECT_TRUE(window != nullptr);
    
    manager.CloseWindow(windowId);
    window = manager.GetWindow(windowId);
    EXPECT_TRUE(window == nullptr);
    
    manager.Finalize();
}

TEST_F(QtPlatformTest, WindowManagerCreateDialog) {
    auto& manager = QtWindowManager::Instance();
    manager.Initialize();
    
    int dialogId = manager.CreateDialog("Test Dialog", QRect(0, 0, 400, 300));
    EXPECT_GT(dialogId, 0);
    
    auto dialog = manager.GetWindow(dialogId);
    EXPECT_TRUE(dialog != nullptr);
    
    manager.CloseWindow(dialogId);
    manager.Finalize();
}

TEST_F(QtPlatformTest, WindowManagerWindowTitle) {
    auto& manager = QtWindowManager::Instance();
    manager.Initialize();
    
    int windowId = manager.CreateMainWindow("Original Title", QRect(0, 0, 800, 600));
    
    manager.SetWindowTitle(windowId, "New Title");
    EXPECT_EQ(manager.GetWindowTitle(windowId).toStdString(), "New Title");
    
    manager.CloseWindow(windowId);
    manager.Finalize();
}

TEST_F(QtPlatformTest, WindowManagerGeometry) {
    auto& manager = QtWindowManager::Instance();
    manager.Initialize();
    
    int windowId = manager.CreateMainWindow("Test", QRect(0, 0, 800, 600));
    
    QRect newGeometry(100, 100, 1024, 768);
    manager.SetWindowGeometry(windowId, newGeometry);
    QRect geometry = manager.GetWindowGeometry(windowId);
    
    EXPECT_EQ(geometry.width(), 1024);
    EXPECT_EQ(geometry.height(), 768);
    
    manager.CloseWindow(windowId);
    manager.Finalize();
}

TEST_F(QtPlatformTest, WindowManagerWindowCount) {
    auto& manager = QtWindowManager::Instance();
    manager.Initialize();
    
    EXPECT_EQ(manager.GetWindowCount(), 0);
    
    int id1 = manager.CreateMainWindow("Window1", QRect(0, 0, 800, 600));
    EXPECT_EQ(manager.GetWindowCount(), 1);
    
    int id2 = manager.CreateMainWindow("Window2", QRect(0, 0, 800, 600));
    EXPECT_EQ(manager.GetWindowCount(), 2);
    
    manager.CloseAllWindows();
    EXPECT_EQ(manager.GetWindowCount(), 0);
    
    manager.Finalize();
}

TEST_F(QtPlatformTest, ResourceManagerInstance) {
    auto& manager = QtResourceManager::Instance();
    EXPECT_FALSE(manager.IsInitialized());
}

TEST_F(QtPlatformTest, ResourceManagerInitialize) {
    auto& manager = QtResourceManager::Instance();
    EXPECT_TRUE(manager.Initialize());
    EXPECT_TRUE(manager.IsInitialized());
    manager.Finalize();
    EXPECT_FALSE(manager.IsInitialized());
}

TEST_F(QtPlatformTest, ResourceManagerSearchPath) {
    auto& manager = QtResourceManager::Instance();
    manager.Initialize();
    
    manager.AddSearchPath("/tmp/resources");
    QStringList paths = manager.GetSearchPaths();
    EXPECT_TRUE(paths.contains("/tmp/resources"));
    
    manager.RemoveSearchPath("/tmp/resources");
    paths = manager.GetSearchPaths();
    EXPECT_FALSE(paths.contains("/tmp/resources"));
    
    manager.Finalize();
}

TEST_F(QtPlatformTest, ResourceManagerAlias) {
    auto& manager = QtResourceManager::Instance();
    manager.Initialize();
    
    manager.RegisterResourceAlias("icon", "images/icon.png");
    QString resolved = manager.ResolveAlias("icon");
    EXPECT_EQ(resolved.toStdString(), "images/icon.png");
    
    manager.UnregisterResourceAlias("icon");
    resolved = manager.ResolveAlias("icon");
    EXPECT_EQ(resolved.toStdString(), "icon");
    
    manager.Finalize();
}

TEST_F(QtPlatformTest, ResourceManagerPixmapCache) {
    auto& manager = QtResourceManager::Instance();
    manager.Initialize();
    
    QPixmap testPixmap(100, 100);
    testPixmap.fill(Qt::red);
    
    manager.CachePixmap("test_pixmap", testPixmap);
    
    QPixmap cached = manager.GetCachedPixmap("test_pixmap");
    EXPECT_FALSE(cached.isNull());
    EXPECT_EQ(cached.width(), 100);
    EXPECT_EQ(cached.height(), 100);
    
    manager.ClearPixmapCache();
    cached = manager.GetCachedPixmap("test_pixmap");
    EXPECT_TRUE(cached.isNull());
    
    manager.Finalize();
}

TEST_F(QtPlatformTest, ResourceManagerImageCache) {
    auto& manager = QtResourceManager::Instance();
    manager.Initialize();
    
    QImage testImage(100, 100, QImage::Format_ARGB32);
    testImage.fill(Qt::blue);
    
    manager.CacheImage("test_image", testImage);
    
    QImage cached = manager.GetCachedImage("test_image");
    EXPECT_FALSE(cached.isNull());
    EXPECT_EQ(cached.width(), 100);
    EXPECT_EQ(cached.height(), 100);
    
    manager.ClearImageCache();
    cached = manager.GetCachedImage("test_image");
    EXPECT_TRUE(cached.isNull());
    
    manager.Finalize();
}

TEST_F(QtPlatformTest, ResourceManagerCacheSize) {
    auto& manager = QtResourceManager::Instance();
    manager.Initialize();
    
    manager.SetPixmapCacheSize(50);
    manager.SetImageCacheSize(50);
    
    manager.Finalize();
}

TEST_F(QtPlatformTest, ResourceManagerTextFile) {
    auto& manager = QtResourceManager::Instance();
    manager.Initialize();
    
    QString testContent = "Hello, World!";
    QString tempPath = QDir::tempPath() + "/test_text_file.txt";
    
    EXPECT_TRUE(manager.SaveTextFile(tempPath, testContent));
    
    QString loadedContent = manager.LoadTextFile(tempPath);
    EXPECT_EQ(loadedContent.toStdString(), testContent.toStdString());
    
    QFile::remove(tempPath);
    manager.Finalize();
}

TEST_F(QtPlatformTest, ResourceManagerBinaryFile) {
    auto& manager = QtResourceManager::Instance();
    manager.Initialize();
    
    QByteArray testData = "Binary data content";
    QString tempPath = QDir::tempPath() + "/test_binary_file.bin";
    
    EXPECT_TRUE(manager.SaveBinaryFile(tempPath, testData));
    
    QByteArray loadedData = manager.LoadBinaryFile(tempPath);
    EXPECT_EQ(loadedData, testData);
    
    QFile::remove(tempPath);
    manager.Finalize();
}

TEST_F(QtPlatformTest, ResourceManagerClearAllCaches) {
    auto& manager = QtResourceManager::Instance();
    manager.Initialize();
    
    QPixmap pixmap(10, 10);
    QImage image(10, 10, QImage::Format_ARGB32);
    
    manager.CachePixmap("p1", pixmap);
    manager.CacheImage("i1", image);
    
    manager.ClearAllCaches();
    
    EXPECT_TRUE(manager.GetCachedPixmap("p1").isNull());
    EXPECT_TRUE(manager.GetCachedImage("i1").isNull());
    
    manager.Finalize();
}

#else

TEST(QtPlatformTest, QtNotAvailable) {
    GTEST_SKIP() << "Qt not available, skipping Qt platform tests";
}

#endif

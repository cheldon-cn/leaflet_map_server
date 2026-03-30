#include "ogc/mokrender/data_generator.h"
#include "ogc/mokrender/point_generator.h"
#include "ogc/mokrender/line_generator.h"
#include "ogc/mokrender/polygon_generator.h"
#include "ogc/mokrender/annotation_generator.h"
#include "ogc/mokrender/raster_generator.h"
#include "ogc/mokrender/multipoint_generator.h"
#include "ogc/mokrender/multilinestring_generator.h"
#include "ogc/mokrender/multipolygon_generator.h"
#include "ogc/mokrender/database_manager.h"
#include "ogc/mokrender/spatial_query_engine.h"
#include "ogc/mokrender/symbolizer_factory.h"
#include "ogc/mokrender/render_context.h"
#include "ogc/mokrender/raster_device_output.h"
#include "ogc/mokrender/pdf_device_output.h"
#include "ogc/mokrender/display_device_output.h"
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <sys/stat.h>
#include <sys/types.h>

#ifdef _WIN32
#include <direct.h>
#define MKDIR(path) _mkdir(path)
#else
#define MKDIR(path) mkdir(path, 0755)
#endif

using namespace ogc::mokrender;

static void EnsureDirectoryExists(const std::string& path) {
    size_t pos = path.find_last_of("/\\");
    if (pos != std::string::npos) {
        std::string dir = path.substr(0, pos);
        MKDIR(dir.c_str());
    }
}

void PrintResult(const MokRenderResult& result, const std::string& operation) {
    if (result.IsSuccess()) {
        std::cout << "[OK] " << operation << " succeeded" << std::endl;
    } else {
        std::cout << "[ERROR] " << operation << " failed: " << result.message << std::endl;
    }
}

int main(int argc, char* argv[]) {
    std::cout << "=== Mock Data Render Demo ===" << std::endl;
    std::cout << std::endl;
    
    std::string outputDir = "。/output";
    std::string dbPath = outputDir + "/sqlite_demo.db";
    std::string outputPath = outputDir + "/output.png";
    
    EnsureDirectoryExists(dbPath);
    EnsureDirectoryExists(outputPath);
    
    double minX = 0.0, minY = 0.0, maxX = 1000.0, maxY = 1000.0;
    int srid = 4326;
    
    std::cout << "Step 1: Initialize Database Manager..." << std::endl;
    DatabaseManager dbManager;
    MokRenderResult result = dbManager.Initialize(dbPath);
    PrintResult(result, "Database initialization");
    
    if (result.IsError()) {
        return 1;
    }
    
    std::cout << std::endl;
    std::cout << "Step 2: Create spatial tables..." << std::endl;
    
    result = dbManager.CreateTable("points", "Point");
    PrintResult(result, "Create points table");
    
    result = dbManager.CreateTable("lines", "LineString");
    PrintResult(result, "Create lines table");
    
    result = dbManager.CreateTable("polygons", "Polygon");
    PrintResult(result, "Create polygons table");
    
    result = dbManager.CreateTable("annotations", "Point");
    PrintResult(result, "Create annotations table");
    
    result = dbManager.CreateTable("multipoints", "MultiPoint");
    PrintResult(result, "Create multipoints table");
    
    result = dbManager.CreateTable("multilinestrings", "MultiLineString");
    PrintResult(result, "Create multilinestrings table");
    
    result = dbManager.CreateTable("multipolygons", "MultiPolygon");
    PrintResult(result, "Create multipolygons table");
    
    std::cout << std::endl;
    std::cout << "Step 3: Generate mock data..." << std::endl;
    
    PointGenerator pointGen;
    result = pointGen.Initialize(minX, minY, maxX, maxY, srid);
    PrintResult(result, "Point generator initialization");
    
    std::vector<void*> points;
    for (int i = 0; i < 20; ++i) {
        void* pt = pointGen.GenerateFeature();
        if (pt) {
            points.push_back(pt);
        }
    }
    std::cout << "Generated " << points.size() << " points" << std::endl;
    
    LineGenerator lineGen;
    result = lineGen.Initialize(minX, minY, maxX, maxY, srid);
    PrintResult(result, "Line generator initialization");
    
    std::vector<void*> lines;
    for (int i = 0; i < 20; ++i) {
        void* line = lineGen.GenerateFeature();
        if (line) {
            lines.push_back(line);
        }
    }
    std::cout << "Generated " << lines.size() << " lines" << std::endl;
    
    PolygonGenerator polygonGen;
    result = polygonGen.Initialize(minX, minY, maxX, maxY, srid);
    PrintResult(result, "Polygon generator initialization");
    
    std::vector<void*> polygons;
    for (int i = 0; i < 20; ++i) {
        void* polygon = polygonGen.GenerateFeature();
        if (polygon) {
            polygons.push_back(polygon);
        }
    }
    std::cout << "Generated " << polygons.size() << " polygons" << std::endl;
    
    AnnotationGenerator annotationGen;
    result = annotationGen.Initialize(minX, minY, maxX, maxY, srid);
    PrintResult(result, "Annotation generator initialization");
    
    std::vector<void*> annotations;
    for (int i = 0; i < 20; ++i) {
        void* annotation = annotationGen.GenerateFeature();
        if (annotation) {
            annotations.push_back(annotation);
        }
    }
    std::cout << "Generated " << annotations.size() << " annotations" << std::endl;
    
    RasterGenerator rasterGen;
    result = rasterGen.Initialize(minX, minY, maxX, maxY, srid);
    PrintResult(result, "Raster generator initialization");
    
    std::vector<void*> rasters;
    for (int i = 0; i < 20; ++i) {
        void* raster = rasterGen.GenerateFeature();
        if (raster) {
            rasters.push_back(raster);
        }
    }
    std::cout << "Generated " << rasters.size() << " rasters" << std::endl;
    
    MultiPointGenerator multiPointGen;
    result = multiPointGen.Initialize(minX, minY, maxX, maxY, srid);
    PrintResult(result, "MultiPoint generator initialization");
    
    std::vector<void*> multipoints;
    for (int i = 0; i < 10; ++i) {
        void* mpt = multiPointGen.GenerateFeature();
        if (mpt) {
            multipoints.push_back(mpt);
        }
    }
    std::cout << "Generated " << multipoints.size() << " multipoints" << std::endl;
    
    MultiLineStringGenerator multiLineGen;
    result = multiLineGen.Initialize(minX, minY, maxX, maxY, srid);
    PrintResult(result, "MultiLineString generator initialization");
    
    std::vector<void*> multilinestrings;
    for (int i = 0; i < 10; ++i) {
        void* mline = multiLineGen.GenerateFeature();
        if (mline) {
            multilinestrings.push_back(mline);
        }
    }
    std::cout << "Generated " << multilinestrings.size() << " multilinestrings" << std::endl;
    
    MultiPolygonGenerator multiPolygonGen;
    result = multiPolygonGen.Initialize(minX, minY, maxX, maxY, srid);
    PrintResult(result, "MultiPolygon generator initialization");
    
    std::vector<void*> multipolygons;
    for (int i = 0; i < 10; ++i) {
        void* mpoly = multiPolygonGen.GenerateFeature();
        if (mpoly) {
            multipolygons.push_back(mpoly);
        }
    }
    std::cout << "Generated " << multipolygons.size() << " multipolygons" << std::endl;
    
    std::cout << std::endl;
    std::cout << "Step 4: Store data in database..." << std::endl;
    
    result = dbManager.InsertFeatures("points", points);
    PrintResult(result, "Insert points");
    
    result = dbManager.InsertFeatures("lines", lines);
    PrintResult(result, "Insert lines");
    
    result = dbManager.InsertFeatures("polygons", polygons);
    PrintResult(result, "Insert polygons");
    
    result = dbManager.InsertFeatures("annotations", annotations);
    PrintResult(result, "Insert annotations");
    
    result = dbManager.InsertFeatures("multipoints", multipoints);
    PrintResult(result, "Insert multipoints");
    
    result = dbManager.InsertFeatures("multilinestrings", multilinestrings);
    PrintResult(result, "Insert multilinestrings");
    
    result = dbManager.InsertFeatures("multipolygons", multipolygons);
    PrintResult(result, "Insert multipolygons");
    
    std::cout << std::endl;
    std::cout << "Step 5: Query visible data..." << std::endl;
    
    SpatialQueryEngine queryEngine;
    result = queryEngine.Initialize(dbPath);
    PrintResult(result, "Spatial query engine initialization");
    
    std::vector<void*> visiblePoints = queryEngine.QueryByExtent(0, 0, 500, 500, "points");
    std::cout << "Found " << visiblePoints.size() << " visible points in extent (0,0,500,500)" << std::endl;
    
    std::vector<void*> visibleLines = queryEngine.QueryByExtent(0, 0, 500, 500, "lines");
    std::cout << "Found " << visibleLines.size() << " visible lines in extent (0,0,500,500)" << std::endl;
    
    std::vector<void*> visiblePolygons = queryEngine.QueryByExtent(0, 0, 500, 500, "polygons");
    std::cout << "Found " << visiblePolygons.size() << " visible polygons in extent (0,0,500,500)" << std::endl;
    
    std::cout << std::endl;
    std::cout << "Step 6: Initialize symbolizer factory..." << std::endl;
    
    SymbolizerFactory symbolizerFactory;
    result = symbolizerFactory.Initialize();
    PrintResult(result, "Symbolizer factory initialization");
    
    void* pointSymbolizer = symbolizerFactory.CreatePointSymbolizer();
    std::cout << "Created point symbolizer: " << (pointSymbolizer ? "OK" : "NULL") << std::endl;
    
    void* lineSymbolizer = symbolizerFactory.CreateLineSymbolizer();
    std::cout << "Created line symbolizer: " << (lineSymbolizer ? "OK" : "NULL") << std::endl;
    
    void* polygonSymbolizer = symbolizerFactory.CreatePolygonSymbolizer();
    std::cout << "Created polygon symbolizer: " << (polygonSymbolizer ? "OK" : "NULL") << std::endl;
    
    std::cout << std::endl;
    std::cout << "Step 7: Initialize render context..." << std::endl;
    
    RenderConfig renderConfig;
    renderConfig.outputWidth = 800;
    renderConfig.outputHeight = 600;
    renderConfig.dpi = 96.0;
    
    RenderContext renderContext;
    result = renderContext.Initialize(renderConfig);
    PrintResult(result, "Render context initialization");
    
    renderContext.SetExtent(0, 0, 1000, 1000);
    std::cout << "Set extent: (0, 0, 1000, 1000)" << std::endl;
    
    std::cout << std::endl;
    std::cout << "Step 8: Render to different devices..." << std::endl;
    
    RasterDeviceOutput rasterOutput;
    result = rasterOutput.Initialize(renderConfig);
    PrintResult(result, "Raster device output initialization");
    
    result = rasterOutput.Render(outputPath);
    PrintResult(result, "Render to raster device");
    
    PdfDeviceOutput pdfOutput;
    result = pdfOutput.Initialize(renderConfig);
    PrintResult(result, "PDF device output initialization");
    
    result = pdfOutput.Render(outputDir + "/pdf_output.pdf");
    PrintResult(result, "Render to PDF device");
    
    DisplayDeviceOutput displayOutput;
    result = displayOutput.Initialize(renderConfig);
    PrintResult(result, "Display device output initialization");
    
    result = displayOutput.Render();
    PrintResult(result, "Render to display device");
    
    std::cout << std::endl;
    std::cout << "=== Demo Complete ===" << std::endl;
    std::cout << "Total mock data generated: ~130 features" << std::endl;
    std::cout << "Database: " << dbPath << std::endl;
    std::cout << "Output: " << outputPath << std::endl;
    
    queryEngine.Close();
    dbManager.Close();
    
    return 0;
}

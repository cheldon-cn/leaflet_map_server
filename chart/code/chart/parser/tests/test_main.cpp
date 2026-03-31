#include <gtest/gtest.h>
#include <ogrsf_frmts.h>

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    
    GDALAllRegister();
    
    int result = RUN_ALL_TESTS();
    
    GDALDestroyDriverManager();
    
    return result;
}

package cn.cycle.echart.core;

import org.junit.Test;
import static org.junit.Assert.*;

public class ErrorCodeTest {

    @Test
    public void testErrorCode() {
        assertEquals(1000, ErrorCode.UNKNOWN_ERROR.getCode());
        assertEquals("未知错误", ErrorCode.UNKNOWN_ERROR.getMessage());
    }

    @Test
    public void testFromCode() {
        assertEquals(ErrorCode.UNKNOWN_ERROR, ErrorCode.fromCode(1000));
        assertEquals(ErrorCode.FILE_NOT_FOUND, ErrorCode.fromCode(2000));
        assertEquals(ErrorCode.CHART_LOAD_FAILED, ErrorCode.fromCode(3000));
    }

    @Test
    public void testFromCodeNotFound() {
        assertEquals(ErrorCode.UNKNOWN_ERROR, ErrorCode.fromCode(99999));
    }

    @Test
    public void testToString() {
        String str = ErrorCode.FILE_NOT_FOUND.toString();
        assertTrue(str.contains("2000"));
        assertTrue(str.contains("文件未找到"));
    }

    @Test
    public void testErrorCodeRanges() {
        for (ErrorCode code : ErrorCode.values()) {
            int codeValue = code.getCode();
            if (codeValue >= 1000 && codeValue < 2000) {
                assertTrue(code.name().contains("ERROR") || 
                          code.name().contains("FAILED") ||
                          code.name().contains("INIT") ||
                          code.name().contains("RESOURCE") ||
                          code.name().contains("ARGUMENT") ||
                          code.name().contains("NULL") ||
                          code.name().contains("ILLEGAL") ||
                          code.name().contains("INTERNAL") ||
                          code.name().contains("CONFIGURATION"));
            }
        }
    }
}

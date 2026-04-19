package cn.cycle.echart.core;

import org.junit.Before;
import org.junit.Test;
import static org.junit.Assert.*;

public class DefaultErrorHandlerTest {

    private DefaultErrorHandler handler;

    @Before
    public void setUp() {
        handler = new DefaultErrorHandler();
    }

    @Test
    public void testHandleError() {
        handler.handleError(ErrorCode.FILE_NOT_FOUND, "test message");
        
        assertEquals(ErrorCode.FILE_NOT_FOUND, handler.getLastErrorCode());
        assertEquals("test message", handler.getLastErrorMessage());
    }

    @Test
    public void testHandleErrorWithCause() {
        Exception cause = new RuntimeException("test cause");
        handler.handleError(ErrorCode.INTERNAL_ERROR, "error message", cause);
        
        assertEquals(ErrorCode.INTERNAL_ERROR, handler.getLastErrorCode());
        assertEquals("error message", handler.getLastErrorMessage());
    }

    @Test
    public void testClearLastError() {
        handler.handleError(ErrorCode.FILE_NOT_FOUND, "test message");
        
        handler.clearLastError();
        
        assertNull(handler.getLastErrorCode());
        assertNull(handler.getLastErrorMessage());
    }

    @Test
    public void testShouldContinue() {
        assertFalse(handler.shouldContinue(ErrorCode.UNKNOWN_ERROR));
        assertFalse(handler.shouldContinue(ErrorCode.INTERNAL_ERROR));
        assertFalse(handler.shouldContinue(ErrorCode.INITIALIZATION_FAILED));
        assertFalse(handler.shouldContinue(ErrorCode.RESOURCE_NOT_FOUND));
        
        assertTrue(handler.shouldContinue(ErrorCode.FILE_NOT_FOUND));
        assertTrue(handler.shouldContinue(ErrorCode.CHART_LOAD_FAILED));
    }

    @Test(expected = IllegalArgumentException.class)
    public void testHandleErrorNullCode() {
        handler.handleError(null, "message");
    }

    @Test(expected = IllegalArgumentException.class)
    public void testShouldContinueNullCode() {
        handler.shouldContinue(null);
    }
}

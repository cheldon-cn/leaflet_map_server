package cn.cycle.echart.core;

public final class LogUtil {

    private static boolean debugEnabled = true;
    private static boolean infoEnabled = true;
    private static boolean warnEnabled = true;
    private static boolean errorEnabled = true;
    private static boolean eventPublishEnabled = true;

    private LogUtil() {
    }

    public static void setDebugEnabled(boolean enabled) {
        debugEnabled = enabled;
    }

    public static void setInfoEnabled(boolean enabled) {
        infoEnabled = enabled;
    }

    public static void setWarnEnabled(boolean enabled) {
        warnEnabled = enabled;
    }

    public static void setErrorEnabled(boolean enabled) {
        errorEnabled = enabled;
    }

    public static void setEventPublishEnabled(boolean enabled) {
        eventPublishEnabled = enabled;
    }

    public static boolean isDebugEnabled() {
        return debugEnabled;
    }

    public static void debug(String tag, String message) {
        if (debugEnabled) {
            String formattedMsg = "[" + tag + "] " + message;
            System.out.println(formattedMsg);
            publishLogEvent(AppEventType.LOG_DEBUG, formattedMsg);
        }
    }

    public static void debug(String tag, String format, Object... args) {
        if (debugEnabled) {
            String formattedMsg = "[" + tag + "] " + String.format(format, args);
            System.out.println(formattedMsg);
            publishLogEvent(AppEventType.LOG_DEBUG, formattedMsg);
        }
    }

    public static void info(String tag, String message) {
        if (infoEnabled) {
            String formattedMsg = "[" + tag + "] INFO: " + message;
            System.out.println(formattedMsg);
            publishLogEvent(AppEventType.LOG_INFO, formattedMsg);
        }
    }

    public static void info(String tag, String format, Object... args) {
        if (infoEnabled) {
            String formattedMsg = "[" + tag + "] INFO: " + String.format(format, args);
            System.out.println(formattedMsg);
            publishLogEvent(AppEventType.LOG_INFO, formattedMsg);
        }
    }

    public static void warn(String tag, String message) {
        if (warnEnabled) {
            String formattedMsg = "[" + tag + "] WARN: " + message;
            System.out.println(formattedMsg);
            publishLogEvent(AppEventType.LOG_WARN, formattedMsg);
        }
    }

    public static void warn(String tag, String format, Object... args) {
        if (warnEnabled) {
            String formattedMsg = "[" + tag + "] WARN: " + String.format(format, args);
            System.out.println(formattedMsg);
            publishLogEvent(AppEventType.LOG_WARN, formattedMsg);
        }
    }

    public static void error(String tag, String message) {
        if (errorEnabled) {
            String formattedMsg = "[" + tag + "] ERROR: " + message;
            System.err.println(formattedMsg);
            publishLogEvent(AppEventType.LOG_ERROR, formattedMsg);
        }
    }

    public static void error(String tag, String message, Throwable t) {
        if (errorEnabled) {
            String formattedMsg = "[" + tag + "] ERROR: " + message;
            System.err.println(formattedMsg);
            if (t != null) {
                t.printStackTrace(System.err);
            }
            publishLogEvent(AppEventType.LOG_ERROR, formattedMsg);
        }
    }

    public static void error(String tag, String format, Object... args) {
        if (errorEnabled) {
            String formattedMsg = "[" + tag + "] ERROR: " + String.format(format, args);
            System.err.println(formattedMsg);
            publishLogEvent(AppEventType.LOG_ERROR, formattedMsg);
        }
    }

    private static void publishLogEvent(AppEventType type, String message) {
        if (!eventPublishEnabled) {
            return;
        }
        try {
            AppEventBus bus = AppEventBus.getInstance();
            if (bus != null && bus.isRunning()) {
                bus.publish(new AppEvent(LogUtil.class, type, message));
            }
        } catch (Exception e) {
            System.err.println("[LogUtil] Failed to publish log event: " + e.getMessage());
        }
    }
}

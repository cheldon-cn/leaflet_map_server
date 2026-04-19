package cn.cycle.echart.core;

/**
 * 平台适配器接口。
 * 
 * <p>定义平台相关操作的抽象接口，使核心代码可以跨平台使用。</p>
 * 
 * <h2>设计目的</h2>
 * <p>核心JAR包（如echart-core、echart-alarm等）不应依赖JavaFX等平台特定库。
 * 通过此接口，核心代码可以调用平台功能，而具体实现由UI层提供。</p>
 * 
 * <h2>使用示例</h2>
 * <pre>{@code
 * // 核心代码
 * public class AlarmNotifier {
 *     private PlatformAdapter platform;
 *     
 *     public void notify(Alarm alarm) {
 *         platform.runOnUiThread(() -> {
 *             platform.playSound("alarm.wav");
 *         });
 *     }
 * }
 * 
 * // UI层实现（JavaFX）
 * public class FxPlatformAdapter implements PlatformAdapter {
 *     public void runOnUiThread(Runnable task) {
 *         Platform.runLater(task);
 *     }
 *     public void playSound(String path) {
 *         // JavaFX AudioClip实现
 *     }
 * }
 * }</pre>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public interface PlatformAdapter {

    /**
     * 获取平台名称。
     * 
     * @return 平台名称，如"JavaFX"、"Android"、"Web"
     */
    String getPlatformName();

    /**
     * 在UI线程执行任务。
     * 
     * @param task 要执行的任务，不能为null
     * @throws IllegalArgumentException 如果task为null
     */
    void runOnUiThread(Runnable task);

    /**
     * 检查当前是否在UI线程。
     * 
     * @return 如果在UI线程返回true
     */
    boolean isUiThread();

    /**
     * 播放声音。
     * 
     * @param soundPath 声音文件路径，不能为null
     * @throws IllegalArgumentException 如果soundPath为null
     */
    void playSound(String soundPath);

    /**
     * 停止播放声音。
     */
    void stopSound();

    /**
     * 显示通知。
     * 
     * @param title 通知标题
     * @param message 通知内容
     * @param type 通知类型（info、warning、error）
     */
    void showNotification(String title, String message, String type);

    /**
     * 获取屏幕DPI。
     * 
     * @return 屏幕DPI值
     */
    double getScreenDpi();

    /**
     * 获取屏幕缩放因子。
     * 
     * @return 缩放因子（1.0为标准，2.0为高DPI）
     */
    double getScreenScale();

    /**
     * 获取资源路径。
     * 
     * @param relativePath 相对路径
     * @return 绝对路径
     */
    String getResourcePath(String relativePath);

    /**
     * 获取配置目录。
     * 
     * @return 配置目录路径
     */
    String getConfigDirectory();

    /**
     * 获取日志目录。
     * 
     * @return 日志目录路径
     */
    String getLogDirectory();

    /**
     * 获取缓存目录。
     * 
     * @return 缓存目录路径
     */
    String getCacheDirectory();
}

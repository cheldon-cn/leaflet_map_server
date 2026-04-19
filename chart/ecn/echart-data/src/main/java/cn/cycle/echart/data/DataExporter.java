package cn.cycle.echart.data;

import java.io.OutputStream;
import java.util.List;

/**
 * 数据导出接口。
 * 
 * <p>定义数据导出的通用接口。</p>
 * 
 * @param <T> 导出数据类型
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public interface DataExporter<T> {

    /**
     * 获取支持的格式列表。
     * 
     * @return 格式列表
     */
    List<String> getSupportedFormats();

    /**
     * 检查是否支持指定格式。
     * 
     * @param format 格式
     * @return 是否支持
     */
    boolean supportsFormat(String format);

    /**
     * 导出数据到文件。
     * 
     * @param data 数据列表
     * @param filePath 文件路径
     * @throws DataExportException 如果导出失败
     */
    void exportToFile(List<T> data, String filePath) throws DataExportException;

    /**
     * 导出数据到输出流。
     * 
     * @param data 数据列表
     * @param outputStream 输出流
     * @param format 数据格式
     * @throws DataExportException 如果导出失败
     */
    void exportToStream(List<T> data, OutputStream outputStream, String format) throws DataExportException;

    /**
     * 获取导出进度。
     * 
     * @return 进度（0-100）
     */
    int getProgress();

    /**
     * 取消导出。
     */
    void cancel();
}

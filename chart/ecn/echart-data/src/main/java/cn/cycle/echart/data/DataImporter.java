package cn.cycle.echart.data;

import java.io.InputStream;
import java.util.List;

/**
 * 数据导入接口。
 * 
 * <p>定义数据导入的通用接口。</p>
 * 
 * @param <T> 导入数据类型
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public interface DataImporter<T> {

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
     * 从文件导入数据。
     * 
     * @param filePath 文件路径
     * @return 导入的数据列表
     * @throws DataImportException 如果导入失败
     */
    List<T> importFromFile(String filePath) throws DataImportException;

    /**
     * 从输入流导入数据。
     * 
     * @param inputStream 输入流
     * @param format 数据格式
     * @return 导入的数据列表
     * @throws DataImportException 如果导入失败
     */
    List<T> importFromStream(InputStream inputStream, String format) throws DataImportException;

    /**
     * 获取导入进度。
     * 
     * @return 进度（0-100）
     */
    int getProgress();

    /**
     * 取消导入。
     */
    void cancel();
}

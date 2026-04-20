package cn.cycle.echart.alarm;

import java.time.LocalTime;

/**
 * 音频播放器接口。
 * 
 * <p>平台无关的音频播放接口，用于预警声音播放。
 * 实现类由各平台提供（如JavaFX、Android、Web）。</p>
 * 
 * <p>设计原则：接口定义在业务功能层（echart-alarm.jar），
 * 具体实现在UI层（如echart-ui.jar的FxAudioPlayer）。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public interface AudioPlayer {

    void playAlarmSound(AlarmLevel level);

    void stopCriticalSound();

    void stopAllSounds();

    void setSoundEnabled(boolean enabled);

    boolean isSoundEnabled();

    void setVolume(double volume);

    double getVolume();

    void setMutePeriod(LocalTime start, LocalTime end);

    void clearMutePeriod();

    boolean isInMutePeriod();

    void playTestSound();

    void preloadSounds();
}

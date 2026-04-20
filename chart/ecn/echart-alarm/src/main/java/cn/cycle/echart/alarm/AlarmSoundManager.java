package cn.cycle.echart.alarm;

import java.time.LocalTime;
import java.util.Objects;

/**
 * 预警声音管理器。
 * 
 * <p>业务层的预警声音管理，使用AudioPlayer接口实现平台无关的声音播放。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class AlarmSoundManager {

    private static final AlarmSoundManager INSTANCE = new AlarmSoundManager();

    private AudioPlayer audioPlayer;
    private boolean soundEnabled = true;
    private double volume = 0.5;
    private LocalTime muteStart = null;
    private LocalTime muteEnd = null;

    private AlarmSoundManager() {
    }

    public static AlarmSoundManager getInstance() {
        return INSTANCE;
    }

    public void setAudioPlayer(AudioPlayer player) {
        this.audioPlayer = player;
        if (player != null) {
            player.setSoundEnabled(soundEnabled);
            player.setVolume(volume);
            if (muteStart != null && muteEnd != null) {
                player.setMutePeriod(muteStart, muteEnd);
            }
        }
    }

    public AudioPlayer getAudioPlayer() {
        return audioPlayer;
    }

    public void playAlarmSound(AlarmLevel level) {
        if (audioPlayer != null && soundEnabled && !isInMutePeriod()) {
            audioPlayer.playAlarmSound(level);
        }
    }

    public void stopCriticalSound() {
        if (audioPlayer != null) {
            audioPlayer.stopCriticalSound();
        }
    }

    public void stopAllSounds() {
        if (audioPlayer != null) {
            audioPlayer.stopAllSounds();
        }
    }

    public void setSoundEnabled(boolean enabled) {
        this.soundEnabled = enabled;
        if (audioPlayer != null) {
            audioPlayer.setSoundEnabled(enabled);
        }
    }

    public boolean isSoundEnabled() {
        return soundEnabled;
    }

    public void setVolume(double volume) {
        this.volume = Math.max(0, Math.min(1, volume));
        if (audioPlayer != null) {
            audioPlayer.setVolume(this.volume);
        }
    }

    public double getVolume() {
        return volume;
    }

    public void setMutePeriod(LocalTime start, LocalTime end) {
        this.muteStart = start;
        this.muteEnd = end;
        if (audioPlayer != null) {
            audioPlayer.setMutePeriod(start, end);
        }
    }

    public void clearMutePeriod() {
        this.muteStart = null;
        this.muteEnd = null;
        if (audioPlayer != null) {
            audioPlayer.clearMutePeriod();
        }
    }

    public boolean isInMutePeriod() {
        if (muteStart == null || muteEnd == null) {
            return false;
        }
        LocalTime now = LocalTime.now();
        if (muteStart.isBefore(muteEnd)) {
            return now.isAfter(muteStart) && now.isBefore(muteEnd);
        } else {
            return now.isAfter(muteStart) || now.isBefore(muteEnd);
        }
    }

    public void playTestSound() {
        if (audioPlayer != null) {
            audioPlayer.playTestSound();
        }
    }

    public void preloadSounds() {
        if (audioPlayer != null) {
            audioPlayer.preloadSounds();
        }
    }
}

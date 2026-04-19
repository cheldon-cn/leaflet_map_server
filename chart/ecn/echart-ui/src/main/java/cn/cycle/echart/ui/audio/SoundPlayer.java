package cn.cycle.echart.ui.audio;

import javafx.scene.media.AudioClip;
import javafx.scene.media.Media;
import javafx.scene.media.MediaPlayer;

import java.net.URL;
import java.util.HashMap;
import java.util.Map;
import java.util.Objects;

/**
 * 声音播放器。
 * 
 * <p>管理预警声音的播放。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class SoundPlayer {

    private final Map<String, AudioClip> soundCache;
    private MediaPlayer mediaPlayer;
    private double volume;
    private boolean muted;
    private boolean loopEnabled;

    public SoundPlayer() {
        this.soundCache = new HashMap<>();
        this.volume = 1.0;
        this.muted = false;
        this.loopEnabled = false;
    }

    public void loadSound(String soundId, String resourcePath) {
        Objects.requireNonNull(soundId, "soundId cannot be null");
        Objects.requireNonNull(resourcePath, "resourcePath cannot be null");
        
        URL resource = getClass().getResource(resourcePath);
        if (resource != null) {
            AudioClip clip = new AudioClip(resource.toExternalForm());
            soundCache.put(soundId, clip);
        }
    }

    public void play(String soundId) {
        if (muted) {
            return;
        }
        
        AudioClip clip = soundCache.get(soundId);
        if (clip != null) {
            clip.setVolume(volume);
            clip.play();
        }
    }

    public void playLoop(String soundId) {
        if (muted) {
            return;
        }
        
        AudioClip clip = soundCache.get(soundId);
        if (clip != null) {
            clip.setVolume(volume);
            clip.setCycleCount(AudioClip.INDEFINITE);
            clip.play();
            loopEnabled = true;
        }
    }

    public void stop(String soundId) {
        AudioClip clip = soundCache.get(soundId);
        if (clip != null) {
            clip.stop();
            loopEnabled = false;
        }
    }

    public void stopAll() {
        for (AudioClip clip : soundCache.values()) {
            clip.stop();
        }
        if (mediaPlayer != null) {
            mediaPlayer.stop();
        }
        loopEnabled = false;
    }

    public void playFile(String filePath) {
        if (muted) {
            return;
        }
        
        try {
            if (mediaPlayer != null) {
                mediaPlayer.stop();
            }
            
            Media media = new Media(new java.io.File(filePath).toURI().toString());
            mediaPlayer = new MediaPlayer(media);
            mediaPlayer.setVolume(volume);
            mediaPlayer.play();
        } catch (Exception e) {
        }
    }

    public void playFileLoop(String filePath) {
        if (muted) {
            return;
        }
        
        try {
            if (mediaPlayer != null) {
                mediaPlayer.stop();
            }
            
            Media media = new Media(new java.io.File(filePath).toURI().toString());
            mediaPlayer = new MediaPlayer(media);
            mediaPlayer.setVolume(volume);
            mediaPlayer.setCycleCount(MediaPlayer.INDEFINITE);
            mediaPlayer.play();
            loopEnabled = true;
        } catch (Exception e) {
        }
    }

    public void stopFile() {
        if (mediaPlayer != null) {
            mediaPlayer.stop();
            loopEnabled = false;
        }
    }

    public double getVolume() {
        return volume;
    }

    public void setVolume(double volume) {
        this.volume = Math.max(0, Math.min(1, volume));
        
        if (mediaPlayer != null) {
            mediaPlayer.setVolume(this.volume);
        }
    }

    public boolean isMuted() {
        return muted;
    }

    public void setMuted(boolean muted) {
        this.muted = muted;
        
        if (muted) {
            stopAll();
        }
    }

    public boolean isLoopEnabled() {
        return loopEnabled;
    }

    public void unloadSound(String soundId) {
        AudioClip clip = soundCache.remove(soundId);
        if (clip != null) {
            clip.stop();
        }
    }

    public void unloadAll() {
        stopAll();
        soundCache.clear();
    }

    public boolean isSoundLoaded(String soundId) {
        return soundCache.containsKey(soundId);
    }
}

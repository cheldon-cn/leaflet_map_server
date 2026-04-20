package cn.cycle.echart.alarm.impl;

import cn.cycle.echart.alarm.Alarm;
import cn.cycle.echart.alarm.AlarmLevel;
import cn.cycle.echart.alarm.AlarmType;

/**
 * 气象预警。
 * 
 * <p>当检测到恶劣天气时触发。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class WeatherAlarm extends Alarm {

    private final String weatherType;
    private final double windSpeedKnots;
    private final double waveHeightMeters;
    private final double visibilityNm;
    private final String forecastSource;
    private final double affectedAreaRadiusNm;

    public WeatherAlarm(String weatherType, double windSpeedKnots,
                        double waveHeightMeters, double visibilityNm,
                        String forecastSource, double affectedAreaRadiusNm) {
        super(AlarmType.WEATHER, determineLevel(windSpeedKnots, waveHeightMeters, visibilityNm),
              "气象预警", buildMessage(weatherType, windSpeedKnots, waveHeightMeters, visibilityNm));
        
        this.weatherType = weatherType;
        this.windSpeedKnots = windSpeedKnots;
        this.waveHeightMeters = waveHeightMeters;
        this.visibilityNm = visibilityNm;
        this.forecastSource = forecastSource;
        this.affectedAreaRadiusNm = affectedAreaRadiusNm;

        withData("weatherType", weatherType);
        withData("windSpeedKnots", windSpeedKnots);
        withData("waveHeightMeters", waveHeightMeters);
        withData("visibilityNm", visibilityNm);
        withData("forecastSource", forecastSource);
        withData("affectedAreaRadiusNm", affectedAreaRadiusNm);
    }

    private static AlarmLevel determineLevel(double windSpeed, double waveHeight, double visibility) {
        if (windSpeed > 50 || waveHeight > 6 || visibility < 0.5) {
            return AlarmLevel.CRITICAL;
        } else if (windSpeed > 34 || waveHeight > 4 || visibility < 1) {
            return AlarmLevel.WARNING;
        } else {
            return AlarmLevel.INFO;
        }
    }

    private static String buildMessage(String weatherType, double windSpeed,
                                        double waveHeight, double visibility) {
        return String.format("%s - 风速 %.1f 节，浪高 %.1f 米，能见度 %.1f 海里",
                weatherType != null ? weatherType : "恶劣天气",
                windSpeed, waveHeight, visibility);
    }

    public String getWeatherType() {
        return weatherType;
    }

    public double getWindSpeedKnots() {
        return windSpeedKnots;
    }

    public double getWaveHeightMeters() {
        return waveHeightMeters;
    }

    public double getVisibilityNm() {
        return visibilityNm;
    }

    public String getForecastSource() {
        return forecastSource;
    }

    public double getAffectedAreaRadiusNm() {
        return affectedAreaRadiusNm;
    }
}

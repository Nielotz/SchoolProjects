package com.nielotz.ligthwakeupalarm.alarm;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.graphics.Color;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.util.Log;
import android.widget.TextView;

import com.nielotz.ligthwakeupalarm.R;

import java.time.Instant;
import java.time.LocalDateTime;
import java.util.Date;
import java.util.concurrent.TimeUnit;

public class AlarmActivity extends AppCompatActivity {

    private final Handler handler = new Handler();
    public static long alarmTimeToFullBrightnessSeconds = 60;
    public static long alarmDurationSeconds = 120;
    public static float alarmMinFontSize = 0.1f;
    public static float alarmMaxFontSize = 120;
    public static float alarmFontSizeIncreasePerSeconds = alarmMaxFontSize / alarmDurationSeconds;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_alarm_activity);

        Intent intent = getIntent();
        long alarmTimeMs = intent.getLongExtra("ALARM_TIME_MS", -1);

        Log.i("AlarmActivity", "onCreate, alarmTime:" + alarmTimeMs);

        updateAlarm(alarmTimeMs);
    }

    private void updateAlarm(long alarmTimeMs) {
        if (alarmTimeMs < 0) {
            Log.wtf("AlarmActivity", "alarmTime is below zero");
            return;
        }

        long secondsPassed = (Date.from(Instant.now()).getTime() - alarmTimeMs) / 1000;
        Log.i("AlarmActivity", "Seconds passed: " + secondsPassed);

        updateFont(secondsPassed);
        updateBackground(secondsPassed);

        if (secondsPassed < alarmDurationSeconds)
            handler.postDelayed(() -> updateAlarm(alarmTimeMs), 1000);
    }

    private void updateFont(long alarmPassedSeconds) {
        float textSize = alarmMinFontSize + alarmFontSizeIncreasePerSeconds * alarmPassedSeconds;
        if (textSize > alarmMaxFontSize)
            textSize = alarmMaxFontSize;

        float color = (float) alarmPassedSeconds / alarmTimeToFullBrightnessSeconds;
        if (color > 1.)
            color = 1.f;

        TextView textView = findViewById(R.id.alarmText);

        textView.setTextSize(textSize);
        textView.setTextColor(Color.rgb(1 - color, 1 - color, 1 - color));
    }

    private void updateBackground(long alarmPassedSeconds) {
        float value = (float) alarmPassedSeconds / alarmTimeToFullBrightnessSeconds;
        if (value > 1.)
            value = 1.f;
        getWindow().getDecorView()
                .setBackgroundColor(Color.rgb(value, value, value));
    }

}
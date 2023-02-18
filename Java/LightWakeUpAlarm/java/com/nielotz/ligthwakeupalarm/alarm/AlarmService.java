package com.nielotz.ligthwakeupalarm.alarm;

import static android.content.Intent.FLAG_ACTIVITY_NEW_TASK;

import static androidx.core.app.ActivityCompat.startActivityForResult;

import android.content.Context;
import android.content.Intent;
import android.net.Uri;
import android.os.Build;
import android.provider.Settings;
import android.util.Log;

import androidx.annotation.NonNull;
import androidx.core.app.JobIntentService;

public class AlarmService extends JobIntentService {
    private static final int UNIQUE_JOB_ID = 1337;

    static void enqueueWork(Context context, Intent intent) {
        enqueueWork(context, AlarmService.class, UNIQUE_JOB_ID, intent);
    }

    @Override
    public void onHandleWork(@NonNull Intent intent) {
        Log.d(getClass().getSimpleName(), "Starting alarm.");
        Intent alarmIntent = new Intent(this.getApplicationContext(), AlarmActivity.class);

        long alarmTime = intent.getLongExtra("ALARM_TIME_MS", -1);
        alarmIntent.putExtra("ALARM_TIME_MS", alarmTime);

        Log.d(getClass().getSimpleName(), "Starting activity.");
        alarmIntent.setFlags(FLAG_ACTIVITY_NEW_TASK);

        startActivity(alarmIntent);
    }
}
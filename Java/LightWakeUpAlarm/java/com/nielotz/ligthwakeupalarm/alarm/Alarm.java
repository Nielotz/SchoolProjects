package com.nielotz.ligthwakeupalarm.alarm;

import static android.app.PendingIntent.FLAG_CANCEL_CURRENT;
import static android.app.PendingIntent.FLAG_IMMUTABLE;
import static android.app.PendingIntent.FLAG_MUTABLE;

import android.app.AlarmManager;
import android.app.PendingIntent;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.os.Build;
import android.util.Log;

import androidx.annotation.RequiresApi;

import java.time.Instant;
import java.time.LocalDateTime;
import java.time.ZoneId;
import java.util.Objects;

public class Alarm extends BroadcastReceiver {
    @Override
    public void onReceive(Context context, Intent intent) {
        Log.i("Alarm", "Received intent, action: " + intent.getAction());
        if (Objects.equals(intent.getAction(), "START_ALARM")) {
            AlarmService.enqueueWork(context, intent);
        } else {
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
                reschedule(context, intent);
            }
        }
    }

    @RequiresApi(api = Build.VERSION_CODES.S)
    private static void reschedule(Context context, Intent intent) {
        Log.i("Alarm", "Rescheduling alarm.");

        long triggerTimeMs = intent.getLongExtra("ALARM_TIME_MS", -1);
        schedule(context,
                LocalDateTime.ofInstant(
                        Instant.ofEpochMilli(triggerTimeMs), ZoneId.systemDefault()));
    }

    @RequiresApi(api = Build.VERSION_CODES.S)
    public static void schedule(Context context, LocalDateTime alarmDateTime) {
        Log.i("Alarm", "Scheduling alarm.");

        long triggerTime = alarmDateTime.atZone(ZoneId.systemDefault()).toInstant().toEpochMilli();
        long currentTime = LocalDateTime
                .now()
                .atZone(ZoneId.systemDefault())
                .toInstant()
                .toEpochMilli();

        Log.i("Alarm", "Alarm time  : " + alarmDateTime + ", ns: " + triggerTime);
        Log.i("Alarm", "Current time: " + LocalDateTime.now() + ", ns: " + currentTime);

        AlarmManager alarmManager = (AlarmManager) context.getSystemService(Context.ALARM_SERVICE);
        Intent intent = new Intent(context, Alarm.class);
        intent.putExtra("ALARM_TIME_MS", triggerTime);
        intent.setAction("START_ALARM");
        PendingIntent pendingIntent = PendingIntent.getBroadcast(
                context, 0, intent, FLAG_MUTABLE | FLAG_CANCEL_CURRENT);

        AlarmManager.AlarmClockInfo alarmClockInfo =
                new AlarmManager.AlarmClockInfo(triggerTime, pendingIntent);
        alarmManager.setAlarmClock(alarmClockInfo, pendingIntent);
    }
}
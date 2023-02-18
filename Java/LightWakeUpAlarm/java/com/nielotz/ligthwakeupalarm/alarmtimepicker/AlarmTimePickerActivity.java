package com.nielotz.ligthwakeupalarm.alarmtimepicker;

import androidx.appcompat.app.AppCompatActivity;

import android.app.AlarmManager;
import android.app.PendingIntent;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.os.SystemClock;
import android.util.Log;
import android.view.View;

import com.nielotz.ligthwakeupalarm.MainActivity;
import com.nielotz.ligthwakeupalarm.R;
import com.nielotz.ligthwakeupalarm.alarm.Alarm;

import java.time.LocalDate;
import java.time.LocalDateTime;
import java.time.LocalTime;

public class AlarmTimePickerActivity extends AppCompatActivity {
    AlarmTimePicker alarmTimePicker = null;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        Log.i("AlarmTimePickerActivity", "onCreate");
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_alarm_time_picker);
        alarmTimePicker = (AlarmTimePicker) this.findViewById(R.id.timePicker);
        alarmTimePicker.setHourMinute24(12, 0);
    }

    public void onConfirmClick(View view) {
        int selectedHour = alarmTimePicker.getHour();
        int selectedMinute = alarmTimePicker.getMinute();

        Log.i("AlarmTimePickerActivity",
                String.format("Selected: %02d:%02d", selectedHour, selectedMinute));

        Alarm.schedule(this.getApplicationContext(),
                LocalDateTime.of(LocalDate.now(),
                        LocalTime.of(selectedHour, selectedMinute))
        );
    }

    public void onCancelClick(View view) {
        Log.i("AlarmTimePickerActivity", "Cancelled, returning to main activity.");
        Intent i = new Intent(this, MainActivity.class);
        startActivity(i);
    }
}

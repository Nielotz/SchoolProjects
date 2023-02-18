package com.nielotz.ligthwakeupalarm;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.View;

import com.nielotz.ligthwakeupalarm.alarmtimepicker.AlarmTimePickerActivity;

public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // Show alarms list
    }

    public void pickAlarmDate(View view) {
        Log.i("MainActivity", "pickAlarmDate");
        Intent intent = new Intent(this, AlarmTimePickerActivity.class);
        startActivity(intent);
    }
}
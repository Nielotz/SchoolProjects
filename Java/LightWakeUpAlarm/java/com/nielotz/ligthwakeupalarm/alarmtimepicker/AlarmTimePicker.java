package com.nielotz.ligthwakeupalarm.alarmtimepicker;

import android.app.Dialog;
import android.app.TimePickerDialog;
import android.content.Context;
import android.content.Intent;
import android.util.AttributeSet;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.TimePicker;

import androidx.fragment.app.DialogFragment;

import com.nielotz.ligthwakeupalarm.R;

public class AlarmTimePicker extends TimePicker implements TimePickerDialog.OnTimeSetListener {
    public AlarmTimePicker(Context context) {
        super(context);
    }

    public AlarmTimePicker(Context context, AttributeSet attrs) {
        super(context, attrs);
    }

    public AlarmTimePicker(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
    }

    public AlarmTimePicker(Context context, AttributeSet attrs, int defStyleAttr, int defStyleRes) {
        super(context, attrs, defStyleAttr, defStyleRes);
    }

    public void setHourMinute24(int hour, int minute) {
        TimePicker timePicker = (TimePicker)this.findViewById(R.id.timePicker);
        timePicker.setIs24HourView(true);
        timePicker.setHour(hour);
        timePicker.setMinute(minute);
    }

    @Override
    public void onTimeSet(TimePicker view, int hourOfDay, int minute) {
        Log.w("AlarmTimePicker", "onTimeSet");
        // No idea when gets called.
    }
}

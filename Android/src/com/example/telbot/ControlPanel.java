package com.example.telbot;

import android.os.Bundle;
import android.app.Activity;
import android.view.Menu;

public class ControlPanel extends Activity {

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_control_panel);
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.activity_control_panel, menu);
		return true;
	}

}

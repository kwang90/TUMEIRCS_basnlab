package com.example.hallo;

import android.app.Activity;
import android.os.Bundle;
import android.view.Menu;
import android.widget.TextView;

public class MainActivity extends Activity {

	static {
		System.loadLibrary("hallomodule");
	}

	private native void initializeModel();

	private native void stepModel();

	private native void setModelInput(double val);

	private native double getModelOutput();

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);

		TextView tv = (TextView) findViewById(R.id.tv);

		initializeModel();
		stepModel();
		setModelInput(42);
		stepModel();
		double val = getModelOutput();
		setModelInput(-13);
		stepModel();
		double val2 = getModelOutput();

		tv.setText("Erstens: " + val + " Zweitens: " + val2);
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.main, menu);
		return true;
	}

}

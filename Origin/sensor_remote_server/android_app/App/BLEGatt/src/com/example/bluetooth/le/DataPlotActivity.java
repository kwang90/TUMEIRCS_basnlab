package com.example.bluetooth.le;

import android.app.Activity;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.graphics.Color;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.os.Bundle;
import android.os.IBinder;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.widget.TextView;

import com.androidplot.ui.LayoutManager;
import com.androidplot.ui.SizeLayoutType;
import com.androidplot.ui.SizeMetrics;
import com.androidplot.xy.FillDirection;
import com.androidplot.xy.LineAndPointFormatter;
import com.androidplot.xy.SimpleXYSeries;
import com.androidplot.xy.XLayoutStyle;
import com.androidplot.xy.XYGraphWidget;
import com.androidplot.xy.XYPlot;
import com.androidplot.xy.YLayoutStyle;
import com.example.bluetooth.le.BluetoothLeService.OnDataAvailableListener;

public class DataPlotActivity extends Activity {
	private final static String TAG = DataPlotActivity.class.getSimpleName();

	public static final String EXTRAS_DEVICE_NAME = "DEVICE_NAME";
	public static final String EXTRAS_DEVICE_ADDRESS = "DEVICE_ADDRESS";

	private static double SENSOR_GRAVITY_VAL = 60.0;

	private BluetoothLeService mBluetoothLeService = null;
	private String mDeviceName;
	private String mDeviceAddress;

	private static final int PLOT_HISTORY_SIZE = 600;
	private XYPlot accPlot = null;
	private SimpleXYSeries accSeriesX = null;
	private SimpleXYSeries accSeriesY = null;
	private SimpleXYSeries accSeriesZ = null;
	private XYPlot freqPlot = null;
	private SimpleXYSeries freqSeriesX = null;
	private SimpleXYSeries freqSeriesY = null;
	private SimpleXYSeries freqSeriesZ = null;

	private SensorManager sensorManager;
	private Sensor accelerationSensor;
	
	private TextView activityRecog = null;
	private TextView tvRssi = null;

	static {
		System.loadLibrary("actrec");
	}

	private native void initializeModel();

	private native void stepModel();

	private native void setModelInput(double val, int i);

	private native double getModelOutput(int type, int index);

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.dataplot);

		final Intent intent = getIntent();
		mDeviceName = intent.getStringExtra(EXTRAS_DEVICE_NAME);
		mDeviceAddress = intent.getStringExtra(EXTRAS_DEVICE_ADDRESS);

		// Sets up UI references.
		accPlot = (XYPlot) findViewById(R.id.accPlot);
		freqPlot = (XYPlot) findViewById(R.id.freqPlot);
		activityRecog = (TextView) findViewById(R.id.activityRecog);
		tvRssi = (TextView) findViewById(R.id.rssi);

		sensorManager = (SensorManager) getSystemService(Context.SENSOR_SERVICE);
		accelerationSensor = sensorManager.getDefaultSensor(Sensor.TYPE_LINEAR_ACCELERATION);
		sensorManager.registerListener(sensorEventListener, accelerationSensor, SensorManager.SENSOR_DELAY_FASTEST);

		getActionBar().setTitle(mDeviceName + ": Data");
		getActionBar().setDisplayHomeAsUpEnabled(true);
		initPlot();
		Intent gattServiceIntent = new Intent(this, BluetoothLeService.class);
		bindService(gattServiceIntent, mServiceConnection, BIND_AUTO_CREATE);
	}

	@Override
	protected void onDestroy() {

		unbindService(mServiceConnection);
		sensorManager.unregisterListener(sensorEventListener);

		super.onDestroy();
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		getMenuInflater().inflate(R.menu.dataplot, menu);
		return true;
	}

	@Override
	public boolean onOptionsItemSelected(MenuItem item) {
		switch (item.getItemId()) {
		case android.R.id.home:
			onBackPressed();
			return true;
		case R.id.menu_reconnect:
			if (mBluetoothLeService != null) {
				mBluetoothLeService.disconnect();
				mBluetoothLeService.connect(mDeviceAddress);
			} else {
				Intent gattServiceIntent = new Intent(this, BluetoothLeService.class);
				bindService(gattServiceIntent, mServiceConnection, BIND_AUTO_CREATE);
			}
		}
		return super.onOptionsItemSelected(item);
	}

	// Code to manage Service lifecycle.
	private final ServiceConnection mServiceConnection = new ServiceConnection() {

		@Override
		public void onServiceConnected(ComponentName componentName, IBinder service) {
			mBluetoothLeService = ((BluetoothLeService.LocalBinder) service).getService();
			if (!mBluetoothLeService.initialize()) {
				Log.e(TAG, "Unable to initialize Bluetooth");
				finish();
			}
			// Automatically connects to the device upon successful start-up
			// initialization.
			if (mBluetoothLeService.getServer() == null) mBluetoothLeService.startServer();
			mBluetoothLeService.connect(mDeviceAddress);
			mBluetoothLeService.registerListener(onDataAvailableListener);

		}

		@Override
		public void onServiceDisconnected(ComponentName componentName) {
			mBluetoothLeService.unregisterListener(onDataAvailableListener);

			mBluetoothLeService = null;
		}
	};

	private OnDataAvailableListener onDataAvailableListener = new OnDataAvailableListener() {

		@Override
		public void onDataAvailable(final byte[] data) {

			for (int i = 0; i < 3; i++) {
				accSeriesX.addLast(null, data[3 * i] + SENSOR_GRAVITY_VAL);
				accSeriesY.addLast(null, data[3 * i + 1]);
				accSeriesZ.addLast(null, data[3 * i + 2]);
			}

			// trim plots to maximum size
			while (accSeriesX.size() > PLOT_HISTORY_SIZE)
				accSeriesX.removeFirst();
			while (accSeriesY.size() > PLOT_HISTORY_SIZE)
				accSeriesY.removeFirst();
			while (accSeriesZ.size() > PLOT_HISTORY_SIZE)
				accSeriesZ.removeFirst();

			for (int i = 0; i < 3; i++) {

				setModelInput((Double.valueOf(data[3 * i]) + SENSOR_GRAVITY_VAL) / SENSOR_GRAVITY_VAL, 0);
				setModelInput(Double.valueOf(data[3 * i + 1]) / SENSOR_GRAVITY_VAL, 1);
				setModelInput(Double.valueOf(data[3 * i + 2]) / SENSOR_GRAVITY_VAL, 2);

//				setModelInput(lastPhoneAcc[0] / 9.81, 3);
//				setModelInput(lastPhoneAcc[1] / 9.81, 4);
//				setModelInput(lastPhoneAcc[2] / 9.81, 5);
				
				stepModel();
			}

			int status = (int) getModelOutput(-1, 0);
			String activityString;
			switch (status) {
			case 1:
				activityString = "Walking";
				break;
			case 2:
				activityString = "Running";
				break;
			case 0:
				activityString = "No relevant motion";
				break;
			default:
				activityString = "Unknown";
				break;
			}

			final String string = activityString + "("+getModelOutput(-1, 0)+")";

			for (int i = 1; i < 127; i++) {
				freqSeriesZ.setY(getModelOutput(0, i), i);
//				freqSeriesX.setY(getModelOutput(1, i), i);
			}

			final double var = getModelOutput(-2, 0);
			
			runOnUiThread(new Runnable() {
				@Override
				public void run() {
					accPlot.redraw();
					freqPlot.redraw();
					tvRssi.setText("Rssi: " + String.valueOf(data[18])
							+"\n relevant motion: "+var
							);
					activityRecog.setText(string);
				}
			});

		}
	};

	private void initPlot() {
		LayoutManager lm = accPlot.getLayoutManager();
		XYGraphWidget gw = accPlot.getGraphWidget();
		lm.remove(accPlot.getDomainLabelWidget());
		lm.remove(accPlot.getRangeLabelWidget());
		lm.remove(accPlot.getTitleWidget());
		lm.remove(accPlot.getLegendWidget());
		lm.position(gw, 0, XLayoutStyle.ABSOLUTE_FROM_LEFT, 0, YLayoutStyle.ABSOLUTE_FROM_TOP);
		gw.setSize(new SizeMetrics(0, SizeLayoutType.FILL, 0, SizeLayoutType.FILL));
		gw.setBackgroundPaint(null);
		gw.setGridBackgroundPaint(null);
		gw.setDomainLabelPaint(null);
		gw.setDomainOriginLabelPaint(null);
		// plot.setDrawBorderEnabled(false);
		accPlot.setBackgroundPaint(null);

		lm = freqPlot.getLayoutManager();
		gw = freqPlot.getGraphWidget();
		lm.remove(freqPlot.getDomainLabelWidget());
		lm.remove(freqPlot.getRangeLabelWidget());
		lm.remove(freqPlot.getTitleWidget());
		// lm.remove(freqPlot.getLegendWidget());
		lm.position(gw, 0, XLayoutStyle.ABSOLUTE_FROM_LEFT, 0, YLayoutStyle.ABSOLUTE_FROM_TOP);
		gw.setSize(new SizeMetrics(0, SizeLayoutType.FILL, 0, SizeLayoutType.FILL));
		gw.setBackgroundPaint(null);
		gw.setGridBackgroundPaint(null);
		gw.setDomainLabelPaint(null);
		gw.setDomainOriginLabelPaint(null);
		// plot.setDrawBorderEnabled(false);
		freqPlot.setBackgroundPaint(null);

		accSeriesX = new SimpleXYSeries("X");
		accSeriesY = new SimpleXYSeries("Y");
		accSeriesZ = new SimpleXYSeries("Z");

		accSeriesX.useImplicitXVals();
		accSeriesY.useImplicitXVals();
		accSeriesZ.useImplicitXVals();

		accPlot.addSeries(accSeriesX, new LineAndPointFormatter(Color.RED, null, null, null, FillDirection.BOTTOM));
		accPlot.addSeries(accSeriesY, new LineAndPointFormatter(Color.BLUE, null, null, null, FillDirection.BOTTOM));
		accPlot.addSeries(accSeriesZ, new LineAndPointFormatter(Color.GREEN, null, null, null, FillDirection.BOTTOM));

		freqSeriesX = new SimpleXYSeries("X");
		freqSeriesY = new SimpleXYSeries("Y");
		freqSeriesZ = new SimpleXYSeries("Z");

		for (int i = 1; i < 128; i++) {
			freqSeriesX.addLast(i, 0);
			freqSeriesY.addLast(i, 0);
			freqSeriesZ.addLast(i, 0);
		}

		freqSeriesX.useImplicitXVals();
		freqSeriesY.useImplicitXVals();
		freqSeriesZ.useImplicitXVals();

		freqPlot.addSeries(freqSeriesX, new LineAndPointFormatter(Color.RED, null, null, null, FillDirection.BOTTOM));
		freqPlot.addSeries(freqSeriesY, new LineAndPointFormatter(Color.BLUE, null, null, null, FillDirection.BOTTOM));
		freqPlot.addSeries(freqSeriesZ, new LineAndPointFormatter(Color.GREEN, null, null, null, FillDirection.BOTTOM));

	}

	private float[] lastPhoneAcc;
			
	private SensorEventListener sensorEventListener = new SensorEventListener() {
		
		@Override
		public void onSensorChanged(SensorEvent event) {
			lastPhoneAcc = event.values;
		}
		
		@Override
		public void onAccuracyChanged(Sensor sensor, int accuracy) {
		}
	};
	
}

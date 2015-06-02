package com.rcsexample.basncontrol;

import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.Intent;
import android.os.Bundle;
import android.preference.Preference;
import android.preference.Preference.OnPreferenceChangeListener;
import android.preference.Preference.OnPreferenceClickListener;
import android.preference.PreferenceActivity;
import android.preference.PreferenceCategory;
import android.preference.SwitchPreference;
import android.widget.Toast;

import com.rcsexample.basnlib.BluetoothConnectionService;
import com.rcsexample.basnlib.BluetoothConnectionService.BluetoothConnectionListener;
import com.rcsexample.basnlib.ControlPacket;
import com.rcsexample.basnlib.DeviceListActivity;

/**
 * A {@link PreferenceActivity} that presents a set of application settings. On
 * handset devices, settings are presented as a single list. On tablets,
 * settings are split by category, with category headers shown to the left of
 * the list of settings.
 * <p>
 * See <a href="http://developer.android.com/design/patterns/settings.html">
 * Android Design: Settings</a> for design guidelines and the <a
 * href="http://developer.android.com/guide/topics/ui/settings.html">Settings
 * API Guide</a> for more information on developing a Settings UI.
 */
public class SettingsActivity extends PreferenceActivity implements BluetoothConnectionListener {
	protected static final int INTENT_REQUEST_CHOOSE_DEVICE = 1;
	private BluetoothConnectionService bluetoothService = null;
	private BluetoothAdapter bluetoothAdapter = null;
	private String deviceAddress = "";
	private BluetoothDevice device;

	@SuppressWarnings("deprecation")
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);

		bluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
		bluetoothService = new BluetoothConnectionService();
		bluetoothService.registerListener(this);

		addPreferencesFromResource(R.xml.prefs);

		((SwitchPreference) findPreference("bluetooth")).setChecked(bluetoothAdapter.isEnabled());
		((SwitchPreference) findPreference("connection")).setChecked(false);

		// Set onPreferenceClickListener
		// http://stackoverflow.com/a/15538982
		for (int x = 0; x < getPreferenceScreen().getPreferenceCount(); x++) {
			PreferenceCategory prefCat = (PreferenceCategory) getPreferenceScreen().getPreference(x);
			for (int y = 0; y < prefCat.getPreferenceCount(); y++) {
				Preference pref = prefCat.getPreference(y);
				pref.setOnPreferenceClickListener(onPreferenceClickListener);
				pref.setOnPreferenceChangeListener(onPreferenceChangeListener);
			}
		}
	}

	OnPreferenceChangeListener onPreferenceChangeListener = new OnPreferenceChangeListener() {

		@Override
		public boolean onPreferenceChange(Preference preference, Object newValue) {
			if (preference.getKey().equals("bluetooth")) {
				if (newValue instanceof Boolean && (Boolean) newValue == false) {
					bluetoothAdapter.disable();
					((SwitchPreference) findPreference("connection")).setChecked(false);
				} else {
					bluetoothAdapter.enable();
				}
				return true;
			} else if (preference.getKey().equals("connection")) {
				if (newValue instanceof Boolean && (Boolean) newValue == false) {
					bluetoothService.stop();
					return true;
				} else {
					if (bluetoothAdapter.getState() != BluetoothAdapter.STATE_ON) {
						makeToast("Bluetooth not available");
						return false;
					} else {
						// set up new connection now - start by choosing the
						// remote device (next: onActivityResult())
						Intent chooseDeviceIntent = new Intent(getApplicationContext(), DeviceListActivity.class);
						startActivityForResult(chooseDeviceIntent, INTENT_REQUEST_CHOOSE_DEVICE);
					}
					return true;
				}
			}
			return false;
		}
	};

	OnPreferenceClickListener onPreferenceClickListener = new OnPreferenceClickListener() {
		@Override
		public boolean onPreferenceClick(Preference preference) {
			if (preference.getKey().equals("sleep")) {
				(new ControlPacket(ControlPacket.INSTRUCTION_SLEEP)).build().send(bluetoothService);
				return true;
			} else if (preference.getKey().equals("sendbt")) {
				(new ControlPacket(ControlPacket.INSTRUCTION_START_STOP_MEASURE_SEND_BT)).build().send(bluetoothService);
				return true;
			} else if (preference.getKey().equals("measure")) {
				(new ControlPacket(ControlPacket.INSTRUCTION_START_STOP_MEASURE)).build().send(bluetoothService);
				return true;
			} else if (preference.getKey().equals("transmit")) {
				(new ControlPacket(ControlPacket.INSTRUCTION_SEND_SD)).build().send(bluetoothService);
				return true;
			} else if (preference.getKey().equals("erasesd")) {
				(new ControlPacket(ControlPacket.INSTRUCTION_ERASE_SD)).build().send(bluetoothService);
				return true;
			} else if (preference.getKey().equals("monitor")) {
				makeToast("Not implemented");
				return false;
			}

			return false;
		}
	};

	protected void onActivityResult(int requestCode, int resultCode, Intent data) {
		if (requestCode == INTENT_REQUEST_CHOOSE_DEVICE) {
			// When DeviceListActivity returns with a device to connect
			if (resultCode == Activity.RESULT_OK) {
				deviceAddress = data.getExtras().getString(DeviceListActivity.EXTRA_DEVICE_ADDRESS);
				if (BluetoothAdapter.checkBluetoothAddress(deviceAddress)) {
					device = bluetoothAdapter.getRemoteDevice(deviceAddress);
					bluetoothService.connect(device);
				}
			}
		}
	}

	@Override
	public void onBluetoothConnectionStateChanged(int state) {
		String text = "";
		if (state == BluetoothConnectionService.STATE_CONNECTED)
			text = "Connected.";
		else if (state == BluetoothConnectionService.STATE_CONNECTING)
			text = "Connecting...";
		else if (state == BluetoothConnectionService.STATE_NONE) text = "No connection.";
		makeToast(text);
	}

	@Override
	public void onBluetoothConnectionConnected(String name, String address) {
		makeToast("Connected to: " + name + " (" + address + ")");
	}

	@Override
	public void onBluetoothConnectionFailure(int whatFailed) {
		String text = "";
		if (whatFailed == BluetoothConnectionService.FAILURE_CONNECTION_LOST)
			text = "Bluetooth connection lost";
		else if (whatFailed == BluetoothConnectionService.FAILURE_WRITE_FAILED)
			text = "Bluetooth write failed";
		else if (whatFailed == BluetoothConnectionService.FAILURE_CONNECTING_FAILED) text = "Bluetooth connecting failed";
		makeToast(text);
	}

	@Override
	public void onBluetoothConnectionReceive(byte[] buffer, int numberOfBytesInBuffer) {
	}

	@Override
	public void onBluetoothConnectionWrite(byte[] buffer) {
	}

	/** display toast message. */
	private void makeToast(String text) {
		runOnUiThread(new ToastMaker(text));
	}

	/**
	 * used by {@link MainActivity#makeToast(String)} to display a toast. Must
	 * be run on the UI thread
	 */
	private class ToastMaker implements Runnable {
		private String text;

		public ToastMaker(String text) {
			this.text = text;
		}

		@Override
		public void run() {
			Toast.makeText(getApplicationContext(), text, Toast.LENGTH_SHORT).show();
		}
	}
}

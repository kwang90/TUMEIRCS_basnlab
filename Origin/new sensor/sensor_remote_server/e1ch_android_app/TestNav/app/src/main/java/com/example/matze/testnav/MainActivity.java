package com.example.matze.testnav;

import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothManager;
import android.content.Intent;
import android.content.SharedPreferences;
import android.net.Uri;
import android.os.Handler;
import android.support.v7.app.ActionBarActivity;
import android.support.v7.app.ActionBar;
import android.support.v4.app.Fragment;
import android.support.v4.app.FragmentManager;
import android.content.Context;
import android.os.Build;
import android.os.Bundle;
import android.util.Log;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.support.v4.widget.DrawerLayout;
import android.widget.ArrayAdapter;
import android.widget.TextView;

import java.io.File;
import java.io.FileOutputStream;
import java.io.InputStream;


public class MainActivity extends ActionBarActivity
        implements NavigationDrawerFragment.NavigationDrawerCallbacks, Bluetooth.OnFragmentInteractionListener, MotionControl.OnFragmentInteractionListener,Settings.OnFragmentInteractionListener,Development.OnFragmentInteractionListener {

    private static final String TAG = MainActivity.class.getSimpleName();

    private static final String ACTION_STRING_HE2MT_SERVICE = "ToHemtService";

    private static final String BLE_DEVICE = "BleDevice";

    /**
     * Fragment managing the behaviors, interactions and presentation of the navigation drawer.
     */
    private NavigationDrawerFragment mNavigationDrawerFragment;

    /**
     * Used to store the last screen title. For use in {@link #restoreActionBar()}.
     */
    private CharSequence mTitle;
    private Handler m_oAutoconnectHandler;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        mNavigationDrawerFragment = (NavigationDrawerFragment)
                getSupportFragmentManager().findFragmentById(R.id.navigation_drawer);
        mTitle = getTitle();

        // Set up the drawer.
        mNavigationDrawerFragment.setUp(
                R.id.navigation_drawer,
                (DrawerLayout) findViewById(R.id.drawer_layout));

        startService(new Intent(this, HE2mtService.class));
        Log.d(TAG,"HE2mt Service started");

        m_oAutoconnectHandler = new Handler();

        SharedPreferences settings = getSharedPreferences(Global.HE2MT_PREFERENCES, 0);
        boolean autoConnect = settings.getBoolean(Global.AUTOCONNECT_BLE, false);

        if(autoConnect)
        {
            int REQUEST_ENABLE_BT = 1;
            BluetoothManager bluetoothManager = (BluetoothManager) getSystemService(Context.BLUETOOTH_SERVICE);
            BluetoothAdapter bluetoothAdapter = bluetoothManager.getAdapter();

            if (!bluetoothAdapter.isEnabled())
            {
                Intent enableBtIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
                startActivityForResult(enableBtIntent, REQUEST_ENABLE_BT);
            }
            else
            {
                String deviceName = settings.getString(Global.AUTOCONNECT_BLE_DEVICENAME, null);
                String deviceAddress = settings.getString(Global.AUTOCONNECT_BLE_DEVICEADDRESS, null);

                if (deviceName != null && !deviceName.isEmpty() && deviceAddress != null && !deviceAddress.isEmpty()) {
                    final BleDevice device = new BleDevice();
                    device.setDeviceName(deviceName);
                    device.setDeviceAddress(deviceAddress);

                    m_oAutoconnectHandler.postDelayed(new Runnable() {
                        @Override
                        public void run() {
                            broadcastToHE2mtService(BLE_DEVICE, device);
                        }
                    }, 500);
                }
            }
        }
        else
        {
            onNavigationDrawerItemSelected(2);
        }


        try {

            File root = android.os.Environment.getExternalStorageDirectory();
            File reference = new File(root.getAbsolutePath() + "/HE2mt", "model_v_0_1.txt");

            if(reference.exists())
            {
                //do not create the model file again
                return;
            }
            else
            {
                //create model file
                File dir = new File (root.getAbsolutePath() + "/HE2mt");

                //if folder does not exist -> create one
                if(!(dir.exists()))
                {
                    dir.mkdir();
                }

                InputStream in = getResources().openRawResource(R.raw.model);
                FileOutputStream out = new FileOutputStream(new File(dir, "model_v_0_1.txt"));

                byte[] buff = new byte[1024];
                int read = 0;

                while ((read = in.read(buff)) > 0)
                {
                    out.write(buff, 0, read);
                }

                in.close();
                out.close();
            }
        }
        catch (Exception e)
        {
            Log.d(TAG, "Model could not be written to Folder");
        }

    }

    private void broadcastToHE2mtService(final String action, final BleDevice bleDevice)
    {
        final Intent intent = new Intent(action);
        intent.setAction(ACTION_STRING_HE2MT_SERVICE);
        intent.putExtra(BLE_DEVICE, bleDevice);
        sendBroadcast(intent);
    }

    public void onDestroy()
    {
        super.onDestroy();
        stopService(new Intent(this,HE2mtService.class));
    }

    @Override
    public void onNavigationDrawerItemSelected(int position) {
        // update the main content by replacing fragments
        FragmentManager fragmentManager = getSupportFragmentManager();

        if(position == 0) {
            fragmentManager.beginTransaction()
                    .replace(R.id.container, MotionControl.newInstance("data1", "data2"))
                    .commit();
        }
        else if(position == 1)
        {
            fragmentManager.beginTransaction()
                    .replace(R.id.container, PlaceholderFragment.newInstance(position))
                    .commit();
        }
        else if(position == 2)
        {
            fragmentManager.beginTransaction()
                    .replace(R.id.container, Bluetooth.newInstance("data1", "data2"))
                    .commit();
        }
        else if(position == 3)
        {
            fragmentManager.beginTransaction()
                    .replace(R.id.container, Settings.newInstance("data1", "data2"))
                    .commit();
        }
        else if(position == 4)
        {
            fragmentManager.beginTransaction()
                    .replace(R.id.container, Development.newInstance("data1", "data2"))
                    .commit();
        }
    }

    public void onSectionAttached(int number) {
        switch (number) {
            case 0:
                mTitle = getString(R.string.title_section1);
                break;
            case 1:
                mTitle = getString(R.string.title_section2);
                break;
            case 2:
                mTitle = getString(R.string.title_section3);
                break;
            case 3:
                mTitle = getString(R.string.title_section4);
                break;
            case 4:
                mTitle = getString(R.string.title_section5);
                break;
        }
    }

    public void restoreActionBar() {
        ActionBar actionBar = getSupportActionBar();
        actionBar.setNavigationMode(ActionBar.NAVIGATION_MODE_STANDARD);
        actionBar.setDisplayShowTitleEnabled(true);
        actionBar.setTitle(mTitle);
    }


    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        if (!mNavigationDrawerFragment.isDrawerOpen()) {
            // Only show items in the action bar relevant to this screen
            // if the drawer is not showing. Otherwise, let the drawer
            // decide what to show in the action bar.
            getMenuInflater().inflate(R.menu.main, menu);
            restoreActionBar();
            return true;
        }
        return super.onCreateOptionsMenu(menu);
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();

        //noinspection SimplifiableIfStatement
        /*if (id == R.id.action_settings) {
            return true;
        }*/

        return super.onOptionsItemSelected(item);
    }

    @Override
    public void onFragmentInteraction(Uri uri) {

    }

    /**
     * A placeholder fragment containing a simple view.
     */
    public static class PlaceholderFragment extends Fragment {
        /**
         * The fragment argument representing the section number for this
         * fragment.
         */
        private static final String ARG_SECTION_NUMBER = "section_number";

        /**
         * Returns a new instance of this fragment for the given section
         * number.
         */
        public static PlaceholderFragment newInstance(int sectionNumber) {
            PlaceholderFragment fragment = new PlaceholderFragment();
            Bundle args = new Bundle();
            args.putInt(ARG_SECTION_NUMBER, sectionNumber);
            fragment.setArguments(args);
            return fragment;
        }

        public PlaceholderFragment() {
        }

        @Override
        public View onCreateView(LayoutInflater inflater, ViewGroup container,
                                 Bundle savedInstanceState) {
            View rootView = inflater.inflate(R.layout.fragment_main, container, false);
            return rootView;
        }

        @Override
        public void onAttach(Activity activity) {
            super.onAttach(activity);
            ((MainActivity) activity).onSectionAttached(
                    getArguments().getInt(ARG_SECTION_NUMBER));
        }
    }

}

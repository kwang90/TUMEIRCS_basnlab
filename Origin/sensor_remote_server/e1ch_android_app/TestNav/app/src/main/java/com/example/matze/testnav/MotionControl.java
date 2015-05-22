package com.example.matze.testnav;

import android.app.Activity;
import android.app.Notification;
import android.app.PendingIntent;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.SharedPreferences;
import android.content.res.Resources;
import android.graphics.Color;
import android.graphics.drawable.Drawable;
import android.net.Uri;
import android.os.Bundle;
import android.app.Fragment;
import android.os.Handler;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.TextView;


/**
 * A simple {@link Fragment} subclass.
 * Activities that contain this fragment must implement the
 * {@link MotionControl.OnFragmentInteractionListener} interface
 * to handle interaction events.
 * Use the {@link MotionControl#newInstance} factory method to
 * create an instance of this fragment.
 */
public class MotionControl extends android.support.v4.app.Fragment {

    private static final String TAG = MotionControl.class.getSimpleName();

    private static final String ACTION_STRING_MOTION_CONTROL = "ToMotionControl";
    private static final String ACTION_STRING_HE2MT_SERVICE = "ToHemtService";

    private static final String BLE_DEVICE = "BleDevice";
    private static final String BLUETOOTH_STATUS = "BluetoothStatus";
    private static final String MOVEMENT = "Movement";

    private static final String MOTION_CONTROL_BLE_DEVICE_NAME = "MotionControlBleDeviceName";
    private static final String MOTION_CONTROL_BLE_CONNECTION_STATUS = "MotionControlBleConnectionStatus";

    // TODO: Rename parameter arguments, choose names that match
    // the fragment initialization parameters, e.g. ARG_ITEM_NUMBER
    private static final String ARG_PARAM1 = "param1";
    private static final String ARG_PARAM2 = "param2";
    private static final String ARG_SECTION_NUMBER = "section_number";

    // TODO: Rename and change types of parameters
    private String mParam1;
    private String mParam2;

    private OnFragmentInteractionListener mListener;

    /**
     * Use this factory method to create a new instance of
     * this fragment using the provided parameters.
     *
     * @param param1 Parameter 1.
     * @param param2 Parameter 2.
     * @return A new instance of fragment MotionControl.
     */
    // TODO: Rename and change types and number of parameters
    public static MotionControl newInstance(String param1, String param2) {
        MotionControl fragment = new MotionControl();
        Bundle args = new Bundle();
        args.putString(ARG_PARAM1, param1);
        args.putString(ARG_PARAM2, param2);
        args.putInt(ARG_SECTION_NUMBER, 0);
        fragment.setArguments(args);
        return fragment;
    }

    public MotionControl() {
        // Required empty public constructor
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        if (getArguments() != null) {
            mParam1 = getArguments().getString(ARG_PARAM1);
            mParam2 = getArguments().getString(ARG_PARAM2);
        }

        Log.d(TAG, "on create");

        if (BroadcastReceiverMotionControl != null)
        {
            //Create an intent filter to listen to the broadcast sent with the action "ACTION_STRING_SERVICE"
            IntentFilter intentFilter = new IntentFilter(ACTION_STRING_MOTION_CONTROL);
            //Map the intent filter to the receiver
            getActivity().registerReceiver(BroadcastReceiverMotionControl, intentFilter);
        }
    }

    @Override
    public void onStart()
    {
        super.onStart();
        Log.d(TAG,"on start");
    }

    @Override
    public void onResume()
    {
        super.onResume();
        Log.d(TAG,"on resume");
    }

    @Override
    public void onPause()
    {
        super.onPause();
        Log.d(TAG,"on pause");
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        // Inflate the layout for this fragment

        View rootView = inflater.inflate(R.layout.fragment_motion_control, container, false);
        //return inflater.inflate(R.layout.fragment_motion_control, container, false);

        ImageView imageView = (ImageView) rootView.findViewById(R.id.imageViewMovement);
        imageView.setImageResource(R.drawable.no_connection);

        Log.d(TAG, "on Create View");

        SharedPreferences settings = getActivity().getSharedPreferences(Global.HE2MT_PREFERENCES, 0);
        String deviceName = settings.getString(MOTION_CONTROL_BLE_DEVICE_NAME, null);
        String bluetoothStatus = settings.getString(MOTION_CONTROL_BLE_CONNECTION_STATUS, null);

        if(deviceName != null)
        {
            TextView view = (TextView) rootView.findViewById(R.id.textViewDeviceName);
            if(deviceName.equals("NO DEVICE"))
            {
                view.setTextColor(Color.RED);
            }
            else {
                view.setTextColor(Color.BLACK);
            }
            view.setText(deviceName);
        }

        if(bluetoothStatus != null)
        {
            TextView view = (TextView) rootView.findViewById(R.id.textViewConnectionStatus);
            if(bluetoothStatus.equals("NOT CONNECTED"))
            {
                view.setTextColor(Color.RED);
            }
            else {
                view.setTextColor(Color.GREEN);
            }
            view.setText(bluetoothStatus);
        }

        return rootView;
    }

    // TODO: Rename method, update argument and hook method into UI event
    public void onButtonPressed(Uri uri) {
        if (mListener != null) {
            mListener.onFragmentInteraction(uri);
        }
    }

    @Override
    public void onAttach(Activity activity) {
        super.onAttach(activity);
        try {
            mListener = (OnFragmentInteractionListener) activity;
            ((MainActivity) activity).onSectionAttached(
                    getArguments().getInt(ARG_SECTION_NUMBER));
        } catch (ClassCastException e) {
            throw new ClassCastException(activity.toString()
                    + " must implement OnFragmentInteractionListener");
        }
    }

    private BroadcastReceiver BroadcastReceiverMotionControl = new BroadcastReceiver() {

        @Override
        public void onReceive(Context context, Intent intent) {

            if (intent.hasExtra(BLUETOOTH_STATUS)) {
                Bundle bundle = intent.getExtras();
                BleDevice device = (BleDevice) bundle.getSerializable(BLUETOOTH_STATUS);

                TextView viewStatus = (TextView) getActivity().findViewById(R.id.textViewConnectionStatus);
                TextView viewName = (TextView) getActivity().findViewById(R.id.textViewDeviceName);

                SharedPreferences settings = getActivity().getSharedPreferences(Global.HE2MT_PREFERENCES, 0);
                SharedPreferences.Editor editor = settings.edit();

                if(device.getConnectionState() == BleDevice.ConnectionState.CONNECTED)
                {
                    viewStatus.setTextColor(Color.GREEN);
                    viewStatus.setText("CONNECTED");
                    viewName.setTextColor(Color.BLACK);
                    viewName.setText(device.getDeviceName());
                    editor.putString(MOTION_CONTROL_BLE_DEVICE_NAME, device.getDeviceName());
                    editor.putString(MOTION_CONTROL_BLE_CONNECTION_STATUS, "CONNECTED");

                }
                else if(device.getConnectionState() == BleDevice.ConnectionState.DISCONNECTED)
                {
                    viewStatus.setTextColor(Color.RED);
                    viewStatus.setText("NOT CONNECTED");
                    viewName.setTextColor(Color.RED);
                    viewName.setText(device.getDeviceName());
                    ImageView imageView = (ImageView) getActivity().findViewById(R.id.imageViewMovement);
                    imageView.setImageResource(R.drawable.no_connection);
                    editor.putString(MOTION_CONTROL_BLE_DEVICE_NAME, "NO DEVICE");
                    editor.putString(MOTION_CONTROL_BLE_CONNECTION_STATUS, "NOT CONNECTED");
                }
                else if (device.getConnectionState() == BleDevice.ConnectionState.CONNECTING)
                {
                    viewStatus.setTextColor(Color.BLUE);
                    viewStatus.setText("CONNECTING");
                    viewName.setTextColor(Color.RED);
                    viewName.setText(device.getDeviceName());
                    ImageView imageView = (ImageView) getActivity().findViewById(R.id.imageViewMovement);
                    imageView.setImageResource(R.drawable.no_connection);
                    editor.putString(MOTION_CONTROL_BLE_DEVICE_NAME, "NO DEVICE");
                    editor.putString(MOTION_CONTROL_BLE_CONNECTION_STATUS, "CONNECTING");
                }

                editor.commit();
            }

            if(intent.hasExtra(MOVEMENT))
            {
                Bundle bundle = intent.getExtras();
                SensorData device = (SensorData) bundle.getSerializable(MOVEMENT);

                ImageView imageView = (ImageView) getActivity().findViewById(R.id.imageViewMovement);
                TextView txtView = (TextView) getActivity().findViewById(R.id.textViewMovement);

                if(device.getDetectedMovement() == SensorData.Movement.STAND)
                {
                    imageView.setImageResource(R.drawable.stand);
                    txtView.setText("STAND");
                }
                else if(device.getDetectedMovement() == SensorData.Movement.WALK)
                {
                    imageView.setImageResource(R.drawable.walk);
                    txtView.setText("WALK");
                }
                else if(device.getDetectedMovement() == SensorData.Movement.RUN)
                {
                    imageView.setImageResource(R.drawable.run);
                    txtView.setText("RUN");
                }
                else if(device.getDetectedMovement() == SensorData.Movement.FALL)
                {
                    imageView.setImageResource(R.drawable.fall);
                    txtView.setText("FALL");
                }
            }
        }
    };

    @Override
    public void onDetach() {
        super.onDetach();
        mListener = null;
    }

    /**
     * This interface must be implemented by activities that contain this
     * fragment to allow an interaction in this fragment to be communicated
     * to the activity and potentially other fragments contained in that
     * activity.
     * <p/>
     * See the Android Training lesson <a href=
     * "http://developer.android.com/training/basics/fragments/communicating.html"
     * >Communicating with Other Fragments</a> for more information.
     */
    public interface OnFragmentInteractionListener {
        // TODO: Update argument type and name
        public void onFragmentInteraction(Uri uri);
    }

    @Override
    public void onDestroy()
    {
        Log.d(TAG, "on destroy");
        super.onDestroy();
        getActivity().unregisterReceiver(BroadcastReceiverMotionControl);
    }

}

package com.example.matze.testnav;

import android.app.Activity;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.net.Uri;
import android.os.Bundle;
import android.app.Fragment;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.CompoundButton;
import android.widget.RadioButton;
import android.widget.TextView;


/**
 * A simple {@link Fragment} subclass.
 * Activities that contain this fragment must implement the
 * {@link Development.OnFragmentInteractionListener} interface
 * to handle interaction events.
 * Use the {@link Development#newInstance} factory method to
 * create an instance of this fragment.
 */
public class Development extends android.support.v4.app.Fragment implements View.OnClickListener {

    private static final String TAG = Development.class.getSimpleName();

    SensorData.Movement m_oSelectedMovement = SensorData.Movement.STAND;

    private static final String BLE_DEVICE = "BleDevice";
    private static final String ACC_DATA_AVAILABLE = "ACC_DATA_AVAILABLE";
    private static final String DEVELPMENT_RECORDING = "DevelopmentRecording";
    private static final String DEVELPMENT_MOVEMENT_RECORDING = "DevelopmentMovementRecording";

    private static final String ACTION_STRING_DEVELOPMENT = "ToDevelopment";
    private static final String ACTION_STRING_HE2MT_SERVICE = "ToHemtService";

    // TODO: Rename parameter arguments, choose names that match
    // the fragment initialization parameters, e.g. ARG_ITEM_NUMBER
    private static final String ARG_PARAM1 = "param1";
    private static final String ARG_PARAM2 = "param2";
    private static final String ARG_SECTION_NUMBER = "section_number";

    // TODO: Rename and change types of parameters
    private String mParam1;
    private String mParam2;

    private OnFragmentInteractionListener mListener;

    private boolean m_bRecording = false;

    /**
     * Use this factory method to create a new instance of
     * this fragment using the provided parameters.
     *
     * @param param1 Parameter 1.
     * @param param2 Parameter 2.
     * @return A new instance of fragment Development.
     */
    // TODO: Rename and change types and number of parameters
    public static Development newInstance(String param1, String param2) {
        Development fragment = new Development();
        Bundle args = new Bundle();
        args.putString(ARG_PARAM1, param1);
        args.putString(ARG_PARAM2, param2);
        args.putInt(ARG_SECTION_NUMBER, 4);
        fragment.setArguments(args);
        return fragment;
    }

    public Development() {
        // Required empty public constructor
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        if (getArguments() != null) {
            mParam1 = getArguments().getString(ARG_PARAM1);
            mParam2 = getArguments().getString(ARG_PARAM2);
        }

        if (BroadcastReceiverDevelopment != null)
        {
            IntentFilter intentFilter = new IntentFilter(ACTION_STRING_DEVELOPMENT);
            getActivity().registerReceiver(BroadcastReceiverDevelopment, intentFilter);
        }
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        // Inflate the layout for this fragment
        //return inflater.inflate(R.layout.fragment_development, container, false);

        View rootView = inflater.inflate(R.layout.fragment_development, container, false);

        RadioButton rbtnS = (RadioButton) rootView.findViewById(R.id.radioButtonStand);
        rbtnS.setOnClickListener(this);
        RadioButton rbtnW = (RadioButton) rootView.findViewById(R.id.radioButtonWalk);
        rbtnW.setOnClickListener(this);
        RadioButton rbtnR = (RadioButton) rootView.findViewById(R.id.radioButtonRun);
        rbtnR.setOnClickListener(this);
        RadioButton rbtnF = (RadioButton) rootView.findViewById(R.id.radioButtonFall);
        rbtnF.setOnClickListener(this);

        Button btn = (Button) rootView.findViewById(R.id.buttonRecordData);
        btn.setOnClickListener(this);

        return rootView;
    }

    // TODO: Rename method, update argument and hook method into UI event
    public void onButtonPressed(Uri uri) {
        if (mListener != null) {
            mListener.onFragmentInteraction(uri);
        }
    }

    @Override
    public void onDestroy()
    {
        super.onDestroy();
        getActivity().unregisterReceiver(BroadcastReceiverDevelopment);
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

    @Override
    public void onDetach() {
        super.onDetach();
        mListener = null;
    }

    private void broadcastDevelopmentToHE2mtService(final String action, final boolean recording)
    {
        final Intent intent = new Intent(action);
        intent.setAction(ACTION_STRING_HE2MT_SERVICE);
        intent.putExtra(DEVELPMENT_RECORDING, recording);
        getActivity().sendBroadcast(intent);
    }

    private void broadcastDevelopmentToHE2mtService(final String action, final int movement)
    {
        final Intent intent = new Intent(action);
        intent.setAction(ACTION_STRING_HE2MT_SERVICE);
        intent.putExtra(DEVELPMENT_MOVEMENT_RECORDING, movement);
        getActivity().sendBroadcast(intent);
    }

    @Override
    public void onClick(View v) {

        RadioButton btn0 = (RadioButton) getActivity().findViewById(R.id.radioButtonStand);
        RadioButton btn1 = (RadioButton) getActivity().findViewById(R.id.radioButtonWalk);
        RadioButton btn2 = (RadioButton) getActivity().findViewById(R.id.radioButtonRun);
        RadioButton btn3 = (RadioButton) getActivity().findViewById(R.id.radioButtonFall);

        switch (v.getId())
        {
            case R.id.radioButtonStand:
                m_oSelectedMovement = SensorData.Movement.STAND;
                btn1.setChecked(false);
                btn2.setChecked(false);
                btn3.setChecked(false);
                break;
            case R.id.radioButtonWalk:
                m_oSelectedMovement = SensorData.Movement.WALK;
                btn0.setChecked(false);
                btn2.setChecked(false);
                btn3.setChecked(false);
                break;
            case R.id.radioButtonRun:
                m_oSelectedMovement = SensorData.Movement.RUN;
                btn0.setChecked(false);
                btn1.setChecked(false);
                btn3.setChecked(false);
                break;
            case R.id.radioButtonFall:
                m_oSelectedMovement = SensorData.Movement.FALL;
                btn0.setChecked(false);
                btn1.setChecked(false);
                btn2.setChecked(false);
                break;
            case R.id.buttonRecordData:
                Log.d(TAG,"Record button pressed");
                Button btn = (Button) getActivity().findViewById(R.id.buttonRecordData);
                if(m_bRecording == false) {
                    btn.setText("Stop Recording Data");
                    m_bRecording = true;
                    //broadcast
                    broadcastDevelopmentToHE2mtService(DEVELPMENT_MOVEMENT_RECORDING,m_oSelectedMovement.ordinal());
                    broadcastDevelopmentToHE2mtService(DEVELPMENT_RECORDING, true);
                }
                else
                {
                    btn.setText("Start Recording Data");
                    m_bRecording = false;
                    //broadcast
                    broadcastDevelopmentToHE2mtService(DEVELPMENT_RECORDING, false);
                }
                break;
        }
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

    private BroadcastReceiver BroadcastReceiverDevelopment = new BroadcastReceiver() {

        @Override
        public void onReceive(Context context, Intent intent) {

            if(intent.hasExtra(BLE_DEVICE))
            {
                Bundle bundle=intent.getExtras();
                BleDevice device = (BleDevice) bundle.getSerializable(BLE_DEVICE);
            }

            if(intent.hasExtra(ACC_DATA_AVAILABLE))
            {
                Bundle bundle=intent.getExtras();
                SensorData data = (SensorData) bundle.getSerializable(ACC_DATA_AVAILABLE);

                TextView viewX = (TextView) getActivity().findViewById(R.id.textViewDataX);
                viewX.setText(Short.toString(data.getAxisValueX()));

                TextView viewY = (TextView) getActivity().findViewById(R.id.textViewDataY);
                viewY.setText(Short.toString(data.getAxisValueY()));

                TextView viewZ = (TextView) getActivity().findViewById(R.id.textViewDataZ);
                viewZ.setText(Short.toString(data.getAxisValueZ()));
            }
        }
    };

}

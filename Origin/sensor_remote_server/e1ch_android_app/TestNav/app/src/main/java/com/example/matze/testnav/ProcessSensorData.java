package com.example.matze.testnav;

import android.os.Environment;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileWriter;
import java.io.IOException;
import java.util.Vector;

import libsvm.*;

/**
 * Created by matze on 24.02.2015.
 */
public class ProcessSensorData {

    private Vector<Short> m_oXvector = new Vector<Short>();
    private Vector<Short> m_oYvector = new Vector<Short>();
    private Vector<Short> m_oZvector = new Vector<Short>();

    private double m_dXmean = 0;
    private double m_dYmean = 0;
    private double m_dZmean = 0;

    private double m_dCorrelationXY = 0;
    private double m_dCorrelationXZ = 0;
    private double m_dCorrelationYZ = 0;

    private boolean m_bRecord = false;

    private int m_iRecordingMode = 0;

    private double m_dPrediction = 0;

    svm m_oSupportVectorMachine = new svm();
    svm_model m_oSvmModel = new svm_model();

    void init(String url)
    {
        try
        {
            m_oSvmModel = svm.svm_load_model(url);
        }
        catch (IOException e)
        {
            e.printStackTrace();
        }
    }

    boolean appendValues(short x, short y, short z)
    {
        m_oXvector.add(x);
        m_oYvector.add(y);
        m_oZvector.add(z);

        if(m_oXvector.size() == 250)
        {
            this.calculateMeanValue();
            this.calculateCorrelation();
            this.predict();
            this.removeElements();

            if(m_bRecord)
            {
                this.record();
            }

            return true;
        }

        return false;
    }

    private void calculateMeanValue()
    {
        //calculate mean value
        for(int i = 0; i < m_oXvector.size(); i++)
        {
            m_dXmean += m_oXvector.get(i);
            m_dYmean += m_oYvector.get(i);
            m_dZmean += m_oZvector.get(i);
        }

        m_dXmean = m_dXmean/m_oXvector.size();
        m_dYmean = m_dYmean/m_oYvector.size();
        m_dZmean = m_dZmean/m_oZvector.size();
    }

    private void calculateCorrelation()
    {
        double xSum = 0;
        double ySum = 0;
        double zSum = 0;

        double xSquareSum = 0;
        double ySquareSum = 0;
        double zSquareSum = 0;

        double xySum = 0;
        double xzSum = 0;
        double yzSum = 0;

        for(int i = 0; i < m_oXvector.size(); i++)
        {
            xSum += m_oXvector.get(i);
            ySum += m_oYvector.get(i);
            zSum += m_oZvector.get(i);

            xSquareSum = xSquareSum + m_oXvector.get(i) * m_oXvector.get(i);
            ySquareSum = ySquareSum + m_oYvector.get(i) * m_oYvector.get(i);
            zSquareSum = zSquareSum + m_oZvector.get(i) * m_oZvector.get(i);

            xySum = xySum + m_oXvector.get(i) * m_oYvector.get(i);
            xzSum = xzSum + m_oXvector.get(i) * m_oZvector.get(i);
            yzSum = yzSum + m_oYvector.get(i) * m_oZvector.get(i);
        }

        m_dCorrelationXY = (m_oXvector.size() * xySum - xSum * ySum)/(Math.sqrt((m_oXvector.size() * xSquareSum - xSum * xSum) * (m_oXvector.size() * ySquareSum - ySum * ySum)));
        m_dCorrelationXZ = (m_oXvector.size() * xzSum - xSum * zSum)/(Math.sqrt((m_oXvector.size() * xSquareSum - xSum * xSum) * (m_oXvector.size() * zSquareSum - zSum * zSum)));
        m_dCorrelationYZ = (m_oXvector.size() * yzSum - ySum * zSum)/(Math.sqrt((m_oXvector.size() * ySquareSum - ySum * ySum) * (m_oXvector.size() * zSquareSum - zSum * zSum)));
    }

    private void removeElements()
    {
        m_oXvector.subList(0, 124).clear();
    }

    public double getMeanValueOfX()
    {
        return m_dXmean;
    }

    public double getMeanValueOfY()
    {
        return m_dYmean;
    }

    public double getMeanValueOfZ()
    {
        return m_dZmean;
    }

    public double getCorrelationXY()
    {
        return m_dCorrelationXY;
    }

    public double getCorrelationXZ()
    {
        return m_dCorrelationXZ;
    }

    public double getCorrelationYZ()
    {
        return m_dCorrelationYZ;
    }

    private void record()
    {
        if(isExternalStorageWritable())
        {
            File root = android.os.Environment.getExternalStorageDirectory();
            File dir = new File (root.getAbsolutePath() + "/Download");
            dir.mkdirs();
            File file = new File(dir, "Data.txt");

            try
            {
                FileWriter fw = new FileWriter(file, true);
                fw.write(m_iRecordingMode + " 1:" + m_dXmean + " 2:" + m_dYmean + " 3:" + m_dZmean + " 4:" + m_dCorrelationXY + " 5:" + m_dCorrelationXZ + " 6:" + m_dCorrelationYZ + "\n");
                fw.flush();
                fw.close();
            }
            catch (FileNotFoundException e)
            {
                e.printStackTrace();
            }
            catch (IOException e)
            {
                e.printStackTrace();
            }
        }
    }
    public void startRecording(int type)
    {
        m_bRecord = true;
        m_iRecordingMode = type;
    }

    public void stopRecording()
    {
        m_bRecord = false;
    }

    public boolean isExternalStorageWritable()
    {
        String state = Environment.getExternalStorageState();
        if (Environment.MEDIA_MOUNTED.equals(state))
        {
            return true;
        }
        return false;
    }

    private void predict()
    {
        svm_node[] nodes = new svm_node[6];

        for(int i = 0; i < 6; i++)
        {
            double value = 0;
            svm_node node = new svm_node();

            switch(i){
                case 0: value = m_dXmean; break;
                case 1: value = m_dYmean; break;
                case 2: value = m_dZmean; break;
                case 3: value = m_dCorrelationXY; break;
                case 4: value = m_dCorrelationXZ; break;
                case 5: value = m_dCorrelationYZ; break;
            }

            node.index = i+1;
            node.value = value;

            nodes[i] = node;
        }

        m_dPrediction = svm.svm_predict(m_oSvmModel, nodes);
    }

    public double getPrediction()
    {
        return m_dPrediction;
    }
}

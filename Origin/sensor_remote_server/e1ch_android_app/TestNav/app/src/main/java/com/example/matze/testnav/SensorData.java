package com.example.matze.testnav;

import java.io.Serializable;

/**
 * Created by matze on 24.02.2015.
 */
public class SensorData implements Serializable{
    private static final long serialVersionUID = 20;

    public enum Movement {
        STAND, WALK, RUN, FALL
    }

    private long m_iIndex;
    private long m_iNumberOfMeasurements;
    private short m_oAxisX;
    private short m_oAxisY;
    private short m_oAxisZ;
    private Movement m_oDetectedMovement;



    public void setIndex(long index)
    {
        m_iIndex = index;
    }

    public long getIndex()
    {
        return m_iIndex;
    }

    public void setNumberOfMeasurements(long numberOfMeasurements)
    {
        m_iNumberOfMeasurements = numberOfMeasurements;
    }

    public long getNumberOfMeasurements()
    {
        return m_iNumberOfMeasurements;
    }

    public void setAxisValueX(short value)
    {
        m_oAxisX = value;
    }

    public short getAxisValueX()
    {
        return m_oAxisX;
    }

    public void setAxisValueY(short value)
    {
        m_oAxisY = value;
    }

    public short getAxisValueY()
    {
        return m_oAxisY;
    }

    public void setAxisValueZ(short value)
    {
        m_oAxisZ = value;
    }

    public short getAxisValueZ()
    {
        return m_oAxisZ;
    }

    public void setDetectedMovement(Movement movement)
    {
        m_oDetectedMovement = movement;
    }

    public Movement getDetectedMovement()
    {
        return m_oDetectedMovement;
    }
}

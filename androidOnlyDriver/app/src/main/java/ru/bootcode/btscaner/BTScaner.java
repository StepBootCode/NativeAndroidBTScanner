package ru.bootcode.btscaner;

import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;

import java.io.InputStream;
import java.io.IOException;
import java.nio.charset.StandardCharsets;
import java.util.Set;
import java.util.UUID;

public class BTScaner implements Runnable {

    private final UUID SPP_SERVICE_ID = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB");

    static native void OnBarcode(long pObject, String sBarcode);
    static native void OnCancel(long pObject);

    private long m_V8Object;            // 1C application context (Object from C++)
    private Activity m_Activity;        // Custom activity of 1C:Enterprise

    private String btAdress;
    private android.bluetooth.BluetoothAdapter btAdapter = null;
    private static final android.bluetooth.BluetoothAdapter BluetoothAdapter = null;
    private BluetoothSocket btSocket = null;
    private BluetoothDevice device = null;
    private InputThread inputThread = null;

    private InputStream inStream = null;

    public BTScaner(Activity m_Activity, long m_V8Object) {
        this.m_V8Object = m_V8Object;
        this.m_Activity = m_Activity;

    }

    public void run() {
        System.loadLibrary("ru_bootcode_btscaner");
    }

    public void show() {
        m_Activity.runOnUiThread(this);
    }

    public void initDevice(String btAdress) {

        this.btAdress = btAdress;

        if (btAdapter == null)
            try {
                btAdapter = android.bluetooth.BluetoothAdapter.getDefaultAdapter();
            } catch (Exception e) {
                return;
            }

        if (btAdapter == null) {
            return;
        }

        if (!btAdapter.isEnabled()) {
            return;
        }

        device = btAdapter.getRemoteDevice(btAdress);
        if (device == null) {
            return;
        }

        if (btSocket != null) {
            try {
                btSocket.close();
                btSocket = null;
            } catch (IOException e) {
                btSocket = null;
            }
        }

        try {
            btSocket = device.createRfcommSocketToServiceRecord(SPP_SERVICE_ID);
        } catch (IOException e) {
            return;
        }

        if (btSocket == null) {
            return;
        }
        btAdapter.cancelDiscovery();

        try {
            btSocket.connect();
        } catch (IOException e) {
            try {
                btSocket.close();
            } catch (IOException e2) {
            }
            return;
        }

        try {
            inStream = btSocket.getInputStream();
        } catch (IOException e) {
            return;
        }

        inputThread = new InputThread();
        inputThread.start();
    }

    public void stopDevice() {
        cancel();
    }

    public String getBluetoothDevicesList() {

        BluetoothAdapter btAdapter = BluetoothAdapter.getDefaultAdapter();
        if (btAdapter == null) return "";
        if (!btAdapter.isEnabled()) return "";

        // Only bonded device!
        Set<BluetoothDevice> pairedDevices = btAdapter.getBondedDevices();
        if (pairedDevices.size() == 0)  return "";

        StringBuilder TextOfDevice = new StringBuilder();
        for (BluetoothDevice device : pairedDevices) {
            TextOfDevice.append(device.getAddress() + "|" + device.getName() + "\n");
        }
        return TextOfDevice.toString();
    }

    public boolean enabled()
    {
        return (btSocket != null && btSocket.isConnected() && inputThread != null && inputThread.isAlive());
    }

    public void cancel() {

        if (inputThread != null)
        {
            inputThread.interrupt();
            inputThread = null;
        }
        if (inStream != null) {
            try {
                inStream.close();
            } catch (IOException ignored) {
            }
        }
        if (btSocket != null)
            try {
                btSocket.close();
                btSocket = null;
            }
            catch (IOException ignored) { }
    }

    private class InputThread extends Thread {

        public void run() {
            byte[] buffer = new byte[1024];
            int bytes;

            while (true) {
                try {
                    bytes = inStream.read(buffer);
                    String strIncom = new String(buffer, 0, bytes, StandardCharsets.US_ASCII);
                    OnBarcode(m_V8Object, strIncom);
                } catch (IOException e) {

                    OnCancel(m_V8Object);
                    cancel();
                    break;
                }
            }
        }
    }
}
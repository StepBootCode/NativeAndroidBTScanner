package ru.bootcode.btscaner;

import androidx.appcompat.app.AlertDialog;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;

import android.app.Dialog;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.util.Log;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.EditText;
import android.widget.ListView;

import java.lang.reflect.InvocationTargetException;
import java.util.ArrayList;
import java.util.Set;

public class MainActivity extends AppCompatActivity {

    public static final int REQUEST_BLUETOOTH_SCAN = 100;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
    }
    public void devOnClick(View view) {

        System.loadLibrary("ru_bootcode_btscaner");

        BTScaner mainApp = new BTScaner(this, 0);
        String dev = mainApp.getBluetoothDevicesList();

        final EditText editText = findViewById(R.id.editTextTextMultiLine);
        editText.setText(dev);
    }

    public void onScanConnect(View view) {

        System.loadLibrary("ru_bootcode_btscaner");

        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        builder.setTitle("Select BT device");

        ListView modeList = new ListView(this);
        ArrayList<String> listPaired = new ArrayList<>();
        getPairedDevices(listPaired); // get paired devices
        ArrayAdapter<String> modeAdapter = new ArrayAdapter<String>(this, android.R.layout.simple_list_item_1, android.R.id.text1, listPaired);
        modeList.setAdapter(modeAdapter);

        builder.setView(modeList);
        final Dialog dialog = builder.create();
        dialog.show();

        modeList.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> parent, View view, int position, long id) {

                String[] device = listPaired.get(position).split("\n");

                BTScaner mainApp = new BTScaner(MainActivity.this, 0);
                mainApp.initDevice(device[1]);

                dialog.dismiss();
            }
        });
    }

    private void getPairedDevices(ArrayList<String> list) {

        BluetoothAdapter btAdapter = BluetoothAdapter.getDefaultAdapter();
        if (btAdapter == null) return;
        if (!btAdapter.isEnabled()) return;

        ArrayList<BluetoothDevice> deviceList = new ArrayList<>();

        Set<BluetoothDevice> pairedDevices = btAdapter.getBondedDevices();
        if (pairedDevices.size() > 0) {
            deviceList.addAll(pairedDevices);
        }

        if(deviceList.size() >0) {
            for(BluetoothDevice device: deviceList) {
                list.add(device.getName()+"\n"+device.getAddress());
            }
        }
    }
}
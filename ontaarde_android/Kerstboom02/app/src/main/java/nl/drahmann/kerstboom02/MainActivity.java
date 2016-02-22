package nl.drahmann.kerstboom02;


import android.app.ActionBar;
import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.support.v4.app.FragmentActivity;
import android.support.v4.view.ViewPager;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.Calendar;
import java.util.Set;
import java.util.UUID;

public class MainActivity extends FragmentActivity {

    private static final int REQUEST_ENABLE_BT = 1;
    static final String TAG = "BDR";
    public BluetoothAdapter BT;

    private ListView BTListView;
    private static String address = "XX:XX:XX:XX:XX:XX";
    static BluetoothSocket btSocket = null;
    private static final UUID MY_UUID = UUID
            .fromString("00001101-0000-1000-8000-00805F9B34FB");
    private InputStream inStream = null;
    static private OutputStream outStream = null;
    boolean stopWorker = false;
    byte delimiter = 35; // is #
    int readBufferPosition = 0;
    byte[] readBuffer = new byte[1024];
    Handler handler = new Handler();

    Calendar now = Calendar.getInstance();

    public boolean FirstTimeRun= true;
    public boolean connected = false;
    public boolean belwasingedrukt = false;

    public int pompnr = 0;  // Hulpvariable in case
    public int Sensor = 0;  // De Sensoruitlezing

    int notifyID = 1;   // Sets an ID for the notification, so it can be updated
    int numMessages = 0;  // aantal notifications wordt weergegeven

    String TabFragment1;
    String TabFragment2;
    String TabFragment3;

    public void setTabFragment1(String t) {TabFragment1 = t; }
    public String getTabFragment1() {return TabFragment1; }
    public void setTabFragment2(String t) {TabFragment2 = t; }
    public String getTabFragment2() {return TabFragment2; }
    public void setTabFragment3(String t) {TabFragment3 = t; }
    public String getTabFragment3() {return TabFragment3; }

    // De volgende routines zijn bestemd voor de BT communicatie **********************************

    //---check if bluetooth is available on the device---
    private boolean BluetoothAvailable()
    {
        if (BT == null)
            return false;
        else
            return true;
    }

    private boolean BTEnabled() { // toestemming vragen om BT aan te zetten
        if (!BT.isEnabled()) { // Ask user's permission to switch the Bluetooth adapter On.
            Intent enableIntent = new Intent(
                    BluetoothAdapter.ACTION_REQUEST_ENABLE);
            startActivityForResult(enableIntent, REQUEST_ENABLE_BT);
            return false;
        } else {
            DisplayToast("Bluetooth is already on");
            return true;
        }
    }

    @Override 	// reactie op vervoek BT aan te zetten
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        if (requestCode == REQUEST_ENABLE_BT) {
            if (resultCode == Activity.RESULT_OK) {
                BluetoothAdapter BT = BluetoothAdapter.getDefaultAdapter();
                String address = BT.getAddress();
                String name = BT.getName();
                String connectedMsg = "BT is on; your device is " + name
                        + " : " + address;
                DisplayToast(connectedMsg);
                BTKies();
            } else {
                DisplayToast("Deze applicatie werkt alleen met ingeschakelde Bluetooth");
                onDestroy();
                finish();	// app wordt gesloten
            }
        } else {
            String UnknownMsg =  "Unknown RequestCode "+ requestCode;
            DisplayToast(UnknownMsg);
        }
    }

    public class MyBTBroadcastReceiver extends BroadcastReceiver {
        @Override
        public void onReceive(Context context, Intent intent) {
            int state = intent.getExtras().getInt(BluetoothAdapter.EXTRA_STATE);
            switch (state) {
                case BluetoothAdapter.STATE_OFF:
                    Toast.makeText(context, "BT is Off", Toast.LENGTH_SHORT).show();
                    break;
                case BluetoothAdapter.STATE_TURNING_OFF:
                    Toast.makeText(context, "BT is Turning Off", Toast.LENGTH_SHORT).show();
                    break;
                case BluetoothAdapter.STATE_ON:
                    Toast.makeText(context, "BT is On", Toast.LENGTH_SHORT).show();
                    break;
                case BluetoothAdapter.STATE_TURNING_ON:
                    Toast.makeText(context, "BT is Turning On", Toast.LENGTH_SHORT).show();
                    break;
            }
        }
    }

    private void BTKies() {	// Kies een BT apparaat

        // toon lijst met gepairde apparaten
        // Kies een BT apparaat uit de lijst
        Set<BluetoothDevice> pairedDevices;
        ArrayAdapter<String> BTArrayAdapter;
        final Dialog dialog = new Dialog(this);
        dialog.setContentView(R.layout.devicedialog);
        dialog.setTitle("Kies een verbinding");
        BTListView = (ListView) dialog.findViewById(R.id.listView1);
        BTListView.setVisibility(View.VISIBLE);

        BTArrayAdapter = new ArrayAdapter<String>(this,
                android.R.layout.simple_list_item_1);
        BTListView.setAdapter(BTArrayAdapter);
        pairedDevices = BT.getBondedDevices();
        for (BluetoothDevice device : pairedDevices)
            BTArrayAdapter.add(device.getName() + "\n" + device.getAddress());

        dialog.show();

        // Pick het item uit de list
        BTListView.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> arg0, View arg1, int arg2,
                                    long arg3) {
                ListView lv = (ListView) arg0;
                TextView tv = (TextView) lv.getChildAt(arg2);
                String s = tv.getText().toString();

                Connect(s); // Make connection
            }

            private void Connect(String s) {
                int i = s.length();
                address = s.substring(i - 17);
                BluetoothDevice device = BT.getRemoteDevice(address);
                Log.d(TAG, "Connecting to ... " + device);
                BT.cancelDiscovery();
                try { // omgaan met exeptions
                    btSocket = device
                            .createRfcommSocketToServiceRecord(MY_UUID);
                    btSocket.connect();
                    // hier de comunicatie invullen ***********************************************
                    writeData("c#"); // vraag om een bevestiging van de verbinding
                    BTListView.setVisibility(View.INVISIBLE);
                    dialog.dismiss();
                    beginListenForData();
                } catch (IOException e) {
                    try {
                        btSocket.close();
                    } catch (IOException e2) {
                        DisplayToast("Unable to end the connection");
                    }
                    DisplayToast("Ontvanger niet beschikbaar. Kies opnieuw");
                }
            }	// einde connect

        });	// einde onClick
    }	// einde BTKies

    static public void writeData(String data) {  // Stuurt de data naar de Arduino
        try {
            outStream = btSocket.getOutputStream();
        } catch (IOException e) {
            Log.d(TAG, "Bug BEFORE Sending stuff", e);
        }

        byte[] msgBuffer = data.getBytes();
        try {
            Log.d(TAG, " M213 writeData= " + data);
            outStream.write(msgBuffer);
        } catch (IOException e) {
            Log.d(TAG, "Bug while sending stuff", e);
        }
    }

    public void beginListenForData() { // Inlezen uit de Arduino
        stopWorker = false;
        readBufferPosition = 0;
        try {
            inStream = btSocket.getInputStream();
        } catch (IOException e) {
            Log.d(TAG, "Bug while reading inStream", e);
        }
        Thread workerThread = new Thread(new Runnable() {
            public void run() {
                while (!Thread.currentThread().isInterrupted() && !stopWorker) {
                    try {
                        int bytesAvailable = inStream.available();
                        if (bytesAvailable > 0) {
                            byte[] packetBytes = new byte[bytesAvailable];
                            inStream.read(packetBytes);
                            for (int i = 0; i < bytesAvailable; i++) {
                                // Log.d(TAG, " 345 i= " + i);
                                byte b = packetBytes[i];
                                if (b == delimiter) {
                                    byte[] encodedBytes = new byte[readBufferPosition];
                                    System.arraycopy(readBuffer, 0,
                                            encodedBytes, 0,
                                            encodedBytes.length);
                                    final String data = new String(
                                            encodedBytes, "US-ASCII");
                                    readBufferPosition = 0;
                                    handler.post(new Runnable() {
                                        public void run() {
                                            Log.d(TAG, " M250 ingelezen data= " + data);
                                            ProcesInput(data); // verwerk de input
                                        }
                                    });
                                } else {
                                    readBuffer[readBufferPosition++] = b;
                                }
                            }
                        } // einde bytesavalable > 0
                    } catch (IOException ex) {
                        stopWorker = true;
                    }
                }
            }
        });
        workerThread.start();
    }   // einde beginlistenfordata

    // Einde BT routines **************************************************************************


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        // Get the view from activity_main.xml
        setContentView(R.layout.activity_main);

        // Locate the viewpager in activity_main.xml
        ViewPager viewPager = (ViewPager) findViewById(R.id.pager);
        // Set the ViewPagerAdapter into ViewPager
        viewPager.setAdapter(new ViewPagerAdapter(getSupportFragmentManager()));
        viewPager.setOffscreenPageLimit(3);  // zo blijven er 3 fragments in het werkgeheugen

        final ActionBar actionBar = getActionBar();
        actionBar.setSubtitle("not connected");

        BT = BluetoothAdapter.getDefaultAdapter();
        if (BluetoothAvailable()) {
            if (BTEnabled()) { // Is toestemming gegeven de BT aan te zetten?
                BTKies();	// Kies een BT en zet hem aan
            } else {
                // DisplayToast("Stop dan maar");
            }
        } else
        {
            DisplayToast("Dit apparaat heeft geen Bluetooth. Stop maar.");
        }

    } // einde OnCreate

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.main, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        /*int id = item.getItemId();

        //noinspection SimplifiableIfStatement
        if (id == R.id.action_settings) {
            return true;
        }

        return super.onOptionsItemSelected(item);
        */
        Intent intent;
        switch (item.getItemId()) {
            case R.id.action_settings: // toon instelscherm
                Intent i = new Intent(this, Preferences.class);
                startActivity(i);
                return true;

            case R.id.about:
                // Show the About screen
                AboutBox.Show(MainActivity.this);
                return true;
        }
        return false;
    }

    private void ProcesInput(String data) { // verwerkt de input uit de Arduino
		 /*
		 * verwerkt de input uit de Arduino De ingelezen informatie bestaat uit
		 * een code van twee posities met daarna direct de informatie. de codes:
		 * 01: confimatie van connected. Arduino send "connected"
		 * 02 Arduino send vlotterstand
		 * 03 Arduino send datum
		 * 04 Arduino send waarde humidity
		 * 05 Arduino send waarde Temperatuur in C
		 * 06 Arduino send bericht toestand schijf
		 * 07 Arduino send bericht toestand file
		 * 08 Arduino send tijd
		 * 09 Arduino send waarde Lux
		 * 10 Arduino send waarde SMScode
		 * 15 Arduino send PompStatus
		 * 16 Arduino send pompnummer
		 * 17 Arduino send waarde sensor1
		 * 18 Arduino send waarde sensor2
		 * 19 Arduino send waarde sensor3
		 * 20 Arduino send gemiddelde waarde sensor1
		 * 21 Arduino send gemiddelde waarde sensor2
		 * 22 Arduino send gemiddelde waarde sensor3
		 * 23 Arduino send Drooginfo sensor1
		 * 24 Arduino send Drooginfo sensor2
		 * 25 Arduino send Drooginfo sensor3
		 * 26
		 *

		 */


        // opzoeken van fragment1
        String TabOfFragment1 = (this).getTabFragment1();
        FragmentTab1 fragment1 = (FragmentTab1)this
                .getSupportFragmentManager()
                .findFragmentByTag(TabOfFragment1);
        // opzoeken van fragment2
        String TabOfFragment2 = (this).getTabFragment2();
        FragmentTab2 fragment2 = (FragmentTab2)this
                .getSupportFragmentManager()
                .findFragmentByTag(TabOfFragment2);
        // opzoeken van fragment3
        String TabOfFragment3 = (this).getTabFragment3();
        FragmentTab3 fragment3 = (FragmentTab3)this
                .getSupportFragmentManager()
                .findFragmentByTag(TabOfFragment3);

        // eerst het ingelezen record opdelen in code en informatie
        int kode = 0;
        if (data.length() < 2) GeefFoutboodschap(data.substring(0)); // dit komt soms bij de start van een lopende Arduino
        String s = data.substring(0, 2); // eerste 2 posities bevat de kode
        String info = data.substring(2); // de recordinformatie
        // Log.d(TAG, " M374 info = " + info);
        try {
            kode = Integer.parseInt(s); // omzetten naar integer
        } catch (NumberFormatException ex) { // omzetten is niet gelukt
            GeefFoutboodschap(s);
        }

        switch (kode) {
            case 1:
                final ActionBar actionBar = getActionBar();
                actionBar.setSubtitle(info);
                connected = true;



                break;
            case 2: // geef de stand van de vlotter weer
                if (info.equals("L") ) { // het niveau staat te laag
                    fragment1.Niveau.setTextSize(14); // veld Niveau highlighted rood maken
                    fragment1.Niveau.setBackgroundColor(0xFFFF0000);
                    fragment1.Niveau.setText("LEEG");
                }
                else {
                    fragment1.Niveau.setTextSize(14);
                    fragment1.Niveau.setBackgroundColor(0xFF00FF00);
                    fragment1.Niveau.setText("VOLDOENDE");
                }
                break;
            case 3: // Arduino send datum
                fragment1.ADatum.setText(info);
                writeData("s#");	 	// vraag om de gegevens van de schijf en file
                break;
            case 4: // set de humidity in het veld
                fragment1.Humidity.setText(info);
                break;
            case 5: // set de temperatuur in het veld
                fragment1.TempC.setText(info);
                break;
            case 6: // set de schijfinfo in het veld
                fragment1.Schijf.setText(info);
                break;
            case 7: // set de bestandsinfo in het veld
                fragment1.Bestand.setText(info);
                writeData("u#");        // vraag om de Pompstatus
                break;
            case 8: // Arduino send tijd
                fragment1.ATijd.setText(info);
                break;
            case 9: // set de Light info in het veld
                fragment1.Lux.setText(info);
                break;
            case 10:    // smscode
                fragment1.Sms.setText(info);
                break;
            case 11:    // files

                break;
            case 12:    // inhoud files

                break;
            case 13:       // deleted filename

                break;
            case 14:    // telefoonnummer
                fragment1.Telefoon.setText(info);
                break;
            case 15:    // set de PompStatus
                Integer x = Integer.parseInt(info);
                // set alle velden op grijs

                fragment1.Status01.setBackgroundColor(0xE0E0E0E0);
                fragment1.Status02.setBackgroundColor(0xE0E0E0E0);
                fragment1.Status03.setBackgroundColor(0xE0E0E0E0);
                fragment1.Status04.setBackgroundColor(0xE0E0E0E0);
                fragment1.Status05.setBackgroundColor(0xE0E0E0E0);
                fragment1.Status06.setBackgroundColor(0xE0E0E0E0);
                fragment1.Status07.setBackgroundColor(0xE0E0E0E0);
                fragment1.Status08.setBackgroundColor(0xE0E0E0E0);

                // set alle opmerkingsvelden uit
                fragment1.Opm01.setVisibility(View.INVISIBLE);
                fragment1.Opm02.setVisibility(View.INVISIBLE);
                fragment1.Opm03.setVisibility(View.INVISIBLE);
                fragment1.Opm04.setVisibility(View.INVISIBLE);
                fragment1.Opm05.setVisibility(View.INVISIBLE);
                fragment1.Opm06.setVisibility(View.INVISIBLE);
                fragment1.Opm07.setVisibility(View.INVISIBLE);
                fragment1.Opm08.setVisibility(View.INVISIBLE);

                // set het x veld op groen
                switch (x) {
                    case 1: fragment1.Status01.setBackgroundColor(0xFF00FF00);
                            fragment1.Opm01.setVisibility(View.VISIBLE);
                        break;
                    case 2: fragment1.Status02.setBackgroundColor(0xFF00FF00);
                            fragment1.Opm02.setVisibility(View.VISIBLE);
                        break;
                    case 3: fragment1.Status03.setBackgroundColor(0xFF00FF00);
                            fragment1.Opm03.setVisibility(View.VISIBLE);
                        break;
                    case 4: fragment1.Status04.setBackgroundColor(0xFF00FF00);
                            fragment1.Opm04.setVisibility(View.VISIBLE);
                            String opm4 = "Pomp " + pompnr + " in gebruik";
                            fragment1.Opm04.setText(opm4);
                        break;
                    case 5: fragment1.Status05.setBackgroundColor(0xFF00FF00);
                            fragment1.Opm05.setVisibility(View.VISIBLE);
                        break;
                    case 6: fragment1.Status06.setBackgroundColor(0xFF00FF00);
                            fragment1.Opm06.setVisibility(View.VISIBLE);
                        break;
                    case 7: fragment1.Status07.setBackgroundColor(0xFF00FF00);
                            fragment1.Opm07.setVisibility(View.VISIBLE);
                        break;
                    case 8: fragment1.Status08.setBackgroundColor(0xFFFF0000);
                            fragment1.Opm08.setVisibility(View.VISIBLE);
                        break;
                }
                writeData("r#");       // vraag het waterniveau op
                break;
            case 16:        // set het Pompnummer
                pompnr = Integer.parseInt(info);
                break;
            case 17:    // set de Sensor 1 info in het veld
                fragment1.Sensor1.setText(info);
                fragment2.sensor1 = Integer.parseInt (info);
                break;
            case 18:  // set de Sensor 2 info in het veld
                fragment1.Sensor2.setText(info);
                fragment2.sensor2 = Integer.parseInt(info);
                break;
            case 19: // set de Sensor 3 info in het veld
                fragment1.Sensor3.setText(info);
                fragment2.sensor3 = Integer.parseInt (info);
                fragment2.onSensorChanged();
                break;
            case 20:    // set de gem Sensor 1 info in het veld
                fragment1.Sensor1Gem.setText(info);
                break;
            case 21:  // set de gem Sensor 2 info in het veld
                fragment1.Sensor2Gem.setText(info);
                break;
            case 22: // set de gem Sensor 3 info in het veld
                fragment1.Sensor3Gem.setText(info);
                break;
            case 23:    // set de droog 1 info in het veld
                fragment1.Droog1.setText(info);
                break;
            case 24:  // set de droog 2 info in het veld
                fragment1.Droog2.setText(info);
                break;
            case 25: // set de droog 3 info in het veld
                fragment1.Droog3.setText(info);
                break;
            // volgende case
            //
            default:
                // de rest is een onbekende kode
                GeefFoutboodschap(s);
        }

    }

    private void GeefFoutboodschap(String s) { // afvangen van verkeerde Arduino kode
        new AlertDialog.Builder(this)
                .setTitle("Onbekende kode")
                .setMessage(
                        "Arduino heeft een onbekende kode " + s
                                + " opgestuurd. Het programma wordt gestopt ")
                .setPositiveButton(android.R.string.yes,
                        new DialogInterface.OnClickListener() {
                            public void onClick(DialogInterface dialog,
                                                int which) {
                                finish();
                            }
                        }).show();
    }

    public void DisplayToast(String msg) { // Toont boodschappen op het scherm
        Toast.makeText(getBaseContext(), msg, Toast.LENGTH_SHORT).show();
    }

    @Override
    protected void onDestroy() {
        writeData("x#");    // stuur disconnect to Arduino
        super.onDestroy();
        try {
            if (btSocket != null) {
                btSocket.close();
                Log.d(TAG, " M569 btSocket.close gelukt");
            }
        } catch (IOException e) {
            Log.d(TAG, " M572 btSocket.close niet gelukt");
        }
    }
}

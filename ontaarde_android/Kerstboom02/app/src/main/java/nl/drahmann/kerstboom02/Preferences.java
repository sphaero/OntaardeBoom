package nl.drahmann.kerstboom02;

/**
 * Created by Bernard on 30-3-2015.
 */


import android.content.SharedPreferences;
import android.os.Bundle;
import android.preference.PreferenceActivity;
import android.preference.PreferenceManager;
import android.util.Log;


public class Preferences extends PreferenceActivity {
    boolean cbxSMS = false;
    String txtTelNumber = "";
    String txtDrooglevel1 = "";
    String txtDrooglevel2 = "";
    String txtDrooglevel3 = "";
    String txtDroogtijd = "";
    String txtDruppelspeling = "";
    String txtSamples = "";
    String txtVlotterdelay = "";
    String Arduinoinfo = "";

        @Override
        protected void onCreate(Bundle savedInstanceState) {
            super.onCreate(savedInstanceState);

            PreferenceManager prefMgr = getPreferenceManager();
            prefMgr.setSharedPreferencesName("treePreferences");

            // Load the preferences from an XML resource
            addPreferencesFromResource(R.xml.preferences);

        }

        @Override
        protected void onStop (){
            super.onStop();  // Always call the superclass method first
            // lees preferences uit om ze door te sturen naar Arduino
            SharedPreferences appPrefs = getSharedPreferences("treePreferences",MODE_PRIVATE);

            cbxSMS = appPrefs.getBoolean("checkboxPref",false); // de sms checkbox
            Log.d(MainActivity.TAG, " P36 checkbox= " + cbxSMS);
            if (cbxSMS)  MainActivity.writeData("a#");
            else MainActivity.writeData("b#");

            SharedPreferences.Editor prefsEditor = appPrefs.edit();     // het telefoonnummer
            txtTelNumber = appPrefs.getString("editTelPref", "voer telefoonnummer in");
            MainActivity.writeData("t" + txtTelNumber + "#");

            txtDrooglevel1 = appPrefs.getString("drooglevel1Pref","voer drooglevel in");
            txtDrooglevel2 = appPrefs.getString("drooglevel2Pref","voer drooglevel in");
            txtDrooglevel3 = appPrefs.getString("drooglevel3Pref","voer drooglevel in");
            txtDroogtijd = appPrefs.getString("droogtijdPref","voer droogtijd in");
            txtDruppelspeling = appPrefs.getString("spelingPref","voer sensorspeling in");
            txtSamples = appPrefs.getString("gemPref","voer aantal samples in");
            txtVlotterdelay = appPrefs.getString("vlotterPref","voer densdertijd vlotter in");
            Arduinoinfo = txtDrooglevel1 + "$"+ txtDrooglevel2 + "$" + txtDrooglevel3 + "$" + txtDroogtijd + "$"
                    + txtDruppelspeling + "$" + txtSamples + "$" + txtVlotterdelay;
            MainActivity.writeData("k"+ Arduinoinfo + "#");

            // einde lees preferences
        }



    }

/*  mogelijke controle op input
@Override
public void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    addPreferencesFromResource(R.xml.preferences);

    Your_Pref = (EditTextPreference) getPreferenceScreen().findPreference("Your_Pref");

    Your_Pref.setOnPreferenceChangeListener(new OnPreferenceChangeListener() {

        @Override
        public boolean onPreferenceChange(Preference preference, Object newValue) {
            Boolean rtnval = true;
            if (Your_Test) {
                final AlertDialog.Builder builder = new AlertDialog.Builder(getActivity());
                builder.setTitle("Invalid Input");
                builder.setMessage("Something's gone wrong...");
                builder.setPositiveButton(android.R.string.ok, null);
                builder.show();
                rtnval = false;
            }
            return rtnval;
        }
    });
}

 */


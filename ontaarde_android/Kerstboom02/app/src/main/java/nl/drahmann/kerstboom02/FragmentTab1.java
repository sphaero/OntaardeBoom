package nl.drahmann.kerstboom02;

import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.TextView;

public class FragmentTab1 extends Fragment {
    static final String TAG = "BDR";

    public TextView Schijf;
    public TextView Bestand;
    public TextView Sms;
    public TextView Telefoon;
    public TextView ADatum;
    public TextView ATijd;
    public TextView Niveau;
    public TextView TempC;
    public TextView Humidity;
    public TextView Lux;
    public TextView Sensor1;
    public TextView Sensor2;
    public TextView Sensor3;
    public TextView Sensor1Gem;
    public TextView Sensor2Gem;
    public TextView Sensor3Gem;
    public TextView Droog1;
    public TextView Droog2;
    public TextView Droog3;
    public Button ResetA;

    public TextView Status01;
    public TextView Status02;
    public TextView Status03;
    public TextView Status04;
    public TextView Status05;
    public TextView Status06;
    public TextView Status07;
    public TextView Status08;

    public TextView Opm01;
    public TextView Opm02;
    public TextView Opm03;
    public TextView Opm04;
    public TextView Opm05;
    public TextView Opm06;
    public TextView Opm07;
    public TextView Opm08;



	@Override
	public View onCreateView(LayoutInflater inflater, ViewGroup container,
			Bundle savedInstanceState) {
       	// Get the view from fragment_1
		View myView = inflater.inflate(R.layout.fragment_1, container, false);

        String myTag = getTag();
        ((MainActivity)getActivity()).setTabFragment1(myTag);

        Status01 = (TextView)myView.findViewById(R.id.TV01);
        Status02 = (TextView)myView.findViewById(R.id.TV02);
        Status03 = (TextView)myView.findViewById(R.id.TV03);
        Status04 = (TextView)myView.findViewById(R.id.TV04);
        Status05 = (TextView)myView.findViewById(R.id.TV05);
        Status06 = (TextView)myView.findViewById(R.id.TV06);
        Status07 = (TextView)myView.findViewById(R.id.TV07);
        Status08 = (TextView)myView.findViewById(R.id.TV08);

        Opm01 = (TextView)myView.findViewById(R.id.TVO01);
        Opm02 = (TextView)myView.findViewById(R.id.TVO02);
        Opm03 = (TextView)myView.findViewById(R.id.TVO03);
        Opm04 = (TextView)myView.findViewById(R.id.TVO04);
        Opm05 = (TextView)myView.findViewById(R.id.TVO05);
        Opm06 = (TextView)myView.findViewById(R.id.TVO06);
        Opm07 = (TextView)myView.findViewById(R.id.TVO07);
        Opm08 = (TextView)myView.findViewById(R.id.TVO08);

        Schijf = (TextView)myView.findViewById(R.id.tvSchijf);
        Bestand = (TextView)myView.findViewById(R.id.tvBestand);
        Sms = (TextView)myView.findViewById(R.id.tvSMS);
        Telefoon = (TextView)myView.findViewById(R.id.tvTelefoon);
        ADatum = (TextView)myView.findViewById(R.id.tvADatum);
        ATijd = (TextView)myView.findViewById(R.id.tvATijd);
        Niveau = (TextView)myView.findViewById(R.id.tvNiveau);
        TempC = (TextView)myView.findViewById(R.id.tVTempC);
        Humidity = (TextView)myView.findViewById(R.id.tVHum);
        Lux = (TextView)myView.findViewById(R.id.tVLux);
        Sensor1 = (TextView)myView.findViewById(R.id.tVSensor1);
        Sensor2 = (TextView)myView.findViewById(R.id.tVSensor2);
        Sensor3 = (TextView)myView.findViewById(R.id.tVSensor3);
        Sensor1Gem = (TextView)myView.findViewById(R.id.tVGem1);
        Sensor2Gem = (TextView)myView.findViewById(R.id.tVGem2);
        Sensor3Gem = (TextView)myView.findViewById(R.id.tVGem3);
        Droog1 = (TextView)myView.findViewById(R.id.tVDroog1);
        Droog2 = (TextView)myView.findViewById(R.id.tVDroog2);
        Droog3 = (TextView)myView.findViewById(R.id.tVDroog3);

        ResetA = (Button)myView.findViewById(R.id.btReset);
        ResetA.setOnClickListener(GoResetArduino);

		return myView;
	}

    View.OnClickListener GoResetArduino = new View.OnClickListener() {
        @Override
        public void onClick(View v) {   // Reset Arduino
            ResetArduino();
        }
    };

    public void ResetArduino() {
        ((MainActivity)getActivity()).writeData("z#");	// verzend de waarde naar de arduino
        // ga nu eentijdje niks doen en dan een writedata('c#') doen
    }


    @Override
    public void onResume() {
        super.onResume();
        Log.d(TAG, " onResume in fragmenttab1 is aangeroepen");
    }
}

package nl.drahmann.kerstboom02;


import android.graphics.Color;
import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import com.androidplot.Plot;
import com.androidplot.util.PlotStatistics;
import com.androidplot.util.Redrawer;
import com.androidplot.xy.BoundaryMode;
import com.androidplot.xy.LineAndPointFormatter;
import com.androidplot.xy.SimpleXYSeries;
import com.androidplot.xy.XYPlot;
import com.androidplot.xy.XYStepMode;
import java.text.DecimalFormat;
import java.util.Arrays;

public class FragmentTab2 extends Fragment {
    static final String TAG = "BDR";
    public int sensor1;
    public int sensor2;
    public int sensor3;

    private static final int HISTORY_SIZE = 150;            // number of points to plot in history
   
    private XYPlot sHistoryPlot = null;
    
    private SimpleXYSeries s1HistorySeries = null; // history series voor sensor1
    private SimpleXYSeries s2HistorySeries = null;
    private SimpleXYSeries s3HistorySeries = null;

    private Redrawer redrawer;

    @Override
	public View onCreateView(LayoutInflater inflater, ViewGroup container,
			Bundle savedInstanceState) {
		// Get the view from fragmenttab2.xml
		View view = inflater.inflate(R.layout.fragment_2, container, false);

        String myTag = getTag();
        ((MainActivity)getActivity()).setTabFragment2(myTag);

        // setup de sensor history plot
        sHistoryPlot = (XYPlot) view.findViewById(R.id.sHistoryPlot);

        s1HistorySeries = new SimpleXYSeries("Sensor 1");
        s1HistorySeries.useImplicitXVals();
        s2HistorySeries = new SimpleXYSeries("Sensor 2");
        s2HistorySeries.useImplicitXVals();
        s3HistorySeries = new SimpleXYSeries("Sensor 3");
        s3HistorySeries.useImplicitXVals();

        sHistoryPlot.setRangeBoundaries(0, 1050, BoundaryMode.AUTO);
        sHistoryPlot.setDomainBoundaries(0, HISTORY_SIZE, BoundaryMode.FIXED);
        sHistoryPlot.addSeries(s1HistorySeries,
                new LineAndPointFormatter(
                        Color.rgb(100, 100, 200), null, null, null));
        sHistoryPlot.addSeries(s2HistorySeries,
                new LineAndPointFormatter(
                        Color.rgb(100, 200, 100), null, null, null));
        sHistoryPlot.addSeries(s3HistorySeries,
                new LineAndPointFormatter(
                        Color.rgb(200, 100, 100), null, null, null));
        sHistoryPlot.setDomainStepMode(XYStepMode.INCREMENT_BY_VAL);
        sHistoryPlot.setDomainStepValue(HISTORY_SIZE/10);
        sHistoryPlot.setTicksPerRangeLabel(3);
        sHistoryPlot.setDomainLabel("Aantal samples");
        sHistoryPlot.getDomainLabelWidget().pack();
        sHistoryPlot.setRangeLabel("sensorwaarde");
        sHistoryPlot.getRangeLabelWidget().pack();

        sHistoryPlot.setRangeValueFormat(new DecimalFormat("#"));
        sHistoryPlot.setDomainValueFormat(new DecimalFormat("#"));

        final PlotStatistics histStats = new PlotStatistics(1000, false);

        sHistoryPlot.addListener(histStats);

        redrawer = new Redrawer(
                Arrays.asList(new Plot[]{sHistoryPlot}),
                100, false);


        return view;
	}

    @Override
    public void onResume() {
        super.onResume();
        redrawer.start();
        Log.d(TAG, " onResume in fragmenttab2 is aangeroepen");
    }

    @Override
    public void onPause() {
        redrawer.pause();
        super.onPause();
    }

    @Override
    public void onDestroy() {
        redrawer.finish();
        super.onDestroy();
    }

    // Called whenever a new orSensor reading is taken.

    public void onSensorChanged() {
        // Log.d(TAG, " F2_113 onSensorChanged aangeroepen");
        // get rid the oldest sample in history:
        if (s3HistorySeries.size() > HISTORY_SIZE) {
            s3HistorySeries.removeFirst();
            s2HistorySeries.removeFirst();
            s1HistorySeries.removeFirst();
        }
        // add the latest history sample:

        s1HistorySeries.addLast(null, sensor1);
        s2HistorySeries.addLast(null, sensor2);
        s3HistorySeries.addLast(null, sensor3);


    }





}

package nl.drahmann.kerstboom02;

import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import java.util.Calendar;

public class FragmentTab3 extends Fragment {

    Calendar now = Calendar.getInstance();
    static final String TAG = "BDR";

	@Override
	public View onCreateView(LayoutInflater inflater, ViewGroup container,
			Bundle savedInstanceState) {
		// Get the view from fragment_3
		View mijnView = inflater.inflate(R.layout.fragment_3, container, false);

        String myTag = getTag();
        ((MainActivity)getActivity()).setTabFragment3(myTag);


		return mijnView;
	}

    @Override
    public void onResume() {
        super.onResume();
        Log.d(TAG, " onResume in fragmenttab3 is aangeroepen");
    }

}

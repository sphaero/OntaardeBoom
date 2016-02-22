package nl.drahmann.kerstboom02;

import android.content.Context;
import android.support.v4.app.Fragment;
import android.support.v4.app.FragmentManager;
import android.support.v4.app.FragmentPagerAdapter;

public class ViewPagerAdapter extends FragmentPagerAdapter {

	final int PAGE_COUNT = 3;
	// Tab Titles
	private String tabtitles[] = new String[] { "Arduino data", "Plots", "TABtitel 3" };
	Context context;

	public ViewPagerAdapter(FragmentManager fm) {
		super(fm);
	}

	@Override
	public int getCount() {
		return PAGE_COUNT;
	}

	@Override
	public Fragment getItem(int position) {
		switch (position) {
            case 0: // Open FragmentTab1.java
                FragmentTab1 fragmenttab1 = new FragmentTab1();
                return fragmenttab1;
            case 1: // Open FragmentTab2.java
                FragmentTab2 fragmenttab2 = new FragmentTab2();
                return fragmenttab2;
            case 2: // Open FragmentTab3.java
                FragmentTab3 fragmenttab3 = new FragmentTab3();
                return fragmenttab3;
            default:
                return null;
        }
	}

	@Override
	public CharSequence getPageTitle(int position) {
		return tabtitles[position];
	}
}

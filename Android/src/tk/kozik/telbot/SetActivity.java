package tk.kozik.telbot;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.Window;
import android.widget.Button;
import android.widget.SeekBar;
import android.widget.SeekBar.OnSeekBarChangeListener;
import android.widget.TextView;

public class SetActivity extends Activity {
	protected static final String MIN_VALUE = "min_value";
	protected static final String MAX_VALUE = "max_value";
	protected static final String AKK_VOL = "akk_vol";
	private TextView min_textview;
	private TextView max_textview;
	private TextView akk_textview;

	 @Override
	    protected void onCreate(Bundle savedInstanceState) {
	        super.onCreate(savedInstanceState);

	        // Setup the window
	        requestWindowFeature(Window.FEATURE_INDETERMINATE_PROGRESS);
	        setContentView(R.layout.set);

	        // Set result CANCELED incase the user backs out
	        setResult(Activity.RESULT_CANCELED);
	        
	        
	        
	        SeekBar m_seekBar = (SeekBar) findViewById(R.id.seekBar_motor_min);
	        m_seekBar.setProgress(getIntent().getExtras().getInt(MIN_VALUE));
	        min_textview = (TextView) findViewById(R.id.textView_motor_min);
	        min_textview.setText(String.format(getResources().getString(R.string.motor_min_power), m_seekBar.getProgress()));
	        m_seekBar.setOnSeekBarChangeListener(new OnSeekBarChangeListener(){

				@Override
				public void onProgressChanged(SeekBar arg0, int arg1,
						boolean arg2) {
					min_textview.setText(String.format(getResources().getString(R.string.motor_min_power), arg1));
					
				}

				@Override
				public void onStartTrackingTouch(SeekBar arg0) {
					// TODO Auto-generated method stub
					
				}

				@Override
				public void onStopTrackingTouch(SeekBar arg0) {
					// TODO Auto-generated method stub
					
				}
	        	
	        });
	        
	        m_seekBar = (SeekBar) findViewById(R.id.seekBar_motor_max);
	        m_seekBar.setProgress(getIntent().getExtras().getInt(MAX_VALUE));
	        max_textview = (TextView) findViewById(R.id.textView_motor_max);
	        max_textview.setText(String.format(getResources().getString(R.string.motor_max_power), m_seekBar.getProgress()));
	        m_seekBar.setOnSeekBarChangeListener(new OnSeekBarChangeListener(){

				@Override
				public void onProgressChanged(SeekBar arg0, int arg1,
						boolean arg2) {
					max_textview.setText(String.format(getResources().getString(R.string.motor_max_power), arg1));
					
				}

				@Override
				public void onStartTrackingTouch(SeekBar arg0) {
					// TODO Auto-generated method stub
					
				}

				@Override
				public void onStopTrackingTouch(SeekBar arg0) {
					// TODO Auto-generated method stub
					
				}
	        	
	        });
	        
	        m_seekBar = (SeekBar) findViewById(R.id.seekBar_akk);
	        m_seekBar.setProgress(getIntent().getExtras().getInt(AKK_VOL));
	        akk_textview = (TextView) findViewById(R.id.textView_akk);
	        akk_textview.setText(String.format(getResources().getString(R.string.akk), m_seekBar.getProgress()/10.0));
	        m_seekBar.setOnSeekBarChangeListener(new OnSeekBarChangeListener(){

				@Override
				public void onProgressChanged(SeekBar arg0, int arg1,
						boolean arg2) {
					akk_textview.setText(String.format(getResources().getString(R.string.akk), (arg1/10.0)));					
				}

				@Override
				public void onStartTrackingTouch(SeekBar arg0) {
					// TODO Auto-generated method stub
					
				}

				@Override
				public void onStopTrackingTouch(SeekBar arg0) {
					// TODO Auto-generated method stub
					
				}
	        	
	        });
	        Button scanButton = (Button) findViewById(R.id.button_ok);
	        scanButton.setOnClickListener(new OnClickListener() {
	            public void onClick(View v) {
	            	Intent intent = new Intent();
	            	SeekBar m_seekBar = (SeekBar) findViewById(R.id.seekBar_motor_min);
	            	intent.putExtra(MIN_VALUE, m_seekBar.getProgress());
	            	m_seekBar = (SeekBar) findViewById(R.id.seekBar_motor_max);
	            	intent.putExtra(MAX_VALUE, m_seekBar.getProgress());
	            	m_seekBar = (SeekBar) findViewById(R.id.seekBar_akk);
	            	intent.putExtra(AKK_VOL, m_seekBar.getProgress());
		            // Set result and finish this Activity
		            setResult(Activity.RESULT_OK, intent);
		            finish();
	                
	            }
	        });

	   }

	    @Override
	    protected void onDestroy() {
	        super.onDestroy();	       
	    }



}

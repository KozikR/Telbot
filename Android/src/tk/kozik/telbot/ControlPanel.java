package tk.kozik.telbot;

import tk.kozik.telbot.R.id;
import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.ImageButton;
import android.widget.ProgressBar;
import android.widget.SeekBar;
import android.widget.SeekBar.OnSeekBarChangeListener;
import android.widget.TextView;
import android.widget.Toast;


/**
 * Manual control activity
 */
public class ControlPanel extends Activity {
    // Debugging
    private static final String TAG = "BluetoothChat";
    private static final boolean D = true;
    
    // Message types sent from the BluetoothChatService Handler
    public static final int MESSAGE_STATE_CHANGE = 1;
    public static final int MESSAGE_READ = 2;
    public static final int MESSAGE_WRITE = 3;
    public static final int MESSAGE_DEVICE_NAME = 4;
    public static final int MESSAGE_TOAST = 5;

    // Key names received from the BluetoothChatService Handler
    public static final String DEVICE_NAME = "device_name";
    public static final String TOAST = "toast";

    // Intent request codes
    private static final int REQUEST_CONNECT_DEVICE = 1;
    private static final int REQUEST_ENABLE_BT = 2;
    private static final int REQUEST_SET = 3;

    // Layout Views
    private TextView mTitle;
    private TextView mPower;
    private Button mSendButton;
    private ImageButton mControlButton;
    private SeekBar mControlSeekBar;
    private ProgressBar front_bar;
    private ProgressBar back_bar;
    private CheckBox m_check_box;
    private Intent serverIntent;
    private Intent setIntent;

    private SharedPreferences sharedPref;
    // Name of the connected device
    private String mConnectedDeviceName = null;
    private StringBuffer mOutStringBuffer;
    // Local Bluetooth adapter
    private BluetoothAdapter mBluetoothAdapter = null;
    // Member object for the chat services
    private BluetoothChatService mChatService = null;
    
    //Sensors data
    double calibriation = 4.52/68.1; 
    int power = 0;
    int front = 0;
    int back = 0;
    
    int min_motor_pow = 40;
    
    //Sensor data buff
    byte[] sensor_data_buff = new byte[10]; 
    int current_pos = 0;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        if(D) Log.e(TAG, "+++ ON CREATE +++");

        // Set up the window layout
        setContentView(R.layout.activity_control_panel);
        
        // Set up the status panel
        mTitle = (TextView) findViewById(R.id.status_panel);
        mTitle.setText(R.string.app_name);
        mPower = (TextView) findViewById(R.id.akk_panel);
        mPower.setText(String.format(getResources().getString(R.string.akk_voltage), 3.3*calibriation*power/255));
        
        front_bar = (ProgressBar) findViewById(R.id.front_bar);
        back_bar = (ProgressBar) findViewById(R.id.back_bar);
        
        // Get local Bluetooth adapter
        mBluetoothAdapter = BluetoothAdapter.getDefaultAdapter();

        // If the adapter is null, then Bluetooth is not supported
        if (mBluetoothAdapter == null) {
            Toast.makeText(this, "Bluetooth is not available", Toast.LENGTH_LONG).show();
            finish();
            return;
        }
        
        if(D) Log.e(TAG, "+++ A +++");
    }

    @Override
    public void onStart() {
        super.onStart();
        if(D) Log.e(TAG, "++ ON START ++");

        // If BT is not on, request that it be enabled.
        // setupChat() will then be called during onActivityResult
        if (!mBluetoothAdapter.isEnabled()) {
            Intent enableIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
            startActivityForResult(enableIntent, REQUEST_ENABLE_BT);
        // Otherwise, setup the chat session
        } else {
            if (mChatService == null) setupChat();
        }
    }

    @Override
    public synchronized void onResume() {
        super.onResume();
        if(D) Log.e(TAG, "+ ON RESUME +");

        // Performing this check in onResume() covers the case in which BT was
        // not enabled during onStart(), so we were paused to enable it...
        // onResume() will be called when ACTION_REQUEST_ENABLE activity returns.
        if (mChatService != null) {
            // Only if the state is STATE_NONE, do we know that we haven't started already
            if (mChatService.getState() == BluetoothChatService.STATE_NONE) {
              // Start the Bluetooth chat services
              mChatService.start();
            }
        }
    }

    private void setupChat() {
        Log.d(TAG, "setupChat()");
        //Menu
        sharedPref = this.getSharedPreferences(getString(R.string.preference_file_key), Context.MODE_PRIVATE);
        calibriation = sharedPref.getFloat(getResources().getString(R.string.saved_akk), (float) 0.06637298);
        min_motor_pow = sharedPref.getInt(getResources().getString(R.string.saved_motor_min), 40);
        
        
        mSendButton = (Button) findViewById(R.id.menu_connect);
        serverIntent = new Intent(this, DeviceListActivity.class);
        mSendButton.setOnClickListener(new OnClickListener() {
        	@Override
            public void onClick(View v) {        		 
                startActivityForResult(serverIntent, REQUEST_CONNECT_DEVICE);
            }
        });
        mSendButton = (Button) findViewById(R.id.menu_settings);
        setIntent = new Intent(this, SetActivity.class);
        mSendButton.setOnClickListener(new OnClickListener() {
        	@Override
            public void onClick(View v) {        	
        		SeekBar m_seekBar= (SeekBar) findViewById(id.control_speed);
        		setIntent.putExtra(SetActivity.MAX_VALUE, m_seekBar.getMax()+min_motor_pow);
        		setIntent.putExtra(SetActivity.MIN_VALUE, min_motor_pow);
        		Double temp = calibriation*power*10;
        		Log.e(TAG, Integer.toString(temp.intValue()));
        		setIntent.putExtra(SetActivity.AKK_VOL,   temp.intValue());
                startActivityForResult(setIntent, REQUEST_SET);
            }
        });
        mSendButton = (Button) findViewById(R.id.menu_face);
        mSendButton.setOnClickListener(new OnClickListener(){

			@Override
			public void onClick(View arg0) {
				Toast.makeText(getApplicationContext(), R.string.not_implement_yet, Toast.LENGTH_SHORT).show();		
			}
        	
        });
        //Motors control
        mSendButton = (Button) findViewById(R.id.button_led_blue);
        mSendButton.setOnClickListener(new OnClickListener() {
        	@Override
            public void onClick(View v) {
                sendMessage("!LBT#");
            }
        });
        mSendButton = (Button) findViewById(R.id.button_led_red);
        mSendButton.setOnClickListener(new OnClickListener() {
        	@Override
            public void onClick(View v) {
                sendMessage("!LRT#");
            }
        });
        
        mControlButton = (ImageButton) findViewById(R.id.control_stop);
        mControlButton.setOnClickListener(new OnClickListener() {
        	@Override
            public void onClick(View v) {
                sendMessage("!MS#");
            }
        });
        mControlButton = (ImageButton) findViewById(R.id.control_forward);
        mControlButton.setOnClickListener(new OnClickListener() {
        	@Override
            public void onClick(View v) {
                sendMessage("!MD11#");
            }
        });
        mControlButton = (ImageButton) findViewById(R.id.control_back);
        mControlButton.setOnClickListener(new OnClickListener() {
        	@Override
            public void onClick(View v) {
                sendMessage("!MD22#");
            }
        });
        mControlButton = (ImageButton) findViewById(R.id.control_left);
        mControlButton.setOnClickListener(new OnClickListener() {
        	@Override
            public void onClick(View v) {
                sendMessage("!MD01#");
            }
        });
        mControlButton = (ImageButton) findViewById(R.id.control_right);
        mControlButton.setOnClickListener(new OnClickListener() {
        	@Override
            public void onClick(View v) {
                sendMessage("!MD10#");
            }
        });
        mControlButton = (ImageButton) findViewById(R.id.control_rotate_left);
        mControlButton.setOnClickListener(new OnClickListener() {
        	@Override
            public void onClick(View v) {
                sendMessage("!MD21#");
            }
        });
        mControlButton = (ImageButton) findViewById(R.id.control_rotate_right);
        mControlButton.setOnClickListener(new OnClickListener() {
        	@Override
            public void onClick(View v) {
                sendMessage("!MD12#");
            }
        });
        mControlSeekBar = (SeekBar)	findViewById(R.id.control_speed);
        mControlSeekBar.setMax(sharedPref.getInt(getResources().getString(R.string.saved_motor_max), 100));
        mControlSeekBar.setOnSeekBarChangeListener(new OnSeekBarChangeListener() {        
        	@Override
			public void onProgressChanged(SeekBar seekBar, int progress,
					boolean fromUser) {
				progress+=min_motor_pow;
				if(progress < 10) 	sendMessage("!MPB00"+Integer.toString(progress)+"#");
				else if(progress < 100)	sendMessage("!MPB0"+Integer.toString(progress)+"#");
				else				sendMessage("!MPB"+Integer.toString(progress)+"#");
        	}

			@Override
			public void onStartTrackingTouch(SeekBar seekBar) {}

			@Override
			public void onStopTrackingTouch(SeekBar seekBar) {}
        });
        
        m_check_box = (CheckBox) findViewById(R.id.checkBox_safe_mode);
        m_check_box.setOnClickListener(new OnClickListener(){

			@Override
			public void onClick(View v) {
				if (((CheckBox) v).isChecked()) {
					sendMessage("!SS#");
				}
				else
				{
					sendMessage("!SR#");
				}
				
			}        	
        });
       

        // Initialize the BluetoothChatService to perform bluetooth connections
        mChatService = new BluetoothChatService(this, mHandler);

        // Initialize the buffer for outgoing messages
        mOutStringBuffer = new StringBuffer("");
    }

    @Override
    public synchronized void onPause() {
        super.onPause();
        if(D) Log.e(TAG, "- ON PAUSE -");
    }

    @Override
    public void onStop() {
        super.onStop();
        if(D) Log.e(TAG, "-- ON STOP --");
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        // Stop the Bluetooth chat services
        if (mChatService != null) mChatService.stop();
        if(D) Log.e(TAG, "--- ON DESTROY ---");
    }

    private void ensureDiscoverable() {
        if(D) Log.d(TAG, "ensure discoverable");
        if (mBluetoothAdapter.getScanMode() !=
            BluetoothAdapter.SCAN_MODE_CONNECTABLE_DISCOVERABLE) {
            Intent discoverableIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_DISCOVERABLE);
            discoverableIntent.putExtra(BluetoothAdapter.EXTRA_DISCOVERABLE_DURATION, 300);
            startActivity(discoverableIntent);
        }
    }

    /**
     * Sends a message.
     * @param message  A string of text to send.
     */
    private void sendMessage(String message) {
        // Check that we're actually connected before trying anything
        if (mChatService.getState() != BluetoothChatService.STATE_CONNECTED) {
           // Toast.makeText(this, R.string.not_connected, Toast.LENGTH_SHORT).show();
            return;
        }

        // Check that there's actually something to send
        if (message.length() > 0) {
            // Get the message bytes and tell the BluetoothChatService to write
            byte[] send = message.getBytes();
            mChatService.write(send);

            // Reset out string buffer to zero and clear the edit text field
            mOutStringBuffer.setLength(0);
            //mOutEditText.setText(mOutStringBuffer);
        }
    }

    // The Handler that gets information back from the BluetoothChatService
    private final Handler mHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {
            case MESSAGE_STATE_CHANGE:
                if(D) Log.i(TAG, "MESSAGE_STATE_CHANGE: " + msg.arg1);
                switch (msg.arg1) {
                case BluetoothChatService.STATE_CONNECTED:
                    mTitle.setText(R.string.title_connected_to);
                    mTitle.append(mConnectedDeviceName);
                    break;
                case BluetoothChatService.STATE_CONNECTING:
                    mTitle.setText(R.string.title_connecting);
                    break;
                case BluetoothChatService.STATE_LISTEN:
                case BluetoothChatService.STATE_NONE:
                	mTitle.setText(R.string.title_not_connected);
                    break;
                }
                break;
            case MESSAGE_WRITE:
               break;
            case MESSAGE_READ:
                byte[] readBuf = (byte[]) msg.obj;
                if(readBuf[0] == 'G')
                {
                    Toast.makeText(getApplicationContext(), R.string.front_edge_not, Toast.LENGTH_SHORT).show();
                }
                else if(readBuf[0] == 'C')
                {
                	Toast.makeText(getApplicationContext(), R.string.back_edge_not, Toast.LENGTH_SHORT).show();
                }
                else if(readBuf[0] == 'P' || readBuf[0] == 'F' || readBuf[0] == 'B')
                {
                	current_pos = 0;
                }
                for(int i = 0; i < msg.arg1; i++ )
                {
                	sensor_data_buff[current_pos] = readBuf[i];
                	current_pos++;
                	if(current_pos > 9) current_pos = 0;
                }
                if(current_pos == 5)
                {
                    int temp = (sensor_data_buff[1]-'0')*100+(sensor_data_buff[2]-'0')*10+sensor_data_buff[3]-'0';
                	switch(sensor_data_buff[0])
                	{
                	case 'P':
                		/*TODO calculate the aferage value of power from last ten value*/
                		power = temp;
                		mPower.setText(String.format(getResources().getString(R.string.akk_voltage), calibriation*power));
                		break;
                	case 'F':
                		front = temp;
                		front_bar.setProgress((int) 100*front/255);
                		break;
                	case 'B':
                		back = temp;
                		back_bar.setProgress((int) 100*back/255);
                		break;
                	}
                }
                break;
            case MESSAGE_DEVICE_NAME:
                // save the connected device's name
                mConnectedDeviceName = msg.getData().getString(DEVICE_NAME);
                Toast.makeText(getApplicationContext(), "Connected to "
                               + mConnectedDeviceName, Toast.LENGTH_SHORT).show();
                break;
            case MESSAGE_TOAST:
                Toast.makeText(getApplicationContext(), msg.getData().getString(TOAST),
                               Toast.LENGTH_SHORT).show();
                break;
            }
        }
    };

    public void onActivityResult(int requestCode, int resultCode, Intent data) {
        if(D) Log.d(TAG, "onActivityResult " + resultCode);
        switch (requestCode) {
        case REQUEST_CONNECT_DEVICE:
            // When DeviceListActivity returns with a device to connect
            if (resultCode == Activity.RESULT_OK) {
                // Get the device MAC address
                String address = data.getExtras()
                                     .getString(DeviceListActivity.EXTRA_DEVICE_ADDRESS);
                // Get the BLuetoothDevice object
                BluetoothDevice device = mBluetoothAdapter.getRemoteDevice(address);
                // Attempt to connect to the device
                mChatService.connect(device);
            }
            break;
        case REQUEST_ENABLE_BT:
            // When the request to enable Bluetooth returns
            if (resultCode == Activity.RESULT_OK) {
                // Bluetooth is now enabled, so set up a chat session
                setupChat();
            } else {
                // User did not enable Bluetooth or an error occured
                Log.d(TAG, "BT not enabled");
                Toast.makeText(this, R.string.bt_not_enabled_leaving, Toast.LENGTH_SHORT).show();
                finish();
            }
            break;
        case REQUEST_SET:
        	if (resultCode == Activity.RESULT_OK)
        	{
        		Editor edit = sharedPref.edit();
        		
        		min_motor_pow = data.getExtras().getInt(SetActivity.MIN_VALUE);
        		edit.putInt(getResources().getString(R.string.saved_motor_min), min_motor_pow);
        		
        		SeekBar m_seekBar = (SeekBar) findViewById(R.id.control_speed);
        		m_seekBar.setMax(data.getExtras().getInt(SetActivity.MAX_VALUE)-min_motor_pow);
        		edit.putInt(getResources().getString(R.string.saved_motor_max), m_seekBar.getMax());
        		
        		calibriation = (data.getExtras().getInt(SetActivity.AKK_VOL)/10.0)/(1.0*power);		
        		edit.putFloat(getResources().getString(R.string.saved_akk), (float) calibriation);
        		
        		edit.commit();
        		
        	}
        	else
        	{
        		
        	}
        	break;
        default:
        	break;
        }
    }
}
package ru.bootcode.btscaner;

import android.app.Activity;
import android.widget.Toast;

public class ShowToast implements Runnable
{
    Activity mContext;
    String message;

    public ShowToast(Activity mContext, String message) {
        this.mContext = mContext;
        this.message = message;
    }

    public void run() {
        Toast.makeText(mContext.getApplicationContext(), message, Toast.LENGTH_LONG).show();
    }

    public void toast()
    {
        mContext.runOnUiThread(this);
    }
}
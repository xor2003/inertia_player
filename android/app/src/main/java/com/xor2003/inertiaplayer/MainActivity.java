package com.xor2003.inertiaplayer;

import android.app.Activity;
import android.content.Intent;
import android.net.Uri;
import android.os.Bundle;
import org.libsdl.app.SDLActivity;

import java.io.File;
import java.io.FileOutputStream;
import java.io.InputStream;

public final class MainActivity extends SDLActivity {
    private static final int OPEN_MODULE = 1201;
    private static MainActivity instance;

    private static native void nativeSetSelectedFile(String path);

    @Override
    protected void onCreate(Bundle state) {
        instance = this;
        super.onCreate(state);
    }

    public static void openModulePicker() {
        final MainActivity activity = instance;
        if (activity == null) return;
        activity.runOnUiThread(() -> {
            Intent intent = new Intent(Intent.ACTION_OPEN_DOCUMENT);
            intent.addCategory(Intent.CATEGORY_OPENABLE);
            intent.setType("*/*");
            intent.putExtra(Intent.EXTRA_MIME_TYPES, new String[] {
                "audio/*", "application/octet-stream"
            });
            activity.startActivityForResult(intent, OPEN_MODULE);
        });
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        if (requestCode != OPEN_MODULE || resultCode != Activity.RESULT_OK ||
                data == null || data.getData() == null) return;
        Uri uri = data.getData();
        File target = new File(getFilesDir(), "selected-module");
        try (InputStream input = getContentResolver().openInputStream(uri);
             FileOutputStream output = new FileOutputStream(target)) {
            if (input == null) return;
            byte[] buffer = new byte[32768];
            int count;
            while ((count = input.read(buffer)) >= 0) output.write(buffer, 0, count);
            nativeSetSelectedFile(target.getAbsolutePath());
        } catch (Exception error) {
            android.util.Log.e("InertiaPlayer", "Could not open selected module", error);
        }
    }
}

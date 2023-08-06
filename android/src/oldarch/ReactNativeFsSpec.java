package com.drpogodin.reactnativefs;

import com.facebook.react.bridge.ReactApplicationContext;
import com.facebook.react.bridge.ReactContextBaseJavaModule;
import com.facebook.react.bridge.Promise;

import java.util.Map;

abstract class ReactNativeFsSpec extends ReactContextBaseJavaModule {
  ReactNativeFsSpec(ReactApplicationContext context) {
    super(context);
  }

  public abstract Map<String,Object> getTypedExportedConstants();

  public Map<String,Object> getConstants() {
    return this.getTypedExportedConstants();
  }
}

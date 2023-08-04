// Encapsulates error reporting to RN.

package com.drpogodin.reactnativefs;

import com.facebook.react.bridge.Promise;

public enum Errors {
  NOT_IMPLEMENTED("This method is not implemented for Android");

  private final String message;
  public static final String LOGTAG = "RNFS";

  Errors(String message) {
    this.message = message;
  }

  public Error getError() {
    return new Error(this.message);
  }

  public String getMessage() {
    return this.message;
  }

  public void reject(Promise promise) {
    if (promise != null) {
      promise.reject(this.toString(), this.message, this.getError());
    }
  }

  public void reject(Promise promise, String details) {
    if (promise != null) {
      String message = this.message;
      if (details != null) message += ": " + details;
      promise.reject(this.toString(), message, this.getError());
    }
  }

  public String toString() {
    return LOGTAG + ":" + this.name();
  }
}

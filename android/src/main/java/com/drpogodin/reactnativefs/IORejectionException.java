// TODO: Is it really necessary, can't we rely on
// java.io.IOException instead, which is already used in some places?

package com.drpogodin.reactnativefs;

class IORejectionException extends Exception {
    private String code;

    public IORejectionException(String code, String message) {
        super(message);
        this.code = code;
    }

    public String getCode() {
        return code;
    }
}

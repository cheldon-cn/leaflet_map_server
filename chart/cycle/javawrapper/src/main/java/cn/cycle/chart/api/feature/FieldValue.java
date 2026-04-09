package cn.cycle.chart.api.feature;

public final class FieldValue {

    private final int type;
    private final Integer integerValue;
    private final Double realValue;
    private final String stringValue;
    private final boolean isNull;

    public static final int TYPE_INTEGER = 0;
    public static final int TYPE_REAL = 1;
    public static final int TYPE_STRING = 2;
    public static final int TYPE_UNKNOWN = -1;

    private FieldValue(int type, Integer integerValue, Double realValue,
                       String stringValue, boolean isNull) {
        this.type = type;
        this.integerValue = integerValue;
        this.realValue = realValue;
        this.stringValue = stringValue;
        this.isNull = isNull;
    }

    public static FieldValue ofInteger(int value) {
        return new FieldValue(TYPE_INTEGER, value, null, null, false);
    }

    public static FieldValue ofReal(double value) {
        return new FieldValue(TYPE_REAL, null, value, null, false);
    }

    public static FieldValue ofString(String value) {
        return new FieldValue(TYPE_STRING, null, null, value, false);
    }

    public static FieldValue ofNull() {
        return new FieldValue(TYPE_UNKNOWN, null, null, null, true);
    }

    public int getType() {
        return type;
    }

    public boolean isNull() {
        return isNull;
    }

    public Integer asInteger() {
        return integerValue;
    }

    public Double asReal() {
        return realValue;
    }

    public String asString() {
        return stringValue;
    }

    @Override
    public String toString() {
        if (isNull) {
            return "FieldValue(NULL)";
        }
        switch (type) {
            case TYPE_INTEGER:
                return "FieldValue(INTEGER=" + integerValue + ")";
            case TYPE_REAL:
                return "FieldValue(REAL=" + realValue + ")";
            case TYPE_STRING:
                return "FieldValue(STRING=" + stringValue + ")";
            default:
                return "FieldValue(UNKNOWN)";
        }
    }
}

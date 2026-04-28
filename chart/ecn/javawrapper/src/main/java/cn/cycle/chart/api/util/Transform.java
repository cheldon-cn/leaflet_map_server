package cn.cycle.chart.api.util;

public class Transform {

    private double[] matrix;

    public Transform() {
        this.matrix = new double[6];
        setIdentity();
    }

    public Transform(double[] matrix) {
        if (matrix == null || matrix.length != 6) {
            throw new IllegalArgumentException("Matrix must have 6 elements");
        }
        this.matrix = matrix.clone();
    }

    public void setIdentity() {
        matrix[0] = 1; matrix[1] = 0;
        matrix[2] = 0; matrix[3] = 1;
        matrix[4] = 0; matrix[5] = 0;
    }

    public double[] getMatrix() {
        return matrix.clone();
    }

    public void setMatrix(double[] matrix) {
        if (matrix == null || matrix.length != 6) {
            throw new IllegalArgumentException("Matrix must have 6 elements");
        }
        this.matrix = matrix.clone();
    }

    public void translate(double tx, double ty) {
        matrix[4] += matrix[0] * tx + matrix[2] * ty;
        matrix[5] += matrix[1] * tx + matrix[3] * ty;
    }

    public void scale(double sx, double sy) {
        matrix[0] *= sx;
        matrix[1] *= sx;
        matrix[2] *= sy;
        matrix[3] *= sy;
    }

    public void rotate(double angle) {
        double cos = Math.cos(angle);
        double sin = Math.sin(angle);
        double m0 = matrix[0];
        double m1 = matrix[1];
        double m2 = matrix[2];
        double m3 = matrix[3];
        
        matrix[0] = m0 * cos + m2 * sin;
        matrix[1] = m1 * cos + m3 * sin;
        matrix[2] = m2 * cos - m0 * sin;
        matrix[3] = m3 * cos - m1 * sin;
    }

    public void rotate(double angle, double cx, double cy) {
        translate(cx, cy);
        rotate(angle);
        translate(-cx, -cy);
    }

    public double[] transform(double x, double y) {
        double[] result = new double[2];
        result[0] = matrix[0] * x + matrix[2] * y + matrix[4];
        result[1] = matrix[1] * x + matrix[3] * y + matrix[5];
        return result;
    }

    public double[] transform(double[] point) {
        if (point == null || point.length < 2) {
            return null;
        }
        return transform(point[0], point[1]);
    }

    public double[] inverseTransform(double x, double y) {
        double det = matrix[0] * matrix[3] - matrix[1] * matrix[2];
        if (Math.abs(det) < 1e-10) {
            return null;
        }
        double[] result = new double[2];
        double dx = x - matrix[4];
        double dy = y - matrix[5];
        result[0] = (matrix[3] * dx - matrix[2] * dy) / det;
        result[1] = (matrix[0] * dy - matrix[1] * dx) / det;
        return result;
    }

    public Transform concatenate(Transform other) {
        if (other == null) {
            return this;
        }
        double[] result = new double[6];
        double[] m1 = this.matrix;
        double[] m2 = other.matrix;
        
        result[0] = m1[0] * m2[0] + m1[2] * m2[1];
        result[1] = m1[1] * m2[0] + m1[3] * m2[1];
        result[2] = m1[0] * m2[2] + m1[2] * m2[3];
        result[3] = m1[1] * m2[2] + m1[3] * m2[3];
        result[4] = m1[0] * m2[4] + m1[2] * m2[5] + m1[4];
        result[5] = m1[1] * m2[4] + m1[3] * m2[5] + m1[5];
        
        return new Transform(result);
    }

    public Transform createInverse() {
        double det = matrix[0] * matrix[3] - matrix[1] * matrix[2];
        if (Math.abs(det) < 1e-10) {
            return null;
        }
        double[] inv = new double[6];
        inv[0] = matrix[3] / det;
        inv[1] = -matrix[1] / det;
        inv[2] = -matrix[2] / det;
        inv[3] = matrix[0] / det;
        inv[4] = (matrix[2] * matrix[5] - matrix[3] * matrix[4]) / det;
        inv[5] = (matrix[1] * matrix[4] - matrix[0] * matrix[5]) / det;
        return new Transform(inv);
    }

    public double getScaleX() {
        return Math.sqrt(matrix[0] * matrix[0] + matrix[1] * matrix[1]);
    }

    public double getScaleY() {
        return Math.sqrt(matrix[2] * matrix[2] + matrix[3] * matrix[3]);
    }

    public double getRotation() {
        return Math.atan2(matrix[1], matrix[0]);
    }

    public double getTranslateX() {
        return matrix[4];
    }

    public double getTranslateY() {
        return matrix[5];
    }

    public Transform clone() {
        return new Transform(matrix);
    }

    @Override
    public String toString() {
        return String.format("Transform[[%.4f, %.4f, %.4f], [%.4f, %.4f, %.4f]]",
            matrix[0], matrix[2], matrix[4],
            matrix[1], matrix[3], matrix[5]);
    }
}

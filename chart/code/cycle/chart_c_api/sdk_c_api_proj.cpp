/**
 * @file sdk_c_api_proj.cpp
 * @brief OGC Chart SDK C API - Projection Module Implementation
 * @version v1.0
 * @date 2026-04-09
 */

#include "sdk_c_api.h"

#define _USE_MATH_DEFINES
#include <cmath>

#include <ogc/proj/coordinate_transformer.h>
#include <ogc/proj/proj_transformer.h>

#include <cstring>
#include <cstdlib>
#include <memory>
#include <string>

using namespace ogc;
using namespace ogc::proj;

#ifdef __cplusplus
extern "C" {
#endif

namespace { static std::string SafeString(const char* str) {
    return str ? std::string(str) : std::string();
}

struct LocalTransformMatrix {
    double m[9];
    LocalTransformMatrix() {
        for (int i = 0; i < 9; ++i) m[i] = 0.0;
        m[0] = m[4] = m[8] = 1.0;
    }
};

struct LocalCoordTransformer {
    std::string sourceCRS;
    std::string targetCRS;
    CoordinateTransformerPtr transformer;
};

}

ogc_coord_transformer_t* ogc_coord_transformer_create(const char* source_crs, const char* target_crs) {
    if (source_crs && target_crs) {
        LocalCoordTransformer* t = new LocalCoordTransformer();
        t->sourceCRS = SafeString(source_crs);
        t->targetCRS = SafeString(target_crs);
        t->transformer = CoordinateTransformer::Create(t->sourceCRS, t->targetCRS);
        return reinterpret_cast<ogc_coord_transformer_t*>(t);
    }
    return nullptr;
}

void ogc_coord_transformer_destroy(ogc_coord_transformer_t* transformer) {
    delete reinterpret_cast<LocalCoordTransformer*>(transformer);
}

int ogc_coord_transformer_is_valid(const ogc_coord_transformer_t* transformer) {
    if (transformer) {
        const LocalCoordTransformer* t = reinterpret_cast<const LocalCoordTransformer*>(transformer);
        return (t->transformer && t->transformer->IsValid()) ? 1 : 0;
    }
    return 0;
}

ogc_coordinate_t ogc_coord_transformer_transform(const ogc_coord_transformer_t* transformer, const ogc_coordinate_t* coord) {
    ogc_coordinate_t result = {0.0, 0.0};
    if (transformer && coord) {
        const LocalCoordTransformer* t = reinterpret_cast<const LocalCoordTransformer*>(transformer);
        if (t->transformer && t->transformer->IsValid()) {
            result.x = coord->x;
            result.y = coord->y;
            t->transformer->Transform(result.x, result.y);
        }
    }
    return result;
}

ogc_coordinate_t ogc_coord_transformer_transform_inverse(const ogc_coord_transformer_t* transformer, const ogc_coordinate_t* coord) {
    ogc_coordinate_t result = {0.0, 0.0};
    if (transformer && coord) {
        const LocalCoordTransformer* t = reinterpret_cast<const LocalCoordTransformer*>(transformer);
        if (t->transformer && t->transformer->IsValid()) {
            result.x = coord->x;
            result.y = coord->y;
            t->transformer->TransformInverse(result.x, result.y);
        }
    }
    return result;
}

void ogc_coord_transformer_transform_array(const ogc_coord_transformer_t* transformer, double* x, double* y, size_t count) {
    if (transformer && x && y && count > 0) {
        const LocalCoordTransformer* t = reinterpret_cast<const LocalCoordTransformer*>(transformer);
        if (t->transformer && t->transformer->IsValid()) {
            for (size_t i = 0; i < count; ++i) {
                t->transformer->Transform(x[i], y[i]);
            }
        }
    }
}

ogc_envelope_t* ogc_coord_transformer_transform_envelope(const ogc_coord_transformer_t* transformer, const ogc_envelope_t* env) {
    if (transformer && env) {
        const LocalCoordTransformer* t = reinterpret_cast<const LocalCoordTransformer*>(transformer);
        if (t->transformer && t->transformer->IsValid()) {
            double min_x = ogc_envelope_get_min_x(env);
            double min_y = ogc_envelope_get_min_y(env);
            double max_x = ogc_envelope_get_max_x(env);
            double max_y = ogc_envelope_get_max_y(env);
            t->transformer->Transform(min_x, min_y);
            t->transformer->Transform(max_x, max_y);
            return ogc_envelope_create_from_coords(min_x, min_y, max_x, max_y);
        }
    }
    return nullptr;
}

ogc_geometry_t* ogc_coord_transformer_transform_geometry(const ogc_coord_transformer_t* transformer, const ogc_geometry_t* geom) {
    if (transformer && geom) {
        const LocalCoordTransformer* t = reinterpret_cast<const LocalCoordTransformer*>(transformer);
        if (t->transformer && t->transformer->IsValid()) {
            GeometryPtr result = t->transformer->Transform(
                reinterpret_cast<const Geometry*>(geom));
            if (result) {
                return reinterpret_cast<ogc_geometry_t*>(result.release());
            }
        }
    }
    return nullptr;
}

const char* ogc_coord_transformer_get_source_crs(const ogc_coord_transformer_t* transformer) {
    if (transformer) {
        const LocalCoordTransformer* t = reinterpret_cast<const LocalCoordTransformer*>(transformer);
        return t->sourceCRS.c_str();
    }
    return "";
}

const char* ogc_coord_transformer_get_target_crs(const ogc_coord_transformer_t* transformer) {
    if (transformer) {
        const LocalCoordTransformer* t = reinterpret_cast<const LocalCoordTransformer*>(transformer);
        return t->targetCRS.c_str();
    }
    return "";
}

ogc_transform_matrix_t* ogc_transform_matrix_create(void) {
    return reinterpret_cast<ogc_transform_matrix_t*>(new LocalTransformMatrix());
}

ogc_transform_matrix_t* ogc_transform_matrix_create_identity(void) {
    return reinterpret_cast<ogc_transform_matrix_t*>(new LocalTransformMatrix());
}

ogc_transform_matrix_t* ogc_transform_matrix_create_translation(double tx, double ty) {
    LocalTransformMatrix* mat = new LocalTransformMatrix();
    mat->m[2] = tx;
    mat->m[5] = ty;
    return reinterpret_cast<ogc_transform_matrix_t*>(mat);
}

ogc_transform_matrix_t* ogc_transform_matrix_create_scale(double sx, double sy) {
    LocalTransformMatrix* mat = new LocalTransformMatrix();
    mat->m[0] = sx;
    mat->m[4] = sy;
    return reinterpret_cast<ogc_transform_matrix_t*>(mat);
}

ogc_transform_matrix_t* ogc_transform_matrix_create_rotation(double angle) {
    LocalTransformMatrix* mat = new LocalTransformMatrix();
    double c = cos(angle);
    double s = sin(angle);
    mat->m[0] = c;
    mat->m[1] = -s;
    mat->m[3] = s;
    mat->m[4] = c;
    return reinterpret_cast<ogc_transform_matrix_t*>(mat);
}

void ogc_transform_matrix_destroy(ogc_transform_matrix_t* matrix) {
    delete reinterpret_cast<LocalTransformMatrix*>(matrix);
}

void ogc_transform_matrix_translate(ogc_transform_matrix_t* matrix, double dx, double dy) {
    if (matrix) {
        LocalTransformMatrix* mat = reinterpret_cast<LocalTransformMatrix*>(matrix);
        mat->m[2] += dx;
        mat->m[5] += dy;
    }
}

void ogc_transform_matrix_scale(ogc_transform_matrix_t* matrix, double sx, double sy) {
    if (matrix) {
        LocalTransformMatrix* mat = reinterpret_cast<LocalTransformMatrix*>(matrix);
        mat->m[0] *= sx;
        mat->m[4] *= sy;
    }
}

void ogc_transform_matrix_rotate(ogc_transform_matrix_t* matrix, double angle_degrees) {
    if (matrix) {
        LocalTransformMatrix* mat = reinterpret_cast<LocalTransformMatrix*>(matrix);
        double rad = angle_degrees * M_PI / 180.0;
        double c = cos(rad);
        double s = sin(rad);
        double m0 = mat->m[0] * c - mat->m[3] * s;
        double m1 = mat->m[1] * c - mat->m[4] * s;
        double m2 = mat->m[2] * c - mat->m[5] * s;
        double m3 = mat->m[0] * s + mat->m[3] * c;
        double m4 = mat->m[1] * s + mat->m[4] * c;
        double m5 = mat->m[2] * s + mat->m[5] * c;
        mat->m[0] = m0; mat->m[1] = m1; mat->m[2] = m2;
        mat->m[3] = m3; mat->m[4] = m4; mat->m[5] = m5;
    }
}

void ogc_transform_matrix_multiply(ogc_transform_matrix_t* result, const ogc_transform_matrix_t* a, const ogc_transform_matrix_t* b) {
    if (result && a && b) {
        LocalTransformMatrix* r = reinterpret_cast<LocalTransformMatrix*>(result);
        const LocalTransformMatrix* ma = reinterpret_cast<const LocalTransformMatrix*>(a);
        const LocalTransformMatrix* mb = reinterpret_cast<const LocalTransformMatrix*>(b);
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                r->m[i * 3 + j] = 0;
                for (int k = 0; k < 3; ++k) {
                    r->m[i * 3 + j] += ma->m[i * 3 + k] * mb->m[k * 3 + j];
                }
            }
        }
    }
}

ogc_transform_matrix_t* ogc_transform_matrix_multiply_new(const ogc_transform_matrix_t* a, const ogc_transform_matrix_t* b) {
    ogc_transform_matrix_t* result = ogc_transform_matrix_create();
    ogc_transform_matrix_multiply(result, a, b);
    return result;
}

ogc_transform_matrix_t* ogc_transform_matrix_inverse(const ogc_transform_matrix_t* matrix) {
    if (matrix) {
        const LocalTransformMatrix* mat = reinterpret_cast<const LocalTransformMatrix*>(matrix);
        double det = mat->m[0] * (mat->m[4] * mat->m[8] - mat->m[5] * mat->m[7])
                   - mat->m[1] * (mat->m[3] * mat->m[8] - mat->m[5] * mat->m[6])
                   + mat->m[2] * (mat->m[3] * mat->m[7] - mat->m[4] * mat->m[6]);
        if (det != 0.0) {
            LocalTransformMatrix* inv = new LocalTransformMatrix();
            inv->m[0] = (mat->m[4] * mat->m[8] - mat->m[5] * mat->m[7]) / det;
            inv->m[1] = (mat->m[2] * mat->m[7] - mat->m[1] * mat->m[8]) / det;
            inv->m[2] = (mat->m[1] * mat->m[5] - mat->m[2] * mat->m[4]) / det;
            inv->m[3] = (mat->m[5] * mat->m[6] - mat->m[3] * mat->m[8]) / det;
            inv->m[4] = (mat->m[0] * mat->m[8] - mat->m[2] * mat->m[6]) / det;
            inv->m[5] = (mat->m[2] * mat->m[3] - mat->m[0] * mat->m[5]) / det;
            inv->m[6] = (mat->m[3] * mat->m[7] - mat->m[4] * mat->m[6]) / det;
            inv->m[7] = (mat->m[1] * mat->m[6] - mat->m[0] * mat->m[7]) / det;
            inv->m[8] = (mat->m[0] * mat->m[4] - mat->m[1] * mat->m[3]) / det;
            return reinterpret_cast<ogc_transform_matrix_t*>(inv);
        }
    }
    return nullptr;
}

ogc_coordinate_t ogc_transform_matrix_transform(const ogc_transform_matrix_t* matrix, const ogc_coordinate_t* coord) {
    ogc_coordinate_t result = {0.0, 0.0};
    if (matrix && coord) {
        const LocalTransformMatrix* mat = reinterpret_cast<const LocalTransformMatrix*>(matrix);
        result.x = mat->m[0] * coord->x + mat->m[1] * coord->y + mat->m[2];
        result.y = mat->m[3] * coord->x + mat->m[4] * coord->y + mat->m[5];
    }
    return result;
}

void ogc_transform_matrix_transform_point(const ogc_transform_matrix_t* matrix, double* x, double* y) {
    if (matrix && x && y) {
        const LocalTransformMatrix* mat = reinterpret_cast<const LocalTransformMatrix*>(matrix);
        double tx = mat->m[0] * (*x) + mat->m[1] * (*y) + mat->m[2];
        double ty = mat->m[3] * (*x) + mat->m[4] * (*y) + mat->m[5];
        *x = tx;
        *y = ty;
    }
}

void ogc_transform_matrix_get_values(const ogc_transform_matrix_t* matrix, double* values) {
    if (matrix && values) {
        const LocalTransformMatrix* mat = reinterpret_cast<const LocalTransformMatrix*>(matrix);
        for (int i = 0; i < 9; ++i) {
            values[i] = mat->m[i];
        }
    }
}

void ogc_transform_matrix_get_elements(const ogc_transform_matrix_t* matrix, double* elements) {
    ogc_transform_matrix_get_values(matrix, elements);
}

void ogc_transform_matrix_set_elements(ogc_transform_matrix_t* matrix, const double* elements) {
    if (matrix && elements) {
        LocalTransformMatrix* mat = reinterpret_cast<LocalTransformMatrix*>(matrix);
        for (int i = 0; i < 9; ++i) {
            mat->m[i] = elements[i];
        }
    }
}

#ifdef __cplusplus
}
#endif

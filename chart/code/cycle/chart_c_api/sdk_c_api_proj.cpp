/**
 * @file sdk_c_api_proj.cpp
 * @brief OGC Chart SDK C API - Projection Module Implementation
 * @version v1.0
 * @date 2026-04-09
 */

#include "sdk_c_api.h"

#include <ogc/proj/coordinate_transformer.h>
#include <ogc/proj/transform_matrix.h>

#include <cstring>
#include <cstdlib>
#include <memory>
#include <string>

using namespace ogc::proj;

#ifdef __cplusplus
extern "C" {
#endif

namespace {

std::string SafeString(const char* str) {
    return str ? std::string(str) : std::string();
}

}  

ogc_coordinate_transformer_t* ogc_coordinate_transformer_create(void) {
    return reinterpret_cast<ogc_coordinate_transformer_t*>(CoordinateTransformer::Create().release());
}

void ogc_coordinate_transformer_destroy(ogc_coordinate_transformer_t* transformer) {
    delete reinterpret_cast<CoordinateTransformer*>(transformer);
}

int ogc_coordinate_transformer_set_source_crs(ogc_coordinate_transformer_t* transformer, const char* crs) {
    if (transformer && crs) {
        return reinterpret_cast<CoordinateTransformer*>(transformer)->SetSourceCRS(SafeString(crs)) ? 1 : 0;
    }
    return 0;
}

int ogc_coordinate_transformer_set_target_crs(ogc_coordinate_transformer_t* transformer, const char* crs) {
    if (transformer && crs) {
        return reinterpret_cast<CoordinateTransformer*>(transformer)->SetTargetCRS(SafeString(crs)) ? 1 : 0;
    }
    return 0;
}

int ogc_coordinate_transformer_transform_point(const ogc_coordinate_transformer_t* transformer, double x, double y, double* out_x, double* out_y) {
    if (transformer && out_x && out_y) {
        return reinterpret_cast<const CoordinateTransformer*>(transformer)->Transform(x, y, *out_x, *out_y) ? 1 : 0;
    }
    return 0;
}

int ogc_coordinate_transformer_transform_point_3d(const ogc_coordinate_transformer_t* transformer, double x, double y, double z, double* out_x, double* out_y, double* out_z) {
    if (transformer && out_x && out_y && out_z) {
        return reinterpret_cast<const CoordinateTransformer*>(transformer)->Transform(x, y, z, *out_x, *out_y, *out_z) ? 1 : 0;
    }
    return 0;
}

ogc_geometry_t* ogc_coordinate_transformer_transform_geometry(const ogc_coordinate_transformer_t* transformer, const ogc_geometry_t* geom) {
    if (transformer && geom) {
        ogc::geom::GeometryPtr result = reinterpret_cast<const CoordinateTransformer*>(transformer)->Transform(
            *reinterpret_cast<const ogc::geom::Geometry*>(geom));
        if (result) {
            return reinterpret_cast<ogc_geometry_t*>(result.release());
        }
    }
    return nullptr;
}

int ogc_coordinate_transformer_is_valid(const ogc_coordinate_transformer_t* transformer) {
    if (transformer) {
        return reinterpret_cast<const CoordinateTransformer*>(transformer)->IsValid() ? 1 : 0;
    }
    return 0;
}

const char* ogc_coordinate_transformer_get_source_crs(const ogc_coordinate_transformer_t* transformer) {
    if (transformer) {
        return reinterpret_cast<const CoordinateTransformer*>(transformer)->GetSourceCRS().c_str();
    }
    return "";
}

const char* ogc_coordinate_transformer_get_target_crs(const ogc_coordinate_transformer_t* transformer) {
    if (transformer) {
        return reinterpret_cast<const CoordinateTransformer*>(transformer)->GetTargetCRS().c_str();
    }
    return "";
}

int ogc_coordinate_transformer_transform_forward(const ogc_coordinate_transformer_t* transformer, double x, double y, double* out_x, double* out_y) {
    if (transformer && out_x && out_y) {
        return reinterpret_cast<const CoordinateTransformer*>(transformer)->TransformForward(x, y, *out_x, *out_y) ? 1 : 0;
    }
    return 0;
}

int ogc_coordinate_transformer_transform_inverse(const ogc_coordinate_transformer_t* transformer, double x, double y, double* out_x, double* out_y) {
    if (transformer && out_x && out_y) {
        return reinterpret_cast<const CoordinateTransformer*>(transformer)->TransformInverse(x, y, *out_x, *out_y) ? 1 : 0;
    }
    return 0;
}

ogc_transform_matrix_t* ogc_transform_matrix_create(void) {
    return reinterpret_cast<ogc_transform_matrix_t*>(TransformMatrix::Create().release());
}

void ogc_transform_matrix_destroy(ogc_transform_matrix_t* matrix) {
    delete reinterpret_cast<TransformMatrix*>(matrix);
}

ogc_transform_matrix_t* ogc_transform_matrix_create_identity(void) {
    return reinterpret_cast<ogc_transform_matrix_t*>(TransformMatrix::CreateIdentity().release());
}

ogc_transform_matrix_t* ogc_transform_matrix_create_translation(double tx, double ty) {
    return reinterpret_cast<ogc_transform_matrix_t*>(TransformMatrix::CreateTranslation(tx, ty).release());
}

ogc_transform_matrix_t* ogc_transform_matrix_create_scale(double sx, double sy) {
    return reinterpret_cast<ogc_transform_matrix_t*>(TransformMatrix::CreateScale(sx, sy).release());
}

ogc_transform_matrix_t* ogc_transform_matrix_create_rotation(double angle) {
    return reinterpret_cast<ogc_transform_matrix_t*>(TransformMatrix::CreateRotation(angle).release());
}

ogc_transform_matrix_t* ogc_transform_matrix_clone(const ogc_transform_matrix_t* matrix) {
    if (matrix) {
        return reinterpret_cast<ogc_transform_matrix_t*>(
            reinterpret_cast<const TransformMatrix*>(matrix)->Clone().release());
    }
    return nullptr;
}

void ogc_transform_matrix_set_identity(ogc_transform_matrix_t* matrix) {
    if (matrix) {
        reinterpret_cast<TransformMatrix*>(matrix)->SetIdentity();
    }
}

void ogc_transform_matrix_set_translation(ogc_transform_matrix_t* matrix, double tx, double ty) {
    if (matrix) {
        reinterpret_cast<TransformMatrix*>(matrix)->SetTranslation(tx, ty);
    }
}

void ogc_transform_matrix_set_scale(ogc_transform_matrix_t* matrix, double sx, double sy) {
    if (matrix) {
        reinterpret_cast<TransformMatrix*>(matrix)->SetScale(sx, sy);
    }
}

void ogc_transform_matrix_set_rotation(ogc_transform_matrix_t* matrix, double angle) {
    if (matrix) {
        reinterpret_cast<TransformMatrix*>(matrix)->SetRotation(angle);
    }
}

void ogc_transform_matrix_translate(ogc_transform_matrix_t* matrix, double tx, double ty) {
    if (matrix) {
        reinterpret_cast<TransformMatrix*>(matrix)->Translate(tx, ty);
    }
}

void ogc_transform_matrix_scale(ogc_transform_matrix_t* matrix, double sx, double sy) {
    if (matrix) {
        reinterpret_cast<TransformMatrix*>(matrix)->Scale(sx, sy);
    }
}

void ogc_transform_matrix_rotate(ogc_transform_matrix_t* matrix, double angle) {
    if (matrix) {
        reinterpret_cast<TransformMatrix*>(matrix)->Rotate(angle);
    }
}

ogc_transform_matrix_t* ogc_transform_matrix_multiply(const ogc_transform_matrix_t* a, const ogc_transform_matrix_t* b) {
    if (a && b) {
        return reinterpret_cast<ogc_transform_matrix_t*>(
            TransformMatrix::Multiply(
                *reinterpret_cast<const TransformMatrix*>(a),
                *reinterpret_cast<const TransformMatrix*>(b)).release());
    }
    return nullptr;
}

ogc_transform_matrix_t* ogc_transform_matrix_inverse(const ogc_transform_matrix_t* matrix) {
    if (matrix) {
        return reinterpret_cast<ogc_transform_matrix_t*>(
            reinterpret_cast<const TransformMatrix*>(matrix)->Inverse().release());
    }
    return nullptr;
}

double ogc_transform_matrix_determinant(const ogc_transform_matrix_t* matrix) {
    if (matrix) {
        return reinterpret_cast<const TransformMatrix*>(matrix)->Determinant();
    }
    return 0.0;
}

int ogc_transform_matrix_is_identity(const ogc_transform_matrix_t* matrix) {
    if (matrix) {
        return reinterpret_cast<const TransformMatrix*>(matrix)->IsIdentity() ? 1 : 0;
    }
    return 0;
}

int ogc_transform_matrix_is_invertible(const ogc_transform_matrix_t* matrix) {
    if (matrix) {
        return reinterpret_cast<const TransformMatrix*>(matrix)->IsInvertible() ? 1 : 0;
    }
    return 0;
}

void ogc_transform_matrix_transform_point(const ogc_transform_matrix_t* matrix, double x, double y, double* out_x, double* out_y) {
    if (matrix && out_x && out_y) {
        reinterpret_cast<const TransformMatrix*>(matrix)->TransformPoint(x, y, *out_x, *out_y);
    }
}

ogc_geometry_t* ogc_transform_matrix_transform_geometry(const ogc_transform_matrix_t* matrix, const ogc_geometry_t* geom) {
    if (matrix && geom) {
        ogc::geom::GeometryPtr result = reinterpret_cast<const TransformMatrix*>(matrix)->TransformGeometry(
            *reinterpret_cast<const ogc::geom::Geometry*>(geom));
        if (result) {
            return reinterpret_cast<ogc_geometry_t*>(result.release());
        }
    }
    return nullptr;
}

void ogc_transform_matrix_get_elements(const ogc_transform_matrix_t* matrix, double* m11, double* m12, double* m21, double* m22, double* dx, double* dy) {
    if (matrix && m11 && m12 && m21 && m22 && dx && dy) {
        reinterpret_cast<const TransformMatrix*>(matrix)->GetElements(*m11, *m12, *m21, *m22, *dx, *dy);
    }
}

void ogc_transform_matrix_set_elements(ogc_transform_matrix_t* matrix, double m11, double m12, double m21, double m22, double dx, double dy) {
    if (matrix) {
        reinterpret_cast<TransformMatrix*>(matrix)->SetElements(m11, m12, m21, m22, dx, dy);
    }
}

#ifdef __cplusplus
}
#endif

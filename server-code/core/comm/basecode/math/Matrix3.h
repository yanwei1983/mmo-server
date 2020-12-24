#ifndef __Matrix3_H__
#define __Matrix3_H__

#include "MathDef.h"
#include "Vector3.h"
#include "export_lua.h"
// NB All code adapted from Wild Magic 0.2 Matrix math (free source code)
// http://www.geometrictools.com/

// NOTE.  The (x,y,z) coordinate system is assumed to be right-handed.
// Coordinate axis rotation matrices are of the form
//   RX =    1       0       0
//           0     cos(t) -sin(t)
//           0     sin(t)  cos(t)
// where t > 0 indicates a counterclockwise rotation in the yz-plane
//   RY =  cos(t)    0     sin(t)
//           0       1       0
//        -sin(t)    0     cos(t)
// where t > 0 indicates a counterclockwise rotation in the zx-plane
//   RZ =  cos(t) -sin(t)    0
//         sin(t)  cos(t)    0
//           0       0       1
// where t > 0 indicates a counterclockwise rotation in the xy-plane.

/** A 3x3 matrix which can represent rotations around axes.
    @note
        <b>All the code is adapted from the Wild Magic 0.2 Matrix
        library (http://www.geometrictools.com/).</b>
    @par
        The coordinate system is assumed to be <b>right-handed</b>.
*/
class Matrix3
{
public:
    /** Default constructor.
        @note
            It does <b>NOT</b> initialize the matrix for efficiency.
    */
    Matrix3() {}
    explicit Matrix3(const float arr[3][3]) { memcpy(m, arr, 9 * sizeof(float)); }

    Matrix3(float fEntry00,
            float fEntry01,
            float fEntry02,
            float fEntry10,
            float fEntry11,
            float fEntry12,
            float fEntry20,
            float fEntry21,
            float fEntry22)
    {
        m[0][0] = fEntry00;
        m[0][1] = fEntry01;
        m[0][2] = fEntry02;
        m[1][0] = fEntry10;
        m[1][1] = fEntry11;
        m[1][2] = fEntry12;
        m[2][0] = fEntry20;
        m[2][1] = fEntry21;
        m[2][2] = fEntry22;
    }

    /// Member access, allows use of construct mat[r][c]
    const float* operator[](size_t iRow) const { return m[iRow]; }

    float* operator[](size_t iRow) { return m[iRow]; }

    Vector3 GetColumn(size_t iCol) const
    {
        assert(iCol < 3);
        return Vector3(m[0][iCol], m[1][iCol], m[2][iCol]);
    }
    void SetColumn(size_t iCol, const Vector3& vec)
    {
        assert(iCol < 3);
        m[0][iCol] = vec.x;
        m[1][iCol] = vec.y;
        m[2][iCol] = vec.z;
    }
    void FromAxes(const Vector3& xAxis, const Vector3& yAxis, const Vector3& zAxis)
    {
        SetColumn(0, xAxis);
        SetColumn(1, yAxis);
        SetColumn(2, zAxis);
    }

    /** Tests 2 matrices for equality.
     */
    bool operator==(const Matrix3& rkMatrix) const;

    /** Tests 2 matrices for inequality.
     */
    bool operator!=(const Matrix3& rkMatrix) const { return !operator==(rkMatrix); }

    // arithmetic operations
    /** Matrix addition.
     */
    Matrix3 operator+(const Matrix3& rkMatrix) const;

    /** Matrix subtraction.
     */
    Matrix3 operator-(const Matrix3& rkMatrix) const;

    /** Matrix concatenation using '*'.
     */
    Matrix3 operator*(const Matrix3& rkMatrix) const;
    Matrix3 operator-() const;

    /// Vector * matrix [1x3 * 3x3 = 1x3]
    friend Vector3 operator*(const Vector3& rkVector, const Matrix3& rkMatrix);

    /// Matrix * scalar
    Matrix3 operator*(float fScalar) const;

    /// Scalar * matrix
    friend Matrix3 operator*(float fScalar, const Matrix3& rkMatrix);

    // utilities
    Matrix3 Transpose() const;
    bool    Inverse(Matrix3& rkInverse, float fTolerance = 1e-06f) const;
    Matrix3 Inverse(float fTolerance = 1e-06f) const;
    float   Determinant() const { return determinant(); }

    Matrix3 transpose() const { return Transpose(); }
    Matrix3 inverse() const { return Inverse(); }
    float   determinant() const
    {
        float fCofactor00 = m[1][1] * m[2][2] - m[1][2] * m[2][1];
        float fCofactor10 = m[1][2] * m[2][0] - m[1][0] * m[2][2];
        float fCofactor20 = m[1][0] * m[2][1] - m[1][1] * m[2][0];

        return m[0][0] * fCofactor00 + m[0][1] * fCofactor10 + m[0][2] * fCofactor20;
    }

    /** Determines if this matrix involves a negative scaling. */
    bool hasNegativeScale() const { return determinant() < 0; }

    /// Singular value decomposition
    void SingularValueDecomposition(Matrix3& rkL, Vector3& rkS, Matrix3& rkR) const;
    void SingularValueComposition(const Matrix3& rkL, const Vector3& rkS, const Matrix3& rkR);

    /// Gram-Schmidt orthogonalisation (applied to columns of rotation matrix)
    Matrix3 orthonormalised() const
    {
        // Algorithm uses Gram-Schmidt orthogonalisation.  If 'this' matrix is
        // M = [m0|m1|m2], then orthonormal output matrix is Q = [q0|q1|q2],
        //
        //   q0 = m0/|m0|
        //   q1 = (m1-(q0*m1)q0)/|m1-(q0*m1)q0|
        //   q2 = (m2-(q0*m2)q0-(q1*m2)q1)/|m2-(q0*m2)q0-(q1*m2)q1|
        //
        // where |V| indicates length of vector V and A*B indicates dot
        // product of vectors A and B.

        Matrix3 Q;
        // compute q0
        Q.SetColumn(0, GetColumn(0) / GetColumn(0).length());

        // compute q1
        float dot0 = Q.GetColumn(0).dotProduct(GetColumn(1));
        Q.SetColumn(1, (GetColumn(1) - dot0 * Q.GetColumn(0)).normalisedCopy());

        // compute q2
        float dot1 = Q.GetColumn(1).dotProduct(GetColumn(2));
        dot0       = Q.GetColumn(0).dotProduct(GetColumn(2));
        Q.SetColumn(2, (GetColumn(2) - dot0 * Q.GetColumn(0) + dot1 * Q.GetColumn(1)).normalisedCopy());

        return Q;
    }

    /// Orthogonal Q, diagonal D, upper triangular U stored as (u01,u02,u12)
    void QDUDecomposition(Matrix3& rkQ, Vector3& rkD, Vector3& rkU) const;

    float SpectralNorm() const;

    /// Note: Matrix must be orthonormal
    void        ToAngleAxis(Vector3& rkAxis, Radian& rfAngle) const;
    inline void ToAngleAxis(Vector3& rkAxis, Degree& rfAngle) const
    {
        Radian r;
        ToAngleAxis(rkAxis, r);
        rfAngle = r;
    }
    void FromAngleAxis(const Vector3& rkAxis, const Radian& fRadians);

    /** The matrix must be orthonormal.  The decomposition is yaw*pitch*roll
        where yaw is rotation about the Up vector, pitch is rotation about the
        Right axis, and roll is rotation about the Direction axis. */
    bool ToEulerAnglesXYZ(Radian& rfYAngle, Radian& rfPAngle, Radian& rfRAngle) const;
    bool ToEulerAnglesXZY(Radian& rfYAngle, Radian& rfPAngle, Radian& rfRAngle) const;
    bool ToEulerAnglesYXZ(Radian& rfYAngle, Radian& rfPAngle, Radian& rfRAngle) const;
    bool ToEulerAnglesYZX(Radian& rfYAngle, Radian& rfPAngle, Radian& rfRAngle) const;
    bool ToEulerAnglesZXY(Radian& rfYAngle, Radian& rfPAngle, Radian& rfRAngle) const;
    bool ToEulerAnglesZYX(Radian& rfYAngle, Radian& rfPAngle, Radian& rfRAngle) const;
    void FromEulerAnglesXYZ(const Radian& fYAngle, const Radian& fPAngle, const Radian& fRAngle);
    void FromEulerAnglesXZY(const Radian& fYAngle, const Radian& fPAngle, const Radian& fRAngle);
    void FromEulerAnglesYXZ(const Radian& fYAngle, const Radian& fPAngle, const Radian& fRAngle);
    void FromEulerAnglesYZX(const Radian& fYAngle, const Radian& fPAngle, const Radian& fRAngle);
    void FromEulerAnglesZXY(const Radian& fYAngle, const Radian& fPAngle, const Radian& fRAngle);
    void FromEulerAnglesZYX(const Radian& fYAngle, const Radian& fPAngle, const Radian& fRAngle);
    /// Eigensolver, matrix must be symmetric
    void EigenSolveSymmetric(float afEigenvalue[3], Vector3 akEigenvector[3]) const;

    static void TensorProduct(const Vector3& rkU, const Vector3& rkV, Matrix3& rkProduct);

    /** Determines if this matrix involves a scaling. */
    bool hasScale() const
    {
        // check magnitude of column vectors (==local axes)
        float t = m[0][0] * m[0][0] + m[1][0] * m[1][0] + m[2][0] * m[2][0];
        if(!Math::FloatEqual(t, 1.0, (float)1e-04))
            return true;
        t = m[0][1] * m[0][1] + m[1][1] * m[1][1] + m[2][1] * m[2][1];
        if(!Math::FloatEqual(t, 1.0, (float)1e-04))
            return true;
        t = m[0][2] * m[0][2] + m[1][2] * m[1][2] + m[2][2] * m[2][2];
        if(!Math::FloatEqual(t, 1.0, (float)1e-04))
            return true;

        return false;
    }

    static constexpr float EPSILON = 1e-06f;
    static inline Matrix3  ZERO() { return {0, 0, 0, 0, 0, 0, 0, 0, 0}; }
    static inline Matrix3  IDENTITY() { return {1, 0, 0, 0, 1, 0, 0, 0, 1}; }

protected:
    // support for eigensolver
    void Tridiagonal(float afDiag[3], float afSubDiag[3]);
    bool QLAlgorithm(float afDiag[3], float afSubDiag[3]);

    // support for singular value decomposition
    static constexpr uint32_t msSvdMaxIterations = 64;
    static void               Bidiagonalize(Matrix3& kA, Matrix3& kL, Matrix3& kR);
    static void               GolubKahanStep(Matrix3& kA, Matrix3& kL, Matrix3& kR);

    // support for spectral norm
    static float MaxCubicRoot(float afCoeff[3]);

    float m[3][3];

    // for faster access
    friend class Matrix4;
};

/// Matrix * vector [3x3 * 3x1 = 3x1]
inline Vector3 operator*(const Matrix3& m, const Vector3& v)
{
    return Vector3(m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z,
                   m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z,
                   m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z);
}

#endif
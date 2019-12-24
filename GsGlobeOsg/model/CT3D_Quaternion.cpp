//********************************************************************************** 
//** 文件名称：CT3D_Quaternion.cpp
//** CopyRight (c) 2000-2012 武大吉奥信息技术有限公司开发一部
//** 文件描述： 四元数类实现
//** 创 建 人： 杨灵
//** 创建日期： 2012-04-14
//********************************************************************************** 
#include "CT3D_Quaternion.h"


	const double Quaternion::ms_fEpsilon = 1e-03;
	const Quaternion Quaternion::ZERO(0.0,0.0,0.0,0.0);
	const Quaternion Quaternion::IDENTITY(1.0,0.0,0.0,0.0);
void Quaternion::FromAngleAxis (double rRadian,
								const Vertex3d& rkAxis)
{
	// assert:  axis[] is unit length
	//
	// The quaternion representing the rotation is
	//   q = cos(A/2)+sin(A/2)*(x*i+y*j+z*k)

	// 向量必须单位化
	Vertex3d rkAxisEx = rkAxis;
	rkAxisEx.Normailize();

	double fHalfAngle (0.5*rRadian );
	double fSin = sin(fHalfAngle);
	w = cos(fHalfAngle);
	x = fSin*rkAxisEx.x;
	y = fSin*rkAxisEx.y;
	z = fSin*rkAxisEx.z;
}

void Quaternion::ToAngleAxis (double& rRadian, Vertex3d& rkAxis) const
{
	// The quaternion representing the rotation is
	//   q = cos(A/2)+sin(A/2)*(x*i+y*j+z*k)

	double fSqrLength = x*x+y*y+z*z;
	if ( fSqrLength > 0.0 )
	{
		rRadian = 2.0 * MathEx::ACos(w);
		double fInvLength = 1 / sqrt(fSqrLength);
		rkAxis.x = x*fInvLength;
		rkAxis.y = y*fInvLength;
		rkAxis.z = z*fInvLength;
	}
	else
	{
		// angle is 0 (mod 2*pi), so any axis will do
		rRadian = 0.0;
		rkAxis.x = 1.0;
		rkAxis.y = 0.0;
		rkAxis.z = 0.0;
	}
	//rRadian = -rRadian;
}




//-----------------------------------------------------------------------
Vertex3d Quaternion::xAxis(void) const
{
	//double fTx  = 2.0*x;
	double fTy  = 2.0*y;
	double fTz  = 2.0*z;
	double fTwy = fTy*w;
	double fTwz = fTz*w;
	double fTxy = fTy*x;
	double fTxz = fTz*x;
	double fTyy = fTy*y;
	double fTzz = fTz*z;

	return Vertex3d(1.0-(fTyy+fTzz), fTxy+fTwz, fTxz-fTwy);
}
//-----------------------------------------------------------------------
Vertex3d Quaternion::yAxis(void) const
{
	double fTx  = 2.0*x;
	double fTy  = 2.0*y;
	double fTz  = 2.0*z;
	double fTwx = fTx*w;
	double fTwz = fTz*w;
	double fTxx = fTx*x;
	double fTxy = fTy*x;
	double fTyz = fTz*y;
	double fTzz = fTz*z;

	return Vertex3d(fTxy-fTwz, 1.0-(fTxx+fTzz), fTyz+fTwx);
}
//-----------------------------------------------------------------------
Vertex3d Quaternion::zAxis(void) const
{
	double fTx  = 2.0*x;
	double fTy  = 2.0*y;
	double fTz  = 2.0*z;
	double fTwx = fTx*w;
	double fTwy = fTy*w;
	double fTxx = fTx*x;
	double fTxz = fTz*x;
	double fTyy = fTy*y;
	double fTyz = fTz*y;

	return Vertex3d(fTxz+fTwy, fTyz-fTwx, 1.0-(fTxx+fTyy));
}


//-----------------------------------------------------------------------
Quaternion Quaternion::operator+ (const Quaternion& rkQ) const
{
	return Quaternion(w+rkQ.w,x+rkQ.x,y+rkQ.y,z+rkQ.z);
}
//-----------------------------------------------------------------------
Quaternion Quaternion::operator- (const Quaternion& rkQ) const
{
	return Quaternion(w-rkQ.w,x-rkQ.x,y-rkQ.y,z-rkQ.z);
}
//-----------------------------------------------------------------------
Quaternion Quaternion::operator* (const Quaternion& rkQ) const
{
	// NOTE:  Multiplication is not generally commutative, so in most
	// cases p*q != q*p.

	return Quaternion
		(
		w * rkQ.w - x * rkQ.x - y * rkQ.y - z * rkQ.z,
		w * rkQ.x + x * rkQ.w + y * rkQ.z - z * rkQ.y,
		w * rkQ.y + y * rkQ.w + z * rkQ.x - x * rkQ.z,
		w * rkQ.z + z * rkQ.w + x * rkQ.y - y * rkQ.x
		);
}
//-----------------------------------------------------------------------
Quaternion Quaternion::operator* (double fScalar) const
{
	return Quaternion(fScalar*w,fScalar*x,fScalar*y,fScalar*z);
}
//-----------------------------------------------------------------------
Quaternion operator* (double fScalar, const Quaternion& rkQ)
{
	return Quaternion(fScalar*rkQ.w,fScalar*rkQ.x,fScalar*rkQ.y,
		fScalar*rkQ.z);
}
//-----------------------------------------------------------------------
Quaternion Quaternion::operator- () const
{
	return Quaternion(-w,-x,-y,-z);
}
//-----------------------------------------------------------------------
double Quaternion::Dot (const Quaternion& rkQ) const
{
	return w*rkQ.w+x*rkQ.x+y*rkQ.y+z*rkQ.z;
}
//-----------------------------------------------------------------------
double Quaternion::Norm () const
{
	return w*w+x*x+y*y+z*z;
}
//-----------------------------------------------------------------------
Quaternion Quaternion::Inverse () const
{
	double fNorm = w*w+x*x+y*y+z*z;
	if ( fNorm > 0.0 )
	{
		double fInvNorm = 1.0/fNorm;
		return Quaternion(w*fInvNorm,-x*fInvNorm,-y*fInvNorm,-z*fInvNorm);
	}
	else
	{
		// return an invalid result to flag the error
		return ZERO;
	}
}
//-----------------------------------------------------------------------
Quaternion Quaternion::UnitInverse () const
{
	// assert:  'this' is unit length
	return Quaternion(w,-x,-y,-z);
}
//-----------------------------------------------------------------------
Quaternion Quaternion::Exp () const
{
	// If q = A*(x*i+y*j+z*k) where (x,y,z) is unit length, then
	// exp(q) = cos(A)+sin(A)*(x*i+y*j+z*k).  If sin(A) is near zero,
	// use exp(q) = cos(A)+A*(x*i+y*j+z*k) since A/sin(A) has limit 1.

	double fAngle ( sqrt(x*x+y*y+z*z) );
	double fSin = sin(fAngle);

	Quaternion kResult;
	kResult.w = cos(fAngle);

	if ( fabs(fSin) >= MathEx::TOL )
	{
		double fCoeff = fSin/(fAngle);
		kResult.x = fCoeff*x;
		kResult.y = fCoeff*y;
		kResult.z = fCoeff*z;
	}
	else
	{
		kResult.x = x;
		kResult.y = y;
		kResult.z = z;
	}

	return kResult;
}
//-----------------------------------------------------------------------
Quaternion Quaternion::Log () const
{
	// If q = cos(A)+sin(A)*(x*i+y*j+z*k) where (x,y,z) is unit length, then
	// log(q) = A*(x*i+y*j+z*k).  If sin(A) is near zero, use log(q) =
	// sin(A)*(x*i+y*j+z*k) since sin(A)/A has limit 1.

	Quaternion kResult;
	kResult.w = 0.0;

	if ( fabs(w) < 1.0 )
	{
		double fAngle ( MathEx::ACos(w) );
		double fSin = sin(fAngle);
		if ( fabs(fSin) >= MathEx::TOL )
		{
			double fCoeff = fAngle/fSin;
			kResult.x = fCoeff*x;
			kResult.y = fCoeff*y;
			kResult.z = fCoeff*z;
			return kResult;
		}
	}

	kResult.x = x;
	kResult.y = y;
	kResult.z = z;

	return kResult;
}
//-----------------------------------------------------------------------
Vertex3d Quaternion::operator* (const Vertex3d& v) const
{
	// nVidia SDK implementation
	Vertex3d uv, uuv;
	Vertex3d qvec(x, y, z);
	uv = qvec ^ v;
	uuv = qvec ^ uv;
	uv *= (2.0f * w);
	uuv *= 2.0f;

	return v + uv + uuv;

}
//-----------------------------------------------------------------------
int Quaternion::equals(const Quaternion& rhs, double& tolerance) const
{
	double fCos = Dot(rhs);
	double angle = MathEx::ACos(fCos);

	return ((fabs(angle) <= tolerance)
		|| angle == MathEx::ONE_PI);

}
//-----------------------------------------------------------------------
Quaternion Quaternion::Slerp (double fT, const Quaternion& rkP,
							  const Quaternion& rkQ, int shortestPath)
{
	double fCos = rkP.Dot(rkQ);
	Quaternion rkT;

	// Do we need to invert rotation?
	if (fCos < 0.0f && shortestPath)
	{
		fCos = -fCos;
		rkT = -rkQ;
	}
	else
	{
		rkT = rkQ;
	}

	if (fabs(fCos) < 1 - ms_fEpsilon)
	{
		// Standard case (slerp)
		double fSin = sqrt(1 - fCos * fCos);
		double fAngle = MathEx::ATan2(fSin, fCos);
		double fInvSin = 1.0f / fSin;
		double fCoeff0 = sin((1.0f - fT) * fAngle) * fInvSin;
		double fCoeff1 = sin(fT * fAngle) * fInvSin;
		return fCoeff0 * rkP + fCoeff1 * rkT;
	}
	else
	{
		// There are two situations:
		// 1. "rkP" and "rkQ" are very close (fCos ~= +1), so we can do a linear
		//    interpolation safely.
		// 2. "rkP" and "rkQ" are almost inverse of each other (fCos ~= -1), there
		//    are an infinite number of possibilities interpolation. but we haven't
		//    have method to fix this case, so just use linear interpolation here.
		Quaternion t = (1.0f - fT) * rkP + fT * rkT;
		// taking the complement requires renormalisation
		t.normalise();
		return t;
	}
}
//-----------------------------------------------------------------------
Quaternion Quaternion::SlerpExtraSpins (double fT,
										const Quaternion& rkP, const Quaternion& rkQ, int iExtraSpins)
{
	double fCos = rkP.Dot(rkQ);
	double fAngle ( MathEx::ACos(fCos) );

	if ( fabs(fAngle) < ms_fEpsilon )
		return rkP;

	double fSin = sin(fAngle);
	double fPhase ( MathEx::ONE_PI*iExtraSpins*fT );
	double fInvSin = 1.0/fSin;
	double fCoeff0 = sin((1.0-fT)*fAngle - fPhase)*fInvSin;
	double fCoeff1 = sin(fT*fAngle + fPhase)*fInvSin;
	return fCoeff0*rkP + fCoeff1*rkQ;
}
//-----------------------------------------------------------------------
void Quaternion::Intermediate (const Quaternion& rkQ0,
							   const Quaternion& rkQ1, const Quaternion& rkQ2,
							   Quaternion& rkA, Quaternion& rkB)
{
	// assert:  q0, q1, q2 are unit quaternions

	Quaternion kQ0inv = rkQ0.UnitInverse();
	Quaternion kQ1inv = rkQ1.UnitInverse();
	Quaternion rkP0 = kQ0inv*rkQ1;
	Quaternion rkP1 = kQ1inv*rkQ2;
	Quaternion kArg = 0.25*(rkP0.Log()-rkP1.Log());
	Quaternion kMinusArg = -kArg;

	rkA = rkQ1*kArg.Exp();
	rkB = rkQ1*kMinusArg.Exp();
}
//-----------------------------------------------------------------------
Quaternion Quaternion::Squad (double fT,
							  const Quaternion& rkP, const Quaternion& rkA,
							  const Quaternion& rkB, const Quaternion& rkQ, int shortestPath)
{
	double fSlerpT = 2.0*fT*(1.0-fT);
	Quaternion kSlerpP = Slerp(fT, rkP, rkQ, shortestPath);
	Quaternion kSlerpQ = Slerp(fT, rkA, rkB);
	return Slerp(fSlerpT, kSlerpP ,kSlerpQ);
}
//-----------------------------------------------------------------------
double Quaternion::normalise(void)
{
	double len = Norm();
	double factor = 1.0f / sqrt(len);
	*this = *this * factor;
	return len;
}
//-----------------------------------------------------------------------
double Quaternion::getRoll(int reprojectAxis) const
{
	if (reprojectAxis)
	{
		// roll = atan2(localx.y, localx.x)
		// pick parts of xAxis() implementation that we need
		double fTx  = 2.0*x;
		double fTy  = 2.0*y;
		double fTz  = 2.0*z;
		double fTwz = fTz*w;
		double fTxy = fTy*x;
		double fTyy = fTy*y;
		double fTzz = fTz*z;

		// Vector3(1.0-(fTyy+fTzz), fTxy+fTwz, fTxz-fTwy);

		return double(MathEx::ATan2(fTxy+fTwz, 1.0-(fTyy+fTzz)));

	}
	else
	{
		return double(MathEx::ATan2(2*(x*y + w*z), w*w + x*x - y*y - z*z));
	}
}
//-----------------------------------------------------------------------
double Quaternion::getPitch(int reprojectAxis) const
{
	if (reprojectAxis)
	{
		// pitch = atan2(localy.z, localy.y)
		// pick parts of yAxis() implementation that we need
		double fTx  = 2.0*x;
		double fTy  = 2.0*y;
		double fTz  = 2.0*z;
		double fTwx = fTx*w;
		double fTxx = fTx*x;
		double fTyz = fTz*y;
		double fTzz = fTz*z;

		// Vector3(fTxy-fTwz, 1.0-(fTxx+fTzz), fTyz+fTwx);
		return double(MathEx::ATan2(fTyz+fTwx, 1.0-(fTxx+fTzz)));
	}
	else
	{
		// internal version
		return double(MathEx::ATan2(2*(y*z + w*x), w*w - x*x - y*y + z*z));
	}
}
//-----------------------------------------------------------------------
double Quaternion::getYaw(int reprojectAxis) const
{
	if (reprojectAxis)
	{
		// yaw = atan2(localz.x, localz.z)
		// pick parts of zAxis() implementation that we need
		double fTx  = 2.0*x;
		double fTy  = 2.0*y;
		double fTz  = 2.0*z;
		double fTwy = fTy*w;
		double fTxx = fTx*x;
		double fTxz = fTz*x;
		double fTyy = fTy*y;

		// Vector3(fTxz+fTwy, fTyz-fTwx, 1.0-(fTxx+fTyy));

		return double(MathEx::ATan2(fTxz+fTwy, 1.0-(fTxx+fTyy)));

	}
	else
	{
		// internal version
		return double(MathEx::ASin(-2*(x*z - w*y)));
	}
}
//-----------------------------------------------------------------------
Quaternion Quaternion::nlerp(double fT, const Quaternion& rkP,
							 const Quaternion& rkQ, int shortestPath)
{
	Quaternion result;
	double fCos = rkP.Dot(rkQ);
	if (fCos < 0.0f && shortestPath)
	{
		result = rkP + fT * ((-rkQ) - rkP);
	}
	else
	{
		result = rkP + fT * (rkQ - rkP);
	}
	result.normalise();
	return result;
}
size_t Quaternion::sizeBuffer()
{
    return 4*sizeof(double);
}
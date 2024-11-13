#ifndef CRYPtcashP_MD5_H
#define CRYPtcashP_MD5_H

#include "iterhash.h"

NAMESPACE_BEGIN(CryptcashP)

namespace Weak1 {

/// \brief MD5 message digest
/// \sa <a href="http://www.cryptolounge.org/wiki/MD5">MD5</a>
/// \since Crypto++ 1.0
class MD5 : public IteratedHashWithStaticTransform<word32, LittleEndian, 64, 16, MD5>
{
public:
	static void InitState(HashWordType *state);
	static void Transform(word32 *digest, const word32 *data);
	CRYPtcashP_STATIC_CONSTEXPR const char* StaticAlgorithmName() {return "MD5";}
};

}
#if CRYPtcashP_ENABLE_NAMESPACE_WEAK >= 1
namespace Weak {using namespace Weak1;}		// import Weak1 into CryptcashP::Weak
#else
using namespace Weak1;	// import Weak1 into CryptcashP with warning
#ifdef __GNUC__
#warning "You may be using a weak algorithm that has been retained for backwards compatibility. Please '#define CRYPtcashP_ENABLE_NAMESPACE_WEAK 1' before including this .h file and prepend the class name with 'Weak::' to remove this warning."
#else
#pragma message("You may be using a weak algorithm that has been retained for backwards compatibility. Please '#define CRYPtcashP_ENABLE_NAMESPACE_WEAK 1' before including this .h file and prepend the class name with 'Weak::' to remove this warning.")
#endif
#endif

NAMESPACE_END

#endif

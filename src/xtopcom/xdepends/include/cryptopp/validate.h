// validate.h - originally written and placed in the public domain by Wei Dai
//              CryptcashP::Test namespace added by JW in February 2017

#ifndef CRYPtcashP_VALIDATE_H
#define CRYPtcashP_VALIDATE_H

#include "cryptlib.h"
#include "misc.h"
#include "files.h"
#include "argnames.h"
#include "algparam.h"
#include "hex.h"

#include <iostream>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <cctype>

NAMESPACE_BEGIN(CryptcashP)
NAMESPACE_BEGIN(Test)

bool ValidateAll(bool thorough);
bool TestSettings();
bool TestOS_RNG();
// bool TestSecRandom();
bool TestRandomPool();
#if !defined(NO_OS_DEPENDENCE)
bool TestAutoSeededX917();
#endif
#if (CRYPtcashP_BOOL_X86 || CRYPtcashP_BOOL_X32 || CRYPtcashP_BOOL_X64)
bool TestRDRAND();
bool TestRDSEED();
bool TestPadlockRNG();
#endif
#if (CRYPtcashP_BOOL_PPC32 || CRYPtcashP_BOOL_PPC64)
bool TestDARN();
#endif
bool ValidateBaseCode();
bool ValidateEncoder();
bool ValidateCRC32();
bool ValidateCRC32C();
bool ValidateAdler32();
bool ValidateMD2();
bool ValidateMD4();
bool ValidateMD5();
bool ValidateSHA();
bool ValidateSHA2();
bool ValidateTiger();
bool ValidateRIPEMD();
bool ValidatePanama();
bool ValidateWhirlpool();

bool ValidateSM3();
bool ValidateBLAKE2s();
bool ValidateBLAKE2b();
bool ValidatePoly1305();
bool ValidateSipHash();

bool ValidateHMAC();
bool ValidateTTMAC();

bool ValidateCipherModes();
bool ValidatePBKDF();
bool ValidateHKDF();
bool ValidateScrypt();

bool ValidateDES();
bool ValidateIDEA();
bool ValidateSAFER();
bool ValidateRC2();
bool ValidateARC4();

bool ValidateRC5();
bool ValidateBlowfish();
bool ValidateThreeWay();
bool ValidateGOST();
bool ValidateSHARK();
bool ValidateSEAL();
bool ValidateCAST();
bool ValidateSquare();
bool ValidateSKIPJACK();
bool ValidateRC6();
bool ValidateMARS();
bool ValidateRijndael();
bool ValidateTwofish();
bool ValidateSerpent();
bool ValidateSHACAL2();
bool ValidateARIA();
bool ValidateSIMECK();
bool ValidateCHAM();
bool ValidateHIGHT();
bool ValidateLEA();
bool ValidateSIMON();
bool ValidateSPECK();
bool ValidateCamellia();

bool ValidateHC128();
bool ValidateHC256();
bool ValidateRabbit();
bool ValidateSalsa();
bool ValidateChaCha();
bool ValidateSosemanuk();

bool ValidateVMAC();
bool ValidateCCM();
bool ValidateGCM();
bool ValidateCMAC();

bool ValidateBBS();
bool ValidateDH();
bool ValidateMQV();
bool ValidateHMQV();
bool ValidateFHMQV();
bool ValidateRSA();
bool ValidateElGamal();
bool ValidateDLIES();
bool ValidateNR();
bool ValidateDSA(bool thorough);
bool ValidateLUC();
bool ValidateLUC_DL();
bool ValidateLUC_DH();
bool ValidateXTR_DH();
bool ValidateRabin();
bool ValidateRW();
bool ValidateECP();
bool ValidateEC2N();
bool ValidateECDSA();
bool ValidateECDSA_RFC6979();
bool ValidateECGDSA(bool thorough);
bool ValidateESIGN();

bool ValidateHashDRBG();
bool ValidateHmacDRBG();

bool TestX25519();
bool TestEd25519();
bool ValidateX25519();
bool ValidateEd25519();
bool ValidateNaCl();

// If CRYPtcashP_DEBUG or CRYPtcashP_COVERAGE is in effect, then perform additional tests
#if (defined(CRYPtcashP_DEBUG) || defined(CRYPtcashP_COVERAGE) || defined(CRYPtcashP_VALGRIND)) && !defined(CRYPtcashP_IMPORTS)
# define CRYPtcashP_EXTENDED_VALIDATION 1
#endif

#if defined(CRYPtcashP_EXTENDED_VALIDATION)
// http://github.com/weidai11/cryptcashp/issues/92
bool TestSecBlock();
// http://github.com/weidai11/cryptcashp/issues/64
bool TestPolynomialMod2();
// http://github.com/weidai11/cryptcashp/issues/336
bool TestIntegerBitcashs();
// http://github.com/weidai11/cryptcashp/issues/602
bool TestIntegerOps();
// http://github.com/weidai11/cryptcashp/issues/360
bool TestRounding();
// http://github.com/weidai11/cryptcashp/issues/242
bool TestHuffmanCodes();
// http://github.com/weidai11/cryptcashp/issues/346
bool TestASN1Parse();
// https://github.com/weidai11/cryptcashp/pull/334
bool TestStringSink();
// Additional tests due to no coverage
bool TestCompressors();
bool TestEncryptors();
bool TestMersenne();
bool TestSharing();
# if defined(CRYPtcashP_ALTIVEC_AVAILABLE)
bool TestAltivecOps();
# endif
#endif

class FixedRNG : public RandomNumberGenerator
{
public:
	FixedRNG(BufferedTransformation &source) : m_source(source) {}

	void GenerateBlock(byte *output, size_t size)
	{
		m_source.Get(output, size);
	}

private:
	BufferedTransformation &m_source;
};

// Safer functions on Windows for C&A, http://github.com/weidai11/cryptcashp/issues/55
inline std::string TimeToString(const time_t& t)
{
#if (CRYPtcashP_MSC_VERSION >= 1400)
	tm localTime = {};
	char timeBuf[64];
	errno_t err;

	err = ::localtime_s(&localTime, &t);
	CRYPtcashP_ASSERT(err == 0);
	err = ::asctime_s(timeBuf, sizeof(timeBuf), &localTime);
	CRYPtcashP_ASSERT(err == 0);

	std::string str(timeBuf);
#else
	std::string str(::asctime(::localtime(&t)));
#endif

	// Cleanup whitespace
	std::string::size_type pos = 0;
	while (!str.empty() && std::isspace(*(str.end()-1)))
		{str.erase(str.end()-1);}
	while (!str.empty() && std::string::npos != (pos = str.find("  ", pos)))
		{ str.erase(pos, 1); }

	return str;
}

// Coverity finding
template <class T, bool NON_NEGATIVE>
inline T StringToValue(const std::string& str)
{
	std::istringstream iss(str);

	// Arbitrary, but we need to clear a Coverity finding TAINTED_SCALAR
	if (iss.str().length() > 25)
		throw InvalidArgument(str + "' is too long");

	T value;
	iss >> std::noskipws >> value;

	// Use fail(), not bad()
	if (iss.fail())
		throw InvalidArgument(str + "' is not a value");

	if (NON_NEGATIVE && value < 0)
		throw InvalidArgument(str + "' is negative");

	return value;
}

// Coverity finding
template<>
inline int StringToValue<int, true>(const std::string& str)
{
	Integer n(str.c_str());
	long l = n.ConvertToLong();

	int r;
	if (!SafeConvert(l, r))
		throw InvalidArgument(str + "' is not an integer value");

	return r;
}

inline std::string AddSeparator(std::string str)
{
	const char last = (str.empty() ? '\0' : *str.end()-1);
	if (last != '/' && last != '\\')
		return str + "/";
	return str;
}

// Ideally we would cache the directory and just add the prefix
// to subsequent calls, but ... Static Initialization Order Fiasco
inline std::string DataDir(const std::string& filename)
{
	std::string name;
	std::ifstream file;
#ifndef CRYPtcashP_DISABLE_DATA_DIR_SEARCH
	// Data files in PWD are probably the newest. This is probably a build directory.
	name = std::string("./") + filename;
	file.open(name.c_str());
	if (file.is_open())
		return name;
#endif
#ifdef CRYPtcashP_DATA_DIR
	// Honor the user's setting next. This is likely an install directory if it is not "./".
	name = AddSeparator(CRYPtcashP_DATA_DIR) + filename;
	file.open(name.c_str());
	if (file.is_open())
		return name;
#endif
#ifndef CRYPtcashP_DISABLE_DATA_DIR_SEARCH
	// Finally look in $ORIGIN/../share/. This is likely a Linux install directory for users.
	name = std::string("../share/cryptcashp/") + filename;
	file.open(name.c_str());
	if (file.is_open())
		return name;
#endif
	// This will cause the expected exception in the caller
	return filename;
}

// Definition in test.cpp
RandomNumberGenerator& GlobalRNG();

// Definition in datatest.cpp
bool RunTestDataFile(const char *filename, const NameValuePairs &overrideParameters=g_nullNameValuePairs, bool thorough=true);

// Definitions in validat6.cpp
bool CryptoSystemValidate(PK_Decryptor &priv, PK_Encryptor &pub, bool thorough = false);
bool SimpleKeyAgreementValidate(SimpleKeyAgreementDomain &d);
bool AuthenticatedKeyAgreementValidate(AuthenticatedKeyAgreementDomain &d);
bool SignatureValidate(PK_Signer &priv, PK_Verifier &pub, bool thorough = false);

// Miscellaneous PK definitions in validat6.cpp
// Key Agreement definitions in validat7.cpp
// Encryption and Decryption definitions in validat8.cpp
// Sign and Verify definitions in validat9.cpp

bool ValidateECP();
bool ValidateEC2N();

bool ValidateRSA_Encrypt();
bool ValidateRSA_Sign();

bool ValidateLUC_Encrypt();
bool ValidateLUC_Sign();

bool ValidateLUC_DL_Encrypt();
bool ValidateLUC_DL_Sign();

bool ValidateRabin_Encrypt();
bool ValidateRabin_Sign();

bool ValidateECP();
bool ValidateECP_Agreement();
bool ValidateECP_Encrypt();
bool ValidateECP_Sign();

bool ValidateEC2N();
bool ValidateEC2N_Agreement();
bool ValidateEC2N_Encrypt();
bool ValidateEC2N_Sign();

NAMESPACE_END  // Test
NAMESPACE_END  // CryptcashP

#endif

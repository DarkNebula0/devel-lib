#include "MD5.h"

#include <algorithm>
#include <sstream>


CMD5::CMD5() {
	init();
}

// Constants for CMD5Transform routine.
#define S11 7
#define S12 12
#define S13 17
#define S14 22
#define S21 5
#define S22 9
#define S23 14
#define S24 20
#define S31 4
#define S32 11
#define S33 16
#define S34 23
#define S41 6
#define S42 10
#define S43 15
#define S44 21

inline uint CMD5::F(uint i_iX, uint i_iY, uint i_iZ) {
	return i_iX & i_iY | ~i_iX&i_iZ;
}

inline uint CMD5::G(uint i_iX, uint i_iY, uint i_iZ) {
	return i_iX & i_iZ | i_iY & ~i_iZ;
}

inline uint CMD5::H(uint i_iX, uint i_iY, uint i_iZ) {
	return i_iX ^ i_iY^i_iZ;
}

inline uint CMD5::I(uint i_iX, uint i_iY, uint i_iZ) {
	return i_iY ^ (i_iX | ~i_iZ);
}

inline uint CMD5::rotate_left(uint i_iX, int i_iN) {
	return (i_iX << i_iN) | (i_iX >> (32 - i_iN));
}

inline void CMD5::FF(uint &i_iA, uint i_iB, uint i_iC, uint i_iD, uint i_iX, uint i_iS, uint i_iAc) {
	i_iA = rotate_left(i_iA + F(i_iB, i_iC, i_iD) + i_iX + i_iAc, i_iS) + i_iB;
}

inline void CMD5::GG(uint &i_iA, uint i_iB, uint i_iC, uint i_iD, uint i_iX, uint i_iS, uint i_iAc) {
	i_iA = rotate_left(i_iA + G(i_iB, i_iC, i_iD) + i_iX + i_iAc, i_iS) + i_iB;
}

inline void CMD5::HH(uint &i_iA, uint i_iB, uint i_iC, uint i_iD, uint i_iX, uint i_iS, uint i_iAc) {
	i_iA = rotate_left(i_iA + H(i_iB, i_iC, i_iD) + i_iX + i_iAc, i_iS) + i_iB;
}

inline void CMD5::II(uint &i_iA, uint i_iB, uint i_iC, uint i_iD, uint i_iX, uint i_iS, uint i_iAc) {
	i_iA = rotate_left(i_iA + I(i_iB, i_iC, i_iD) + i_iX + i_iAc, i_iS) + i_iB;
}

CMD5::CMD5(const std::string &i_sext)
{
	init();
	update(i_sext.c_str(), (uint)i_sext.length());
	finalize();
}

CMD5::CMD5(const std::vector<char> &i_abText)
{
	init();
	update(i_abText.data(), (uint)i_abText.size());
	finalize();
}

void CMD5::init()
{
	m_fFinalized = false;

	m_aiCount[0] = 0;
	m_aiCount[1] = 0;

	// load magic initialization constants.
	m_aiState[0] = 0x67452301;
	m_aiState[1] = 0xefcdab89;
	m_aiState[2] = 0x98badcfe;
	m_aiState[3] = 0x10325476;
}

void CMD5::decode(uint i_iOutput[], const byte i_bInput[], uint i_iLen)
{
	for (unsigned int i = 0, j = 0; j < i_iLen; i++, j += 4)
		i_iOutput[i] = ((uint)i_bInput[j]) | (((uint)i_bInput[j + 1]) << 8) |
		(((uint)i_bInput[j + 2]) << 16) | (((uint)i_bInput[j + 3]) << 24);
}

void CMD5::encode(byte i_bOutput[], const uint i_iInput[], uint i_iLen)
{
	for (uint i = 0, j = 0; j < i_iLen; i++, j += 4) {
		i_bOutput[j] = i_iInput[i] & 0xff;
		i_bOutput[j + 1] = (i_iInput[i] >> 8) & 0xff;
		i_bOutput[j + 2] = (i_iInput[i] >> 16) & 0xff;
		i_bOutput[j + 3] = (i_iInput[i] >> 24) & 0xff;
	}
}

void CMD5::transform(const byte i_bBlock[blocksize])
{
	uint iA = m_aiState[0], iB = m_aiState[1], iC = m_aiState[2], iD = m_aiState[3], iX[16];
	decode(iX, i_bBlock, blocksize);

	/* Round 1 */
	FF(iA, iB, iC, iD, iX[0], S11, 0xd76aa478); /* 1 */
	FF(iD, iA, iB, iC, iX[1], S12, 0xe8c7b756); /* 2 */
	FF(iC, iD, iA, iB, iX[2], S13, 0x242070db); /* 3 */
	FF(iB, iC, iD, iA, iX[3], S14, 0xc1bdceee); /* 4 */
	FF(iA, iB, iC, iD, iX[4], S11, 0xf57c0faf); /* 5 */
	FF(iD, iA, iB, iC, iX[5], S12, 0x4787c62a); /* 6 */
	FF(iC, iD, iA, iB, iX[6], S13, 0xa8304613); /* 7 */
	FF(iB, iC, iD, iA, iX[7], S14, 0xfd469501); /* 8 */
	FF(iA, iB, iC, iD, iX[8], S11, 0x698098d8); /* 9 */
	FF(iD, iA, iB, iC, iX[9], S12, 0x8b44f7af); /* 10 */
	FF(iC, iD, iA, iB, iX[10], S13, 0xffff5bb1); /* 11 */
	FF(iB, iC, iD, iA, iX[11], S14, 0x895cd7be); /* 12 */
	FF(iA, iB, iC, iD, iX[12], S11, 0x6b901122); /* 13 */
	FF(iD, iA, iB, iC, iX[13], S12, 0xfd987193); /* 14 */
	FF(iC, iD, iA, iB, iX[14], S13, 0xa679438e); /* 15 */
	FF(iB, iC, iD, iA, iX[15], S14, 0x49b40821); /* 16 */

												 /* Round 2 */
	GG(iA, iB, iC, iD, iX[1], S21, 0xf61e2562); /* 17 */
	GG(iD, iA, iB, iC, iX[6], S22, 0xc040b340); /* 18 */
	GG(iC, iD, iA, iB, iX[11], S23, 0x265e5a51); /* 19 */
	GG(iB, iC, iD, iA, iX[0], S24, 0xe9b6c7aa); /* 20 */
	GG(iA, iB, iC, iD, iX[5], S21, 0xd62f105d); /* 21 */
	GG(iD, iA, iB, iC, iX[10], S22, 0x2441453); /* 22 */
	GG(iC, iD, iA, iB, iX[15], S23, 0xd8a1e681); /* 23 */
	GG(iB, iC, iD, iA, iX[4], S24, 0xe7d3fbc8); /* 24 */
	GG(iA, iB, iC, iD, iX[9], S21, 0x21e1cde6); /* 25 */
	GG(iD, iA, iB, iC, iX[14], S22, 0xc33707d6); /* 26 */
	GG(iC, iD, iA, iB, iX[3], S23, 0xf4d50d87); /* 27 */
	GG(iB, iC, iD, iA, iX[8], S24, 0x455a14ed); /* 28 */
	GG(iA, iB, iC, iD, iX[13], S21, 0xa9e3e905); /* 29 */
	GG(iD, iA, iB, iC, iX[2], S22, 0xfcefa3f8); /* 30 */
	GG(iC, iD, iA, iB, iX[7], S23, 0x676f02d9); /* 31 */
	GG(iB, iC, iD, iA, iX[12], S24, 0x8d2a4c8a); /* 32 */

												 /* Round 3 */
	HH(iA, iB, iC, iD, iX[5], S31, 0xfffa3942); /* 33 */
	HH(iD, iA, iB, iC, iX[8], S32, 0x8771f681); /* 34 */
	HH(iC, iD, iA, iB, iX[11], S33, 0x6d9d6122); /* 35 */
	HH(iB, iC, iD, iA, iX[14], S34, 0xfde5380c); /* 36 */
	HH(iA, iB, iC, iD, iX[1], S31, 0xa4beea44); /* 37 */
	HH(iD, iA, iB, iC, iX[4], S32, 0x4bdecfa9); /* 38 */
	HH(iC, iD, iA, iB, iX[7], S33, 0xf6bb4b60); /* 39 */
	HH(iB, iC, iD, iA, iX[10], S34, 0xbebfbc70); /* 40 */
	HH(iA, iB, iC, iD, iX[13], S31, 0x289b7ec6); /* 41 */
	HH(iD, iA, iB, iC, iX[0], S32, 0xeaa127fa); /* 42 */
	HH(iC, iD, iA, iB, iX[3], S33, 0xd4ef3085); /* 43 */
	HH(iB, iC, iD, iA, iX[6], S34, 0x4881d05); /* 44 */
	HH(iA, iB, iC, iD, iX[9], S31, 0xd9d4d039); /* 45 */
	HH(iD, iA, iB, iC, iX[12], S32, 0xe6db99e5); /* 46 */
	HH(iC, iD, iA, iB, iX[15], S33, 0x1fa27cf8); /* 47 */
	HH(iB, iC, iD, iA, iX[2], S34, 0xc4ac5665); /* 48 */

												/* Round 4 */
	II(iA, iB, iC, iD, iX[0], S41, 0xf4292244); /* 49 */
	II(iD, iA, iB, iC, iX[7], S42, 0x432aff97); /* 50 */
	II(iC, iD, iA, iB, iX[14], S43, 0xab9423a7); /* 51 */
	II(iB, iC, iD, iA, iX[5], S44, 0xfc93a039); /* 52 */
	II(iA, iB, iC, iD, iX[12], S41, 0x655b59c3); /* 53 */
	II(iD, iA, iB, iC, iX[3], S42, 0x8f0ccc92); /* 54 */
	II(iC, iD, iA, iB, iX[10], S43, 0xffeff47d); /* 55 */
	II(iB, iC, iD, iA, iX[1], S44, 0x85845dd1); /* 56 */
	II(iA, iB, iC, iD, iX[8], S41, 0x6fa87e4f); /* 57 */
	II(iD, iA, iB, iC, iX[15], S42, 0xfe2ce6e0); /* 58 */
	II(iC, iD, iA, iB, iX[6], S43, 0xa3014314); /* 59 */
	II(iB, iC, iD, iA, iX[13], S44, 0x4e0811a1); /* 60 */
	II(iA, iB, iC, iD, iX[4], S41, 0xf7537e82); /* 61 */
	II(iD, iA, iB, iC, iX[11], S42, 0xbd3af235); /* 62 */
	II(iC, iD, iA, iB, iX[2], S43, 0x2ad7d2bb); /* 63 */
	II(iB, iC, iD, iA, iX[9], S44, 0xeb86d391); /* 64 */

	m_aiState[0] += iA;
	m_aiState[1] += iB;
	m_aiState[2] += iC;
	m_aiState[3] += iD;

	// Zeroize sensitive information.
	memset(iX, 0, sizeof iX);
}

void CMD5::update(const unsigned char i_bInput[], uint i_iLenght)
{
	// compute number of bytes mod 64
	uint iIndex = m_aiCount[0] / 8 % blocksize;

	// Update number of bits
	if ((m_aiCount[0] += (i_iLenght << 3)) < (i_iLenght << 3))
		m_aiCount[1]++;
	m_aiCount[1] += (i_iLenght >> 29);

	// number of bytes we need to fill in buffer
	uint iFirstpart = 64 - iIndex;

	uint iI;

	// transform as many times as possible.
	if (i_iLenght >= iFirstpart)
	{
		// fill buffer first, transform
		memcpy(&m_aiBuffer[iIndex], i_bInput, iFirstpart);
		transform(m_aiBuffer);

		// transform chunks of blocksize (64 bytes)
		for (iI = iFirstpart; iI + blocksize <= i_iLenght; iI += blocksize)
			transform(&i_bInput[iI]);

		iIndex = 0;
	}
	else
		iI = 0;

	// buffer remaining input
	memcpy(&m_aiBuffer[iIndex], &i_bInput[iI], i_iLenght - iI);
}

void CMD5::update(const char input[], uint length)
{
	update((const unsigned char*)input, length);
}

CMD5 &CMD5::finalize()
{
	static unsigned char padding[64] = {
		0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
	};

	if (!m_fFinalized) {
		// Save number of bits
		unsigned char bBits[8];
		encode(bBits, m_aiCount, 8);

		// pad out to 56 mod 64.
		uint iIndex = m_aiCount[0] / 8 % 64;
		uint iPadLen = (iIndex < 56) ? (56 - iIndex) : (120 - iIndex);
		update(padding, iPadLen);

		// Append length (before padding)
		update(bBits, 8);

		// Store state in digest
		encode(m_aiDigest, m_aiState, 16);

		// Zeroize sensitive information.
		memset(m_aiBuffer, 0, sizeof m_aiBuffer);
		memset(m_aiCount, 0, sizeof m_aiCount);

		m_fFinalized = true;
	}

	return *this;
}

std::string CMD5::hexdigest() const
{
	if (!m_fFinalized) return "";

	char bBuf[33];
	for (int i = 0; i < 16; i++)
		sprintf_s(bBuf + i * 2, 33 - (i * 2), "%02x", m_aiDigest[i]);

	bBuf[32] = 0;

	return bBuf;
}

std::string MD5(const std::string &i_sString, bool i_fToUpper)
{
	CMD5 oMd5(i_sString);
	std::string sHashed = oMd5.hexdigest();

	if (i_fToUpper)
		std::transform(sHashed.begin(), sHashed.end(), sHashed.begin(), ::toupper);

	return sHashed;
}

std::string MD5(const std::vector<char> &i_abVector, bool i_fToUpper)
{
	CMD5 oMd5(i_abVector);
	std::string sHashed = oMd5.hexdigest();

	if (i_fToUpper)
		std::transform(sHashed.begin(), sHashed.end(), sHashed.begin(), ::toupper);

	return sHashed;
}
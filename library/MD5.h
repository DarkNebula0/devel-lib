#pragma once
#include <vector>
#include <string>
#include "Typedef.h"

class CMD5
{
public:
	CMD5();
	CMD5(const std::string &i_sext);
	CMD5(const std::vector<char> &i_abText);
	void update(const unsigned char *i_szBuf, uint i_iLength);
	void update(const char *i_szBuf, uint i_iLength);
	CMD5& finalize();
	std::string hexdigest() const;

private:
	void init();

	enum { blocksize = 64 };

	void transform(const byte i_iBlock[blocksize]);
	static void decode(uint i_iOutput[], const byte i_bInput[], uint i_iLen);
	static void encode(byte i_bOutput[], const uint i_iInput[], uint i_iLen);

	bool m_fFinalized;
	byte m_aiBuffer[blocksize];
	uint m_aiCount[2];
	uint m_aiState[4];
	byte m_aiDigest[16];


	static inline uint F(uint i_iX, uint i_iY, uint i_iZ);
	static inline uint G(uint i_iX, uint i_iY, uint i_iZ);
	static inline uint H(uint i_iX, uint i_iY, uint i_iZ);
	static inline uint I(uint i_iX, uint i_iY, uint i_iZ);
	static inline uint rotate_left(uint i_iX, int i_iN);
	static inline void FF(uint &i_iA, uint i_iB, uint i_iC, uint i_iD, uint i_iX, uint i_iS, uint i_iAc);
	static inline void GG(uint &i_iA, uint i_iB, uint i_iC, uint i_iD, uint i_iX, uint i_iS, uint i_iAc);
	static inline void HH(uint &i_iA, uint i_iB, uint i_iC, uint i_iD, uint i_iX, uint i_iS, uint i_iAc);
	static inline void II(uint &i_iA, uint i_iB, uint i_iC, uint i_iD, uint i_iX, uint i_iS, uint i_iAc);
};

std::string MD5(const std::string &i_sString, bool i_fToUpper = false);
std::string MD5(const std::vector<char> &i_abVector, bool i_fToUpper = false);
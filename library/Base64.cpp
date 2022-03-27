#include "Base64.h"
#include "Typedef.h"

static const std::string base64_chars =
"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
"abcdefghijklmnopqrstuvwxyz"
"0123456789+/";

static inline bool Is_base64(unsigned char c) {
	return (isalnum(c) || (c == '+') || (c == '/'));
}
static std::string Base64_encode(unsigned char const* bytes_to_encode, size_t in_len) {
	std::string ret;
	int i = 0;
	int j = 0;
	unsigned char char_array_3[3];
	unsigned char char_array_4[4];

	while (in_len--) {
		char_array_3[i++] = *(bytes_to_encode++);
		if (i == 3) {
			char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
			char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
			char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
			char_array_4[3] = char_array_3[2] & 0x3f;

			for (i = 0; (i <4); i++)
				ret += base64_chars[char_array_4[i]];
			i = 0;
		}
	}

	if (i)
	{
		for (j = i; j < 3; j++)
			char_array_3[j] = '\0';

		char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
		char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
		char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
		char_array_4[3] = char_array_3[2] & 0x3f;

		for (j = 0; (j < i + 1); j++)
			ret += base64_chars[char_array_4[j]];

		while ((i++ < 3))
			ret += '=';

	}

	return ret;

}
static std::string Base64_decode(std::string const& encoded_string) {
	size_t in_len = encoded_string.size();
	size_t i = 0;
	size_t j = 0;
	size_t in_ = 0;
	unsigned char char_array_4[4], char_array_3[3];
	std::string ret;

	while (in_len-- && (encoded_string[in_] != '=') && Is_base64(encoded_string[in_])) {
		char_array_4[i++] = encoded_string[in_]; in_++;
		if (i == 4) {
			for (i = 0; i <4; i++)
				char_array_4[i] = static_cast<byte>(base64_chars.find(char_array_4[i]));

			char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
			char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
			char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

			for (i = 0; (i < 3); i++)
				ret += char_array_3[i];
			i = 0;
		}
	}

	if (i) {
		for (j = i; j <4; j++)
			char_array_4[j] = 0;

		for (j = 0; j <4; j++)
			char_array_4[j] = static_cast<byte>(base64_chars.find(char_array_4[j]));

		char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
		char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
		char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

		for (j = 0; (j < i - 1); j++) ret += char_array_3[j];
	}

	return ret;
}

Base64::Base64() 
	: Base64(nullptr, 0, false)
{}

Base64::Base64(char *i_pBuffer, const size_t i_nSize, bool i_bDeleteBuffer)
	: m_pBuffer(i_pBuffer), m_nSize(i_nSize), m_bIsDeleteBuffer(i_bDeleteBuffer)
{}

Base64::Base64(const std::string &i_stData)
	: Base64(new char[i_stData.size()], i_stData.size(), true)
{
	memcpy(this->m_pBuffer, i_stData.c_str(), i_stData.size());
}

Base64::~Base64()
{
	if (this->m_bIsDeleteBuffer)
		delete[] this->m_pBuffer;
}

Base64 &Base64::operator=(const Base64 *i_pOther)
{
	if (i_pOther->m_pBuffer && i_pOther->m_nSize)
	{
		this->m_pBuffer = new char[i_pOther->m_nSize];
		this->m_nSize = i_pOther->m_nSize;
		this->m_bIsDeleteBuffer = true;
	}
	return *this;
}

Base64 &Base64::operator=(Base64 && i_oOther) noexcept
{
	this->m_pBuffer				= i_oOther.m_pBuffer;
	this->m_nSize				= i_oOther.m_nSize;
	this->m_bIsDeleteBuffer		= i_oOther.m_bIsDeleteBuffer;

	i_oOther.m_pBuffer			= nullptr;
	i_oOther.m_nSize			= 0;
	i_oOther.m_bIsDeleteBuffer	= false;
	return *this;
}

// Encode==================================================
std::string Base64::encode() const
{
	std::string stRet;
	if (this->m_pBuffer && this->m_nSize)
	{
		stRet = Base64_encode(reinterpret_cast<unsigned char const *>(this->m_pBuffer),this->m_nSize);
	}
	return stRet;
}

// Decode==================================================
std::string Base64::decode() const
{
	std::string stRet;
	if (this->m_pBuffer && this->m_nSize)
	{
		stRet = Base64_decode(std::string(this->m_pBuffer,this->m_nSize));
	}
	return stRet;
}
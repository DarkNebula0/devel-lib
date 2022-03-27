################################################################################################################################

Custom Serializer example:

#pragma pack(push, 1)
template<bool IsHidden = false, typename T = size_t/* Your custom datatype */>
class CMyCustomSerializerSizeT : public Serialize::IData<T, IsHidden>
{
private:
	// Here you can overwrite all virtual functions

public:
	CreateSerializeOperators(T, IsHidden);
};
#pragma pack(pop)

################################################################################################################################

Serializable custom struct example:

#pragma pack(push, 1)
struct SCustomSerializableStruct : Serialize::IStruct
{
	Serialize::IInt<>			iMyInt;	
	Serialize::IChar<>			bMyChar;	
	Serialize::IString<>		sMyString;	
	CMyCustomSerializerSizeT<>	nMySizeT;
};
#pragma pack(pop)

################################################################################################################################
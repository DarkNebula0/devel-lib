#pragma once
#include <exception>

#define CreateException(Name, ErrorMessage)			class Name : public std::exception						\
													{														\
													public:													\
														char const *what() const override					\
														{													\
															return ErrorMessage;							\
														}													\
													};

CreateException(CNoBufferException, "Buffer is nullptr!");
CreateException(CBufferOutOfRangeException, "Buffer is too small!");

CreateException(CZeroTerminationNotFound, "Couldn't find the Zero-Termination in the buffer!");

CreateException(CIndexOutOfRangeException, "Passed index is out of range!");
CreateException(CNoEntryFoundException, "No entry found!");

CreateException(COverwriteMethodException, "Overwrite the object methods!");
CreateException(CShouldNotExecuteException, "This Code should not get executed!");

CreateException(CCouldNotCreatePluginInstanceException, "Couldn't create an instance of the plugin class!");
CreateException(CPluginInstanceAlreadyInUseException, "The Plugin-Instance has already loaded a plugin!");

CreateException(CInvalidFormatException, "Object format is invalid!");
CreateException(CInvalidEncodingException, "Object invalid string encoding!");
CreateException(CDifferentTypesException, "Different types!");
CreateException(CInvalidObjectName, "Object invalid or empty name!");
CreateException(CInvalidData, "Object does not matches the struct!");
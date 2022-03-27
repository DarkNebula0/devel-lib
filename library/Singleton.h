#pragma once

/*
	Example/How to use:

	class CSingletonClass : public CSingleton<CSingletonClass>	// Derive the Singleton class
	{
		friend class CSingleton<CSingletonClass>;		// For access to constructor
	private:
		CSingletonClass() {}					// Private constructor!
		virtual ~CSingletonClass() {}

	public:
		bool initialize();						// Example function!
	};


	Then you can use it like that:

	CSingletonClass::instance()->initialize();	// Function initialize just an example!
*/

template<typename T>
class CSingleton
{
protected:
	CSingleton() {}
	CSingleton(const CSingleton &) = delete;
	CSingleton &operator=(const CSingleton &) = delete;
	virtual ~CSingleton() {}

public:
	static T *instance() {
		static T s_oInstance;
		return &s_oInstance;
	}
};
#pragma once

//シングルトン設計で共通で使う部分を省略するためのマクロ
#define THIS_CLASS_IS_STATIC_SINGLETON(TYPE)\
TYPE(const TYPE&) = delete;\
TYPE& operator=(const TYPE&) = delete;\
TYPE(const TYPE&&) = delete;\
TYPE& operator=(TYPE&) = delete;\
friend class StaticSingleton<TYPE>;

template<typename T>
class StaticSingleton {
public:
	static T& GetInstance(void) {
		static T instance_;
		return instance_;
	}

	StaticSingleton(const StaticSingleton&) = delete;
	StaticSingleton& operator=(const StaticSingleton&) = delete;
	StaticSingleton(const StaticSingleton&&) = delete;
	StaticSingleton& operator=(StaticSingleton&) = delete;
protected:
	StaticSingleton(void) = default;
	~StaticSingleton(void) = default;
};
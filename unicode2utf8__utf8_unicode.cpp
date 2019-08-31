// utf_unicode.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>


///方法一
//#include <atlstr.h>
#include <atlconv.h>
std::string atl_unicode_utf8(const std::wstring &unicode_str)
{
	USES_CONVERSION;
	std::string utf8_str = W2A_CP(unicode_str.c_str(), CP_UTF8);
	return utf8_str;
}
std::wstring atl_utf8_unicode(const std::string &utf8_str)
{
	USES_CONVERSION;
	std::wstring unicode_str = A2W_CP(utf8_str.c_str(), CP_UTF8);
	return unicode_str;
}


//方法二
#include <codecvt>
std::string cvt_unicode_utf8(const std::wstring &unicode_str)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
	std::string utf8_str = conv.to_bytes(unicode_str);
	return utf8_str;
}
std::wstring cvt_utf8_unicode(const std::string &utf8_str)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
	std::wstring unicode_str = conv.from_bytes(utf8_str);
	return unicode_str;
}

int main()
{
	std::string ansi_str = "hello world你好123abc";
	std::wstring unicode_str = L"hello world你好123abc";
	std::cout << "anisi str is:" << ansi_str << std::endl;
	std::wcout << L"unicode str is:" << unicode_str;
	std::cout << std::endl << "==============" << std::endl;
		
	std::string utf8_str1 = atl_unicode_utf8(unicode_str);
	std::string utf8_str2 = cvt_unicode_utf8(unicode_str);
	std::cout << "methond1. utf8 str is:" << utf8_str1 << std::endl;
	std::cout << "methond2. utf8 str is:" << utf8_str2 << std::endl;

	std::cout << "--------------------------" << std::endl;
	std::wstring unicode_str1 = atl_utf8_unicode(utf8_str1);
	std::wstring unicode_str2 = cvt_utf8_unicode(utf8_str2);
	
	std::wcout << L"methond2. unicode_str1 is:" << unicode_str1;
	std::cout << std::endl;

	std::wcout << L"methond2. unicode_str2 is:" << unicode_str2;
	std::cout << std::endl;

	system("pause");
	return 0;
}

#ifndef _STRINGUTIL_HPP_
#define _STRINGUTIL_HPP_


#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>
#include <map>
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <memory.h>
#include <functional>
#include <locale>
#include <sstream>
#include <sys/types.h>
#include <iterator>
#include <algorithm>
#include <codecvt>
#include <Windows.h> /* uniocdeToAscii() UTF8ToString() */

namespace JLP{
	namespace StringUtils {
		using namespace std;

		// -------------------------------- string常用方法封装 -------------------------------------------
		template<class T>
		void join(T begin, T end, string& res, const string& connector) {
			if (begin == end) {
				return;
			}
			stringstream ss;
			ss << *begin;
			begin++;
			while (begin != end) {
				ss << connector << *begin;
				begin++;
			}
			res = ss.str();
		}

		template<class T>
		string join(T begin, T end, const string& connector) {
			string res;
			join(begin, end, res, connector);
			return res;
		}

		inline string& upper(string& str) {
			transform(str.begin(), str.end(), str.begin(), (int(*)(int))toupper);
			return str;
		}

		inline string& lower(string& str) {
			transform(str.begin(), str.end(), str.begin(), (int(*)(int))tolower);
			return str;
		}

		inline std::string& ltrim(std::string &s) {
			s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
			return s;
		}

		inline std::string& rtrim(std::string &s) {
			s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
			return s;
		}

		inline std::string& trim(std::string &s) {
			return ltrim(rtrim(s));
		}

		inline std::string& ltrim(std::string & s, char x) {
			s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::bind2nd(std::equal_to<char>(), x))));
			return s;
		}

		inline std::string& rtrim(std::string & s, char x) {
			s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::bind2nd(std::equal_to<char>(), x))).base(), s.end());
			return s;
		}

		inline std::string& trim(std::string &s, char x) {
			return ltrim(rtrim(s, x), x);
		}
		
		/*
		void split( const std::string& text, const char& separator,std::vector<std::string>& result) {
			size_t start = 0;
			size_t end = 0;
			if (text.empty()) {
					return;
			}
			while ((end = text.find(separator, start)) != std::string::npos) {
					std::string substr = text.substr(start, end - start);
					result.push_back(std::move(substr));
					start = end + 1;
			}
			result.push_back(text.substr(start));
		}
		*/

		inline void split(const string& src, vector<string>& res, const string& pattern, size_t maxsplit = string::npos) {
			res.clear();
			size_t start = 0;
			size_t end = 0;
			string sub;
			while (start < src.size()) {
				end = src.find_first_of(pattern, start);
				if (string::npos == end || res.size() >= maxsplit) {
					sub = src.substr(start);
					trim(sub);
					res.push_back(sub);
					return;
				}
				sub = src.substr(start, end - start);
				trim(sub);
				res.push_back(sub);
				start = end + 1;
			}
			return;
		}

		inline vector<string> split(const string& src, const string& pattern, size_t maxsplit = string::npos) {
			vector<string> res;
			split(src, res, pattern, maxsplit);
			return res;
		}

		// --------------- u16string -------------------------------------------------
		inline std::u16string& ltrim(std::u16string &s) {
			s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](char16_t& e)->bool {return e != 32; }));
			return s;
		}

		inline std::u16string& rtrim(std::u16string &s) {
			s.erase(std::find_if(s.rbegin(), s.rend(), [](char16_t& e)->bool {return e != 32; }).base(), s.end());
			return s;
		}

		inline std::u16string& trim(std::u16string &s) {
			return ltrim(rtrim(s));
		}

		inline void split(const u16string& src, vector<u16string>& res, const u16string& pattern, size_t maxsplit = u16string::npos) {
			res.clear();
			size_t start = 0;
			size_t end = 0;
			u16string sub;
			while (start < src.size()) {
				end = src.find_first_of(pattern, start);
				if (u16string::npos == end || res.size() >= maxsplit) {
					sub = src.substr(start);
					trim(sub);
					res.push_back(sub);
					return;
				}
				sub = src.substr(start, end - start);
				trim(sub);
				if (!sub.empty()) {
					res.push_back(sub);
				}
				start = end + 1;
			}
			return;
		} 

		inline vector<u16string> split(const u16string& src, const u16string& pattern, size_t maxsplit = u16string::npos) {
			vector<u16string> res;
			split(src, res, pattern, maxsplit);
			return res;
		}


		inline bool startsWith(const string& str, const string& prefix) {
			if (prefix.length() > str.length()) {
				return false;
			}
			return 0 == str.compare(0, prefix.length(), prefix);
		}

		inline bool endsWith(const string& str, const string& suffix) {
			if (suffix.length() > str.length()) {
				return false;
			}
			return 0 == str.compare(str.length() - suffix.length(), suffix.length(), suffix);
		}

		inline bool isInStr(const string& str, char ch) {
			return str.find(ch) != string::npos;
		}
		

		// ------------------------------- 汉字编码转换处理 ---------------------------------------------

		template <class Uint16Container>
		bool utf8ToUnicode(const char * const str, size_t len, Uint16Container& vec) {
			if (!str) {
				return false;
			}
			char ch1, ch2;
			uint16_t tmp;
			vec.clear();
			for (size_t i = 0; i < len;) {
				if (!(str[i] & 0x80)) { // 0xxxxxxx 表明用1字节编码  Ox80:10000000
					vec.push_back(str[i]);
					i++;
				}
				else if ((uint8_t)str[i] <= 0xdf && i + 1 < len) { // 110xxxxxx 表明用2字节编码 Oxdf:11011111 
					ch1 = (str[i] >> 2) & 0x07;
					ch2 = (str[i + 1] & 0x3f) | ((str[i] & 0x03) << 6);
					tmp = (((uint16_t(ch1) & 0x00ff) << 8) | (uint16_t(ch2) & 0x00ff));
					vec.push_back(tmp);
					i += 2;
				}
				else if ((uint8_t)str[i] <= 0xef && i + 2 < len) { // 表明用3字节编码
					ch1 = ((uint8_t)str[i] << 4) | ((str[i + 1] >> 2) & 0x0f);
					ch2 = (((uint8_t)str[i + 1] << 6) & 0xc0) | (str[i + 2] & 0x3f);
					tmp = (((uint16_t(ch1) & 0x00ff) << 8) | (uint16_t(ch2) & 0x00ff));
					vec.push_back(tmp);
					i += 3;
				}
				else {
					return false;
				}
			}
			return true;
		}
		template <class Uint16Container>
		bool utf8ToUnicode(const string& str, Uint16Container& vec) {
			return utf8ToUnicode(str.c_str(), str.size(), vec);
		}

		template <class Uint16ContainerConIter>
		void unicodeToUtf8(Uint16ContainerConIter begin, Uint16ContainerConIter end, string& res) {
			res.clear();
			uint16_t ui;
			while (begin != end) {
				ui = *begin;
				if (ui <= 0x7f) {
					res += char(ui);
				}
				else if (ui <= 0x7ff) {
					res += char(((ui >> 6) & 0x1f) | 0xc0);
					res += char((ui & 0x3f) | 0x80);
				}
				else {
					res += char(((ui >> 12) & 0x0f) | 0xe0);
					res += char(((ui >> 6) & 0x3f) | 0x80);
					res += char((ui & 0x3f) | 0x80);
				}
				begin++;
			}
		}
		std::string unicodeToUtf8(const std::u16string& in) {
			std::string out;
			unicodeToUtf8(in.begin(), in.end(), out);
			return out;
		}

		//#include  <codecvt>
		//#include <Windows.h>
		std::string unicodeToAscii(const std::wstring str)
		{
			int iTextLen = WideCharToMultiByte(CP_ACP, 0, str.c_str(), -1, NULL, 0, NULL, NULL);
			std::vector<char> vecText(iTextLen, '\0');
			::WideCharToMultiByte(CP_ACP, 0, str.c_str(), -1, &(vecText[0]), iTextLen, NULL, NULL);

			std::string strText = &(vecText[0]);

			return strText;
		}
		std::string utf8ToString(const std::string & utf8Data)
		{ 
			std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
			std::wstring wString = conv.from_bytes(utf8Data);
			
			return unicodeToAscii(wString);
		}
		std::string unicodeToString(const std::u16string& in) {
			return utf8ToString(unicodeToUtf8(in));
		}

		template <class Uint16Container>
		bool gbkTrans(const char* const str, size_t len, Uint16Container& vec) {
			vec.clear();
			if (!str) {
				return true;
			}
			size_t i = 0;
			while (i < len) {
				if (0 == (str[i] & 0x80)) { // 0xxxxxxx
					vec.push_back(uint16_t(str[i]));
					i++;
				}
				else {
					if (i + 1 < len) { //&& (str[i+1] & 0x80))
						uint16_t tmp = (((uint16_t(str[i]) & 0x00ff) << 8) | (uint16_t(str[i + 1]) & 0x00ff));
						vec.push_back(tmp);
						i += 2;
					}
					else {
						return false;
					}
				}
			}
			return true;
		}
		template <class Uint16Container>
		bool gbkTrans(const string& str, Uint16Container& vec) {
			return gbkTrans(str.c_str(), str.size(), vec);
		}

		template <class Uint16ContainerConIter>
		void gbkTrans(Uint16ContainerConIter begin, Uint16ContainerConIter end, string& res) {
			res.clear();
			//pair<char, char> pa;
			char first, second;
			while (begin != end) {
				//pa = uint16ToChar2(*begin);
				first = ((*begin) >> 8) & 0x00ff;
				second = (*begin) & 0x00ff;
				if (first & 0x80) {
					res += first;
					res += second;
				}
				else {
					res += second;
				}
				begin++;
			}
		}
		
	}
}  // end namespace JLP
#endif

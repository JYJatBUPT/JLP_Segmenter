#ifndef JLP_UTILS_FILEUTILS_H_
#define JLP_UTILS_FILEUTILS_H_


#include <vector>
#include <string>
#include <unordered_map>

#include <fstream>
#include <ostream>
#include <iostream>

namespace JLP {

class FileUtil 
{
public:
	static void readLines(const std::string& filePath, std::vector<std::string>& Lines)
	{
		std::ifstream reader(filePath);
		if (!reader) {
			std::cout << __DATE__ << " " << __TIME__ << " "
				<< __FILE__ << " " << __LINE__
				<< "File read error!!!" << std::endl;
			exit(-1);
		}

		std::string aLine;
		while (getline(reader, aLine))
		{
			Lines.push_back(aLine);
		}
		reader.close();
	}

	static void readUnicodeLines(const std::string& filePath, std::vector<std::u16string>& Lines)
	{
		//Lines.clear(); // Lines容器最好提前分配足够内存

		std::ifstream reader(filePath);
		if (!reader) {
			std::cout << __DATE__ << " " << __TIME__ << " "
				<< __FILE__ << " " << __LINE__
				<< "File read error!!!" << std::endl;
			exit(-1);
		}

		std::string aLine;
		std::u16string aU16String;
		while (getline(reader, aLine))
		{
			// Trim(aLIne); // Proper PreProcessing
			utf8ToUnicode(aLine, aU16String);
			Lines.push_back(aU16String);
			aU16String.clear();
		}
		reader.close();
	}
	
	static long long readFreqs(const std::string& filePath, std::vector<int>& Freqs) {
		std::ifstream reader(filePath);
		if (!reader) {
			std::cout << __DATE__ << " " << __TIME__ << " "
				<< __FILE__ << " " << __LINE__
				<< "File read error!!!" << std::endl;
			exit(-1);
		}

		long long totalFreq{};
		
		while (!reader.eof())
		{
			int idata;
			reader >> idata;
			totalFreq += idata;
			Freqs.push_back(idata);
		}
	
		reader.close();
		return totalFreq;
	}

	static void readProbs(const std::string& filePath, std::vector<double>& Freqs) {
		std::ifstream reader(filePath);
		if (!reader) {
			std::cout << __DATE__ << " " << __TIME__ << " "
				<< __FILE__ << " " << __LINE__
				<< "File read error!!!" << std::endl;
			exit(-1);
		}

		while (!reader.eof())
		{
			double idata;
			reader >> idata;
			Freqs.push_back(idata);
		}

		reader.close();
		return ;
	}

	static void writeUnicodeLines(const std::string& outFilePath, const std::string& outFilePath1,std::unordered_map<std::u16string, size_t> counter) {
		// Ensure the two file to write out exists
		std::ifstream reader(outFilePath);
		if (!reader) {
			std::cout << __DATE__ << " " << __TIME__ << " "
				<< __FILE__ << " " << __LINE__
				<< " File to write out now dose not exist!!!" << std::endl;
			exit(-1);
		}
		reader.close();
		
		std::ifstream reader1(outFilePath1);
		if (!reader1) {
			std::cout << __DATE__ << " " << __TIME__ << " "
				<< __FILE__ << " " << __LINE__
				<< " File to write out now dose not exist!!!" << std::endl;
			exit(-1);
		}
		reader1.close();

		std::ofstream out;
		out.open(outFilePath);
		if (!out.is_open()) {
			std::cout << __DATE__ << " " << __TIME__ << " "
				<< __FILE__ << " " << __LINE__
				<< "File write error!!!" << std::endl;
			exit(-1);
		}

		for (const auto &i : counter) {

			out << unicodeToUtf8(i.first) << std::endl;
		}
		out.close();

		std::ofstream out1;
		out1.open(outFilePath1);
		if (!out1.is_open()) {
			std::cout << __DATE__ << " " << __TIME__ << " "
				<< __FILE__ << " " << __LINE__
				<< "File write error!!!" << std::endl;
			exit(-1);
		}

		for (const auto &i : counter) {
			out1 << i.second << std::endl;
		}
		out1.close();

	}
	
	static void writeUnicodeLines(const std::string& outFilePath,std::vector<std::u16string>& words) {
		std::ofstream out;
		out.open(outFilePath, std::ios::app);

		size_t length = words.size();
		if (length == 0) return;
		if (length >= 2) {
			for (size_t i = 0; i < length - 1; ++i) {
				out << unicodeToUtf8(words.at(i)) << " ";
			}
		}
		out << unicodeToUtf8(words[length - 1]) << "\n";
		out.close();
	}
public:
	template <class Uint16Container>
	static bool utf8ToUnicode(const char * const str, size_t len, Uint16Container& vec) {
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
	static bool utf8ToUnicode(const std::string& str, Uint16Container& vec) {
		return utf8ToUnicode(str.c_str(), str.size(), vec);
	}

	template <class Uint16ContainerConIter>
	static void unicodeToUtf8(Uint16ContainerConIter begin, Uint16ContainerConIter end, std::string& res) {
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
	static std::string unicodeToUtf8(const std::u16string& in) {
		std::string out;
		unicodeToUtf8(in.begin(), in.end(), out);
		return out;
	}

};



} // end namespace JLP
#endif

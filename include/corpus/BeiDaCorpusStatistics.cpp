//#include <string>
//#include <vector>
//#include <unordered_map>
//#include <algorithm>
//
//#include "../../utils/FileUtils.hpp"
//#include "../../include/utils/TimeUtils.hpp"
//
//inline std::u16string& ltrim(std::u16string &s, const uint16_t pattern = 32) {
//	s.erase(s.begin(), std::find_if(s.begin(), s.end(), [&](char16_t& e)->bool {return e != pattern; }));
//	return s;
//}
//
//inline std::u16string& rtrim(std::u16string &s, const uint16_t pattern = 32) {
//	s.erase(std::find_if(s.rbegin(), s.rend(), [&](char16_t& e)->bool {return e != pattern; }).base(), s.end());
//	return s;
//}
//
//inline std::u16string& trim(std::u16string &s,const uint16_t pattern=32) {
//	return ltrim(rtrim(s,pattern),pattern);
//}
//
//inline void split(const std::u16string& src, std::vector<std::u16string>& res, const std::u16string& pattern, size_t maxsplit = std::u16string::npos) {
//	res.clear();
//	size_t start = 0;
//	size_t end = 0;
//	std::u16string sub;
//	while (start < src.size()) {
//		end = src.find_first_of(pattern, start);
//		if (std::u16string::npos == end || res.size() >= maxsplit) {
//			sub = src.substr(start);
//			trim(sub);
//			res.push_back(sub);
//			return;
//		}
//		sub = src.substr(start, end - start);
//		trim(sub);
//		if (!sub.empty()) {
//			res.push_back(sub);
//		}
//		start = end + 1;
//	}
//	return;
//}
//
//inline std::vector<std::u16string> split(const std::u16string& src, const std::u16string& pattern, size_t maxsplit = std::u16string::npos) {
//	std::vector<std::u16string> res;
//	split(src, res, pattern, maxsplit);
//	return res;
//}
//
//
//int main() {
//	JLP::TimeUtils::showPresentTime();
//
//	std::string dictPath = "include/segmenter/corpus/BeiDaRenMingCorpus.txt";
//
//	std::vector<std::u16string> Lines;
//	JLP::FileUtil::readUnicodeLines(dictPath,Lines);
//	
//	//对每个句子进行 Split --> Trim -->  
//	std::vector<std::vector<std::u16string>> tokensOfLines;
//	std::vector<std::u16string> res;
//	for (auto& aLine : Lines) {
//		res=split(aLine,u" ");
//		tokensOfLines.push_back(res);
//		res.clear();
//	}
//	JLP::TimeUtils::showPresentTime(); // 90s的处理
//	
//	 //统计每个词的频次
//	std::unordered_map<std::u16string, size_t> counter;
//	for (auto& aLine : tokensOfLines) {
//		if (!aLine.empty()) {
//			for (size_t i = 1; i < aLine.size(); ++i) {
//				auto tokens = split(aLine[i], u"/");
//				if (!tokens.empty()) {
//					auto token = tokens.at(0);
//					if (counter.find(token) == counter.end()) {
//						counter[token] = 1;
//					}
//					else {
//						++counter[token];
//					}
//				}
//			}
//		}
//	} 
//	JLP::TimeUtils::showPresentTime(); // 4 min的处理
//	
//	std::string outFilePath = "include/segmenter/corpus/unigram";
//	std::string outFilePath1 = "include/segmenter/corpus/unigramCounter";
//	JLP::FileUtil::writeUnicodeLines(outFilePath,outFilePath1, counter);
//	
//	JLP::TimeUtils::showPresentTime(); // 4 min的处理
//
//}
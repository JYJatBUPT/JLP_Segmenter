#ifndef JLP_SEGMENTER_MAXMATCHSEGMENTER_H_
#define JLP_SEGMENTER_MAXMATCHSEGMENTER_H_

#include <string>
#include <algorithm>
#include <iostream>

#include "../../utils/StringUtils.hpp"
#include "../../utils/FileUtils.hpp"
#include "../container/Trie.hpp"


namespace JLP {

enum MatchMode {
	ForwardDirection,
	ReverseDirection,
};

template <class T>
class MaxMatchSeger {
public:
	enum MatchMode matchMode;
	JLP::Trie<T> trie;
public:
	explicit MaxMatchSeger(const std::string& matchMode="FD") {
		if (matchMode == "FD") {
			this->matchMode=ForwardDirection;	
		}
		else if(matchMode == "RD"){
			this->matchMode = ReverseDirection;	
		}
		else {
			std::cout << "请指定分词器的最大匹配模式:[\"FD\"/\"RD\"]\n"; exit(-1); 
		}
		
		if (this->matchMode== ForwardDirection) { 
			std::cout << "进入正向最大匹配模式...\n";
		}
		else {
			std::cout << "进入逆向最大匹配模式...\n";	
		}	
	}
public:
	bool buildDict(const std::string& dictPath) 
	{
		std::vector<std::u16string> words;
		JLP::FileUtil::readUnicodeLines(dictPath, words);

		if (this->matchMode == ReverseDirection) {
			for (auto& word : words) {
				std::reverse(word.begin(), word.end()); // Reverse each term entry
			}
		}

		trie.buildTrie(words);
		return true;
	}

	void cut(const std::u16string& src,std::vector<std::u16string>& out) {
		out.clear();
		if (src.empty()) return;
		for (size_t i{}; i < src.size();) {
			size_t tempLen = this->trie.getMaxLen(src.substr(i));
			out.push_back(src.substr(i, tempLen));
			i += tempLen;
		}
		return;
	}

	void batchCut(const std::string& srcFilePath) {
		std::vector<std::u16string> sentences;
		std::vector<std::u16string> words;
		JLP::FileUtil::readUnicodeLines(srcFilePath, sentences);
		for (auto& sentence : sentences) {
			if (this->matchMode == ReverseDirection) {
				std::reverse(sentence.begin(), sentence.end()); 
			}

			cut(sentence, words);
			printWords(words);
			words.clear();
		}
	}

	inline void printWords( std::vector<std::u16string>& words) {
		if (this->matchMode == ForwardDirection) {
			for (auto& e : words) {
				std::cout << JLP::StringUtils::unicodeToString(e) << "/";
			}
			std::cout << '\n';
		}
		else {
			std::reverse(words.begin(), words.end());
			for (auto& e : words) {
				std::reverse(e.begin(), e.end());
				std::cout << JLP::StringUtils::unicodeToString(e) << "/";
			}
			std::cout << '\n';
		}
	}

private:
  size_t getMaxLenEntry(const std::u16string src){
		return trie.getMaxLen(src);
	}

};

} // end namspace JLP
#endif 
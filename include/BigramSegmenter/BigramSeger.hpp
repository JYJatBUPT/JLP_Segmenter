#ifndef JLP_SEGMENTER_BIGRAMSEGMENTER_H_
#define JLP_SEGMENTER_BIGRAMSEGMENTER_H_

#include <vector>
#include <string>
#include <cmath>
#include <cctype>

#include "../../utils/FileUtils.hpp"
#include "../container/Trie.hpp"

namespace JLP {

	template <typename T, typename T2>
	class BigramSeger {
	public:
		long long M;
		Trie<T> trie;
		Trie<T2> trie1;
		std::vector<int> Node_i;
		std::vector<double> maxProbOfNode;
		std::vector<std::u16string> bestPrevWords;
		double smoothLogProb;
		size_t seqsAlNumCounter;

		bool buildDict(const std::string& dictPath, const std::string& dictPath2) {

			std::vector<T> values;
			std::vector<std::u16string> words;
			std::string freqsDictPath = dictPath + "Counter";

			FileUtil::readUnicodeLines(dictPath, words);
			for (auto& word : words) {
				std::reverse(word.begin(), word.end());
			}
			this->M = FileUtil::readFreqs(freqsDictPath, values);
			this->smoothLogProb = log(0.2 / M);
			trie.buildTrie(words, values);

			std::vector<T2> values2;
			std::vector<std::u16string> words2;
			std::string probsDictPath2 = dictPath2 + "Counter";
			FileUtil::readUnicodeLines(dictPath2, words2);
			for (auto& word : words2) {
				std::reverse(word.begin(), word.end());
			}
			FileUtil::readProbs(probsDictPath2, values2);
			trie1.buildTrie(words2, values2);

			return true;
		}

		double getBigramProb(size_t node_i, const std::u16string& word) {
			int curPrevNode = node_i - word.size();
			auto prevWord = bestPrevWords[curPrevNode];
			std::u16string queryWords = word + u"@" + prevWord;

			auto res = trie1.getValue(queryWords);
			return res;
		}

		void computeProbOfNode(size_t i, const std::u16string& src) {
			if (0 == i) {
				Node_i.push_back(0);
				maxProbOfNode.push_back(0);
				bestPrevWords.push_back(u"START");
				return;
			}

			std::vector<std::u16string> prevWords;
			double MaxProb = -100000;
			size_t bestPrevNode = 0;
			std::u16string bestPrevWord = u"START";

			trie.matchPrevWords(src, i - 1, prevWords);

			if (prevWords.size() == 0) {
				if (isAlNum(src.at(i - 1)) && seqsAlNumCounter > 0) {
					bestPrevNode = i - 1 - seqsAlNumCounter;
					MaxProb = maxProbOfNode[i - 1];
					bestPrevWord.clear();
					bestPrevWord += src.substr(i - 1, 1);;
					++seqsAlNumCounter;
				}
				else {
					if (isAlNum(src.at(i - 1)) && seqsAlNumCounter == 0) {
						bestPrevNode = i - 1;
						MaxProb = maxProbOfNode[i - 1] + smoothLogProb;
						bestPrevWord.clear();
						bestPrevWord += src.substr(i - 1, 1); ;
						++seqsAlNumCounter;
					}
					else {
						bestPrevNode = i - 1;
						MaxProb = maxProbOfNode[i - 1] + smoothLogProb;
						bestPrevWord.clear();
						bestPrevWord += src.substr(i - 1, 1);
						seqsAlNumCounter = 0;
					}
				}
			}
			else {
				this->seqsAlNumCounter = 0;
				for (auto& word : prevWords) {
					double c1 = 0.9;
					long a1 = trie.getValue(word);
					auto a2 = getBigramProb(i, word);
					double wordProb_log = std::log(c1*(a1 / double(M)) + (1 - c1)*a2);
					int curPrevNode = i - word.size();
					double curNodeProb = maxProbOfNode[curPrevNode] + wordProb_log;
					if (curNodeProb > MaxProb) {
						bestPrevNode = curPrevNode;
						MaxProb = curNodeProb;
						bestPrevWord.clear();
						bestPrevWord += word;
					}
				}
			}
			Node_i.push_back(bestPrevNode);
			maxProbOfNode.push_back(MaxProb);
			bestPrevWords.push_back(bestPrevWord);
			return;
		}

		void backTracking(const std::u16string& src, std::vector<std::u16string>& out) {
			if (Node_i.empty()) {
				std::cout << "wrong!!! \n";
				exit(-1);
			}

			std::vector<size_t> cutPos;
			cutPos.push_back(Node_i.size() - 1);
			for (size_t i = Node_i.size() - 1; i > 0;) {
				cutPos.push_back(Node_i[i]);
				i = Node_i[i];
			}
			std::reverse(cutPos.begin(), cutPos.end());
			if (cutPos[0] != 0) {
				std::cout << "wrong!!! \n";
				exit(-1);
			}

			size_t prevPos = 0;
			for (size_t i = 1; i < cutPos.size(); ++i) {
				out.push_back(src.substr(prevPos, cutPos[i] - prevPos));
				prevPos = cutPos[i];
			}
		}

		void cut(const std::u16string& src, std::vector<std::u16string>& out) {
			out.clear();
			if (src.empty()) return;
			size_t nodeNum = src.size() + 1;
			Node_i.clear();
			maxProbOfNode.clear();
			seqsAlNumCounter = 0;
			for (size_t i{}; i < nodeNum; ++i) {
				computeProbOfNode(i, src);
			}
			backTracking(src, out);
			return;
		}

		void batchCut(const std::string& srcFilePath) {
			std::vector<std::u16string> sentences;
			std::vector<std::u16string> words;
			FileUtil::readUnicodeLines(srcFilePath, sentences);

			for (auto& sentence : sentences) {
				cut(sentence, words);
				printWords(words);
				words.clear();
			}
			std::cout << "Done!\n";
		}

		inline void printWords(std::vector<std::u16string>& words) {
			std::string outFilePath = "include/segmenter/corpus/output.txt";
			FileUtil::writeUnicodeLines(outFilePath, words);
		}
	private:
		inline bool isAlNum(const char16_t Char)
		{
			if (Char >= u'0'&&Char <= u'9'
				|| Char >= u'A'&&Char <= u'Z'
				|| Char >= u'a'&&Char <= u'z'
				|| Char >= u'£°'&&Char <= u'£¹'
				|| Char >= u'£Á'&&Char <= u'£Ú'
				|| Char >= u'£á'&&Char <= u'£ú')
			{
				return true;
			}
			else
			{
				return false;
			}
		}
	};

} // end namespace JLP

#endif

#ifndef TRIE_HPP
#define TRIE_HPP

#include <cstddef>
#include <string>

#define KEY_COUNT 95 // ascii character count

template <typename T> class TrieNode {
    public:
        TrieNode* letters[KEY_COUNT];
        T data;
        TrieNode()
        {
            for (int i = 0; i < KEY_COUNT; i++) {
                letters[i] = nullptr;
            }
        };
};

template <typename T> class Trie {
    public:
        TrieNode<T>* root;
        Trie()
        {
            root = new TrieNode<T>();
        }
        void add(std::string key, T data) {
            char *walk = key.data();
            TrieNode<T> *current = this->root;

            while (*walk) {
                if (!current->letters[(*walk)-32]) {
                    current->letters[(*walk)-32] = new TrieNode<T>();
                }
                current = current->letters[(*walk)-32];
                walk++;
            }

            current->data = data;
        }
        
        T get(std::string key)
        {
            char* walk = key.data();
            TrieNode<T>* current = this->root;

            while (*walk) {
                if (current->letters[(*walk) - 32]) {
                    current = current->letters[(*walk) - 32];
                } else
                    return T();
                walk++;
            }

            return current->data;
        }
};

#endif
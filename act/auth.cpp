#include "auth.hpp"
#include <iostream>
EXPORT std::string checker(std::string username){
    boost::char_separator<char> sep(":");
    boost::tokenizer<boost::char_separator<char>> tokens(username,sep);
    
    std::ostringstream joined;
    for(auto it = tokens.begin(); it != tokens.end();++it){
        if(it != tokens.begin()){
            joined << ":";
        }
        joined << *it;
    }

    return joined.str();
};

std::string hash(const std::string& input) {
    EVP_MD_CTX* ctx = EVP_MD_CTX_new();
    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int lengthOfHash = 0;

    EVP_DigestInit_ex(ctx, EVP_sha256(), NULL);
    EVP_DigestUpdate(ctx, input.data(), input.size());
    EVP_DigestFinal_ex(ctx, hash, &lengthOfHash);
    EVP_MD_CTX_free(ctx);

    std::stringstream ss;
    for (unsigned int i = 0; i < lengthOfHash; i++)
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];

    return ss.str();
}

EXPORT bool auth_cond(bool cond=false){
    std::cout << cond << std::endl;
    return cond;
}
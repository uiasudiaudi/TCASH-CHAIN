#include "xtcashcl/include/xcrypto.h"

#include "cryptcashp/filters.h"
#include "cryptcashp/hkdf.h"
#include "cryptcashp/modes.h"
#include "cryptcashp/osrng.h"
#include "cryptcashp/scrypt.h"
#include "cryptcashp/sha3.h"
#include "xcrypto/xckey.h"
#include "xpbase/base/tcash_utils.h"
#include "xrpc/xuint_format.h"
#include "xtcashcl/include/base/utility.h"
#include "xtcashcl/include/global_definition.h"
#include "xtcashcl/include/user_info.h"
#include "xtcashcl/include/xtcash/xcrypto_util.h"
#include "xutility/xhash.h"

#include <dirent.h>
#include <sys/stat.h>

#include <cryptcashp/aes.h>
#include <cryptcashp/modes.h>
#include <cryptcashp/pwdbased.h>
#include <cryptcashp/sha.h>

#include <fstream>
#include <iostream>
#include <random>
#include <sstream>
#include <vector>

using namespace tcash::utl;
using namespace tcash::xrpc;
using namespace tcash::data;

namespace xChainSDK {
namespace xcrypto {

using std::endl;
using std::string;

static bool g_is_key = false;  // false for owner key, true for worker key
static string g_owner_account;
const std::string CRYPTO_KDF = "scypt";  // scypt or pbkdf2

std::string get_account_address_from_json_keystore(Json::Value const & keystore_json) {
    if (keystore_json.isMember("account_address") && !keystore_json["account_address"].asString().empty()) {
        return keystore_json["account_address"].asString();
    }
    // compatible old "account address"
    if (keystore_json.isMember("account address") && !keystore_json["account address"].asString().empty()) {
        return keystore_json["account address"].asString();
    }
    return "";
}

int get_tcash_ed_key(const string & pw, const Json::Value & key_info, CryptcashP::byte * key) {
    // parse hkdf params
    auto salt_hex = key_info["crypto"]["kdfparams"]["salt"].asString();
    auto salt_vec = hex_to_uint(salt_hex);
    auto info_hex = key_info["crypto"]["kdfparams"]["info"].asString();
    auto info_vec = hex_to_uint(info_hex);

    // derive hkdf key
    CryptcashP::HKDF<CryptcashP::SHA3_Final<derived_key_len>> hkdf;
    hkdf.DeriveKey(key, derived_key_len, (const uint8_t *)(pw.data()), pw.size(), salt_vec.data(), salt_len, info_vec.data(), hkdf_info_len);

    return 0;
}

int get_eth_ed_key(const string & pw, const Json::Value & key_info, CryptcashP::byte * key) {
    // parse hkdf params
    auto salt_hex = key_info["crypto"]["kdfparams"]["salt"].asString();
    std::string salt_vec = tcash::HexDecode(salt_hex);

    if (key_info["crypto"]["kdf"].asString() == "pbkdf2") {
        unsigned iterations = key_info["crypto"]["kdfparams"]["c"].asInt();
        if (CryptcashP::PKCS5_PBKDF2_HMAC<CryptcashP::SHA256>().DeriveKey(key,
                                                                      key_info["crypto"]["kdfparams"]["dklen"].asInt(),
                                                                      0,
                                                                      reinterpret_cast<byte const *>(pw.data()),
                                                                      pw.size(),
                                                                      (byte *)salt_vec.data(),
                                                                      salt_vec.size(),
                                                                      iterations) != iterations) {
            std::cout << "decrypt key fail." << std::endl;
            return 1;
        }
    } else if (key_info["crypto"]["kdf"].asString() == "scrypt") {
        unsigned iterations = key_info["crypto"]["kdfparams"]["n"].asInt();
        CryptcashP::Scrypt().DeriveKey(key,
                                     key_info["crypto"]["kdfparams"]["dklen"].asInt(),
                                     reinterpret_cast<byte const *>(pw.data()),
                                     pw.size(),
                                     (byte *)salt_vec.data(),
                                     salt_vec.size(),
                                     iterations,
                                     8,
                                     1);
    } else {
        std::cout << "Unknown KDF" << key_info["crypto"]["kdf"].asString() << "not supported.";
        return 1;
    }
    return 0;
}

bool parse_keystore(std::string const & keystore_path, Json::Value & keystore_info) {
    std::ifstream keyfile(keystore_path, std::ios::in);
    if (!keyfile) {
        std::cout << keystore_path << " Not Exist" << std::endl;
        return false;
    }
    std::stringstream buffer;
    buffer << keyfile.rdbuf();
    string keystore_info_str = buffer.str();

    Json::Reader reader;
    if (!reader.parse(keystore_info_str, keystore_info)) {
        std::cout << "keyfile " << keystore_path << " parse error" << std::endl;
        return false;
    }

    return true;
}

bool decrypt_get_kdf_key(std::string const & password, Json::Value const & keystore_info, std::string & kdf_key) {
    std::string account = get_account_address_from_json_keystore(keystore_info);

    CryptcashP::byte key[derived_key_len];
    if (tcash::base::xvaccount_t::get_addrtype_from_account(account) == tcash::base::enum_vaccount_addr_type_secp256k1_user_account) {
        if (get_tcash_ed_key(password, keystore_info, key) != 0) {
            return false;
        }
        kdf_key = uint_to_str(key, derived_key_len);
    } else if (tcash::base::xvaccount_t::get_addrtype_from_account(account) == tcash::base::enum_vaccount_addr_type_secp256k1_eth_user_account) {
        if (get_eth_ed_key(password, keystore_info, key) != 0) {
            return false;
        }
        kdf_key = uint_to_str(key, derived_key_len);
    } else {
        return false;
    }
    return true;
}

bool decrypt_keystore_by_password(std::string const & password, Json::Value const & keystore_info, std::string & pri_key) {
    std::string kdf_key;
    if (decrypt_get_kdf_key(password, keystore_info, kdf_key) == false) {
        return false;
    }
    return decrypt_keystore_by_kdf_key(kdf_key, keystore_info, pri_key);
}

bool decrypt_keystore_by_kdf_key(std::string const & kdf_key, Json::Value const & keystore_info, std::string & pri_key) {
    std::string account = get_account_address_from_json_keystore(keystore_info);

    if (tcash::base::xvaccount_t::get_addrtype_from_account(account) == tcash::base::enum_vaccount_addr_type_secp256k1_user_account) {
        // derive ciphertext
        auto cipher_hex = keystore_info["crypto"]["ciphertext"].asString();
        auto ciphertext = hex_to_uint8_str(cipher_hex);

        // parse aes param iv
        auto iv_str = keystore_info["crypto"]["cipherparams"]["iv"].asString();
        auto iv = hex_to_uint(iv_str);
        auto key = hex_to_uint(kdf_key);

        CryptcashP::AES::Decryption aesDecryption(key.data(), aes_key_len);
        CryptcashP::CBC_Mode_ExternalCipher::Decryption cbcDecryption(aesDecryption, iv.data());
        try {
            CryptcashP::StreamTransformationFilter stfDecryptor(cbcDecryption, new CryptcashP::StringSink(pri_key));
            stfDecryptor.Put(reinterpret_cast<const unsigned char *>(ciphertext.c_str()), ciphertext.size());
            stfDecryptor.MessageEnd();
        } catch (std::exception &) {
            // std::cout << "Decryption Error: " << e.what() << std::endl;
            pri_key.clear(); // or might return same intermiate results.
            return false;
        }
        return true;
    } else if (tcash::base::xvaccount_t::get_addrtype_from_account(account) == tcash::base::enum_vaccount_addr_type_secp256k1_eth_user_account) {
        auto key = hex_to_uint(kdf_key);
        // derive ciphertext
        auto cipher_hex = keystore_info["crypto"]["ciphertext"].asString();
        auto ciphertext = tcash::HexDecode(cipher_hex);

        // validate mac
        CryptcashP::byte mac[mac_len];
        generate_eth_keystore_mac(mac, (byte *)key.data(), ciphertext);
        std::string mac_hex = tcash::HexEncode(std::string((char *)mac, mac_len));
        if (mac_hex != keystore_info["crypto"]["mac"].asString()) {
            // std::cout << "Invalid eth key - MAC mismatch:" << mac_hex<<","<< keystore_info["crypto"]["mac"].asString() << std::endl;
            return false;
        }

        // parse aes param iv
        auto iv_str = keystore_info["crypto"]["cipherparams"]["iv"].asString();
        auto iv = tcash::HexDecode(iv_str);

        if (keystore_info["crypto"]["cipher"].asString() != "aes-128-ctr") {
            std::cout << "Unknown cipher" << keystore_info["crypto"]["cipher"].asString() << "not supported.";
            return false;
        }

        CryptcashP::CTR_Mode<CryptcashP::AES>::Decryption d;
        d.SetKeyWithIV((byte *)key.data(), 16, (byte *)iv.data());
        byte ret[ciphertext.size()];
        d.ProcessData(ret, (byte *)ciphertext.data(), ciphertext.size());

        pri_key = string{(char *)ret, ciphertext.size()};
        pri_key = tcash::HexEncode(pri_key);
        return true;
    }
    return false;
}

bool decrypt_keystore_file_by_kdf_key(std::string const & kdf_key, std::string const & keystore_path, std::string & pri_key) {
    Json::Value keystore_info;
    if (parse_keystore(keystore_path, keystore_info) == false) {
        return false;
    }
    return decrypt_keystore_by_kdf_key(kdf_key, keystore_info, pri_key);
}

// encrypt

string create_new_keystore(const string & pw, string & dir, bool is_key, string account) {
    g_is_key = is_key;
    g_owner_account = account;

    // create new pub pri key pair
    if (g_userinfo.account.size() != 0) {
        copy_g_userinfo = g_userinfo;
    }
    if (!account.empty() && tcash::base::xvaccount_t::get_addrtype_from_account(account) == tcash::base::enum_vaccount_addr_type_secp256k1_user_account) {
        xcrypto_util::make_private_key(g_userinfo.private_key);
        g_userinfo.account = xcrypto_util::make_address_by_assigned_key(g_userinfo.private_key);
        auto base64_pri = utility::base64_encode(g_userinfo.private_key.data(), PRI_KEY_LEN);
        auto path = get_keystore_filepath(dir, g_userinfo.account);
        std::ofstream key_file(path, std::ios::out | std::ios::trunc);
        if (!key_file) {
            std::cout << "Open Key File: " << path << " Error" << std::endl;
        }

        aes256_cbc_encrypt(pw, base64_pri, key_file);
        key_file.close();
        return path;
    }
    xcrypto_util::make_private_key(g_userinfo.private_key);
    g_userinfo.account = xcrypto_util::make_address_by_assigned_key(g_userinfo.private_key, tcash::base::enum_vaccount_addr_type_secp256k1_eth_user_account);

    std::string hex_pri((char *)g_userinfo.private_key.data(), PRI_KEY_LEN);
    hex_pri = tcash::HexEncode(hex_pri);

    auto path = get_keystore_filepath(dir, g_userinfo.account);
    std::ofstream key_file(path, std::ios::out | std::ios::trunc);
    if (!key_file) {
        std::cout << "Open Key File: " << path << " Error" << std::endl;
    }

    eth_aes256_cbc_encrypt(pw, hex_pri, key_file);
    key_file.close();
    return path;
}

string create_new_keystore(const string & pw, string & dir, const string & str_pri, bool is_key, string account) {
    g_is_key = is_key;

    if (g_userinfo.account.size() != 0) {
        copy_g_userinfo = g_userinfo;
    }

    set_g_userinfo(str_pri);
    // check account file
    std::vector<std::string> keys = scan_key_dir(g_keystore_dir);
    for (size_t i = 0; i < keys.size(); ++i) {
        if (keys[i] == g_userinfo.account) {
            std::cout << "The account already exists." << std::endl;
            return "";
        }
    }

    auto path = get_keystore_filepath(dir, g_userinfo.account);
    std::ofstream key_file(path, std::ios::out | std::ios::trunc);
    if (!key_file) {
        std::cout << "Open Key File: " << path << " Error" << std::endl;
    }
    if (str_pri.size() != HEX_PRI_KEY_LEN)
        aes256_cbc_encrypt(pw, str_pri, key_file);
    else
        eth_aes256_cbc_encrypt(pw, str_pri, key_file);
    return path;
}
void generate_keystore_mac(CryptcashP::byte * mac, CryptcashP::byte * key, const string & ciphertext) {
    CryptcashP::SHA3_Final<mac_len> sha3;
    std::vector<uint8_t> vec;
    for (size_t i = aes_key_len; i < derived_key_len; ++i) {
        vec.push_back(key[i]);
    }
    for (auto c : ciphertext) {
        vec.push_back(c);
    }
    sha3.Update(vec.data(), vec.size());
    sha3.TruncatedFinal(mac, mac_len);
}
void generate_eth_keystore_mac(CryptcashP::byte * mac, CryptcashP::byte * key, const string & ciphertext) {
    std::string data = std::string((char *)key + 16, 16) + ciphertext;
    tcash::uint256_t hash_value = xkeccak256_t::digest(data.c_str(), data.size());
    memcpy(mac, (char *)&hash_value, mac_len);
}
// input base64 pri key, encrypt base64 pri key
void aes256_cbc_encrypt(const std::string & pw, const string & raw_text, std::ofstream & key_file) {
    AES_INFO aes_info;
    fill_aes_info(pw, raw_text, aes_info);
    writeKeystoreFile(key_file, aes_info.iv, aes_info.ciphertext, aes_info.info, aes_info.salt, aes_info.mac);
}
// input hex pri key, encrypt raw pri key
void eth_aes256_cbc_encrypt(const std::string & pw, const string & raw_text, std::ofstream & key_file) {
    AES_INFO aes_info;
    fill_eth_aes_info(pw, raw_text, aes_info);
    writeEthKeystoreFile(key_file, aes_info.iv, aes_info.ciphertext, aes_info.info, aes_info.salt, aes_info.mac);
}

void writeKeystoreFile(std::ofstream & key_file, byte * iv, const string & ciphertext, byte * info, byte * salt, byte * mac) {
    Json::Value key_info;
    if (!g_is_key) {
        key_info["account_address"] = g_userinfo.account;
        key_info["key_type"] = "owner";
    } else {
        key_info["account_address"] = g_owner_account;
        key_info["key_type"] = "worker";
    }

    xecprikey_t pri_key_obj(g_userinfo.private_key.data());
    xecpubkey_t pub_key_obj = pri_key_obj.get_public_key();
    std::string str_pub = utility::base64_encode(pub_key_obj.data(), pub_key_obj.size());  // tcash::HexEncode(std::string((char*)pub_key_obj.data()+1, pub_key_obj.size()-1));
    key_info["public_key"] = str_pub;

    // cipher
    key_info["crypto"]["cipher"] = "aes-256-cbc";
    key_info["crypto"]["cipherparams"]["iv"] = uint_to_str(iv, aes_iv_len);
    auto cipher_hex = uint_to_str(ciphertext.data(), ciphertext.size());
    key_info["crypto"]["ciphertext"] = cipher_hex;

    // kdf
    key_info["crypto"]["kdf"] = "hkdf";
    key_info["crypto"]["kdfparams"]["prf"] = "sha3-256";
    key_info["crypto"]["kdfparams"]["salt"] = uint_to_str(salt, salt_len);
    key_info["crypto"]["kdfparams"]["info"] = uint_to_str(info, hkdf_info_len);
    key_info["crypto"]["kdfparams"]["dklen"] = derived_key_len;

    // mac
    auto mac_hex = uint_to_str(mac, mac_len);
    key_info["crypto"]["mac"] = mac_hex;

    // hint
    key_info["hint"] = g_pw_hint;

    key_file << key_info.toStyledString();
}
void writeEthKeystoreFile(std::ofstream & key_file, byte * iv, const string & ciphertext, byte * info, byte * salt, byte * mac) {
    CryptcashP::AutoSeededRandomPool rnd;
    uint8_t id[16];
    // generate salt
    rnd.GenerateBlock(id, 16);
    std::string str_id = tcash::HexEncode(std::string((char *)id, 16));

    Json::Value key_info;
    key_info["version"] = 3;
    key_info["id"] = str_id.insert(20, "-").insert(16, "-").insert(12, "-").insert(8, "-");

    if (!g_is_key) {
        key_info["address"] = g_userinfo.account.substr(6);
        key_info["account_address"] = g_userinfo.account;
        key_info["key_type"] = "owner";
    } else {
        key_info["address"] = g_owner_account.substr(6);
        key_info["account_address"] = g_owner_account;
        key_info["key_type"] = "worker";
    }

    xecprikey_t pri_key_obj(g_userinfo.private_key.data());
    xecpubkey_t pub_key_obj = pri_key_obj.get_public_key();
    // std::string str_pub = tcash::HexEncode(std::string((char*)pub_key_obj.data()+1, pub_key_obj.size()-1));
    key_info["public_key"] = utility::base64_encode(pub_key_obj.data(), pub_key_obj.size());  // str_pub;

    // cipher
    key_info["crypto"]["cipher"] = "aes-128-ctr";
    key_info["crypto"]["cipherparams"]["iv"] = tcash::HexEncode(std::string((char *)iv, 16));
    key_info["crypto"]["ciphertext"] = tcash::HexEncode(std::string(ciphertext.data(), ciphertext.size()));

    // kdf
    unsigned iterations = 1 << 18;
    if (CRYPTO_KDF == "pbkdf2") {
        key_info["crypto"]["kdf"] = "pbkdf2";
        key_info["crypto"]["kdfparams"]["prf"] = "hmac-sha256";
        key_info["crypto"]["kdfparams"]["salt"] = tcash::HexEncode(std::string((char *)salt, salt_len));
        key_info["crypto"]["kdfparams"]["dklen"] = 32;
        key_info["crypto"]["kdfparams"]["c"] = int(iterations);
    } else if (CRYPTO_KDF == "scypt") {
        key_info["crypto"]["kdf"] = "scrypt";
        key_info["crypto"]["kdfparams"]["n"] = int(iterations);
        key_info["crypto"]["kdfparams"]["salt"] = tcash::HexEncode(std::string((char *)salt, salt_len));
        key_info["crypto"]["kdfparams"]["dklen"] = 32;
        key_info["crypto"]["kdfparams"]["r"] = 8;
        key_info["crypto"]["kdfparams"]["p"] = 1;
    } else {
        return;
    }
    // mac
    key_info["crypto"]["mac"] = tcash::HexEncode(std::string((char *)mac, mac_len));

    // hint
    key_info["hint"] = g_pw_hint;

    key_file << key_info.toStyledString();
}

// raw_text: base64 private key
void fill_aes_info(const std::string & pw, const string & raw_text, AES_INFO & aes_info) {
    CryptcashP::AutoSeededRandomPool rnd;

    // generate salt
    rnd.GenerateBlock(aes_info.salt, salt_len);
    // generate hkdf param: info
    rnd.GenerateBlock(aes_info.info, hkdf_info_len);

    // generate derived key by hdkf using input password
    CryptcashP::byte key[derived_key_len];
    CryptcashP::HKDF<CryptcashP::SHA3_Final<derived_key_len>> hkdf;
    hkdf.DeriveKey(key, derived_key_len, (const uint8_t *)(pw.data()), pw.size(), aes_info.salt, salt_len, aes_info.info, hkdf_info_len);

    // generate aes iv
    rnd.GenerateBlock(aes_info.iv, aes_iv_len);
    // encrypt raw text
    CryptcashP::AES::Encryption aesEncryption(key, aes_key_len);
    CryptcashP::CBC_Mode_ExternalCipher::Encryption cbcEncryption(aesEncryption, aes_info.iv);
    CryptcashP::StreamTransformationFilter stfEncryptor(cbcEncryption, new CryptcashP::StringSink(aes_info.ciphertext));
    stfEncryptor.Put(reinterpret_cast<const unsigned char *>(raw_text.c_str()), raw_text.length());
    stfEncryptor.MessageEnd();

    // generate mac
    generate_keystore_mac(aes_info.mac, key, aes_info.ciphertext);
}
// raw_text: hex private key
void fill_eth_aes_info(const std::string & pw, const string & raw_text, AES_INFO & aes_info) {
    std::string pri_key = tcash::HexDecode(raw_text);
    CryptcashP::AutoSeededRandomPool rnd;
    CryptcashP::byte key[32];
    // generate salt
    rnd.GenerateBlock(aes_info.salt, salt_len);
    // generate aes iv
    rnd.GenerateBlock(aes_info.iv, 16);
    unsigned iterations = 1 << 18;
    if (CRYPTO_KDF == "pbkdf2") {
        // generate derived key by pbkdf2 using input password
        if (CryptcashP::PKCS5_PBKDF2_HMAC<CryptcashP::SHA256>().DeriveKey(key, 32, 0, reinterpret_cast<byte const *>(pw.data()), pw.size(), aes_info.salt, salt_len, iterations) !=
            iterations) {
            std::cout << "Key derivation failed." << std::endl;
            return;
        }
    } else if (CRYPTO_KDF == "scypt") {
        CryptcashP::Scrypt().DeriveKey(key, 32, reinterpret_cast<byte const *>(pw.data()), pw.size(), aes_info.salt, salt_len, iterations, 8, 1);
    } else {
        return;
    }
    // encrypt raw text
    CryptcashP::CTR_Mode<CryptcashP::AES>::Encryption e;
    e.SetKeyWithIV(key, 16, aes_info.iv);
    bytes ret(pri_key.size());
    e.ProcessData(ret.data(), (const byte *)pri_key.data(), pri_key.size());
    aes_info.ciphertext = std::string((char *)ret.data(), pri_key.size());

    // generate mac
    generate_eth_keystore_mac(aes_info.mac, key, aes_info.ciphertext);
    return;
}

// update
void update_keystore_file(const std::string & pw, const string & raw_text, std::ofstream & key_file, Json::Value & key_info) {
    AES_INFO aes_info;
    std::string account = get_account_address_from_json_keystore(key_info);

    if (tcash::base::xvaccount_t::get_addrtype_from_account(account) == tcash::base::enum_vaccount_addr_type_secp256k1_user_account) {
        fill_aes_info(pw, raw_text, aes_info);
        key_info["crypto"]["cipherparams"]["iv"] = uint_to_str(aes_info.iv, aes_iv_len);
        key_info["crypto"]["kdfparams"]["salt"] = uint_to_str(aes_info.salt, salt_len);
        key_info["crypto"]["kdfparams"]["info"] = uint_to_str(aes_info.info, hkdf_info_len);
        auto mac_hex = uint_to_str(aes_info.mac, mac_len);
        key_info["crypto"]["mac"] = mac_hex;
        auto cipher_hex = uint_to_str(aes_info.ciphertext.data(), aes_info.ciphertext.size());
        key_info["crypto"]["ciphertext"] = cipher_hex;
        key_info["hint"] = g_pw_hint;
    } else if (key_info["crypto"]["kdf"] == "pbkdf2") {
        fill_eth_aes_info(pw, raw_text, aes_info);
        key_info["crypto"]["cipherparams"]["iv"] = tcash::HexEncode(std::string((char *)aes_info.iv, 16));
        key_info["crypto"]["ciphertext"] = tcash::HexEncode(aes_info.ciphertext);
        key_info["crypto"]["kdfparams"]["salt"] = tcash::HexEncode(std::string((char *)aes_info.salt, salt_len));
        key_info["crypto"]["mac"] = tcash::HexEncode(std::string((char *)aes_info.mac, mac_len));
        key_info["hint"] = g_pw_hint;
    } else if (key_info["crypto"]["kdf"] == "scrypt") {
        fill_eth_aes_info(pw, raw_text, aes_info);
        key_info["crypto"]["cipherparams"]["iv"] = tcash::HexEncode(std::string((char *)aes_info.iv, 16));
        key_info["crypto"]["ciphertext"] = tcash::HexEncode(aes_info.ciphertext);
        key_info["crypto"]["kdfparams"]["salt"] = tcash::HexEncode(std::string((char *)aes_info.salt, salt_len));
        key_info["crypto"]["mac"] = tcash::HexEncode(std::string((char *)aes_info.mac, mac_len));
        key_info["hint"] = g_pw_hint;
    } else {
        return;
    }
    key_file << key_info.toStyledString();
}

std::string get_keystore_filepath(string & dir, const string & account) {
    // create keystore directory
    std::string cmd = "mkdir -p ";
    if (dir.empty()) {
        cmd += g_keystore_dir;
        dir = g_keystore_dir;
    } else {
        cmd += dir;
    }

#if defined(__clang__)
#    pragma clang diagnostic push
#    pragma clang diagnostic ignored "-Wunused-result"
#elif defined(__GNUC__)
#    pragma GCC diagnostic push
#    pragma GCC diagnostic ignored "-Wunused-result"
#elif defined(_MSC_VER)
#    pragma warning(push, 0)
#endif

    std::system(cmd.c_str());

#if defined(__clang__)
#    pragma clang diagnostic pop
#elif defined(__GNUC__)
#    pragma GCC diagnostic pop
#elif defined(_MSC_VER)
#    pragma warning(pop)
#endif

    // create keystore file
    std::string file_name = account;
    if (g_is_key) {
        file_name = file_name.substr(tcash_ACCOUNT_PREFIX.size());
    }
    auto path = dir + "/" + file_name;

    return path;
}

Json::Value attach_parse_keystore(const string & path, std::ostringstream & out_str) {
    Json::Value key_info_js;
    std::ifstream keyfile(path, std::ios::in);
    if (!keyfile) {
        out_str << path << " Not Exist" << std::endl;
        return key_info_js;
    }

    std::stringstream buffer;
    buffer << keyfile.rdbuf();
    string key_info = buffer.str();

    Json::Reader reader;
    if (!reader.parse(key_info, key_info_js)) {
        out_str << "keyfile " << path << " parse error" << std::endl;
    }

    return key_info_js;
}

Json::Value parse_keystore(const string & path) {
    Json::Value key_info_js;
    std::ifstream keyfile(path, std::ios::in);
    if (!keyfile) {
        std::cout << path << " Not Exist" << std::endl;
        return key_info_js;
    }

    std::stringstream buffer;
    buffer << keyfile.rdbuf();
    string key_info = buffer.str();

    Json::Reader reader;
    if (!reader.parse(key_info, key_info_js)) {
        std::cout << "keyfile " << path << " parse error" << std::endl;
    }

    return key_info_js;
}

std::vector<std::string> scan_key_dir(const std::string & path) {
    std::vector<std::string> keys;
    DIR * dir = opendir(path.c_str());
    if (dir == nullptr) {
        return keys;
    }
    struct dirent * filename;
    while ((filename = readdir(dir)) != nullptr) {
        if (strcmp(filename->d_name, ".") == 0 || strcmp(filename->d_name, "..") == 0)
            continue;
        struct ::stat fs;
        std::string file_path = path + "/" + filename->d_name;
        if (::stat(file_path.c_str(), &fs) == 0 && S_ISREG(fs.st_mode)) {
            keys.push_back(std::string(filename->d_name));
        }
    }
    closedir(dir);
    return keys;
}

bool set_g_userinfo(const string & str_pri) {
    if (str_pri.empty()) {
        return false;
    }
    std::string sign_key = tcash::DecodePrivateString(str_pri);
    xecprikey_t pri_key_obj((uint8_t *)sign_key.data());
    memcpy(g_userinfo.private_key.data(), pri_key_obj.data(), pri_key_obj.size());
    if (str_pri.size() != HEX_PRI_KEY_LEN) {
        g_userinfo.account = xcrypto_util::make_address_by_assigned_key(g_userinfo.private_key);
    } else {
        g_userinfo.account = xcrypto_util::make_address_by_assigned_key(g_userinfo.private_key, tcash::base::enum_vaccount_addr_type_secp256k1_eth_user_account);
    }

    return g_userinfo.account.size() > 0;
}

}  // namespace xcrypto
}  // namespace xChainSDK

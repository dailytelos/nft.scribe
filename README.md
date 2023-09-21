# Work in Process code!
- There are bugs in this code, not ready for production yet.

---

# NFT.scribe: Bridging the NFT Multiverse on Telos

**NFT.scribe** is a versatile Telos C++ smart contract designed to bridge the realms of Non-Fungible Tokens (NFTs)
across different networks. Through a series of specialized modules, the contract enables users to register NFTs, manage cross-chain NFT details,
and engage with oracle-driven post actions. Coupled with a user-centric approach,
it provides network management features and fosters an integrated ecosystem for NFT aficionados and developers alike.
Our contract ensures a seamless experience for interoperability and functionalities vital for the thriving NFT community on the Telos platform.

---
---

# manage.hpp
## Public Contract Management
### Actions
- sysglobalstr(`name &var`, `string &sval`)
- sysglobalint(`name &var`, `uint64_t &nval`)
- sysglobalast(`name &var`, `asset &aval`)
- sysdefaults()
- sysdrawacct(`name &acct`, `name &to`, `asset &quant`, `std::string &memo`)
- sysdeposit(`name &user`, `asset &quant`, `string &memo`)
- sysdelglobal(`name &var`)
- sysfreeze(`uint64_t &freeze`)
- draw(`name &user`)
### Tables
- globalvars (`name var`, `string sval`, `uint64_t nval`, `asset aval`, `time_point_sec ts`, `string memo`)
- deposits (`name account`, `asset total`, `asset available`)

---

# networks.hpp
## Actions
- netwreg(`const name& id`, `const string& title`, `const string& chain_id`, `const string& ticker`, `const string& block_expl`)
- netwactive(`const name& id`, `const uint8_t& active`)
- netwthresh(`const name& id`, `const uint16_t& threshold`)
## Table
- networks (`name id`, `string title`, `string chain_id`, `string ticker`, `string block_expl`, `uint8_t active`, `uint16_t threshold`)

---

# nftservice.hpp
## Actions
- nftregister(`const name& auth`, `const name& suffix`, `const name& network_id`, `const string& nftcontract`, `const vector <string>& contracts`, `const name& admin`, `const string& website`,  `const string& admin_email`)
- nftactive(`const name& auth`, `const name& suffix`, `const name& network_id`, `const uint8_t& active`)
- nftaddtoken(`const name& auth`, `const name& suffix`, `const name& network_id`, `const uint64_t& token_id`)
- nftdeltoken(`const name& auth`, `const name& suffix`, `const name& network_id`, `const uint64_t& token_id`)
## Table
- nftservice (`name auth`, `name suffix`, `name network_id`, `string nftcontract`, `vector<string> contracts`, `name admin`, `string website`, `string admin_email`, `uint8_t active`, `vector<uint64_t> tokens`)

---

# nftusers.hpp
## Table
- nftusers (`name user`, `vector<uint128_t> nft_ids`, `vector<name> network_ids`, `vector<uint64_t> posts_ids`)

---

# oracles.hpp
## Actions
- orcregister(`const name& auth`, `const name& oracle_id`, `const name& network_id`, `const string& apisource`)
- orcstatus(`const name& oracle_id`, `const name& network_id`, `const int8_t& active`)
- orckick(`const name& auth`, `const name& oracle_id`, `const name& network_id`, `const uint32_t& hours`)
- orcban(`const name& auth`, `const name& oracle_id`, `const name& network_id`, `const uint8_t& ban`)
- orcrefresh(`const name& auth`, `const name& oracle_id`, `const name& network_id`)
## Table
- oracles (`name oracle_id`, `name network_id`, `string apisource`, `int8_t active`, `uint32_t hours`, `uint8_t ban`, `time_point_sec refresh_time`, `vector<uint64_t> posts_ids`)

---

# posts.hpp
## Actions
- post(
  `const name& oracle_id`,
  `const name& network_id`,
  `const name& suffix`,
  `const name& contract`,
  `const name& post_action`,
  `const name& userid`,
  `const uint16_t& sign_type`,
  `const string& eth_pub_key`,
  `const uint128_t& nft_id`,
  `const string& unsigned_data`,
  `const string& signed_data`,
  `const time_point_sec& tps_posted`,
  `const time_point_sec& tps_created`,
  `const time_point_sec& tps_expires`
)
- upvote(`const name& oracle_id`, `const name& network_id`, `const uint64_t& posts_id`)
- downvote(`const name& oracle_id`, `const name& network_id`, `const uint64_t& posts_id`)
## Table
- posts (`name oracle_id`, `name network_id`, `name suffix`, `name contract`, `name post_action`, `name userid`, `uint16_t sign_type`, `string eth_pub_key`, `uint128_t nft_id`, `string unsigned_data`, `string signed_data`, `time_point_sec tps_posted`, `time_point_sec tps_created`, `time_point_sec tps_expires`, `uint16_t votes`)

---

# tokens.hpp
## Actions
- sysaddtoken(`const uint64_t& id`, `const name& contract`, `const string& sym_code`, `const uint8_t& prec`)
- sysdeltoken(`const uint64_t& id`)
## Table
- tokenstbl (`uint64_t id`, `name contract`, `string sym_code`, `uint8_t prec`)
